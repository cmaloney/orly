/* <orly/code_gen/inline_scope.h>

   TODO

   Copyright 2010-2014 OrlyAtomics, Inc.

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

#include <orly/code_gen/inline.h>
#include <orly/code_gen/package_base.h>
#include <orly/code_gen/scope.h>
#include <orly/expr/expr.h>

namespace Orly {

  namespace CodeGen {

    class TInlineScope : public TInline {
      NO_COPY(TInlineScope);
      public:

      typedef std::shared_ptr<const TInlineScope> TPtr;

      static TInlineScope::TPtr New(const L0::TPackage *package, const Expr::TExpr::TPtr &expr, bool keep_mutable=true);

      ~TInlineScope();

      void WriteExpr(TCppPrinter &out) const;

      /* Dependency graph */
      virtual void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const override {
        assert(this);
        dependency_set.insert(Body);
        Body->AppendDependsOn(dependency_set);
      }

      private:
      /* NOTE: Takes ownership of the scope. */
      TInlineScope(const L0::TPackage *package, TCodeScope *scope, const TInline::TPtr &body);
      TInline::TPtr Body;
      TCodeScope *Scope;
    }; // TInlineScope

    Orly::CodeGen::TCppPrinter &operator<<(Orly::CodeGen::TCppPrinter &out,
        const Orly::CodeGen::TInlineScope::TPtr &ptr);

  } // CodeGen

} // Orly