/* <stig/rt/opt.test.cc>

   Unit test for <stig/rt/opt.h>

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

#include <stig/rt/get_size.h>
#include <stig/rt/is_empty.h>
#include <stig/rt/operator.h>

#include <test/kit.h>

using namespace std;
using namespace Stig::Rt;

/* Opt Bool */
TOpt<bool> unknown_bool;
TOpt<bool> opt_true(true);
TOpt<bool> opt_false(false);

/* Opt Int */
TOpt<int64_t> unknown_int;
TOpt<int64_t> opt_1(1);
TOpt<int64_t> opt_2(2);

FIXTURE(IsKnown) {
  EXPECT_FALSE(unknown_int.IsKnown());
  EXPECT_TRUE(opt_1.IsKnown());
}

FIXTURE(IsUnknown) {
  EXPECT_TRUE(unknown_int.IsUnknown());
  EXPECT_FALSE(opt_1.IsUnknown());
}

FIXTURE(IsKnownBool) {
  EXPECT_TRUE(IsKnownTrue(opt_true));
  EXPECT_FALSE(IsKnownTrue(opt_false));
  EXPECT_FALSE(IsKnownFalse(opt_true));
  EXPECT_TRUE(IsKnownFalse(opt_false));
}

FIXTURE(Hash) {
  TSet<TOpt<int64_t>> set_of_opts({
      *TOpt<int64_t>::Unknown,
      TOpt<int64_t>(1),
      TOpt<int64_t>(1),
      *TOpt<int64_t>::Unknown});
  EXPECT_EQ(set_of_opts.size(), 2UL);
}

FIXTURE(Comparison) {
  /* unknown comp unknown -> unknown */
  EXPECT_TRUE(IsUnknown(EqEq(unknown_int, unknown_int)));
  EXPECT_TRUE(IsUnknown(Neq (unknown_int, unknown_int)));
  EXPECT_TRUE(IsUnknown(Lt  (unknown_int, unknown_int)));
  EXPECT_TRUE(IsUnknown(LtEq(unknown_int, unknown_int)));
  EXPECT_TRUE(IsUnknown(Gt  (unknown_int, unknown_int)));
  EXPECT_TRUE(IsUnknown(GtEq(unknown_int, unknown_int)));

  /* opt comp unknown -> unknown */
  EXPECT_TRUE(IsUnknown(EqEq(opt_1, unknown_int)));
  EXPECT_TRUE(IsUnknown(Neq (opt_1, unknown_int)));
  EXPECT_TRUE(IsUnknown(Lt  (opt_1, unknown_int)));
  EXPECT_TRUE(IsUnknown(LtEq(opt_1, unknown_int)));
  EXPECT_TRUE(IsUnknown(Gt  (opt_1, unknown_int)));
  EXPECT_TRUE(IsUnknown(GtEq(opt_1, unknown_int)));

  /* unknown comp opt -> unknown */
  EXPECT_TRUE(IsUnknown(EqEq(unknown_int, opt_2)));
  EXPECT_TRUE(IsUnknown(Neq (unknown_int, opt_2)));
  EXPECT_TRUE(IsUnknown(Lt  (unknown_int, opt_2)));
  EXPECT_TRUE(IsUnknown(LtEq(unknown_int, opt_2)));
  EXPECT_TRUE(IsUnknown(Gt  (unknown_int, opt_2)));
  EXPECT_TRUE(IsUnknown(GtEq(unknown_int, opt_2)));

  /* opt comp opt -> opt */
  EXPECT_TRUE(IsKnownFalse(EqEq(opt_1, opt_2)));
  EXPECT_TRUE(IsKnownTrue (Neq (opt_1, opt_2)));
  EXPECT_TRUE(IsKnownTrue (Lt  (opt_1, opt_2)));
  EXPECT_TRUE(IsKnownTrue (LtEq(opt_1, opt_2)));
  EXPECT_TRUE(IsKnownFalse(Gt  (opt_1, opt_2)));
  EXPECT_TRUE(IsKnownFalse(GtEq(opt_1, opt_2)));

  /* opt comp known -> opt */
  EXPECT_TRUE(IsKnownFalse(EqEq(opt_1, 2)));
  EXPECT_TRUE(IsKnownTrue (Neq (opt_1, 2)));
  EXPECT_TRUE(IsKnownTrue (Lt  (opt_1, 2)));
  EXPECT_TRUE(IsKnownTrue (LtEq(opt_1, 2)));
  EXPECT_TRUE(IsKnownFalse(Gt  (opt_1, 2)));
  EXPECT_TRUE(IsKnownFalse(GtEq(opt_1, 2)));

  /* known comp opt -> opt */
  EXPECT_TRUE(IsKnownFalse(EqEq(1, opt_2)));
  EXPECT_TRUE(IsKnownTrue (Neq (1, opt_2)));
  EXPECT_TRUE(IsKnownTrue (Lt  (1, opt_2)));
  EXPECT_TRUE(IsKnownTrue (LtEq(1, opt_2)));
  EXPECT_TRUE(IsKnownFalse(Gt  (1, opt_2)));
  EXPECT_TRUE(IsKnownFalse(GtEq(1, opt_2)));
}