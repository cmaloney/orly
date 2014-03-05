/* <io/stream.test.cc>

   Unit test for <io/stream.h>.

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

#include <io/stream.h>

#include <string>

#include <test/kit.h>

using namespace std;
using namespace Io;

class TMyFormat {
  public:

  TMyFormat()
      : X(DefaultX), Y(DefaultY) {}

  int X, Y;

  string Foo;

  static const int DefaultX, DefaultY;

};

const int
    TMyFormat::DefaultX = 101,
    TMyFormat::DefaultY = 202;

class TMyStream
    : public TStream<TMyFormat> {
  NO_COPY_SEMANTICS(TMyStream);
  public:

  TMyStream() {}

};

typedef TFormatter<TMyFormat> TMyFormatter;

FIXTURE(Typical) {
  TMyStream strm;
  EXPECT_EQ(strm.GetFormat().X, TMyFormat::DefaultX);
  EXPECT_EQ(strm.GetFormat().Y, TMyFormat::DefaultY);
  static const int expected = 303;
  strm << SetFormat(&TMyFormat::X, expected);
  EXPECT_EQ(strm.GetFormat().X, expected);
  /* extra */ {
    TFormatter<TMyFormat> formatter(&strm);
    formatter->Y = expected;
    EXPECT_EQ(strm.GetFormat().Y, expected);
  }
  EXPECT_EQ(strm.GetFormat().Y, TMyFormat::DefaultY);
}

FIXTURE(Strings) {
  TMyStream strm;
  EXPECT_TRUE(strm.GetFormat().Foo.empty());
  const string expected = "mofo";
  strm << SetFormat(&TMyFormat::Foo, expected);
  EXPECT_EQ(strm.GetFormat().Foo, expected);
}
