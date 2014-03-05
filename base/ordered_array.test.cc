/* <base/ordered_array.test.cc>

   Unit test for <base/ordered_array.h>.

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

#include <base/ordered_array.h>

#include <sstream>
#include <string>

#include <test/kit.h>

using namespace std;
using namespace Base;

template <typename TKey, typename TVal>
string ToString(const Base::TOrderedArray<TKey, TVal> &that) {
  assert(&that);
  ostringstream strm;
  bool has_written = false;
  that.ForEach(
      [&strm, &has_written](const TKey &key, const TVal &val) {
        strm << (has_written ? ", " : "{ ") << key << ": " << val;
        has_written = true;
        return true;
      }
  );
  strm << (has_written ? " }" : "{}");
  return strm.str();
}

FIXTURE(BasicSizes) {
  TOrderedArray<int, int> a;
  EXPECT_EQ(a.GetSize(), 0u);
  EXPECT_EQ(a.GetMaxSize(), 0u);
  const size_t max_size = 16;
  a.SetMaxSize(max_size);
  EXPECT_EQ(a.GetSize(), 0u);
  EXPECT_EQ(a.GetMaxSize(), max_size);
  a.Reset();
  EXPECT_EQ(a.GetSize(), 0u);
  EXPECT_EQ(a.GetMaxSize(), 0u);
}

FIXTURE(Insert) {
  TOrderedArray<int, int> a(16);
  EXPECT_EQ(a.GetSize(), 0u);
  EXPECT_FALSE(a);
  EXPECT_EQ(ToString(a), "{}");
  a.Insert(50, 1001);
  EXPECT_EQ(a.GetSize(), 1u);
  EXPECT_TRUE(a);
  EXPECT_EQ(ToString(a), "{ 50: 1001 }");
  a.Insert(75, 1002);
  EXPECT_EQ(a.GetSize(), 2u);
  EXPECT_TRUE(a);
  EXPECT_EQ(ToString(a), "{ 50: 1001, 75: 1002 }");
  a.Insert(25, 1003);
  EXPECT_EQ(a.GetSize(), 3u);
  EXPECT_TRUE(a);
  EXPECT_EQ(ToString(a), "{ 25: 1003, 50: 1001, 75: 1002 }");
  a.Insert(37, 1004);
  EXPECT_EQ(a.GetSize(), 4u);
  EXPECT_TRUE(a);
  EXPECT_EQ(ToString(a), "{ 25: 1003, 37: 1004, 50: 1001, 75: 1002 }");
  a.Insert(62, 1005);
  EXPECT_EQ(a.GetSize(), 5u);
  EXPECT_TRUE(a);
  EXPECT_EQ(ToString(a), "{ 25: 1003, 37: 1004, 50: 1001, 62: 1005, 75: 1002 }");
  EXPECT_TRUE(a.Contains(25));
  EXPECT_TRUE(a.Contains(50));
  EXPECT_TRUE(a.Contains(75));
  EXPECT_FALSE(a.Contains(0));
  EXPECT_FALSE(a.Contains(-1));
  EXPECT_FALSE(a.Contains(100));
  EXPECT_EQ(a.Find(50), 1001);
  EXPECT_EQ(a.Find(75), 1002);
  EXPECT_EQ(a.Find(25), 1003);
  a.Find(50) = 1000;
  EXPECT_EQ(a.Find(50), 1000);
  a[100] = 1006;
  EXPECT_EQ(a[100], 1006);
  a.InsertOrReplace(50, 1001);
  a.InsertOrReplace(0, 1007);
  EXPECT_EQ(ToString(a), "{ 0: 1007, 25: 1003, 37: 1004, 50: 1001, 62: 1005, 75: 1002, 100: 1006 }");
  a.Remove(50);
  EXPECT_EQ(ToString(a), "{ 0: 1007, 25: 1003, 37: 1004, 62: 1005, 75: 1002, 100: 1006 }");
  a.Reset();
  EXPECT_EQ(a.GetSize(), 0u);
  EXPECT_FALSE(a);
  EXPECT_EQ(ToString(a), "{}");
}

FIXTURE(MoveAndCopy) {
  TOrderedArray<int, int> a(16);
  a.Insert(50, 1001);
  a.Insert(75, 1002);
  a.Insert(25, 1003);
  EXPECT_EQ(ToString(a), "{ 25: 1003, 50: 1001, 75: 1002 }");
  TOrderedArray<int, int> b(a);
  EXPECT_EQ(ToString(a), "{ 25: 1003, 50: 1001, 75: 1002 }");
  EXPECT_EQ(ToString(b), "{ 25: 1003, 50: 1001, 75: 1002 }");
  TOrderedArray<int, int> c(move(b));
  EXPECT_EQ(ToString(b), "{}");
  EXPECT_EQ(ToString(c), "{ 25: 1003, 50: 1001, 75: 1002 }");
  b = move(c);
  EXPECT_EQ(ToString(b), "{ 25: 1003, 50: 1001, 75: 1002 }");
  EXPECT_EQ(ToString(c), "{}");
  c = a;
  EXPECT_EQ(ToString(c), "{ 25: 1003, 50: 1001, 75: 1002 }");
}

FIXTURE(SetMaxSize) {
  TOrderedArray<int, int> a;
  a.SetMaxSize(16);
  a.Insert(50, 1001);
  a.Insert(75, 1002);
  a.Insert(25, 1003);
  EXPECT_EQ(ToString(a), "{ 25: 1003, 50: 1001, 75: 1002 }");
}
