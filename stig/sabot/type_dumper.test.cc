/* <stig/sabot/type_dumper.test.cc>

   Unit test for <stig/sabot/type_dumper.h>.

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

#include <stig/sabot/type_dumper.h>

#include <sstream>
#include <string>

#include <stig/native/point.h>
#include <stig/native/all.h>
#include <stig/sabot/all.h>
#include <stig/sabot/type_dumper.h>
#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig;

template <typename TVal>
static string ToString() {
  ostringstream strm;
  Native::Accept<TVal>(Sabot::TTypeDumper(strm));
  return strm.str();
}

FIXTURE(Typical) {
  EXPECT_EQ(ToString<int8_t>(), "int8");
  EXPECT_EQ(ToString<vector<int8_t>>(), "vector(int8)");
  EXPECT_EQ(ToString<int16_t>(), "int16");
  EXPECT_EQ(ToString<int32_t>(), "int32");
  EXPECT_EQ(ToString<int64_t>(), "int64");
  EXPECT_EQ(ToString<uint8_t>(), "uint8");
  EXPECT_EQ(ToString<uint16_t>(), "uint16");
  EXPECT_EQ(ToString<uint32_t>(), "uint32");
  EXPECT_EQ(ToString<uint64_t>(), "uint64");
  EXPECT_EQ(ToString<bool>(), "bool");
  EXPECT_EQ(ToString<char>(), "char");
  EXPECT_EQ(ToString<float>(), "float");
  EXPECT_EQ(ToString<double>(), "double");
  EXPECT_EQ(ToString<Sabot::TStdDuration>(), "duration");
  EXPECT_EQ(ToString<Sabot::TStdTimePoint>(), "time_point");
  EXPECT_EQ(ToString<TUuid>(), "uuid");
  EXPECT_EQ(ToString<Native::TBlob>(), "blob");
  EXPECT_EQ(ToString<string>(), "str");
  EXPECT_EQ(ToString<const char *>(), "str");
  EXPECT_EQ(ToString<char[1]>(), "str");
  EXPECT_EQ(ToString<Native::TTombstone>(), "tombstone");
  EXPECT_EQ(ToString<void>(), "void");
  EXPECT_EQ(ToString<TDesc<double>>(), "desc(double)");
  EXPECT_EQ(ToString<Native::TFree<bool>>(), "free(bool)");
  EXPECT_EQ(ToString<TOpt<bool>>(), "opt(bool)");
  EXPECT_EQ(ToString<set<bool>>(), "set(bool)");
  EXPECT_EQ(ToString<vector<bool>>(), "vector(bool)");
  EXPECT_EQ((ToString<map<bool, char>>()), "map(bool, char)");
  EXPECT_EQ((ToString<tuple<int8_t, std::vector<int8_t>, int8_t>>()), "tuple(int8, vector(int8), int8)");
  EXPECT_EQ(ToString<TPoint>(), "record(X: double, Y: double)");
  EXPECT_EQ(
      (ToString<tuple<tuple<>, set<int32_t>, map<TUuid, tuple<double, bool, TPoint>>>>()),
      "tuple(tuple(), set(int32), map(uuid, tuple(double, bool, record(X: double, Y: double))))"
  );
}
