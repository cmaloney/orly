/* <stig/code_gen/built_in_call.h>

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

#include <stig/code_gen/inline.h>
#include <stig/symbol/built_in_function.h>

namespace Stig {

  namespace CodeGen {

    class TBuiltInCall : public TInline {
      public:

      typedef std::unordered_map<std::string, TInline::TPtr> TArguments;
      typedef std::shared_ptr<const Symbol::TBuiltInFunction> TFunctionPtr;

      TBuiltInCall(const L0::TPackage *package,
                   const TFunctionPtr &func,
                   TArguments &&arguments);

      void WriteExpr(TCppPrinter &out) const final;

      /* Dependency graph */
      virtual void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const override {
        assert(this);
        for (const auto &iter : Arguments) {
          dependency_set.insert(iter.second);
          iter.second->AppendDependsOn(dependency_set);
        }
      }

      private:
      TArguments Arguments;
      TFunctionPtr Func;
    }; // TBuiltInCall


  } // CodeGen

} // Stig