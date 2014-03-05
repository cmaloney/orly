/* <tools/nycr/escape.test.cc>

   Unit test for <tools/nycr/escape.h>

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

#include <tools/nycr/escape.h>

#include <test/kit.h>

using namespace std;
using namespace Tools::Nycr;

FIXTURE(Typical) {
  ostringstream oss;
  oss<<TEscape("foo");
  EXPECT_EQ(oss.str(), "\"foo\"");

  oss.str("");
  oss<<TEscape("He\rllo\nWor\"ld!\\n");
  EXPECT_EQ(oss.str(), "\"He\\rllo\\nWor\\\"ld!\\\\n\"");
}