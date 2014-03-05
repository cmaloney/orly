/* <base/murmur.h>

   Digest an array of 64-bit words, producing a 64-bit hash.

   This is based on the Murmur hash function, version 3.
   (http://en.wikipedia.org/wiki/MurmurHash)

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
#include <cstddef>

namespace Base {

  /* Digest an array of 64-bit words, producing a 64-bit hash. */
  uint64_t Murmur(const uint64_t *words, size_t word_count, uint64_t seed = 0);

}  // Base
