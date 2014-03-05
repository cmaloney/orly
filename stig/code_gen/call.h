/* <stig/code_gen/call.h>

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

#include <vector>

#include <base/no_copy_semantics.h>
#include <stig/code_gen/function.h>
#include <stig/code_gen/inline.h>

namespace Stig {

  namespace CodeGen {

    class TCall : public TInline {
      NO_COPY_SEMANTICS(TCall);
      public:

      typedef std::vector<TInline::TPtr> TArgs;
      typedef std::shared_ptr<const TCall> TPtr;

      //Note: The args vector MUST be ordered asciibetically by variable name.
      TCall(const L0::TPackage *package, const TFunction::TPtr &func, const TArgs &args);

      void WriteExpr(TCppPrinter &out) const;

      /* Dependency graph */
      virtual void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const override {
        assert(this);
        for (const auto &iter : Args) {
          dependency_set.insert(iter);
          iter->AppendDependsOn(dependency_set);
        }
        dependency_set.insert(Func->GetBody());
        Func->GetBody()->AppendDependsOn(dependency_set);
      }

      private:
        TArgs Args;
        TFunction::TPtr Func;
    }; // TCall


  } // CodeGen

} // Stig