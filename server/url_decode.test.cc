/* <server/url_decode.test.cc>

   Unit test for <server/url_decode.h>.

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

#include <server/url_decode.h>

#include <test/kit.h>

using namespace Base;
using namespace std;
using namespace Server;

FIXTURE(Typical) {
  string result_buf;

  UrlDecode(AsPiece("Hello%2C%20World%26%25Ab%fF"), result_buf);

  EXPECT_EQ(result_buf, "Hello, World&%Ab\xff");


}

FIXTURE(Empty) {
  string result_buf;

  UrlDecode(AsPiece(""), result_buf);
  EXPECT_EQ(result_buf, "");

  UrlDecode(AsPiece("foo"), result_buf);
  EXPECT_EQ(result_buf, "foo");

  UrlDecode(Base::TPiece<const char>(), result_buf);
  EXPECT_EQ(result_buf, "");

  UrlDecode(AsPiece("%3C%7B+.x%3A448.0%2C+.y%3A147.0+%7D%3E"), result_buf);
  EXPECT_EQ(result_buf, "<{ .x:448.0, .y:147.0 }>");
  UrlDecode(AsPiece("%3C%7B.text%3A%27%5Cn%3Cp%3EWhat%27s+up%3C%2Fp%3E%27%7D%3E"), result_buf);
  EXPECT_EQ(result_buf, "<{.text:'\\n<p>What's up</p>'}>");
  UrlDecode(AsPiece("%7B%22likes%22%3A%5B%5D%2C%22userref%22%3A%22test2%22%2C%22content%22%3A%22aaa%22%2C%22cid%22%3A1%7D"), result_buf);
  EXPECT_EQ(result_buf, "{\"likes\":[],\"userref\":\"test2\",\"content\":\"aaa\",\"cid\":1}");
}

//TODO: Test for error cases
