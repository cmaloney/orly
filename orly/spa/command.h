/* <orly/spa/command.h>

   A literal which needs to be parsed into a orly variable.

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

#include <base/class_traits.h>
#include <base/code_location.h>
#include <orly/var.h>

namespace Orly {

  namespace Spa {

    DEFINE_ERROR(TGeneralCommandError, std::runtime_error, "error parsing");

    //Parses the given text using the orly command sublanguage
    bool ParseCommand(const char *text, Var::TVar &var);

  }  // Spa

}  // Orly
