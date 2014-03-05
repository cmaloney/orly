/* <stig/expr/add.test.cc>

   Unit test for <stig/expr/add.h>.

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

FIXTURE(IntAddition) {
  /* int  + int  -> int  */    EXPECT_EQ(TAdd::New(int0    , int1    , TPosRange())->GetType(), int_type    );
  /* int  + int? -> int? */    EXPECT_EQ(TAdd::New(int0    , opt_int0, TPosRange())->GetType(), opt_int_type);
  /* int? + int  -> int? */    EXPECT_EQ(TAdd::New(opt_int0, int0    , TPosRange())->GetType(), opt_int_type);
  /* int? + int? -> int? */    EXPECT_EQ(TAdd::New(opt_int0, opt_int1, TPosRange())->GetType(), opt_int_type);
}

FIXTURE(RealAddition) {
  /* real  + real  -> real  */ EXPECT_EQ(TAdd::New(real0    , real1    , TPosRange())->GetType(), real_type    );
  /* real  + real? -> real? */ EXPECT_EQ(TAdd::New(real0    , opt_real0, TPosRange())->GetType(), opt_real_type);
  /* real? + real  -> real? */ EXPECT_EQ(TAdd::New(opt_real0, real0    , TPosRange())->GetType(), opt_real_type);
  /* real? + real? -> real? */ EXPECT_EQ(TAdd::New(opt_real0, opt_real1, TPosRange())->GetType(), opt_real_type);
}

FIXTURE(StringConcatenation) {
  /* str  + str  -> str  */    EXPECT_EQ(TAdd::New(str0    , str1    , TPosRange())->GetType(), str_type    );
  /* str  + str? -> str? */    EXPECT_EQ(TAdd::New(str0    , opt_str0, TPosRange())->GetType(), opt_str_type);
  /* str? + str  -> str? */    EXPECT_EQ(TAdd::New(opt_str0, str0    , TPosRange())->GetType(), opt_str_type);
  /* str? + str? -> str? */    EXPECT_EQ(TAdd::New(opt_str0, opt_str1, TPosRange())->GetType(), opt_str_type);
}

FIXTURE(TimeDiffAddition) {
  /* tdf  + tdf  -> tdf  */    EXPECT_EQ(TAdd::New(tdf0    , tdf1    , TPosRange())->GetType(), tdf_type    );
  /* tdf  + tdf? -> tdf? */    EXPECT_EQ(TAdd::New(tdf0    , opt_tdf0, TPosRange())->GetType(), opt_tdf_type);
  /* tdf? + tdf  -> tdf? */    EXPECT_EQ(TAdd::New(opt_tdf0, tdf0    , TPosRange())->GetType(), opt_tdf_type);
  /* tdf? + tdf? -> tdf? */    EXPECT_EQ(TAdd::New(opt_tdf0, opt_tdf1, TPosRange())->GetType(), opt_tdf_type);
}

FIXTURE(TimeDiffTimePntAddition) {
  /* tdf  + tpt  -> tpt  */    EXPECT_EQ(TAdd::New(tdf0    , tpt1    , TPosRange())->GetType(), tpt_type    );
  /* tdf  + tpt? -> tpt? */    EXPECT_EQ(TAdd::New(tdf0    , opt_tpt0, TPosRange())->GetType(), opt_tpt_type);
  /* tdf? + tpt  -> tpt? */    EXPECT_EQ(TAdd::New(opt_tdf0, tpt0    , TPosRange())->GetType(), opt_tpt_type);
  /* tdf? + tpt? -> tpt? */    EXPECT_EQ(TAdd::New(opt_tdf0, opt_tpt1, TPosRange())->GetType(), opt_tpt_type);
}

FIXTURE(TimePntTimeDiffAddition) {
  /* tpt  + tdf  -> tpt  */    EXPECT_EQ(TAdd::New(tpt0    , tdf1    , TPosRange())->GetType(), tpt_type    );
  /* tpt  + tdf? -> tpt? */    EXPECT_EQ(TAdd::New(tpt0    , opt_tdf0, TPosRange())->GetType(), opt_tpt_type);
  /* tpt? + tdf  -> tpt? */    EXPECT_EQ(TAdd::New(opt_tpt0, tdf0    , TPosRange())->GetType(), opt_tpt_type);
  /* tpt? + tdf? -> tpt? */    EXPECT_EQ(TAdd::New(opt_tpt0, opt_tdf1, TPosRange())->GetType(), opt_tpt_type);
}

FIXTURE(ListConcatenation) {
  /* list  + list  -> list  */ EXPECT_EQ(TAdd::New(empty_list, list0, TPosRange())->GetType(), list_type);
}

FIXTURE(DictConcatenation) {
  /* dict  + dict  -> dict  */ EXPECT_EQ(TAdd::New(empty_dict, dict0, TPosRange())->GetType(), dict_type);
}
