/* <orly/spa/error.h>

   Error classes for use in the Spa desktop server code

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

#include <base/thrower.h>

namespace Orly {

  namespace Spa {

    DEFINE_ERROR(TGeneralError, std::runtime_error, "spa general error");
    DEFINE_ERROR(TNeededArgError, std::runtime_error, "needed argument");
    DEFINE_ERROR(TArgValueError, std::runtime_error, "invalid argument value");

  } // Spa

} // Orly
