/* <utf8/pos_map.test.cc>

   Unit test for <utf8/pos_map.h>.

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

#include <utf8/pos_map.h>

#include <iostream>

#include <test/kit.h>

using namespace std;
using namespace Utf8;

FIXTURE(Typical) {
  const char buf[] = "hello\ndoctor\nname\n\nmy\n2\xC2\xA2 worth\ndone";
  const size_t line_count = 7;
  const size_t lens[line_count] = { 5, 6, 4, 0, 2, 8, 4 };
  const TPiece text = buf;
  const TPosMap pos_map = text;
  EXPECT_EQ(pos_map.GetLineCount(), line_count);
  TPiece csr = text;
  for (size_t i = 0; i < line_count; ++i) {
    for (size_t j = 0; j <= lens[i]; ++j) {
      const char *start, *limit;
      if (csr) {
        TPiece c = csr.Split();
        start = c.GetStart();
        limit = c.GetLimit();
      } else {
        start = text.GetLimit();
        limit = start + 1;
      }
      for (const char *ptr = start; ptr < limit; ++ptr) {
        TPos pos = pos_map.GetPos(ptr);
        EXPECT_EQ(pos.GetLine(), i + 1);
        EXPECT_EQ(pos.GetCol(), j + 1);
      }
    }
  }
}
