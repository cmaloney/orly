/* <base/uuid.test.cc>

   Unit test for <base/uuid.h>.

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

#include <base/uuid.h>

#include <sstream>
#include <string>
#include <unordered_set>

#include <io/binary_input_only_stream.h>
#include <io/binary_output_only_stream.h>
#include <io/recorder_and_player.h>
#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Io;

static const char
    *Lower = "1b4e28ba-2fa1-11d2-883f-b9a761bde3fb",
    *Upper = "1B4E28BA-2FA1-11D2-883F-B9A761BDE3FB";

FIXTURE(FormatAndParse) {
  static const size_t case_count = 2;
  static const pair<string, TUuid::TCasing> case_array[2] = {
      { Lower, TUuid::Lower },
      { Upper, TUuid::Upper }
  };
  char actual_str[TUuid::MinBufSize];
  for (size_t i = 0; i < case_count; ++i) {
    TUuid(case_array[i].first.c_str()).Format(actual_str, case_array[i].second);
    EXPECT_EQ(string(actual_str), case_array[i].first);
  }
}

FIXTURE(NullAndNotNull) {
  TUuid null, not_null(Lower);
  EXPECT_FALSE(null);
  EXPECT_TRUE(not_null);
}

FIXTURE(Uniqueness) {
  static const size_t algo_count = 3;
  static const TUuid::TAlgo algos[algo_count] = { TUuid::Best, TUuid::Random, TUuid::TimeAndMAC };
  static const size_t expected_set_size = 100;
  for (size_t algo_idx = 0; algo_idx < algo_count; ++algo_idx) {
    unordered_set<TUuid> uuid_set;
    for (size_t i = 0; i < expected_set_size; ++i) {
      uuid_set.insert(algos[algo_idx]);
    }
    EXPECT_EQ(uuid_set.size(), expected_set_size);
  }
}

FIXTURE(StdStrm) {
  const TUuid expected(Lower);
  string blob;
  /* extra */ {
    ostringstream strm;
    strm << expected;
    blob = strm.str();
  }
  TUuid actual;
  EXPECT_FALSE(actual);
  /* extra */ {
    istringstream strm(blob);
    strm >> actual;
  }
  EXPECT_EQ(actual, expected);
}

FIXTURE(BinaryStrm) {
  const TUuid expected(Lower);
  auto recorder = make_shared<TRecorder>();
  /* extra */ {
    TBinaryOutputOnlyStream strm(recorder);
    strm << expected;
  }
  TUuid actual;
  EXPECT_FALSE(actual);
  /* extra */ {
    TBinaryInputOnlyStream strm(make_shared<TPlayer>(recorder));
    strm >> actual;
  }
  EXPECT_EQ(actual, expected);
}
