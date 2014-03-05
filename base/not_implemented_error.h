/* <base/not_implemented_error.h>

   Whatever you tried to do hasn't been written yet.

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

#define NOT_IMPLEMENTED() throw ::Base::TNotImplementedError(HERE)

namespace Base {

  /* TODO */
  class TNotImplementedError : public TFinalError<TNotImplementedError> {
    public:

    /* TODO */
    TNotImplementedError(const TCodeLocation &code_location, const char *description = 0) {
      PostCtor(code_location, description);
    }

    /* TODO */
    TNotImplementedError(const TCodeLocation &code_location, const char *desc_start, const char *desc_end) {
      PostCtor(code_location, desc_start, desc_end);
    }

  };

}
