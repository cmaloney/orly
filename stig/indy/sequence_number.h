/* <stig/indy/sequence_number.h>

   The type of a number used to identify uniquely an update within a repo.

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

namespace Stig {

  namespace Indy {

    /* The type of a number used to identify uniquely an update within a repo.
       A repo assigns these numbers in sequence, starting at zero, as updates are pushed to it.
       Sequence numbers cannot be maintained between repos.  A sequence number is meaningful only within a single repo.
       Throughout its life, a repo may never accept a number of updates greater than can be represented by this type; hence, it's a big type. */
    typedef uint64_t TSequenceNumber;

  }  // Indy

}  // Stig

