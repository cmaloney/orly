/* <base/string_utils.test.cc>

   Unit test for <base/string_utils.h>.

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

#include <base/string_utils.h>

#include <vector>

#include <test/kit.h>

using namespace std;
using namespace placeholders;

using namespace Base;

FIXTURE(ConcatCStrArray) {
  static const char *c_strs[] = {
    "hello", "doctor", "name", nullptr
  };
  EXPECT_EQ(ConcatCStrArray(c_strs), "hellodoctorname");
}

FIXTURE(ConcatCStrList) {
  EXPECT_EQ(ConcatCStrList({ "hello", "doctor", "name" }), "hellodoctorname");
}

FIXTURE(ConcatCStrs) {
  vector<const char *> c_strs = { "hello", "doctor", "name" };
  EXPECT_EQ(ConcatCStrs(c_strs.begin(), c_strs.end()), "hellodoctorname");
}

FIXTURE(GenCStrArray) {
  TCStrGen gen = [](const TCStrUser &c_str_user) {
    static const char *c_strs[] = {
      "hello", "doctor", "name", nullptr
    };
    return GenCStrArray(c_str_user, c_strs);
  };
  EXPECT_EQ(ToString(bind(WriteJoin, _1, gen, CommaSep)), "hello, doctor, name");
  EXPECT_EQ(ToString(bind(WriteBracketedJoin, _1, gen, '[', ']', CommaSep)), "[ hello, doctor, name ]");
}

FIXTURE(GenCStrList) {
  TCStrGen gen = [](const TCStrUser &c_str_user) {
    return GenCStrList(c_str_user, { "hello", "doctor", "name" });
  };
  EXPECT_EQ(ToString(bind(WriteJoin, _1, gen, CommaSep)), "hello, doctor, name");
  EXPECT_EQ(ToString(bind(WriteBracketedJoin, _1, gen, '[', ']', CommaSep)), "[ hello, doctor, name ]");
}

FIXTURE(GenCStrs) {
  TCStrGen gen = [](const TCStrUser &c_str_user) {
    vector<const char *> c_strs = { "hello", "doctor", "name" };
    return GenCStrs(c_str_user, c_strs.begin(), c_strs.end());
  };
  EXPECT_EQ(ToString(bind(WriteJoin, _1, gen, CommaSep)), "hello, doctor, name");
  EXPECT_EQ(ToString(bind(WriteBracketedJoin, _1, gen, '[', ']', CommaSep)), "[ hello, doctor, name ]");
}
