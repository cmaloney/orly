/* <stig/spa/error.h>

   Error classes for use in the Spa desktop server code

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

  namespace Spa {

    /* For when you just need an error in the spa server */
    class TGeneralError : public Base::TFinalError<TGeneralError> {
      public:

      /* Constructor. */
      TGeneralError(const Base::TCodeLocation &loc, const char *msg=0) {
        PostCtor(loc, msg);
      }
    };  // TGeneralError

    class TNeededArgError : public Base::TFinalError<TNeededArgError> {
      public:

      /* Construct with the name of the problematic argument. */
      TNeededArgError(const Base::TCodeLocation &loc, const char *msg) {
        PostCtor(loc, msg);
      }

    };  // TNeededArgError

    class TArgValueError : public Base::TFinalError<TArgValueError> {
      public:

      /* Construct with the name of the problematic argument. */
      TArgValueError(const Base::TCodeLocation &loc, const char *msg) {
        PostCtor(loc, msg);
      }

    };  // TArgValueError

  } // Spa

} // Stig