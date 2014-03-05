/* <stig/rt/div.test.cc>

   Unit test for <stig/rt/div.h>

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

#include <stig/rt/opt.h>

#include <stig/rt/div.h>
#include <stig/rt/operator.h>

#include <test/kit.h>

using namespace std;
using namespace Stig::Rt;

TOpt<int64_t> unknown_int;

FIXTURE(Div) {
  /* TLhs / TRhs */
  for (int64_t i = 0; i < 100; ++i) {
    for (int64_t j = 1; j < 100; ++j) {
      EXPECT_EQ((Div(i, j)), i / j);
    }
  }

  /* TOpt<TLhs> / TRhs */
  EXPECT_TRUE(IsUnknown(Div(unknown_int, 1)));
  for (int64_t i = 0; i < 100; ++i) {
    for (int64_t j = 1; j < 100; ++j) {
      EXPECT_EQ(Div(TOpt<int64_t>(i), j).GetVal(), i / j);
    }
  }

  /* TLhs / TOpt<TRhs> */
  EXPECT_TRUE(IsUnknown(Div(1, unknown_int)));
  for (int64_t i = 0; i < 100; ++i) {
    for (int64_t j = 1; j < 100; ++j) {
      EXPECT_EQ(Div(i, TOpt<int64_t>(j)).GetVal(), i / j);
    }
  }

  /* TOpt<TLhs> / TOpt<TRhs> */
  EXPECT_TRUE(Div(unknown_int, unknown_int).IsUnknown());
  EXPECT_TRUE(Div(unknown_int, TOpt<int64_t>(1)).IsUnknown());
  EXPECT_TRUE(Div(TOpt<int64_t>(1), unknown_int).IsUnknown());
  for (int64_t i = 0; i < 100; ++i) {
    for (int64_t j = 1; j < 100; ++j) {
      EXPECT_EQ(Div(TOpt<int64_t>(i), TOpt<int64_t>(j)).GetVal(), i / j);
    }
  }
}
