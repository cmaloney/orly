/* <stig/rt/reverse.test.cc>

   Unit test for <stig/rt/reverse.h>

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

#include <stig/rt/reverse.h>

#include <stig/rt/operator.h>
#include <stig/rt/opt.h>

#include <test/kit.h>

using namespace std;
using namespace Stig::Rt;

/* Empties */
string empty_str;
vector<int64_t> empty_li;

/* Non-empties */
string str("hello");
vector<int64_t> li({1, 2, 3});

/* Unknowns */
TOpt<string> unknown_str;
TOpt<vector<int64_t>> unknown_li;

/* Opt empties */
TOpt<string> opt_empty_str(empty_str);
TOpt<vector<int64_t>> opt_empty_li(empty_li);

/* Opt Non-empties */
TOpt<string> opt_str(str);
TOpt<vector<int64_t>> opt_li(li);

/* Reversed */
string reverse_str("olleh");
vector<int64_t> reverse_li({3, 2, 1});

FIXTURE(ReverseOnEmpties) {
  EXPECT_TRUE(Reverse(empty_str) == empty_str);
  EXPECT_TRUE(Reverse(empty_li) == empty_li);
}

FIXTURE(ReverseOnNonEmpties) {
  EXPECT_TRUE(Reverse(str) == reverse_str);
  EXPECT_TRUE(Reverse(li) == reverse_li);
}

FIXTURE(ReverseOnUnknowns) {
  EXPECT_TRUE(Reverse(unknown_str).IsUnknown());
  EXPECT_TRUE(Reverse(unknown_li).IsUnknown());
}

FIXTURE(ReverseOnOptEmpties) {
  EXPECT_TRUE(Reverse(opt_empty_str).GetVal() == empty_str);
  EXPECT_TRUE(Reverse(opt_empty_li).GetVal() == empty_li);
}

FIXTURE(ReverseOnOptNonEmpties) {
  EXPECT_TRUE(Reverse(opt_str).GetVal() == reverse_str);
  EXPECT_TRUE(Reverse(opt_li).GetVal() == reverse_li);
}