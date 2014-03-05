/* <tools/nycr/decl.h>

   The base for all declarations.

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
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include <base/no_copy_semantics.h>
#include <base/opt.h>
#include <tools/nycr/cst_redirect.h>

namespace Tools {

  namespace Nycr {

    /* Unravel the TOptFoo / TNoFoo / TFoo pattern. */
    template <typename TYesNode, typename TNoNode, typename TOptNode>
    static const TYesNode *TryGetNode(const TOptNode *opt_node) {
      assert(opt_node);
      class TVisitor : public TOptNode::TVisitor {
        public:
        TVisitor(const TYesNode *&yes_node) : YesNode(yes_node) {}
        virtual void operator()(const TYesNode *that) const { YesNode = that; }
        virtual void operator()(const TNoNode *) const      { YesNode = 0; }
        private:
        const TYesNode *&YesNode;
      };
      const TYesNode *yes_node;
      opt_node->Accept(TVisitor(yes_node));
      return yes_node;
    }

    /* Retrieve an int literal, if given. */
    template <typename TYesNode, typename TNoNode, typename TOptNode>
    static void GetOptInt(const TOptNode *opt_node, Base::TOpt<int> &out) {
      assert(opt_node);
      assert(&out);
      const TYesNode *yes_node = TryGetNode<TYesNode, TNoNode>(opt_node);
      if (yes_node) {
        out = yes_node->GetIntLiteral()->GetLexeme().AsInt();
      } else {
        out.Reset();
      }
    }

    /* The base for all declarations. */
    class TDecl {
      NO_COPY_SEMANTICS(TDecl);
      public:

      /* The our name as plain text. */
      const std::string &GetNameText() const {
        assert(this);
        return Name->GetLexeme().GetText();
      }

      /* Bind all the refs in all the declarations, then moves through the build phases until all symbols are build. */
      static void BindAndBuildEach();

      /* Delete all the declarations. */
      static void DeleteEach();

      protected:

      /* Specialize this template for each subclass of TDecl, final or not.  In each specialization, provide a static const char * member called
         Name which gives the human-readable name of the subclass in lower-case. */
      template <typename TFinalDecl>
      struct TInfo {};

      /* The base for all references to be bound. */
      class TAnyRef {
        NO_COPY_SEMANTICS(TAnyRef);
        public:

        /* Do-little. */
        virtual ~TAnyRef();

        /* Bind the reference to the declaration it names. */
        void Bind();

        /* Set the reference by giving it a syntax tree node which represent it.  We must not already be bound. */
        void Set(const Syntax::TName *name) {
          assert(this);
          assert(!IsBound);
          Name = name;
        }

        /* Conditionally set the reference, when the name is optional.  We must not already be bound. */
        template <typename TYesNode, typename TNoNode, typename TOptNode>
        void Set(const TOptNode *opt_node) {
          assert(this);
          assert(!IsBound);
          const TYesNode *yes_node = TryGetNode<TYesNode, TNoNode>(opt_node);
          if (yes_node) {
            Name = yes_node->GetName();
          }
        }

        protected:

        /* Start out unbound. */
        TAnyRef(const Syntax::TName *name = 0)
            : Name(name), IsBound(false) {}

        /* Cache the type-specific pointer, or return an error message if the type is incorrect. */
        virtual const char *SetDecl(TDecl *decl) = 0;

        /* The syntax node which gave rise to this reference. */
        const Syntax::TName *Name;

        /* True when we're bound, even if we're bound to null. */
        bool IsBound;

      };  // TAnyRef

      /* A type-specific reference. */
      template <typename TSomeDecl>
      class TRef
          : public TAnyRef {
        NO_COPY_SEMANTICS(TRef);
        public:

        /* Do little. */
        TRef(const Syntax::TName *name = 0)
            : TAnyRef(name), SomeDecl(0) {}

        /* Are we not null? */
        operator bool() const {
          assert(this);
          assert(IsBound);
          return SomeDecl != 0;
        }

        /* Dereference the reference.  We must be bound first. */
        TSomeDecl *operator*() const {
          assert(this);
          assert(IsBound);
          return SomeDecl;
        }

        /* Dereference the reference.  We must be bound first. */
        TSomeDecl *operator->() const {
          assert(this);
          assert(IsBound);
          return SomeDecl;
        }

        private:

        /* See our base class. */
        virtual const char *SetDecl(TDecl *decl) {
          assert(this);
          assert(decl);
          SomeDecl = dynamic_cast<TSomeDecl *>(decl);
          return !SomeDecl ? TInfo<TSomeDecl>::Name : 0;
        }

        private:

        /* The declaration to which we're bound, if any. */
        TSomeDecl *SomeDecl;

      };  // TRef<TObj>

      /* TODO */
      TDecl(const Syntax::TName *name);

      /* TODO */
      virtual ~TDecl();

      /* TODO */
      virtual bool Build(int pass) = 0;

      /* TODO */
      virtual void ForEachPred(int pass, const std::function<void (TDecl *)> &cb);

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      private:

      /* TODO */
      enum TState { Unstarted, Started, Finished };

      /* TODO */
      void Bind();

      /* TODO */
      void BuildPredsAndSelf(int pass, bool &again);

      /* TODO */
      static void ForEachUniqueDecl(const std::function<void (TDecl *)> &cb);

      /* TODO */
      static TDecl *TryGetDecl(const Syntax::TName *name);

      /* TODO */
      const Syntax::TName *Name;

      /* TODO */
      int Readiness;

      /* TODO */
      TState State;

      /* TODO */
      static std::map<std::string, std::vector<TDecl *>> DeclsByName;

    };  // TDecl

  }  // Nycr

}  // Tools
