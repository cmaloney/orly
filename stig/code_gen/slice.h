/* <stig/code_gen/slice.h>

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

namespace Stig {

  namespace CodeGen {

    //TODO: Seems to be moderately similar to range.
    class TSlice : public TInline {
      NO_COPY_SEMANTICS(TSlice);

      public:

      TSlice(const L0::TPackage *package,
             const Type::TType &ret_type,
             const TInline::TPtr &container,
             const TInline::TPtr &opt_lhs,
             const TInline::TPtr &opt_rhs,
             bool colon);

      void WriteExpr(TCppPrinter &out) const;

      /* Dependency graph */
      virtual void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const override {
        assert(this);
        dependency_set.insert(Container);
        Container->AppendDependsOn(dependency_set);
        if (OptLhs) {
          dependency_set.insert(OptLhs);
          OptLhs->AppendDependsOn(dependency_set);
        }
        if (OptRhs) {
          dependency_set.insert(OptRhs);
          OptRhs->AppendDependsOn(dependency_set);
        }
      }

      private:
      bool Colon;
      TInline::TPtr Container, OptLhs, OptRhs;
    };

  } // CodeGen

} // Stig