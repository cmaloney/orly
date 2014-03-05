/* <stig/sabot/all.test.cc>

   Unit test for <stig/sabot/all.h>.

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

#include <stig/sabot/all.h>

#include <test/kit.h>

using namespace Stig::Sabot;

FIXTURE(Typical) {
  std::cout << "GetMaxTypePinSize() = " << TSizeChecker::GetMaxTypePinSize() << std::endl;
  std::cout << "GetMaxStatePinSize() = " << TSizeChecker::GetMaxStatePinSize() << std::endl;
  std::cout << "GetMaxTypeSize() = " << TSizeChecker::GetMaxTypeSize() << std::endl;
  std::cout << "GetMaxStateSize() = " << TSizeChecker::GetMaxStateSize() << std::endl;
  EXPECT_EQ((Maxer<1, 2, 3, 4, 5, 6, 7, 8>::Max), 8UL);
  EXPECT_EQ((Maxer<8, 7, 6, 5, 4, 3, 2, 1>::Max), 8UL);
}