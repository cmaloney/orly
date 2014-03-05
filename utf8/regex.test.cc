/* <utf8/regex.test.cc>

   Unit test for <utf8/regex.h>.

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

#include <utf8/regex.h>

#include <test/kit.h>

#include <iostream>
#include <string>

using namespace Utf8;

FIXTURE(Pattern1) {
  TRegex regex("ba+r");

  /* Make sure cursor returns all matches. */
  const char matching_string[] = "foo bar baz baar gorf baaar";
  TRegex::TCursor cursor(&regex, matching_string);
  EXPECT_TRUE(cursor);
  EXPECT_TRUE(*cursor == "bar");
  EXPECT_TRUE(++cursor);
  EXPECT_TRUE(*cursor == "baar");
  EXPECT_TRUE(++cursor);
  EXPECT_TRUE(*cursor == "baaar");
  EXPECT_FALSE(++cursor);

  /* Make sure cursor behaves properly if no matches. */
  const char nonmatching_string[] = "nonmatching string";
  TRegex::TCursor cursor2(&regex, nonmatching_string);
  EXPECT_FALSE(cursor);

  /* Test IsMatch() method. */
  EXPECT_TRUE(regex.IsMatch(matching_string));
  EXPECT_FALSE(regex.IsMatch(nonmatching_string));

  /* Test TryGetMatch() method. */
  TPiece piece1;
  EXPECT_TRUE(regex.TryGetMatch(matching_string, piece1));
  EXPECT_TRUE(piece1 == "bar");
  const char piece2str[] = "some stuff";
  TPiece piece2(piece2str);
  EXPECT_FALSE(regex.TryGetMatch(nonmatching_string, piece2));
  /* piece2 should be unchanged afterr no match */
  EXPECT_TRUE(piece2 == piece2str);
}