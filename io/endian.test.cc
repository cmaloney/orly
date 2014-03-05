/* <io/endian.test.cc>

   Unit test for <io/endian.h>.

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

#include <io/endian.h>

#include <test/kit.h>

using namespace Io;

FIXTURE(UInt16) {
  const uint16_t val = 0x1234, expected = 0x3412;
  EXPECT_EQ(SwapEnds(val), expected);
}

FIXTURE(UInt32) {
  const uint32_t val = 0x12345678, expected = 0x78563412;
  EXPECT_EQ(SwapEnds(val), expected);
}

FIXTURE(UInt64) {
  const uint64_t val = 0x123456789ABCDEF0, expected = 0xF0DEBC9A78563412;
  EXPECT_EQ(SwapEnds(val), expected);
}
