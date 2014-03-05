/* <stig/code_gen/function.h>

   TODO

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

#include <unordered_set>

#include <stig/code_gen/cpp_printer.h>
#include <stig/code_gen/scope.h>
#include <stig/code_gen/id.h>
#include <stig/code_gen/inline.h>
#include <stig/code_gen/inline_scope.h>
#include <stig/code_gen/package_base.h>
#include <stig/symbol/function.h>

namespace Stig {

  namespace CodeGen {

    class TInlineFunc;

    /* Base class for code gen functions. See leaf classes TTopFunc, TInnerFunc, TImplicitFunc for instantiable
       instanecs. */
    class TFunction : public std::enable_shared_from_this<TFunction> {
      NO_COPY_SEMANTICS(TFunction);
      public:

      typedef std::shared_ptr<TFunction> TPtr;
      typedef std::map<std::string, Type::TType> TNamedArgs;

      class TArg {
        NO_COPY_SEMANTICS(TArg);
        public:
        /* An inline for a function argument.

          TRefs aren't interned using the interner because that would make variable aliases like 'v0 = a0'. Rather
          every arg has one, and only one TRef which it gives pointers to to anyone who asks for an inline version. */
        class TRef : public TInline {
          NO_COPY_SEMANTICS(TRef);
          public:

          typedef std::shared_ptr<const TRef> TPtr;

          void WriteExpr(TCppPrinter &out) const;

          /* Dependency graph */
          virtual void AppendDependsOn(std::unordered_set<TInline::TPtr> &/*dependency_set*/) const override {
            assert(this);
          }

          private:
          TRef(const L0::TPackage *package, const TArg *arg);

          const TArg *Arg;

          friend class TArg;
        }; // TRef

        typedef std::shared_ptr<TArg> TPtr;

        static TPtr New(const L0::TPackage *package, TId<TIdKind::Arg> &&id, const Type::TType &type);

        const TId<TIdKind::Arg> &GetId() const;
        const TRef::TPtr &GetRef() const;
        Type::TType GetType() const;

        private:
        TArg(const L0::TPackage *package, TId<TIdKind::Arg> &&id, const Type::TType &type);
        TId<TIdKind::Arg> Id;
        Type::TType Type;
        TRef::TPtr Ref;
      }; // TArg

      //TODO: This feels much more "ugh" than necessary.
      //A map so that we absolutely order the arguments.
      typedef std::map<std::string, TArg::TPtr> TArgs;

      virtual ~TFunction() {}

      void AddChild(const std::shared_ptr<TInlineFunc> &func);

      /* Build the function body, if necessary. */
      void Build();

      TArg::TRef::TPtr GetArg(const std::string &name) const;
      const TArgs &GetArgs() const;

      TCodeScope *GetCodeScope();

      /* Get the original name of the function. */
      virtual std::string GetName() const = 0;

      virtual Type::TType GetReturnType() const = 0;
      virtual Type::TType GetType() const = 0;
      bool HasArgs() const;

      /* Return true if the function is top level, which means it needs context passed as a parameter. */
      virtual bool IsTopLevel() const = 0;

      /* Write the function's arguments out in C++. */
      void WriteArgs(TCppPrinter &out) const;

      /* Writes body of function. */
      void WriteBody(TCppPrinter &out) const;

      /* Writes forward declaration (Ex. void FuncFoobar() or std::function<void ()> foobar) */
      virtual void WriteDecl(TCppPrinter &out) const = 0;

      /* Writes the standard C++ definition of this function. */
      virtual void WriteDef(TCppPrinter &out) const = 0;

      /* Write out the C++ code gen name of the function. */
      virtual void WriteName(TCppPrinter &out) const = 0;

      /* Access to body's DependsOn. */
      const TInline::TPtr &GetBody() const {
        assert(this);
        return Body;
      }

      protected:
      TFunction(const L0::TPackage *package, const TIdScope::TPtr &id_scope);

      //TODO: I think the PostCtor could go away...
      void PostCtor(const TNamedArgs &args, const Expr::TExpr::TPtr &expr, bool keep_mutable, bool implicit=false);

      //TODO: These should really be private
      TCodeScope CodeScope;
      std::unordered_set<std::shared_ptr<TInlineFunc>> ChildFuncs;

      /* TODO */
      const L0::TPackage *Package;

      private:
      TArgs Args;
      TInline::TPtr Body;
      Expr::TExpr::TPtr Expr;
      bool Implicit, KeepMutable;

      friend class TCall;
    }; // TFunction

  } // CodeGen

} // Stig