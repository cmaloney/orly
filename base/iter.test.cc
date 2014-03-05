/* <base/iter.test.cc>

   Unit test for <base/iter.test.h>.

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

#include <base/iter.h>

#include <cassert>

#include <base/no_copy_semantics.h>
#include <test/kit.h>

using namespace Base;

/* Generic iter test suite. */
template <typename TVal>
void TestIter(TIter<TVal> &iter, TVal match[3]) {
  assert(&iter);
  EXPECT_TRUE(iter);
  EXPECT_EQ(*iter, match[0]);
  EXPECT_EQ(*(++iter), match[1]);
  EXPECT_TRUE(iter);
  EXPECT_TRUE(++iter);
  EXPECT_EQ(*iter, match[2]);
}