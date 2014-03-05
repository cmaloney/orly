/* <stig/rt/is_empty.test.cc>

   Unit test for <stig/rt/is_empty.h>

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

#include <stig/rt/is_empty.h>

#include <stig/rt/operator.h>
#include <stig/rt/opt.h>

#include <test/kit.h>

using namespace std;
using namespace Stig::Rt;

/* Opt Vector */
TOpt<vector<int64_t>> unknown_li;
vector<int64_t> empty_li;
TOpt<vector<int64_t>> opt_empty_li(empty_li);
vector<int64_t> li({1, 2, 3});
TOpt<vector<int64_t>> opt_li(li);

/* Opt Set */
TOpt<TSet<string>> unknown_se;
TSet<string> empty_se;
TOpt<TSet<string>> opt_empty_se(empty_se);
TSet<string> se({"a", "b"});
TOpt<TSet<string>> opt_se(se);

/* Opt Dict */
TOpt<TDict<int64_t, double>> unknown_di;
TDict<int64_t, double> empty_di;
TOpt<TDict<int64_t, double>> opt_empty_di(empty_di);
TDict<int64_t, double> di({{1, 1.0}, {2, 2.0}});
TOpt<TDict<int64_t, double>> opt_di(di);

FIXTURE(IsEmpty) {
  /* IsEmpty on non-optionals */
  EXPECT_TRUE(IsEmpty(empty_li));
  EXPECT_TRUE(IsEmpty(empty_se));
  EXPECT_TRUE(IsEmpty(empty_di));

  EXPECT_FALSE(IsEmpty(li));
  EXPECT_FALSE(IsEmpty(se));
  EXPECT_FALSE(IsEmpty(di));

  /* IsEmpty on optionals */
  EXPECT_TRUE(IsEmpty(opt_empty_li).GetVal());
  EXPECT_TRUE(IsEmpty(opt_empty_se).GetVal());
  EXPECT_TRUE(IsEmpty(opt_empty_di).GetVal());

  EXPECT_FALSE(IsEmpty(opt_li).GetVal());
  EXPECT_FALSE(IsEmpty(opt_se).GetVal());
  EXPECT_FALSE(IsEmpty(opt_di).GetVal());
}