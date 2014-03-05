/* <io/recorder_and_player.test.cc>

   Unit test for <io/recorder_and_player.h>.

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

#include <io/recorder_and_player.h>

#include <io/binary_input_only_stream.h>
#include <io/binary_output_only_stream.h>
#include <io/chunk_and_pool.h>
#include <test/kit.h>

using namespace std;
using namespace Io;

FIXTURE(RepeatedPlayback) {
  auto recorder = make_shared<TRecorder>();
  TBinaryOutputOnlyStream out_strm(recorder);
  static const int expected = 101;
  out_strm << expected;
  out_strm.Flush();
  for (size_t i = 0; i < 2; ++i) {
    TBinaryInputOnlyStream in_strm(make_shared<TPlayer>(recorder));
    EXPECT_FALSE(in_strm.IsAtEnd());
    int actual = 0;
    EXPECT_NE(actual, expected);
    in_strm >> actual;
    EXPECT_EQ(actual, expected);
    EXPECT_TRUE(in_strm.IsAtEnd());
  }
}

FIXTURE(IndirectData) {
  auto recorder = make_shared<TRecorder>();
  static const uint8_t expected = 101;
  recorder->ConsumeOutput(make_shared<TChunk>(TChunk::Full, &expected, sizeof(expected)));
  TBinaryInputOnlyStream in_strm(make_shared<TPlayer>(recorder));
  EXPECT_FALSE(in_strm.IsAtEnd());
  uint8_t actual = 0;
  EXPECT_NE(actual, expected);
  in_strm >> actual;
  EXPECT_EQ(actual, expected);
  EXPECT_TRUE(in_strm.IsAtEnd());
}

FIXTURE(StringRoundTrip) {
  const string expected = "Age cannot wither nor custom stale her infinite variety.";
  auto recorder = make_shared<TRecorder>(expected);
  string actual;
  recorder->CopyOut(actual);
  EXPECT_EQ(actual, expected);
}

FIXTURE(StringPieces) {
  const char *words[] = { "hello", "doctor", "name", nullptr };
  string serialized_form;
  /* extra */ {
    auto recorder = make_shared<TRecorder>();
    TBinaryOutputOnlyStream strm(recorder);
    for (const char **word = words; *word; ++word) {
      strm << string(*word);
      strm.Flush();
    }
    recorder->CopyOut(serialized_form);
  }
  TBinaryInputOnlyStream strm(make_shared<TPlayer>(make_shared<TRecorder>(serialized_form)));
  for (const char **word = words; *word; ++word) {
    string actual, expected(*word);
    strm >> actual;
    EXPECT_EQ(actual, expected);
  }
}

enum TColor {
  Red,
  Green,
  Blue
};

FIXTURE(Enums) {
  const size_t count = 5;
  const TColor expected[count] = { Green, Red, Blue, Red, Green };
  string serialized_form;
  /* extra */ {
    auto recorder = make_shared<TRecorder>();
    TBinaryOutputOnlyStream strm(recorder);
    for (size_t i = 0; i < count; ++i) {
      strm << expected[i];
    }
    strm.Flush();
    recorder->CopyOut(serialized_form);
  }
  EXPECT_EQ(serialized_form.size(), count * 4);
  TBinaryInputOnlyStream strm(make_shared<TPlayer>(make_shared<TRecorder>(serialized_form)));
  string dumped_str;
  strm.PeekAndDump(dumped_str);
  EXPECT_EQ(dumped_str, "size = 20, bytes = <0000000100000000000000020000000000000001>");
  for (size_t i = 0; i < count; ++i) {
    TColor actual;
    strm >> TBinaryInputEnum<TColor>(actual);
    EXPECT_EQ(actual, expected[i]);
  }
}
