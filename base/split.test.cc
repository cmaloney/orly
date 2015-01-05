/* <base/split.test.cc>

   Unit test for <base/split.h>

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <base/as_str.h>
#include <base/split.h>

#include <test/kit.h>

using namespace std;
using namespace Base;

FIXTURE(Typical) {
  vector<string> out;
  const vector<string> res1 = {"a", "b", "c", "d"};
  Split(",", "a,b,c,d", out);
  EXPECT_TRUE(out == res1);
  out.clear();

  const vector<string> res5 = {"a"};
  Split(":", "a", out);
  EXPECT_TRUE(out == res5);
  out.clear();

  const vector<string> res2 = {"1", "2", " 3", ""};
  Split(":", "1:2: 3:", out);
  EXPECT_TRUE(out == res2);
  out.clear();

  const vector<string> res3;
  Split(":", "", out);
  EXPECT_TRUE(out == res3);
  out.clear();

  const vector<string> res4 = {"", "", ""};
  Split(":", "::", out);
  EXPECT_TRUE(out == res4);
}

FIXTURE(Join) {
  std::vector<int> ints = {101, 202, 303};
  EXPECT_EQ(AsStr(Join(ints, ", ")), "101, 202, 303");
  EXPECT_EQ(AsStr(Join(ints, ", ", [](ostream &strm, int elem) { strm << '(' << elem << ')'; })),
            "(101), (202), (303)");
  EXPECT_EQ(AsStr(Join(std::make_tuple(101, 1.1, "hello"), ", ")), "101, 1.1, hello");
  EXPECT_EQ(AsStr(Join(std::make_tuple(101, 1.1, "hello"),
                       ", ",
                       [](ostream &strm, const auto &elem) { strm << '(' << elem << ')'; })),
            "(101), (1.1), (hello)");
  EXPECT_EQ(AsStr(Join(std::string("abc"), '/')), "a/b/c");
}
