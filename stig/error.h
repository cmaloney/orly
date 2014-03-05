/* <stig/error.h>

   Error classes for the stig compiler.

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

#include <base/error.h>
#include <stig/pos_range.h>

namespace Stig {

  class TSourceError
      : public virtual Base::TError {
    public:

    const TPosRange &GetPosRange() const {
      assert(this);
      return PosRange;
    }

    protected:

    TSourceError(const TPosRange &pos_range)
        : PosRange(pos_range) {}

    private:

    const TPosRange PosRange;

  };  // TSourceError

  class TNotImplementedError
      : public TSourceError, public Base::TFinalError<TNotImplementedError> {
    public:

    TNotImplementedError(
        const Base::TCodeLocation &code_location,
        const TPosRange &pos_range,
        const char *message = "This feature is not yet implemented")
          : TSourceError(pos_range) {
      PostCtor(code_location, message);
    }

  };  // TNotImplementedError

  class TImpossibleError
      : public TSourceError, public Base::TFinalError<TImpossibleError> {
    public:

    TImpossibleError(
        const Base::TCodeLocation &code_location,
        const TPosRange &pos_range,
        const char *message = "Internal Compiler Error: We shouldn't have reached this line of code in the compiler.")
          : TSourceError(pos_range) {
      PostCtor(code_location, message);
    }

  };  // TImpossibleError

  class TCompileError
      : public TSourceError, public Base::TFinalError<TCompileError> {
    public:

    TCompileError(
        const Base::TCodeLocation &code_location,
        const TPosRange &pos_range,
        const char *message)
          : TSourceError(pos_range) {
      PostCtor(code_location, message);
    }

  };  // TCompileError

  class TExprError
      : public TSourceError, public Base::TFinalError<TExprError> {
    public:

    TExprError(
        const Base::TCodeLocation &code_location,
        const TPosRange &pos_range,
        const char *message = "This expression is invalid.")
          : TSourceError(pos_range) {
      PostCtor(code_location, message);
    }

  };  // TExprError

}  // Stig
