/* <strm/bin/zig_zag.test.cc>

   Unit test for <strm/bin/zig_zag.h>.

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

#include <strm/bin/zig_zag.h>

#include <cstdint>
#include <vector>

#include <test/kit.h>

using namespace std;
using namespace Strm::Bin;

FIXTURE(Types) {
  EXPECT_TRUE((is_same<decltype(
      ToZigZag  (static_cast<int8_t>(0))),   uint8_t >::value));
  EXPECT_TRUE((is_same<decltype(
      ToZigZag  (static_cast<int16_t>(0))),  uint16_t>::value));
  EXPECT_TRUE((is_same<decltype(
      ToZigZag  (static_cast<int32_t>(0))),  uint32_t>::value));
  EXPECT_TRUE((is_same<decltype(
      ToZigZag  (static_cast<int64_t>(0))),  uint64_t>::value));
  EXPECT_TRUE((is_same<decltype(
      FromZigZag(static_cast<uint8_t>(0))),  int8_t  >::value));
  EXPECT_TRUE((is_same<decltype(
      FromZigZag(static_cast<uint16_t>(0))), int16_t >::value));
  EXPECT_TRUE((is_same<decltype(
      FromZigZag(static_cast<uint32_t>(0))), int32_t >::value));
  EXPECT_TRUE((is_same<decltype(
      FromZigZag(static_cast<uint64_t>(0))), int64_t >::value));
}

FIXTURE(RoundTrip8) {
  static const vector<int8_t> vals = {
    numeric_limits<int8_t >::min(),
    numeric_limits<int8_t >::min() + 1,
    -101, -10, -5, -1, 0, 1, 5, 10, 101,
    numeric_limits<int8_t >::max() - 1,
    numeric_limits<int8_t >::max()
  };
  for (auto val: vals) {
    EXPECT_EQ(FromZigZag(ToZigZag(val)), val);
  }
}

FIXTURE(RoundTrip16) {
  static const vector<int16_t> vals = {
    numeric_limits<int16_t>::min(),
    numeric_limits<int16_t>::min() + 1,
    numeric_limits<int8_t >::min() - 1,
    numeric_limits<int8_t >::min(),
    numeric_limits<int8_t >::min() + 1,
    -101, -10, -5, -1, 0, 1, 5, 10, 101,
    numeric_limits<int8_t >::max() - 1,
    numeric_limits<int8_t >::max(),
    numeric_limits<int8_t >::max() + 1,
    numeric_limits<int16_t>::max() - 1,
    numeric_limits<int16_t>::max()
  };
  for (auto val: vals) {
    EXPECT_EQ(FromZigZag(ToZigZag(val)), val);
  }
}

FIXTURE(RoundTrip32) {
  static const vector<int32_t> vals = {
    numeric_limits<int32_t>::min(),
    numeric_limits<int32_t>::min() + 1,
    numeric_limits<int16_t>::min() - 1,
    numeric_limits<int16_t>::min(),
    numeric_limits<int16_t>::min() + 1,
    numeric_limits<int8_t >::min() - 1,
    numeric_limits<int8_t >::min(),
    numeric_limits<int8_t >::min() + 1,
    -101, -10, -5, -1, 0, 1, 5, 10, 101,
    numeric_limits<int8_t >::max() - 1,
    numeric_limits<int8_t >::max(),
    numeric_limits<int8_t >::max() + 1,
    numeric_limits<int16_t>::max() - 1,
    numeric_limits<int16_t>::max(),
    numeric_limits<int16_t>::min() + 1,
    numeric_limits<int32_t>::max() - 1,
    numeric_limits<int32_t>::max()
  };
  for (auto val: vals) {
    EXPECT_EQ(FromZigZag(ToZigZag(val)), val);
  }
}

FIXTURE(RoundTrip64) {
  static const vector<int64_t> vals = {
    numeric_limits<int64_t>::min(),
    numeric_limits<int64_t>::min() + 1,
    numeric_limits<int32_t>::min() - 1,
    numeric_limits<int32_t>::min(),
    numeric_limits<int32_t>::min() + 1,
    numeric_limits<int16_t>::min() - 1,
    numeric_limits<int16_t>::min(),
    numeric_limits<int16_t>::min() + 1,
    numeric_limits<int8_t >::min() - 1,
    numeric_limits<int8_t >::min(),
    numeric_limits<int8_t >::min() + 1,
    -101, -10, -5, -1, 0, 1, 5, 10, 101,
    numeric_limits<int8_t >::max() - 1,
    numeric_limits<int8_t >::max(),
    numeric_limits<int8_t >::max() + 1,
    numeric_limits<int16_t>::max() - 1,
    numeric_limits<int16_t>::max(),
    numeric_limits<int16_t>::min() + 1,
    numeric_limits<int32_t>::max() - 1,
    numeric_limits<int32_t>::max(),
    numeric_limits<int32_t>::max() + 1,
    numeric_limits<int64_t>::max() - 1,
    numeric_limits<int64_t>::max()
  };
  for (auto val: vals) {
    EXPECT_EQ(FromZigZag(ToZigZag(val)), val);
  }
}
