/* <base/stl_utils.test.cc>

   Unit test for <base/stl_utils.h>.

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

#include <base/stl_utils.h>

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

#include <test/kit.h>

using namespace std;
using namespace Base;

FIXTURE(Contains) {
  unordered_set<int> container;
  container.insert(101);
  EXPECT_TRUE(Contains(container, 101));
  EXPECT_FALSE(Contains(container, 202));
}

FIXTURE(FindOrDefault) {
  unordered_map<int, double> container;
  const int key = 101;
  const double expected = 98.6;
  container[key] = expected;
  EXPECT_EQ(FindOrDefault(container, key, expected + 1), expected);
  EXPECT_EQ(FindOrDefault(container, key + 1, expected + 1), expected + 1);
}

FIXTURE(FindOrInsert) {
  unordered_map<int, double> container;
  const int key = 101;
  const double expected = 98.6;
  EXPECT_EQ(FindOrInsert(container, key, expected), expected);
  EXPECT_EQ(FindOrInsert(container, key, expected + 1), expected);
}

FIXTURE(RotatedLeft) {
  EXPECT_EQ(RotatedLeft<unsigned short>(0x1234, 4), 0x2341);
}

FIXTURE(RotatedRight) {
  EXPECT_EQ(RotatedRight<unsigned short>(0x1234, 4), 0x4123);
}

FIXTURE(TryFind) {
  unordered_map<int, double> container;
  const int key = 101;
  const double expected = 98.6;
  container[key] = expected;
  const double *result = TryFind(container, key);
  if (EXPECT_TRUE(result)) {
    EXPECT_EQ(*result, expected);
  }
  EXPECT_FALSE(TryFind(container, key + 1));
}

FIXTURE(EqEq) {
  unordered_set<int> lhs, rhs;
  EXPECT_TRUE(eqeq(lhs,rhs));
  lhs.insert(101);
  EXPECT_FALSE(eqeq(lhs,rhs));
  EXPECT_FALSE(eqeq(rhs,lhs));
  rhs.insert(101);
  EXPECT_TRUE(eqeq(lhs,rhs));
  lhs.insert(202);
  EXPECT_FALSE(eqeq(lhs,rhs));
  rhs.insert(303);
  EXPECT_FALSE(eqeq(lhs,rhs));
  lhs.insert(303);
  EXPECT_FALSE(eqeq(lhs,rhs));
  rhs.insert(202);
  EXPECT_TRUE(eqeq(lhs,rhs));
}

FIXTURE(EqEqMap) {
  unordered_map<int,double> lhs, rhs;
  EXPECT_TRUE(eqeq_map(lhs,rhs));
  lhs[101]=98.7;
  EXPECT_FALSE(eqeq_map(lhs,rhs));
  EXPECT_FALSE(eqeq_map(rhs,lhs));
  rhs[101]=98.7;
  EXPECT_TRUE(eqeq_map(lhs,rhs));
  lhs[202]=1.23;
  EXPECT_FALSE(eqeq_map(lhs,rhs));
  rhs[303]=3.14;
  EXPECT_FALSE(eqeq_map(lhs,rhs));
  lhs[303]=3.14;
  EXPECT_FALSE(eqeq_map(lhs,rhs));
  rhs[202]=1.23;
  EXPECT_TRUE(eqeq_map(lhs,rhs));
}

FIXTURE(ForIter) {
  EXPECT_TRUE((is_same<int, ForIter<vector<int>::iterator>::TVal>::value));
  EXPECT_TRUE((is_same<int, ForIter<vector<int>::const_iterator>::TVal>::value));
  EXPECT_TRUE((is_same<string, ForIter<vector<string>::iterator>::TVal>::value));
  EXPECT_TRUE((is_same<string, ForIter<vector<string>::const_iterator>::TVal>::value));
  EXPECT_FALSE((is_same<int, ForIter<vector<string>::iterator>::TVal>::value));
  EXPECT_FALSE((is_same<string, ForIter<vector<int>::iterator>::TVal>::value));
  EXPECT_TRUE((is_same<int, ForIter<int *>::TVal>::value));
}
