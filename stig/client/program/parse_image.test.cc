/* <stig/client/program/parse_image.test.cc>

   Unit test for <stig/client/program/parse_image.h>.

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

#include <stig/client/program/parse_image.h>

#include <test/kit.h>

using namespace Stig::Client::Program;

FIXTURE(Typical) {
  int xact_count = 0;
  ParseImageStr(
      "image { xact {} xact {} xact {} }",
      [&xact_count](const TXact *) {
        ++xact_count;
        return true;
      }
  );
  EXPECT_EQ(xact_count, 3);
}
