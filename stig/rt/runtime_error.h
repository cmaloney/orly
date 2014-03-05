/* <stig/rt/runtime_error.h>

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

#include <base/error.h>

namespace Stig {

  namespace Rt {

    class TRuntimeError : public virtual Base::TError {
      protected:
      TRuntimeError() = default;
    };  // TRuntimeError

    /* TODO */
    class TSystemError : public TRuntimeError, public Base::TFinalError<TSystemError> {
      public:

      /* Constructor. */
      TSystemError(const Base::TCodeLocation &loc, const char *msg=0) {
        PostCtor(loc, msg);
      }
    };  // TSystemError

  }  // Rt

}  // Stig