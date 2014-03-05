/* <stig/protocol.test.cc>

   Unit test for <stig/protocol.h>.

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

#include <stig/protocol.h>

#include <cstdint>
#include <cstring>

#include <io/binary_input_only_stream.h>
#include <io/binary_output_only_stream.h>
#include <io/recorder_and_player.h>
#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Io;
using namespace Stig::Handshake;

FIXTURE(HealthCheck) {
  constexpr size_t size = sizeof(THeader);
  auto recorder = make_shared<TRecorder>();
  /* extra */ {
    THeader header('H', seconds(0));
    TBinaryOutputOnlyStream strm(recorder);
    strm.WriteExactly(&header, size);
  }
  uint8_t actual[size];
  /* extra */ {
    TBinaryInputOnlyStream strm(make_shared<TPlayer>(recorder));
    strm.ReadExactly(&actual, size);
  }
  const uint8_t expected[size] = { 0x47, 0x49, 0x54, 0x53, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x48 };
  EXPECT_FALSE(memcmp(actual, expected, size));
}
