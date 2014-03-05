/* <base/pos.test.cc>

   Unit test for <base/pos.h>.

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

#include <base/pos.h>

#include <test/kit.h>

using namespace Base;

FIXTURE(Constants) {
  EXPECT_EQ(TPos::Start->GetAbsOffset(10),  0);
  EXPECT_EQ(TPos::Limit->GetAbsOffset(10), 10);
  EXPECT_EQ(TPos::ReverseStart->GetAbsOffset(10),  9);
  EXPECT_EQ(TPos::ReverseLimit->GetAbsOffset(10), -1);
  EXPECT_EQ(TPos::GetStart(TPos::Forward).GetAbsOffset(10),  0);
  EXPECT_EQ(TPos::GetLimit(TPos::Forward).GetAbsOffset(10), 10);
  EXPECT_EQ(TPos::GetStart(TPos::Reverse).GetAbsOffset(10),  9);
  EXPECT_EQ(TPos::GetLimit(TPos::Reverse).GetAbsOffset(10), -1);
}
