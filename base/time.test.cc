/* <base/time.test.cc>

   Unit test for <base/time.h>.

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

#include <base/time.h>

#include <unistd.h>

#include <test/kit.h>

using namespace Base;

FIXTURE(Now) {
  TTime t1, t2;
  t1.Now();
  sleep(1);
  t2.Now();
  EXPECT_TRUE(t2 > t1);
  EXPECT_TRUE(t2 >= t1);
  EXPECT_TRUE(t1 < t2);
  EXPECT_TRUE(t1 <= t2);
  EXPECT_TRUE(t1 != t2);
  EXPECT_FALSE(t1 == t2);
  EXPECT_FALSE(t2 < t1);
  EXPECT_FALSE(t2 <= t1);
  EXPECT_FALSE(t1 > t2);
  EXPECT_FALSE(t1 >= t2);
}

FIXTURE(Plus) {
  TTime t1(1, 500000000);
  TTime t2(1, 500000000);
  TTime t3(3, 0);
  TTime t4 = t1 + t2;
  EXPECT_TRUE(t4 == t3);
}

FIXTURE(Minus) {
  TTime t1(1, 500000000);
  TTime t2(1, 500000000);
  TTime t3(3, 0);
  TTime t4 = t3 - t2;
  EXPECT_TRUE(t4 == t1);
  EXPECT_TRUE(t4 == t2);
}

FIXTURE(PlusEqMsec) {
  TTime t1(1, 500000000);
  TTime t2(3, 0);
  t1 += 1500;
  EXPECT_TRUE(t1 == t2);
}

FIXTURE(MinusEqMsec) {
  TTime t1(1, 500000000);
  TTime t2(3, 0);
  t2 -= 1500;
  EXPECT_TRUE(t1 == t2);
}

FIXTURE(Remaining) {
  TTime t1(1, 500000000);
  TTime t2;
  t2.Now();
  t2 += t1;
  size_t remaining = t2.Remaining();
  EXPECT_TRUE(remaining < 1500);
}