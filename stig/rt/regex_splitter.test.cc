/* <stig/rt/regex_splitter.test.cc>

   Unit test for <stig/rt/regex_splitter.h>.

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

#include <stig/rt/regex_splitter.h>

#include <initializer_list>

#include <test/kit.h>

#include <iostream>

using namespace std;
using namespace Utf8;
using namespace Stig::Rt;

static void TestPieces(const char *text, const char *delim, initializer_list<const char *> expected_strs) {
  auto
      csr = expected_strs.begin(),
      end = expected_strs.end();
  TRegex regex(delim);
  TRegexSplitter splitter(&regex, text);
  for (;;) {
    if (!EXPECT_TRUE(csr != end)) {
      break;
    }
    EXPECT_EQ(get<0>(*splitter), string(*csr++));
    if (get<1>(*splitter).IsKnown()) {
      if (!EXPECT_TRUE(csr != end)) {
        break;
      }
      EXPECT_EQ(get<1>(*splitter).GetVal(), string(*csr++));
      ++splitter;
    } else {
      break;
    }
  }
  EXPECT_TRUE(csr == end);
}

static void TestMatches(const char *text, const char *delim, initializer_list<const char *> expected_strs) {
  auto
      csr = expected_strs.begin(),
      end = expected_strs.end();
  TRegex regex(delim);
  TRegexMatcher matcher(&regex, text);
  while (csr != end) {
    EXPECT_EQ(string(*matcher), string(*csr++));
    ++matcher;
  }
}

FIXTURE(Whatever) {
  TestMatches("hello world helllo stig", "hel+o", { "hello", "helllo" });
}


FIXTURE(EndOnAWord) {
  TestPieces("lionsandtigersaaaaandbears", "a+nd", { "lions", "and", "tigers", "aaaaand", "bears" });
}


FIXTURE(EndOnADelimiter) {
  TestPieces("bobandcarolandtedand", "a+nd", { "bob", "and", "carol", "and", "ted", "and", "" });
}
