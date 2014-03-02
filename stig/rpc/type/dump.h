/* <stig/rpc/type/dump.h> 

   Dump a type object as a human-readable string.

   Copyright 2010-2014 Tagged
   
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

#include <ostream>
#include <string>

#include <stig/rpc/type.h>

namespace Stig {
  namespace Rpc {
    /* Dump the type onto the stream. */
    std::ostream &operator<<(std::ostream &strm, const Stig::Rpc::TType &type);
  } // Rpc
} // Stig

/* Dump the type as a string. */
std::string ToString(const Stig::Rpc::TType &type);

