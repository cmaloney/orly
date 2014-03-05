/* <stig/synth/scope_and_def.h>

   A scope and the definitions which reside therein.

   INHERITING FROM DEF:

     * If the syntactic representation of the def contains any named references to other defs,
       then your def should aggregate one or more instances of TRef<>.
     * If the syntactic representation of the def implies a local scope (such as a where-clause has),
       then your def should aggregate an instance of TScope.

     (1) Specialize TDef::TInfo<>.
     (2) If the def contains any inner scopes, override TDef::ForEachInnerScope().
     (3) If the def contains any refs, override TDef::ForEachRef().
     (4) If the def requires out-of-order building in any pass, override TDef::ForEachPred().
     (5) Override TDef::Build(int pass).

   Copyright 2010-2014 Stig LLC

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#pragma once

#include <cassert>
#include <functional>

#include <base/assert_true.h>
#include <base/no_copy_semantics.h>
#include <inv_con/unordered_multimap.h>
#include <stig/symbol/scope.h>
#include <stig/synth/cst_utils.h>
#include <stig/synth/name.h>

namespace Stig {

  namespace Synth {

    /* Forward declarations. */
    class TScope;
    class TDef;

    /* The base for all references. */
    class TAnyRef {
      NO_COPY_SEMANTICS(TAnyRef);
      public:

      /* Do-little. */
      virtual ~TAnyRef() {}

      /* True iff. we're non-null. */
      operator bool() const {
        assert(this);
        return Name;
      }

      /* Bind the reference to the def it names w.r.t. the given scope. */
      virtual void Bind(const TScope *scope) = 0;

      /* Dereference the reference without being type-specific.  We must be bound first. */
      virtual TDef *GetDef() const = 0;

      /* True when we're bound, even if we're bound to null. */
      bool GetIsBound() const {
        assert(this);
        return IsBound;
      }

      /* The name to which we want to bind (or have already bound). */
      const TName &GetName() const {
        assert(this);
        return Name;
      }

      /* Set the name of the def to which the ref will point.  The ref must must not already be bound. */
      void SetName(const TName::TNode *node) {
        assert(this);
        assert(!IsBound);
        Name = node;
      }

      /* Conditionally set the reference, when the name is optional.  We must not already be bound. */
      template <typename TYesNode, typename TNoNode, typename TOptNode>
      void SetName(const TOptNode *opt_node) {
        assert(this);
        assert(!IsBound);
        const TYesNode *yes_node = TryGetNode<TYesNode, TNoNode>(opt_node);
        if (yes_node) {
          Name = yes_node->GetName();
        }
      }

      protected:

      /* Start out unbound. */
      TAnyRef(const TName::TNode *node)
          : Name(node), IsBound(false) {}

      /* Mark us as having been bound.  We must not already be bound. */
      void SetIsBound() {
        assert(this);
        assert(!IsBound);
        IsBound = true;
      }

      private:

      /* See accessor. */
      TName Name;

      /* See accessor. */
      bool IsBound;

    };  // TAnyRef

    /* The action to take after completing a build step. */
    enum TAction {

      /* Continue with another pass. */
      Continue,

      /* No additional pass is needed; we're all done. */
      Finish,

      /* The build has failed.  Do not make another pass.  Exit as quickly as possible. */
      Fail

    };  // TAction

    /* A scope in which definitions reside.
       All the scopes in a package form a tree, rooted at the global scope of a package.
       The sub-scopes occur inside some definitions.
       Every definition has a name, which, in a correct program, must be unique within a scope.
       However, we don't assume that the program is correct,
       each scope maintains a multimap between itself and its defs. */
    class TScope {
      NO_COPY_SEMANTICS(TScope);
      public:

      /* The type of our collection of defs. */
      typedef InvCon::UnorderedMultimap::TCollection<TScope, TDef, TName> TDefs;

      /* Destroys all defs in the scope as well. */
      virtual ~TScope();

      /* Binds each controlled ref and the refs in each (unique) local def.
         Floods into sub-scopes. */
      void Bind() const;

      /* Build the scope symbol */
      virtual void BuildSymbol() = 0;

      /* Perform the given build pass on each (unique) local def.
         Return:
           Continue: another pass is required
           Finish: no more passes needed, all defs are finished
           Fail: no more passes needed, one or more defs failed to build
         Performs out-of-border builds as necessary and steps into local scopes. */
      TAction BuildEachDef(int pass) const;

      /* The collection of defs. */
      TDefs *GetDefs() const {
        assert(this);
        return &Defs;
      }

      virtual Symbol::TScope::TPtr GetScopeSymbol() const = 0;

      virtual bool HasSymbol() const = 0;

      /* Look up the given name in this scope.
         If the name is null, return null and don't add an error.
         If the type of the def does not match the type requested, add an error and return null.
         If name mapes to multiple (local) defs, add an error and return null.
         If it is not locally defined, repeat the process in our parent scope.
         If we run out of parents, add an error and return null. */
      template <typename TSomeDef>
      TSomeDef *TryGetDef(const TName &name) const;

      protected:

      /* The root scope has no parent; all other scopes do. */
      TScope(TScope *parent = nullptr);

      /* Call back for each ref controlled directly by this scope.
         The default implementation does not call back.
         Ovveride for any scope with controlled refs in it. */
      virtual void ForEachControlledRef(const std::function<void (TAnyRef &)> &cb) const;

      private:

      /* Does all the work for TryGetDef(), but doesn't do the type checking. */
      TDef *TryGetAnyDef(const TName &name) const;

      /* Add an error indicating that the requested def was not of the expected type. */
      static void AddError(const TName &name, const char *expected_type);

      /* Our parent scope.  This will be null iff. we're the root scope, . */
      TScope *Parent;

      /* See accessor. */
      mutable TDefs::TImpl Defs;

    };  // TScope

    /* A definition which lives within a scope.
       The scope in which a def exists is the outer scope of the def.
       For most defs, this is the only scope they will deal with.
       However, some defs define their own, local scope as well.
       This is the inner scope of the def, and sub-defs will live within it. */
    class TDef {
      NO_COPY_SEMANTICS(TDef);
      public:

      /* The type of our membership in scope's collection of defs. */
      typedef InvCon::UnorderedMultimap::TMembership<TDef, TScope, TName> TScopeMembership;

      /* Specialize this template for each subclass of TDef, final or not.
         In each specialization, provide a static const char * member called Name
         which gives the human-readable name of the subclass in lower-case and
         with a leading indefinite article.
         For example, if we have a class called TElephant which inherits (directly or indirectly)
         from TDef, we should write the following in the header file for the class:
           template <>
           struct TDef::TInfo<TElephant> {
             static const char *Name;
           };
         And then in the .cc file:
           const char *TDef::TInfo<TElephant>::Name = "an elephant";
         Users will actually see this string.  We use it to format error messages, so be presentable. */
      template <typename TSomeDef>
      struct TInfo {};

      /* A type-specific reference. */
      template <typename TSomeDef>
      class TRef
          : public TAnyRef {
        NO_COPY_SEMANTICS(TRef);
        public:

        /* Do little. */
        TRef(const TName::TNode *node = nullptr)
            : TAnyRef(node), Def(nullptr) {}

        /* Dereference the reference.  We must be bound first. */
        TSomeDef *operator*() const {
          assert(this);
          assert(GetIsBound());
          return Def;
        }

        /* Dereference the reference.  We must be bound first. */
        TSomeDef *operator->() const {
          assert(this);
          assert(GetIsBound());
          return Def;
        }

        /* See base class. */
        virtual void Bind(const TScope *scope) {
          assert(this);
          assert(scope);
          Def = scope->TryGetDef<TSomeDef>(GetName());
          SetIsBound();
        }

        /* See base class. */
        virtual TDef *GetDef() const {
          assert(this);
          return Def;
        }

        private:

        /* The declaration to which we're bound, if any.
           This will be null before we're bound and may remain so after we're bound. */
        TSomeDef *Def;

      };  // TRef<TSomeDef>

      /* Unlinks from scope on its way out. */
      virtual ~TDef();

      /* Bind each ref in this def.  If the def has a local scope, we'll step into it as well. */
      void BindEachRef();

      /* The name associated with this def. */
      const TName &GetName() const {
        assert(this);
        return ScopeMembership.GetKey();
      }

      /* The scope in which we reside.  Never null. */
      TScope *GetOuterScope() const {
        assert(this);
        return Base::AssertTrue(ScopeMembership.TryGetCollector());
      }

      /* Our membership in our scope's collection of defs. */
      TScopeMembership *GetScopeMembership() const {
        assert(this);
        return &ScopeMembership;
      }

      /* Perform the given build pass on each def yielded by the given generator.
         It's ok for the generator to generate null pointers.
         Return:
           Continue: another pass is required
           Finish: no more passes needed, all defs are finished
           Fail: no more passes needed, one or more defs failed to build
         Performs out-of-border builds as necessary and steps into local scopes. */
      static TAction BuildEachDef(
          int pass,
          const std::function<void (const std::function<bool (TDef *)> &)> &generate);

      protected:

      /* Scope must be non-null. */
      TDef(TScope *outer_scope, const TName &name);

      /* Take whatever build action is appropriate for the given pass.
         Return:
           Continue: another pass is required
           Finish: no more passes needed, this def is finished
           Fail: no more passes needed, this def has failed
         This function will be called at most once for each def for each pass. */
      virtual TAction Build(int pass) = 0;

      /* Call back for each inner scope in this def.
         The default implementation does not call back.
         Override for any def with a local scope. */
      virtual void ForEachInnerScope(const std::function<void (TScope *)> &cb);

      /* Call back for each predecessor for the given pass.
         The default implementation does not call back.
         Override for any def which must perform one or more passes out of natural order.
         If the callback returns false, do not call it again.  Exit early. */
      virtual void ForEachPred(int pass, const std::function<bool (TDef *)> &cb);

      /* Call back for each ref in this def.
         The default implementation does not call back.
         Override for any def with refs in it. */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      private:

      /* The states used in building. */
      enum TState {

        /* The def is ready for another pass. */
        Ready,

        /* The def is currently building itself. */
        InProgress,

        /* The def has finished all passes successfully. */
        Shiny,

        /* The def has failed to build and more passes won't help. */
        Cruddy

      };  // TState

      /* Take whatever build action is appropriate for the given pass.
         Return:
           Continue: another pass is required
           Finish: no more passes needed, this def is finished
           Fail: no more passes needed, this def has failed
         Fails automatically if a dependency cycle is found.  Steps into local scope (if any).
         This function may be called multiple times for a given pass but will not call Build() more than once. */
      TAction BuildHelper(int pass);

      /* See accessor. */
      mutable TScopeMembership::TImpl ScopeMembership;

      /* The number of build passes which this def has completed.  This value starts at zero. */
      int Readiness;

      /* We start out as Ready. */
      TState State;

    };  // TDef

    /* See declaration. */
    template <typename TSomeDef>
    TSomeDef *TScope::TryGetDef(const TName &name) const {
      assert(this);
      TSomeDef *some_def;
      TDef *def = TryGetAnyDef(name);
      if (def) {
        some_def = dynamic_cast<TSomeDef *>(def);
        if (!some_def) {
          AddError(name, TDef::TInfo<TSomeDef>::Name);
        }
      } else {
        some_def = nullptr;
      }
      return some_def;
    }

  }  // Synth

}  // Stig
