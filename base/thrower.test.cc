/* <base/thrower.test.cc>

   Unit test for <base/thrower.h>.

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

#include <base/thrower.h>

#include <string>
#include <vector>

#include <test/kit.h>

using namespace std;
using namespace Base;

static void GetParts(const char *msg, vector<string> &parts) {
  assert(msg);
  assert(&parts);
  parts.clear();
  const char
      *start  = nullptr,
      *end    = nullptr,
      *cursor = msg;
  for (;;) {
    char c = *cursor;
    if (start) {
      if (!c || c == ';') {
        parts.push_back(string(start, end + 1));
        if (!c) {
          break;
        }
        start = nullptr;
        end   = nullptr;
      } else if (!isspace(c)) {
        end = cursor;
      }
    } else {
      if (!c) {
        break;
      }
      if (!isspace(c)) {
        start = cursor;
        end   = cursor;
      }
    }
    ++cursor;
  }
}

FIXTURE(GetParts) {
  const vector<string> expected = { "hello", "doctor", "name", "continue  yesterday" };
  vector<string> actual;
  GetParts("   hello; doctor;name   ;  continue  yesterday", actual);
  if (EXPECT_EQ(actual.size(), expected.size())) {
    for (size_t i = 0; i < expected.size(); ++i) {
      EXPECT_EQ(actual[i], expected[i]);
    }
  }
}

DEFINE_ERROR(TFoo, invalid_argument, "a fooness has occurred");

static const char
    *Extra1 = "some stuff",
    *Extra2 = "some more stuff";

template <typename TThrowAs, typename TCatchAs>
static void ThrowIt() {
  vector<string> parts;
  try {
    THROW_ERROR(TThrowAs) << Extra1 << EndOfPart << Extra2;
  } catch (const TCatchAs &ex) {
    GetParts(ex.what(), parts);
  }
  if (EXPECT_EQ(parts.size(), 4u)) {
    EXPECT_EQ(parts[1], TFoo::GetDesc());
    EXPECT_EQ(parts[2], Extra1);
    EXPECT_EQ(parts[3], Extra2);
  }
}

FIXTURE(Typical) {
  ThrowIt<TFoo, TFoo>();
  ThrowIt<TFoo, invalid_argument>();
  ThrowIt<TFoo, logic_error>();
  ThrowIt<TFoo, exception>();
}
