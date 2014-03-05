/* <stig/rt/div.h>

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

#include <stig/rt/opt.h>
#include <stig/rt/runtime_error.h>
#include <stig/rt/operator.h>

namespace Stig {

  namespace Rt {

    class TDivisionByZeroError
        : public TRuntimeError, public Base::TFinalError<TDivisionByZeroError> {
      public:

      /* Constructor. */
      TDivisionByZeroError(const Base::TCodeLocation &code_location) {
        PostCtor(code_location, "Attempted to perform a division by 0.");
      }

    };  // TDivisionByZeroError

    inline double Div(double lhs, double rhs) {
      if (rhs == 0) {
        throw TDivisionByZeroError(HERE);
      }
      return lhs / rhs;
    }

    inline int64_t Div(int64_t lhs, int64_t rhs) {
      if (rhs == 0) {
        throw TDivisionByZeroError(HERE);
      }
      return lhs / rhs;
    }

    template <typename TLhs, typename TRhs>
    TOpt<int64_t> Div(TLhs &&lhs, TRhs &&rhs) {
      assert(&lhs);
      assert(&rhs);
      return IsKnown(lhs) && IsKnown(rhs) ? Div(GetVal(lhs), GetVal(rhs)) : *TOpt<int64_t>::Unknown;
    }

  }  // Rt

}  // Stig