/* <stig/spa/command.h>

   A literal which needs to be parsed into a stig variable.

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

#include <base/code_location.h>
#include <base/error.h>
#include <base/no_copy_semantics.h>
#include <stig/var.h>

namespace Stig {

  namespace Spa {

    /* Your errors should inherit from this */
    class TGeneralCommandError : public Base::TFinalError<TGeneralCommandError> {

      public:

      /* TODO */
      TGeneralCommandError(const Base::TCodeLocation &loc, const char *error) {
        PostCtor(loc, error);
      }
    };  // TGeneralCommandError

    //Parses the given text using the stig command sublanguage
    bool ParseCommand(const char *text, Var::TVar &var);

  }  // Spa

}  // Stig