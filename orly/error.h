/* <orly/error.h>

   Error classes for the orly compiler.

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

#include <cassert>

#include <base/as_str.h>
#include <base/thrower.h>
#include <orly/pos_range.h>

namespace Orly {

  class TSourceError : public std::runtime_error {
    public:

    const TPosRange &GetPosRange() const {
      assert(this);
      return PosRange;
    }

    protected:

    TSourceError(const TPosRange &pos_range, const char *msg)
        : std::runtime_error(msg), PosRange(pos_range) {}

    private:

    const TPosRange PosRange;

  };  // TSourceError

  class TNotImplementedError : public TSourceError {
    public:

    TNotImplementedError(
        const Base::TCodeLocation &code_location,
        const TPosRange &pos_range,
        const char *message = "This feature is not yet implemented")
          : TSourceError(pos_range, Base::AsStr(code_location, message).c_str()) {}

  };  // TNotImplementedError

  class TImpossibleError : public TSourceError {
    public:

    TImpossibleError(
        const Base::TCodeLocation &code_location,
        const TPosRange &pos_range,
        const char *message = "Internal Compiler Error: We shouldn't have reached this line of code in the compiler.")
          : TSourceError(pos_range, Base::AsStr(code_location, message).c_str()) {}

  };  // TImpossibleError

  class TCompileError : public TSourceError {
    public:

    TCompileError(
        const Base::TCodeLocation &code_location,
        const TPosRange &pos_range,
        const char *message)
          : TSourceError(pos_range, Base::AsStr(code_location, message).c_str()) {}

  };  // TCompileError

  class TExprError : public TSourceError {
    public:

    TExprError(
        const Base::TCodeLocation &code_location,
        const TPosRange &pos_range,
        const char *message = "This expression is invalid.")
          : TSourceError(pos_range, Base::AsStr(code_location, message).c_str()) {}

  };  // TExprError

}  // Orly
