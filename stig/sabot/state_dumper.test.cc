/* <stig/sabot/state_dumper.test.cc>

   Unit test for <stig/sabot/state_dumper.h>.

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

#include <stig/sabot/state_dumper.h>

#include <sstream>
#include <string>

#include <stig/native/point.h>
#include <stig/native/all.h>
#include <stig/sabot/state.h>
#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig;

template <typename TVal>
static string ToString(const TVal &val) {
  ostringstream strm;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Sabot::State::TAny::TWrapper(Native::State::New<TVal>(val, state_alloc))->Accept(Stig::Sabot::TStateDumper(strm));
  return strm.str();
}

FIXTURE(Empties) {
  EXPECT_EQ(ToString(Native::TFree<bool>::Free), "free(bool)");
  EXPECT_EQ(ToString(Native::TTombstone::Tombstone), "tombstone");
  // Note: There is no native void state.
}

FIXTURE(Int8) {
  EXPECT_EQ(ToString<int8_t>(-101), "-101");
  EXPECT_EQ(ToString<int8_t>(0), "0");
  EXPECT_EQ(ToString<int8_t>(101), "101");
}

FIXTURE(Int16) {
  EXPECT_EQ(ToString<int16_t>(-101), "-101");
  EXPECT_EQ(ToString<int16_t>(0), "0");
  EXPECT_EQ(ToString<int16_t>(101), "101");
}

FIXTURE(Int32) {
  EXPECT_EQ(ToString<int32_t>(-101), "-101");
  EXPECT_EQ(ToString<int32_t>(0), "0");
  EXPECT_EQ(ToString<int32_t>(101), "101");
}

FIXTURE(Int64) {
  EXPECT_EQ(ToString<int64_t>(-101), "-101");
  EXPECT_EQ(ToString<int64_t>(0), "0");
  EXPECT_EQ(ToString<int64_t>(101), "101");
}

FIXTURE(UInt8) {
  EXPECT_EQ(ToString<uint8_t>(0), "0");
  EXPECT_EQ(ToString<uint8_t>(101), "101");
}

FIXTURE(UInt16) {
  EXPECT_EQ(ToString<uint16_t>(0), "0");
  EXPECT_EQ(ToString<uint16_t>(101), "101");
}

FIXTURE(UInt32) {
  EXPECT_EQ(ToString<uint32_t>(0), "0");
  EXPECT_EQ(ToString<uint32_t>(101), "101");
}

FIXTURE(UInt64) {
  EXPECT_EQ(ToString<uint64_t>(0), "0");
  EXPECT_EQ(ToString<uint64_t>(101), "101");
}

FIXTURE(Bool) {
  EXPECT_EQ(ToString(true), "true");
  EXPECT_EQ(ToString(false), "false");
}

FIXTURE(Char) {
  EXPECT_EQ(ToString('x'), "'x'");
}

FIXTURE(TFloat) {
  EXPECT_EQ(ToString<float>(-98.6), "-98.6");
  EXPECT_EQ(ToString<float>(0), "0");
  EXPECT_EQ(ToString<float>(98.6), "98.6");
}

FIXTURE(TDouble) {
  EXPECT_EQ(ToString<double>(-98.6), "-98.6");
  EXPECT_EQ(ToString<double>(0), "0");
  EXPECT_EQ(ToString<double>(98.6), "98.6");
}

FIXTURE(TDuration) {
  EXPECT_EQ(ToString(Sabot::TStdDuration(0)), "0ms");
  EXPECT_EQ(ToString(Sabot::TStdDuration(1234)), "1234ms");
}

FIXTURE(TTimePoint) {
  EXPECT_EQ(ToString(Sabot::TStdTimePoint()), "1970:01:01:00:00:00utc");
}

FIXTURE(TUuid) {
  const char *str = "1b4e28ba-2fa1-11d2-883f-b9a761bde3fb";
  EXPECT_EQ(ToString(TUuid(str)), str);
}

FIXTURE(Blob) {
  uint8_t data[3] = { 65, 66, 67 };
  EXPECT_EQ(ToString(Native::TBlob(data, 3)), "{{ 65, 66, 67 }}");
}

FIXTURE(String) {
  EXPECT_EQ(ToString<string>("hello\ndoctor"), "\"hello\\ndoctor\"");
  EXPECT_EQ(ToString<const char *>("hello"), "\"hello\"");
  EXPECT_EQ(ToString("hello"), "\"hello\"");
}

FIXTURE(Desc) {
  EXPECT_EQ(ToString(TDesc<int>(101)), "desc(101)");
}

FIXTURE(Opt) {
  EXPECT_EQ(ToString(TOpt<int>(101)), "opt(101)");
  EXPECT_EQ(ToString(TOpt<bool>()), "unknown opt(bool)");
}

FIXTURE(Set) {
  EXPECT_EQ(ToString(set<int>({ 101, 102, 103 })), "set(101, 102, 103)");
  EXPECT_EQ(ToString(set<bool>()), "empty set(bool)");
}

FIXTURE(Vector) {
  EXPECT_EQ(ToString(vector<int>({ 101, 102, 103 })), "vector(101, 102, 103)");
  EXPECT_EQ(ToString(vector<bool>()), "empty vector(bool)");
}

FIXTURE(Map) {
  EXPECT_EQ(ToString(map<int, string>({ { 101, "hello"}, { 102, "doctor"} })), "map(101: \"hello\", 102: \"doctor\")");
  EXPECT_EQ(ToString(map<double, bool>()), "empty map(double, bool)");
}

FIXTURE(Record) {
  EXPECT_EQ(ToString(TPoint(1.5, 2.5)), "record(X: 1.5, Y: 2.5)");
}

FIXTURE(Tuple) {
  EXPECT_EQ((ToString(tuple<bool, int, double>(true, 101, 98.6))), "tuple(true, 101, 98.6)");
  EXPECT_EQ(ToString(tuple<>()), "tuple()");
}
