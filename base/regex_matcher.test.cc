/* <base/regex_matcher.test.cc>

   Unit test for <base/regex_matcher.h>.

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

#include <base/regex_matcher.h>

#include <test/kit.h>

using namespace Base;

FIXTURE(Pattern1) {
  TRegexMatcher::TPattern p1("foo(bar)", REG_EXTENDED | REG_NEWLINE);
  bool caught = false;
  try {
    TRegexMatcher::TPattern p2("unmatched(paren", REG_EXTENDED | REG_NEWLINE);
  } catch (const TRegexMatcher::TRegexError &) {
    caught = true;
  }
  EXPECT_TRUE(caught);
}

FIXTURE(Match1) {
  const char pattern_str[] = "a[[:space:]]+(simple)[[:space:]]+"
                             "(pattern[[:space:]]+to[[:space:]]+match)";
  TRegexMatcher::TPattern p1(pattern_str, REG_EXTENDED | REG_NEWLINE);
  const char str1[] =
      "this contains a simple  pattern   to match and some other junk\n";
  TRegexMatcher::TMatches matches1(3);
  EXPECT_FALSE(matches1);
  EXPECT_TRUE(TRegexMatcher().Match(p1, str1, matches1, 0));

  /* Make sure all substrings were found. */
  EXPECT_TRUE(matches1);
  EXPECT_TRUE(RegexMatchToString(str1, *matches1) ==
              "a simple  pattern   to match");
  EXPECT_TRUE(++matches1);
  EXPECT_TRUE(RegexMatchToString(str1, *matches1) == "simple");
  EXPECT_TRUE(++matches1);
  EXPECT_TRUE(RegexMatchToString(str1, *matches1) ==
              "pattern   to match");
  EXPECT_FALSE(++matches1);

  /* Rewind matches1 ... */
  matches1.Rewind();

  /* ... and repeat the same sequence of tests. */
  EXPECT_TRUE(matches1);
  EXPECT_TRUE(RegexMatchToString(str1, *matches1) ==
              "a simple  pattern   to match");
  EXPECT_TRUE(++matches1);
  EXPECT_TRUE(RegexMatchToString(str1, *matches1) == "simple");
  EXPECT_TRUE(++matches1);
  EXPECT_TRUE(RegexMatchToString(str1, *matches1) ==
              "pattern   to match");
  EXPECT_FALSE(++matches1);

  /* Try using a TMatches object that's not big enough to hold all
     substrings. */
  TRegexMatcher::TMatches matches2(2);
  EXPECT_FALSE(matches2);
  EXPECT_TRUE(TRegexMatcher().Match(p1, str1, matches2, 0));
  EXPECT_TRUE(matches2);
  EXPECT_TRUE(RegexMatchToString(str1, *matches2) ==
              "a simple  pattern   to match");
  EXPECT_TRUE(++matches2);
  EXPECT_TRUE(RegexMatchToString(str1, *matches2) == "simple");
  EXPECT_FALSE(++matches2);

  /* Try using a TMatches object with more space than necessary. */
  TRegexMatcher::TMatches matches3(4);
  EXPECT_FALSE(matches3);
  EXPECT_TRUE(TRegexMatcher().Match(p1, str1, matches3, 0));
  EXPECT_TRUE(matches3);
  EXPECT_TRUE(RegexMatchToString(str1, *matches3) ==
              "a simple  pattern   to match");
  EXPECT_TRUE(++matches3);
  EXPECT_TRUE(RegexMatchToString(str1, *matches3) == "simple");
  EXPECT_TRUE(++matches3);
  EXPECT_TRUE(RegexMatchToString(str1, *matches3) ==
              "pattern   to match");
  EXPECT_FALSE(++matches3);

  /* Try using a TMatches object with no space for any substrings. */
  TRegexMatcher::TMatches matches4(0);
  EXPECT_FALSE(matches4);
  EXPECT_TRUE(TRegexMatcher().Match(p1, str1, matches4, 0));
  EXPECT_FALSE(matches4);

  /* Try skipping the first and last substrings. */
  TRegexMatcher::TMatches matches5(3);
  EXPECT_FALSE(matches5);
  EXPECT_TRUE(TRegexMatcher().Match(p1, str1, matches5, 0));
  ++matches5;
  EXPECT_TRUE(RegexMatchToString(str1, *matches5) == "simple");
  ++matches5;
  EXPECT_FALSE(++matches5);

  /* Try skipping the middle substring.  This time we redo the match and make
     sure the TMatches object is reset to the beginning. */
  EXPECT_TRUE(TRegexMatcher().Match(p1, str1, matches5, 0));
  EXPECT_TRUE(matches5);
  EXPECT_TRUE(RegexMatchToString(str1, *matches5) ==
              "a simple  pattern   to match");
  ++matches5;
  EXPECT_TRUE(++matches5);
  EXPECT_TRUE(RegexMatchToString(str1, *matches5) ==
              "pattern   to match");
  EXPECT_FALSE(++matches5);

  /* Try skipping all 3 substrings. */
  matches5.Rewind();
  ++matches5;
  ++matches5;
  EXPECT_FALSE(++matches5);

  /* Try a nonmatching string and make sure matches are empty afterwards. */
  EXPECT_FALSE(TRegexMatcher().Match(p1, "nonmatching string", matches5, 0));
  EXPECT_FALSE(matches5);

  /* Make sure matches are empty if pattern was compiled with REG_NOSUB. */
  TRegexMatcher::TPattern p2(pattern_str,
                             REG_EXTENDED | REG_NEWLINE | REG_NOSUB);
  EXPECT_TRUE(TRegexMatcher().Match(p2, str1, matches5, 0));
  EXPECT_FALSE(matches5);
}