/* <stig/sabot/assert_tuple.h>

   Asserts that the given type is a tuple.

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
#include <ostream>

#include <stig/sabot/type.h>

namespace Stig {

  namespace Sabot {

    /* TODO */
    const Type::TAny &AssertTuple(const Type::TAny &type);

  }  // Sabot

}  // Stig
