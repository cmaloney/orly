/* <strm/utf8/in.test.cc>

   Unit test for <strm/utf8/in.h>.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <strm/utf8/in.h>

#include <strm/mem/static_in.h>
#include <test/kit.h>

using namespace std;
using namespace Strm;

FIXTURE(Typical) {
  static constexpr size_t expected_size = 4;
  static const char32_t expected[expected_size] = {
      0x00024, 0x000A2, 0x020AC, 0x24B62 };
  Mem::TStaticIn mem("\x24\xC2\xA2\xE2\x82\xAC\xF0\xA4\xAD\xA2");
  Utf8::TIn strm(&mem);
  for (size_t i = 0; i < expected_size; ++i, ++strm) {
    if (!EXPECT_TRUE(strm)) {
      break;
    }
    EXPECT_EQ(*strm, expected[i]);
  }
  EXPECT_FALSE(strm);
}
