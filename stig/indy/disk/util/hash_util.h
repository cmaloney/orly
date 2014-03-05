/* <stig/indy/disk/util/hash_util.h>

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
#include <set>
#include <unistd.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      namespace Util {

        /* TODO */
        extern const std::set<size_t> GenSizeSet;

        /* TODO */
        constexpr size_t HighestPrecalculatedHashSize = 1111976UL;

        /* TODO */
        extern const std::set<size_t> HashSizeSet;

        /* TODO */
        constexpr double MaximumLoadFactor = 0.8;

        /* TODO */
        size_t SuggestHashSize(size_t num_keys);

        /* TODO */
        size_t SuggestGeneration(size_t num_keys);

      }  // Util

    }  // Disk

  }  // Indy

}  // Stig