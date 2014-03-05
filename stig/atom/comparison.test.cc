/* <stig/atom/comparison.test.cc>

   Unit test for <stig/atom/comparison.h>.

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

#include <stig/atom/comparison.h>

#include <set>
#include <unordered_set>

#include <test/kit.h>

using namespace std;
using namespace Stig::Atom;

struct TTest {
  int X;
  bool Y;
  double Z;
  TComparison Compare(const TTest &that) const {
    return CompareOrderedMembers(*this, that, &TTest::X, &TTest::Y, &TTest::Z);
  }
};

FIXTURE(Int) {
  EXPECT_TRUE(CompareOrdered(0, 0) == TComparison::Eq);
  EXPECT_TRUE(CompareOrdered(0, 1) == TComparison::Lt);
  EXPECT_TRUE(CompareOrdered(1, 0) == TComparison::Gt);
}

FIXTURE(Set) {
  set<int> a = { 1, 2 }, b = { 1, 2, 3 };
  EXPECT_TRUE(CompareOrdered(a, a) == TComparison::Eq);
  EXPECT_TRUE(CompareOrdered(a, b) == TComparison::Lt);
  EXPECT_TRUE(CompareOrdered(b, a) == TComparison::Gt);
}

FIXTURE(UnorderedSet) {
  unordered_set<int> a = { 1, 2 }, b = { 1, 2, 3 };
  EXPECT_TRUE(CompareUnordered(a, a) == TComparison::Eq);
  EXPECT_TRUE(CompareUnordered(a, b) == TComparison::Ne);
}

FIXTURE(Members) {
  TTest a = { 0, false, 0.0 }, b = { 1, true, 0.0 }, c = { 1, false, 98.6 };
  EXPECT_TRUE(CompareOrderedMembers(a, a) == TComparison::Eq);
  EXPECT_TRUE(CompareOrderedMembers(a, a, &TTest::X) == TComparison::Eq);
  EXPECT_TRUE(CompareOrderedMembers(a, a, &TTest::X, &TTest::Y) == TComparison::Eq);
  EXPECT_TRUE(CompareOrderedMembers(b, c, &TTest::X, &TTest::Z, &TTest::Y) == TComparison::Lt);
  EXPECT_TRUE(a.Compare(a) == TComparison::Eq);
  EXPECT_TRUE(a.Compare(b) == TComparison::Lt);
  EXPECT_TRUE(b.Compare(a) == TComparison::Gt);
  EXPECT_TRUE(a.Compare(c) == TComparison::Lt);
  EXPECT_TRUE(c.Compare(a) == TComparison::Gt);
  EXPECT_TRUE(b.Compare(c) == TComparison::Gt);
  EXPECT_TRUE(c.Compare(b) == TComparison::Lt);
}

FIXTURE(IsEq) {
  EXPECT_TRUE(IsEq(TComparison::Eq));
  EXPECT_FALSE(IsEq(TComparison::Lt));
  EXPECT_FALSE(IsEq(TComparison::Gt));
  EXPECT_FALSE(IsEq(TComparison::Ne));
}

FIXTURE(IsNe) {
  EXPECT_FALSE(IsNe(TComparison::Eq));
  EXPECT_TRUE(IsNe(TComparison::Lt));
  EXPECT_TRUE(IsNe(TComparison::Gt));
  EXPECT_TRUE(IsNe(TComparison::Ne));
}

FIXTURE(IsLt) {
  EXPECT_FALSE(IsLt(TComparison::Eq));
  EXPECT_TRUE(IsLt(TComparison::Lt));
  EXPECT_FALSE(IsLt(TComparison::Gt));
  bool caught = false;
  try {
    EXPECT_FALSE(IsLt(TComparison::Ne));
  } catch (const TNotOrdered &) {
    caught = true;
  }
  EXPECT_TRUE(caught);
}

FIXTURE(IsLe) {
  EXPECT_TRUE(IsLe(TComparison::Eq));
  EXPECT_TRUE(IsLe(TComparison::Lt));
  EXPECT_FALSE(IsLe(TComparison::Gt));
  bool caught = false;
  try {
    EXPECT_FALSE(IsLe(TComparison::Ne));
  } catch (const TNotOrdered &) {
    caught = true;
  }
  EXPECT_TRUE(caught);
}

FIXTURE(IsGt) {
  EXPECT_FALSE(IsGt(TComparison::Eq));
  EXPECT_TRUE(IsGt(TComparison::Gt));
  EXPECT_FALSE(IsGt(TComparison::Lt));
  bool caught = false;
  try {
    EXPECT_FALSE(IsGt(TComparison::Ne));
  } catch (const TNotOrdered &) {
    caught = true;
  }
  EXPECT_TRUE(caught);
}

FIXTURE(IsGe) {
  EXPECT_TRUE(IsGe(TComparison::Eq));
  EXPECT_TRUE(IsGe(TComparison::Gt));
  EXPECT_FALSE(IsGe(TComparison::Lt));
  bool caught = false;
  try {
    EXPECT_FALSE(IsGe(TComparison::Ne));
  } catch (const TNotOrdered &) {
    caught = true;
  }
  EXPECT_TRUE(caught);
}
