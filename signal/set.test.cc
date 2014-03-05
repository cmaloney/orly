/* <signal/set.test.cc>

   Unit test for <signal/set.h>.

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

#include <signal/set.h>

#include <test/kit.h>

using namespace Signal;

FIXTURE(Empty) {
  TSet a;
  EXPECT_FALSE(a[SIGINT]);
  a += SIGINT;
  EXPECT_TRUE(a[SIGINT]);
  a -= SIGINT;
  EXPECT_FALSE(a[SIGINT]);
}

FIXTURE(Full) {
  TSet a(TSet::Full);
  EXPECT_TRUE(a[SIGINT]);
  a -= SIGINT;
  EXPECT_FALSE(a[SIGINT]);
  a += SIGINT;
  EXPECT_TRUE(a[SIGINT]);
}

FIXTURE(Copy) {
  TSet a(TSet::Include, { SIGINT });
  EXPECT_TRUE(a[SIGINT]);
  TSet b(a);
  EXPECT_TRUE(a[SIGINT]);
  EXPECT_TRUE(b[SIGINT]);
}

FIXTURE(Assign) {
  TSet a(TSet::Include, { SIGINT });
  EXPECT_TRUE(a[SIGINT]);
  TSet b;
  EXPECT_FALSE(b[SIGINT]);
  b = a;
  EXPECT_TRUE(a[SIGINT]);
  EXPECT_TRUE(b[SIGINT]);
}

FIXTURE(Exclude) {
  TSet a(TSet::Exclude, { SIGINT });
  EXPECT_TRUE(a[SIGPIPE]);
  EXPECT_FALSE(a[SIGINT]);
}
