/* <base/var_int.test.cc>

   Unit test for <base/var_int.h>.

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

#include <base/var_int.h>

#include <utility>

#include <test/kit.h>

using namespace std;
using namespace Base;

typedef pair<size_t, uint64_t> TItem;

FIXTURE(Typical) {
  static const TItem expected_items[] = {
    { 1,                  0},
    { 1,                  1},
    { 1,                101},
    { 1,                127},
    { 2,                128},
    { 2,                300},
    { 3,              19380},
    { 3,              40000},
    { 3,              65535},
    { 3,            0x10000},
    { 5,         0x10000000},
    { 5,         0xFFFFFFFF},
    { 9, 0x1000000000000000},
    {10, 0xFFFFFFFFFFFFFFFF},
    {0, 0}
  };
  for (const TItem *expected_item = expected_items; expected_item->first; ++expected_item) {
    char buffer[VarInt<uint64_t>::MaxSize];
    size_t actual_size = WriteVarInt(buffer, expected_item->second) - buffer;
    if (EXPECT_EQ(actual_size, expected_item->first)) {
      uint64_t actual_value;
      actual_size = ReadVarInt(buffer, actual_value) - buffer;
      EXPECT_EQ(actual_size, expected_item->first);
      EXPECT_EQ(actual_value, expected_item->second);
    }
  }
}
