/* <stig/expr/if_else.test.cc>

   Unit test for <stig/expr/if_else.h>

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

#include <stig/expr/test_kit.h>

FIXTURE(IfElse) {
  /* (bool if bool else bool) -> bool */
  EXPECT_EQ(TIfElse::New(bool0,    bool1, bool2   , TPosRange())->GetType(), bool_type   );
  /* (int if bool else int) = int */
  EXPECT_EQ(TIfElse::New(int0,     bool0, int2    , TPosRange())->GetType(), int_type    );
  /* (real if bool else real) = real */
  EXPECT_EQ(TIfElse::New(real0,    bool0, real2   , TPosRange())->GetType(), real_type   );
  /* (int? if bool else int?) = int? */
  EXPECT_EQ(TIfElse::New(opt_int0, bool0, opt_int2, TPosRange())->GetType(), opt_int_type);
}
