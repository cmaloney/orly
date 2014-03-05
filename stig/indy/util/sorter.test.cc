/* <stig/indy/util/sorter.test.cc>

   Unit test for <stig/indy/util/sorter.h>.

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

#include <algorithm>

#include <stig/indy/util/sorter.h>

#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig::Indy::Util;

FIXTURE(Typical) {
  const size_t max_size = 100;
  TSorter<size_t, max_size> sorter;
  for(size_t i = 0; i < max_size; ++i) {
    sorter.Emplace(max_size - i);
  }
  EXPECT_EQ(sorter.GetSize(), max_size);
  size_t found = 0U;
  size_t expected = 100U;
  for (auto iter : sorter) {
    EXPECT_EQ(iter, expected--);
    ++found;
  }
  EXPECT_EQ(found, 100U);
  EXPECT_EQ(sorter.GetSize(), found);
  sort(sorter.begin(), sorter.end());
  found = 0U;
  expected = 1U;
  for (auto iter : sorter) {
    ++found;
    EXPECT_EQ(iter, expected++);
  }
  EXPECT_EQ(found, 100U);
  EXPECT_EQ(sorter.GetSize(), found);
  sorter.Clear();
  found = 0U;
  for (auto iter : sorter) {
    assert(iter);
    ++found;
  }
  EXPECT_EQ(found, 0U);
  EXPECT_EQ(sorter.GetSize(), found);
}