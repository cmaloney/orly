/* <stig/atom/suprena.test.cc>

   Unit test for <stig/atom/suprena.h>.

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

#include <stig/atom/suprena.h>

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

#include <test/kit.h>

using namespace std;
using namespace Stig::Atom;

FIXTURE(Uniqueness) {
  const size_t str_count = 4;
  const char *str_array[str_count] = {
    "A is for Alice, who fell down the stairs.",
    "B is for Basil, assaulted by bears.",
    "C is for Clara, who wasted away.",
    "D is for Desmond, thrown out of a sleigh."
  };
  vector<size_t> idxs(str_count);
  for (size_t i = 0; i < str_count; ++i) {
    idxs[i] = i;
  }
  do {
    TSuprena arena;
    for (int repeat = 0; repeat < 3; ++repeat) {
      for (int idx: idxs) {
        arena.Propose(TCore::TNote::New(str_array[idx], false));
      }
    }
    if (EXPECT_EQ(arena.GetSize(), str_count)) {
      #if 0
      size_t idx = 0;
      arena.ForEachNoteInMergeOrder(
          [str_count, &str_array, &idx](const TNote *note) {
            bool success;
            if (idx < str_count) {
              const char
                  *expected_str = str_array[idx++],
                  *actual_str;
              note->As(actual_str);
              success = EXPECT_EQ(string(actual_str), string(expected_str));
            } else {
              success = !note->IsOrdered();
            }
            return success;
          }
      );
      #endif
    }
  } while (next_permutation(idxs.begin(), idxs.end()));
}
