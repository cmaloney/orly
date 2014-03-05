/* <stig/code_gen/if_else.h>

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

#include <iostream>
#include <stig/code_gen/inline.h>
#include <stig/code_gen/inline_scope.h>

namespace Stig {

  namespace CodeGen {

    class TIfElse : public TInline {
      NO_COPY_SEMANTICS(TIfElse);
      public:

      typedef std::shared_ptr<const TIfElse> TPtr;

      static TPtr New(
          const L0::TPackage *package,
          const Type::TType &ret_type,
          const Expr::TExpr::TPtr &true_case,
          const TInline::TPtr &predicate,
          const Expr::TExpr::TPtr &false_case);

      void WriteExpr(TCppPrinter &out) const;

      /* Dependency graph */
      virtual void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const override {
        assert(this);
        if (!InDependsOn) {
          InDependsOn = true;
          dependency_set.insert(Predicate);
          Predicate->AppendDependsOn(dependency_set);
          dependency_set.insert(True);
          True->AppendDependsOn(dependency_set);
          dependency_set.insert(False);
          False->AppendDependsOn(dependency_set);
          InDependsOn = false;
        }
      }

      private:
      TIfElse(const L0::TPackage *package,
              const Type::TType &ret_type,
              const Expr::TExpr::TPtr &true_case,
              const TInline::TPtr &predicate,
              const Expr::TExpr::TPtr &false_case);

      TInline::TPtr Predicate;
      TInlineScope::TPtr True, False;

      /* we turn this on when we're in the AppendDependsOn function so that we can tell when we recurse. */
      mutable bool InDependsOn;
    };

  } // CodeGen

} // Stig