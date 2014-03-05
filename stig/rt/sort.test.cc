/* <stig/rt/sort.test.cc>

   Unit test for <stig/rt/sort.h>

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

#include <stig/rt/sort.h>

#include <functional>

#include <stig/rt/operator.h>
#include <stig/rt/opt.h>

#include <test/kit.h>

using namespace std;
using namespace Stig::Rt;

/* Empty */
vector<int64_t> empty_li;

/* Non-empties */
vector<int64_t> li({2, 1, 3});

/* Unknowns */
TOpt<vector<int64_t>> unknown_li;

/* Opt empties */
TOpt<vector<int64_t>> opt_empty_li(empty_li);

/* Opt Non-empties */
TOpt<vector<int64_t>> opt_li(li);

/* Sorted */
vector<int64_t> sorted_li({1, 2, 3});

static const std::function<bool (const int64_t &, const int64_t &)> &lt = LtStruct<int64_t, int64_t>::Do;

FIXTURE(SortOnEmpty) {
  EXPECT_TRUE(Sort(empty_li, lt) == empty_li);
}

FIXTURE(SortOnNonEmpty) {
  EXPECT_TRUE(Sort(li, lt) == sorted_li);
}

FIXTURE(SortOnUnknowns) {
  EXPECT_TRUE(Sort(unknown_li, lt).IsUnknown());
}

FIXTURE(SortOnOptEmpty) {
  EXPECT_TRUE(Sort(opt_empty_li, lt).GetVal() == empty_li);
}

FIXTURE(SortOnOptNonEmpty) {
  EXPECT_TRUE(Sort(opt_li, lt).GetVal() == sorted_li);
}
