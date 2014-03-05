/* <base/interner.test.cc>

   Unit test for <indy/interner.h>.

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

#include <base/interner.h>

#include <test/kit.h>

using namespace Base;
using namespace std;

class TPnt {
  public:

  TPnt(int x, int y) : X(x), Y(y) {
    ++Count;
  }

  ~TPnt() {
    --Count;
  }

  int X, Y;

  static int Count;

};

int TPnt::Count;

FIXTURE(Typical) {
  TInterner<TPnt, int, int> i;
  TPnt::Count = 0;
  auto p1 = i.Get(101, 202);
  EXPECT_EQ(TPnt::Count, 1);
  if (EXPECT_TRUE(p1)) {
    EXPECT_EQ(p1->X, 101);
    EXPECT_EQ(p1->Y, 202);
  }
  /* extra */ {
    auto p2 = i.Get(303, 404);
    EXPECT_EQ(TPnt::Count, 2);
    if (EXPECT_TRUE(p2)) {
      EXPECT_EQ(p2->X, 303);
      EXPECT_EQ(p2->Y, 404);
    }
    EXPECT_TRUE(p1 != p2);
    auto p3 = i.Get(101, 202);
    EXPECT_EQ(TPnt::Count, 2);
    EXPECT_TRUE(p1 == p3);
  }
  EXPECT_EQ(TPnt::Count, 1);
  auto p4 = i.Get(303, 404);
  EXPECT_EQ(TPnt::Count, 2);
  if (EXPECT_TRUE(p4)) {
    EXPECT_EQ(p4->X, 303);
    EXPECT_EQ(p4->Y, 404);
  }
  EXPECT_TRUE(p1 != p4);
}
