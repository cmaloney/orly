/* <base/hash.test.cc>

   Unit test for <base/hash.h>

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

#include <base/hash.h>

#include <algorithm>
#include <vector>

#include <test/kit.h>

FIXTURE(Typical) {
  // sample tuples
  auto tuple0 = std::make_tuple(true, "hello", false, "world", 42, 101UL);
  auto tuple1 = std::make_tuple(true, false, "hello", "world", 101UL, 42);
  // hash
  size_t result0 = std::hash<decltype(tuple0)>()(tuple0);
  size_t result1 = std::hash<decltype(tuple1)>()(tuple1);
  // make sure tuples with same elements in different order hashes differently
  EXPECT_NE(result0, result1);
}

FIXTURE(SameHash) {
  // tuple and array
  auto tup = std::make_tuple(101, 102, 103, 104, 105);
  std::array<int, 5> arr = {{101, 102, 103, 104, 105}};
  // hash
  size_t result0 = std::hash<decltype(tup)>()(tup);
  size_t result1 = std::hash<decltype(arr)>()(arr);
  // make sure tuple and array of exact same elements hash the same
  EXPECT_EQ(result0, result1);
}

FIXTURE(Chain) {
  EXPECT_NE(Base::ChainHashes(101, 202), Base::ChainHashes(202, 101));
}
