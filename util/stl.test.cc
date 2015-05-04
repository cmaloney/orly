/* <util/stl.test.cc>

   Unit test for <util/stl.h>.

   Copyright 2015 Theoretical Chaos.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <util/stl.h>

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

#include <test/kit.h>

using namespace std;
using namespace Util;

FIXTURE(Contains) {
  unordered_set<int> container;
  container.insert(101);
  EXPECT_TRUE(Contains(container, 101));
  EXPECT_FALSE(Contains(container, 202));
}

FIXTURE(TryFind) {
  unordered_map<int, uint64_t> container;
  const int key = 101;
  const uint64_t expected = 98;
  container[key] = expected;
  const uint64_t *result = TryFind(container, key);
  if(EXPECT_TRUE(result)) {
    EXPECT_EQ(*result, expected);
  }
  EXPECT_FALSE(TryFind(container, key + 1));
}
