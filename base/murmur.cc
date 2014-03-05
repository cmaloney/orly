/* <base/murmur.cc>

   Implements <base/murmur.h>.

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

#include <base/murmur.h>

#include <cassert>

using namespace std;

uint64_t Base::Murmur(const uint64_t *words, size_t word_count, uint64_t seed) {
  assert(words || !word_count);
  /* Iterate over the words, building up the hash. */
  uint64_t hash = seed;
  for (size_t i = 0; i < word_count; i++) {
    auto word = words[i];
    /* Mix the bits of the word. */
    word *= 0x87c37b91114253d5;
    word  = (word << 31) | (word >> 33);
    word *= 0x4cf5ad432745937f;
    /* Fold the mixed word into the hash. */
    hash ^= word;
    hash  = (hash << 27) | (hash >> 37);
    hash  = hash * 5 + 0x52dce729;
  }
  /* Mix the bits of the hash. */
  hash ^= hash >> 33;
  hash *= 0xff51afd7ed558ccd;
  hash ^= hash >> 33;
  hash *= 0xc4ceb9fe1a85ec53;
  hash ^= hash >> 33;
  return hash;
}
