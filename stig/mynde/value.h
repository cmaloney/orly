/* <stig/mynde/value.h>

   The struct we use to store memcache values in the database

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

#include <cstdint>
#include <string>
#include <tuple>

#include <stig/native/defs.h>

/* Contains the value (a Native::TBlob), and the flags (a 32 bit uint) */

namespace Stig {
   namespace Mynde {

      using TKey = std::tuple<Native::TBlob>;

      struct TValue {
         Native::TBlob Value;
         uint32_t Flags;
      };

   } // Mynde
} // Stig
