/* <stig/sabot/order_types.test.cc>

   Unit test for <stig/sabot/order_types.h>.

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

#include <stig/sabot/order_types.h>

#include <stig/sabot/order_states.h>

#include <sstream>
#include <string>

#include <stig/native/all.h>
#include <stig/native/point.h>
#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::Atom;
using namespace Stig::Native;
using namespace Stig::Sabot;

template <typename TLhs, typename TRhs>
TComparison CheckTypes() {
  void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
  void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
  return OrderTypes(*Sabot::Type::TAny::TWrapper(Native::Type::For<TLhs>::GetType(lhs_type_alloc)),
                    *Sabot::Type::TAny::TWrapper(Native::Type::For<TRhs>::GetType(rhs_type_alloc)));
}

FIXTURE(TupleKeySpace) {
  EXPECT_TRUE(IsEq(CheckTypes<tuple<int8_t>, tuple<int8_t, int8_t>>()));
  EXPECT_TRUE(IsEq(CheckTypes<tuple<int8_t>, tuple<int8_t, int8_t, int8_t>>()));
  EXPECT_TRUE(IsEq(CheckTypes<tuple<int8_t>, tuple<int8_t, int16_t, int16_t>>()));
  EXPECT_TRUE(IsEq(CheckTypes<tuple<int8_t>, tuple<int8_t, string, int16_t>>()));
  EXPECT_TRUE(IsEq(CheckTypes<tuple<int8_t>, tuple<int16_t>>()));
  EXPECT_TRUE(IsEq(CheckTypes<tuple<int8_t>, tuple<int16_t, int8_t>>()));

  EXPECT_TRUE(IsEq(CheckTypes<tuple<int16_t>, tuple<int8_t>>()));
  EXPECT_TRUE(IsEq(CheckTypes<tuple<int16_t>, tuple<int8_t, int8_t>>()));
}

FIXTURE(Int8) {
  EXPECT_TRUE(IsGt(CheckTypes<int8_t, TFree<int8_t>>()));
  EXPECT_TRUE(IsGt(CheckTypes<int8_t, TOpt<int8_t>>()));

  EXPECT_TRUE(IsLt(CheckTypes<int8_t, TFree<int16_t>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, TOpt<int16_t>>()));

  EXPECT_TRUE(IsEq(CheckTypes<int8_t, int8_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, int16_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, int32_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, int64_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, uint8_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, uint16_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, uint32_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, uint64_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, bool>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, char>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, float>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, double>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, TStdDuration>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, TStdTimePoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, TUuid>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<int8_t, tuple<bool>>()));
}

FIXTURE(Int16) {
  EXPECT_TRUE(IsGt(CheckTypes<int16_t, TFree<int16_t>>()));
  EXPECT_TRUE(IsGt(CheckTypes<int16_t, TOpt<int16_t>>()));

  EXPECT_TRUE(IsGt(CheckTypes<int16_t, TFree<int8_t>>()));
  EXPECT_TRUE(IsGt(CheckTypes<int16_t, TOpt<int8_t>>()));

  EXPECT_TRUE(IsLt(CheckTypes<int16_t, TFree<int32_t>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, TOpt<int32_t>>()));

  EXPECT_TRUE(IsGt(CheckTypes<int16_t, int8_t>()));
  EXPECT_TRUE(IsEq(CheckTypes<int16_t, int16_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, int32_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, int64_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, uint8_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, uint16_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, uint32_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, uint64_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, bool>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, char>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, float>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, double>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, TStdDuration>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, TStdTimePoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, TUuid>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<int16_t, tuple<bool>>()));
}

FIXTURE(Int32) {
  EXPECT_TRUE(IsGt(CheckTypes<int32_t, TFree<int32_t>>()));
  EXPECT_TRUE(IsGt(CheckTypes<int32_t, TOpt<int32_t>>()));

  EXPECT_TRUE(IsGt(CheckTypes<int32_t, TFree<int16_t>>()));
  EXPECT_TRUE(IsGt(CheckTypes<int32_t, TOpt<int16_t>>()));

  EXPECT_TRUE(IsLt(CheckTypes<int32_t, TFree<int64_t>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, TOpt<int64_t>>()));

  EXPECT_TRUE(IsGt(CheckTypes<int32_t, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<int32_t, int16_t>()));
  EXPECT_TRUE(IsEq(CheckTypes<int32_t, int32_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, int64_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, uint8_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, uint16_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, uint32_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, uint64_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, bool>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, char>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, float>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, double>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, TStdDuration>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, TStdTimePoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, TUuid>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<int32_t, tuple<bool>>()));
}

FIXTURE(Int64) {
  EXPECT_TRUE(IsGt(CheckTypes<int64_t, TFree<int64_t>>()));
  EXPECT_TRUE(IsGt(CheckTypes<int64_t, TOpt<int64_t>>()));

  EXPECT_TRUE(IsGt(CheckTypes<int64_t, TFree<int32_t>>()));
  EXPECT_TRUE(IsGt(CheckTypes<int64_t, TOpt<int32_t>>()));

  EXPECT_TRUE(IsLt(CheckTypes<int64_t, TFree<uint8_t>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, TOpt<uint8_t>>()));

  EXPECT_TRUE(IsGt(CheckTypes<int64_t, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<int64_t, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<int64_t, int32_t>()));
  EXPECT_TRUE(IsEq(CheckTypes<int64_t, int64_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, uint8_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, uint16_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, uint32_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, uint64_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, bool>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, char>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, float>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, double>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, TStdDuration>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, TStdTimePoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, TUuid>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<int64_t, tuple<bool>>()));
}

FIXTURE(UInt8) {
  EXPECT_TRUE(IsGt(CheckTypes<uint8_t, TFree<uint8_t>>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint8_t, TOpt<uint8_t>>()));

  EXPECT_TRUE(IsGt(CheckTypes<uint8_t, TFree<int64_t>>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint8_t, TOpt<int64_t>>()));

  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, TFree<uint16_t>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, TOpt<uint16_t>>()));

  EXPECT_TRUE(IsGt(CheckTypes<uint8_t, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint8_t, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint8_t, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint8_t, int64_t>()));
  EXPECT_TRUE(IsEq(CheckTypes<uint8_t, uint8_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, uint16_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, uint32_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, uint64_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, bool>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, char>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, float>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, double>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, TStdDuration>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, TStdTimePoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, TUuid>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint8_t, tuple<bool>>()));
}

FIXTURE(UInt16) {
  EXPECT_TRUE(IsGt(CheckTypes<uint16_t, TFree<uint16_t>>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint16_t, TOpt<uint16_t>>()));

  EXPECT_TRUE(IsGt(CheckTypes<uint16_t, TFree<uint8_t>>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint16_t, TOpt<uint8_t>>()));

  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, TFree<uint32_t>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, TOpt<uint32_t>>()));

  EXPECT_TRUE(IsGt(CheckTypes<uint16_t, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint16_t, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint16_t, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint16_t, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint16_t, uint8_t>()));
  EXPECT_TRUE(IsEq(CheckTypes<uint16_t, uint16_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, uint32_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, uint64_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, bool>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, char>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, float>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, double>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, TStdDuration>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, TStdTimePoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, TUuid>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint16_t, tuple<bool>>()));
}

FIXTURE(UInt32) {
  EXPECT_TRUE(IsGt(CheckTypes<uint32_t, TFree<uint32_t>>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint32_t, TOpt<uint32_t>>()));

  EXPECT_TRUE(IsGt(CheckTypes<uint32_t, TFree<uint16_t>>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint32_t, TOpt<uint16_t>>()));

  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, TFree<uint64_t>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, TOpt<uint64_t>>()));

  EXPECT_TRUE(IsGt(CheckTypes<uint32_t, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint32_t, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint32_t, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint32_t, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint32_t, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint32_t, uint16_t>()));
  EXPECT_TRUE(IsEq(CheckTypes<uint32_t, uint32_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, uint64_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, bool>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, char>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, float>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, double>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, TStdDuration>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, TStdTimePoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, TUuid>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint32_t, tuple<bool>>()));
}

FIXTURE(UInt64) {
  EXPECT_TRUE(IsGt(CheckTypes<uint64_t, TFree<uint64_t>>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint64_t, TOpt<uint64_t>>()));

  EXPECT_TRUE(IsGt(CheckTypes<uint64_t, TFree<uint32_t>>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint64_t, TOpt<uint32_t>>()));

  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, TFree<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, TOpt<bool>>()));

  EXPECT_TRUE(IsGt(CheckTypes<uint64_t, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint64_t, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint64_t, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint64_t, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint64_t, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint64_t, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<uint64_t, uint32_t>()));
  EXPECT_TRUE(IsEq(CheckTypes<uint64_t, uint64_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, bool>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, char>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, float>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, double>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, TStdDuration>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, TStdTimePoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, TUuid>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<uint64_t, tuple<bool>>()));
}

FIXTURE(Bool) {
  EXPECT_TRUE(IsGt(CheckTypes<bool, TFree<bool>>()));
  EXPECT_TRUE(IsGt(CheckTypes<bool, TOpt<bool>>()));

  EXPECT_TRUE(IsGt(CheckTypes<bool, TFree<uint64_t>>()));
  EXPECT_TRUE(IsGt(CheckTypes<bool, TOpt<uint64_t>>()));

  EXPECT_TRUE(IsLt(CheckTypes<bool, TFree<char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<bool, TOpt<char>>()));

  EXPECT_TRUE(IsGt(CheckTypes<bool, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<bool, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<bool, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<bool, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<bool, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<bool, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<bool, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<bool, uint64_t>()));
  EXPECT_TRUE(IsEq(CheckTypes<bool, bool>()));
  EXPECT_TRUE(IsLt(CheckTypes<bool, char>()));
  EXPECT_TRUE(IsLt(CheckTypes<bool, float>()));
  EXPECT_TRUE(IsLt(CheckTypes<bool, double>()));
  EXPECT_TRUE(IsLt(CheckTypes<bool, TStdDuration>()));
  EXPECT_TRUE(IsLt(CheckTypes<bool, TStdTimePoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<bool, TUuid>()));
  EXPECT_TRUE(IsLt(CheckTypes<bool, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<bool, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<bool, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<bool, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<bool, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<bool, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<bool, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<bool, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<bool, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<bool, tuple<bool>>()));
}

FIXTURE(Char) {
  EXPECT_TRUE(IsGt(CheckTypes<char, TFree<char>>()));
  EXPECT_TRUE(IsGt(CheckTypes<char, TOpt<char>>()));

  EXPECT_TRUE(IsGt(CheckTypes<char, TFree<bool>>()));
  EXPECT_TRUE(IsGt(CheckTypes<char, TOpt<bool>>()));

  EXPECT_TRUE(IsLt(CheckTypes<char, TFree<float>>()));
  EXPECT_TRUE(IsLt(CheckTypes<char, TOpt<float>>()));

  EXPECT_TRUE(IsGt(CheckTypes<char, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<char, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<char, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<char, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<char, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<char, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<char, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<char, uint64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<char, bool>()));
  EXPECT_TRUE(IsEq(CheckTypes<char, char>()));
  EXPECT_TRUE(IsLt(CheckTypes<char, float>()));
  EXPECT_TRUE(IsLt(CheckTypes<char, double>()));
  EXPECT_TRUE(IsLt(CheckTypes<char, TStdDuration>()));
  EXPECT_TRUE(IsLt(CheckTypes<char, TStdTimePoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<char, TUuid>()));
  EXPECT_TRUE(IsLt(CheckTypes<char, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<char, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<char, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<char, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<char, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<char, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<char, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<char, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<char, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<char, tuple<bool>>()));
}

FIXTURE(Float) {
  EXPECT_TRUE(IsGt(CheckTypes<float, TFree<float>>()));
  EXPECT_TRUE(IsGt(CheckTypes<float, TOpt<float>>()));

  EXPECT_TRUE(IsGt(CheckTypes<float, TFree<char>>()));
  EXPECT_TRUE(IsGt(CheckTypes<float, TOpt<char>>()));

  EXPECT_TRUE(IsLt(CheckTypes<float, TFree<double>>()));
  EXPECT_TRUE(IsLt(CheckTypes<float, TOpt<double>>()));

  EXPECT_TRUE(IsGt(CheckTypes<float, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<float, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<float, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<float, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<float, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<float, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<float, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<float, uint64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<float, bool>()));
  EXPECT_TRUE(IsGt(CheckTypes<float, char>()));
  EXPECT_TRUE(IsEq(CheckTypes<float, float>()));
  EXPECT_TRUE(IsLt(CheckTypes<float, double>()));
  EXPECT_TRUE(IsLt(CheckTypes<float, TStdDuration>()));
  EXPECT_TRUE(IsLt(CheckTypes<float, TStdTimePoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<float, TUuid>()));
  EXPECT_TRUE(IsLt(CheckTypes<float, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<float, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<float, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<float, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<float, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<float, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<float, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<float, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<float, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<float, tuple<bool>>()));
}

FIXTURE(Double) {
  EXPECT_TRUE(IsGt(CheckTypes<double, TFree<double>>()));
  EXPECT_TRUE(IsGt(CheckTypes<double, TOpt<double>>()));

  EXPECT_TRUE(IsGt(CheckTypes<double, TFree<float>>()));
  EXPECT_TRUE(IsGt(CheckTypes<double, TOpt<float>>()));

  EXPECT_TRUE(IsLt(CheckTypes<double, TFree<TStdDuration>>()));
  EXPECT_TRUE(IsLt(CheckTypes<double, TOpt<TStdDuration>>()));

  EXPECT_TRUE(IsGt(CheckTypes<double, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<double, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<double, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<double, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<double, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<double, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<double, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<double, uint64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<double, bool>()));
  EXPECT_TRUE(IsGt(CheckTypes<double, char>()));
  EXPECT_TRUE(IsGt(CheckTypes<double, float>()));
  EXPECT_TRUE(IsEq(CheckTypes<double, double>()));
  EXPECT_TRUE(IsLt(CheckTypes<double, TStdDuration>()));
  EXPECT_TRUE(IsLt(CheckTypes<double, TStdTimePoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<double, TUuid>()));
  EXPECT_TRUE(IsLt(CheckTypes<double, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<double, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<double, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<double, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<double, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<double, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<double, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<double, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<double, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<double, tuple<bool>>()));
}

FIXTURE(Duration) {
  EXPECT_TRUE(IsGt(CheckTypes<TStdDuration, TFree<TStdDuration>>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdDuration, TOpt<TStdDuration>>()));

  EXPECT_TRUE(IsGt(CheckTypes<TStdDuration, TFree<double>>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdDuration, TOpt<double>>()));

  EXPECT_TRUE(IsLt(CheckTypes<TStdDuration, TFree<TStdTimePoint>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdDuration, TOpt<TStdTimePoint>>()));

  EXPECT_TRUE(IsGt(CheckTypes<TStdDuration, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdDuration, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdDuration, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdDuration, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdDuration, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdDuration, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdDuration, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdDuration, uint64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdDuration, bool>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdDuration, char>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdDuration, float>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdDuration, double>()));
  EXPECT_TRUE(IsEq(CheckTypes<TStdDuration, TStdDuration>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdDuration, TStdTimePoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdDuration, TUuid>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdDuration, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdDuration, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdDuration, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdDuration, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdDuration, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdDuration, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdDuration, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdDuration, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdDuration, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdDuration, tuple<bool>>()));
}

FIXTURE(Timepoint) {
  EXPECT_TRUE(IsGt(CheckTypes<TStdTimePoint, TFree<TStdTimePoint>>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdTimePoint, TOpt<TStdTimePoint>>()));

  EXPECT_TRUE(IsGt(CheckTypes<TStdTimePoint, TFree<TStdDuration>>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdTimePoint, TOpt<TStdDuration>>()));

  EXPECT_TRUE(IsLt(CheckTypes<TStdTimePoint, TFree<TUuid>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdTimePoint, TOpt<TUuid>>()));

  EXPECT_TRUE(IsGt(CheckTypes<TStdTimePoint, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdTimePoint, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdTimePoint, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdTimePoint, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdTimePoint, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdTimePoint, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdTimePoint, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdTimePoint, uint64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdTimePoint, bool>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdTimePoint, char>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdTimePoint, float>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdTimePoint, double>()));
  EXPECT_TRUE(IsGt(CheckTypes<TStdTimePoint, TStdDuration>()));
  EXPECT_TRUE(IsEq(CheckTypes<TStdTimePoint, TStdTimePoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdTimePoint, TUuid>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdTimePoint, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdTimePoint, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdTimePoint, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdTimePoint, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdTimePoint, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdTimePoint, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdTimePoint, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdTimePoint, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdTimePoint, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<TStdTimePoint, tuple<bool>>()));
}

FIXTURE(Uuid) {
  EXPECT_TRUE(IsGt(CheckTypes<TUuid, TFree<TUuid>>()));
  EXPECT_TRUE(IsGt(CheckTypes<TUuid, TOpt<TUuid>>()));

  EXPECT_TRUE(IsGt(CheckTypes<TUuid, TFree<TStdTimePoint>>()));
  EXPECT_TRUE(IsGt(CheckTypes<TUuid, TOpt<TStdTimePoint>>()));

  EXPECT_TRUE(IsLt(CheckTypes<TUuid, TFree<TBlob>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TUuid, TOpt<TBlob>>()));

  EXPECT_TRUE(IsGt(CheckTypes<TUuid, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TUuid, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TUuid, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TUuid, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TUuid, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TUuid, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TUuid, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TUuid, uint64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TUuid, bool>()));
  EXPECT_TRUE(IsGt(CheckTypes<TUuid, char>()));
  EXPECT_TRUE(IsGt(CheckTypes<TUuid, float>()));
  EXPECT_TRUE(IsGt(CheckTypes<TUuid, double>()));
  EXPECT_TRUE(IsGt(CheckTypes<TUuid, TStdDuration>()));
  EXPECT_TRUE(IsGt(CheckTypes<TUuid, TStdTimePoint>()));
  EXPECT_TRUE(IsEq(CheckTypes<TUuid, TUuid>()));
  EXPECT_TRUE(IsLt(CheckTypes<TUuid, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<TUuid, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<TUuid, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<TUuid, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<TUuid, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TUuid, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TUuid, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TUuid, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TUuid, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<TUuid, tuple<bool>>()));
}

FIXTURE(Blob) {
  EXPECT_TRUE(IsGt(CheckTypes<TBlob, TFree<TBlob>>()));
  EXPECT_TRUE(IsGt(CheckTypes<TBlob, TOpt<TBlob>>()));

  EXPECT_TRUE(IsGt(CheckTypes<TBlob, TFree<TUuid>>()));
  EXPECT_TRUE(IsGt(CheckTypes<TBlob, TOpt<TUuid>>()));

  EXPECT_TRUE(IsLt(CheckTypes<TBlob, TFree<string>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TBlob, TOpt<string>>()));

  EXPECT_TRUE(IsGt(CheckTypes<TBlob, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TBlob, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TBlob, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TBlob, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TBlob, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TBlob, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TBlob, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TBlob, uint64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TBlob, bool>()));
  EXPECT_TRUE(IsGt(CheckTypes<TBlob, char>()));
  EXPECT_TRUE(IsGt(CheckTypes<TBlob, float>()));
  EXPECT_TRUE(IsGt(CheckTypes<TBlob, double>()));
  EXPECT_TRUE(IsGt(CheckTypes<TBlob, TStdDuration>()));
  EXPECT_TRUE(IsGt(CheckTypes<TBlob, TStdTimePoint>()));
  EXPECT_TRUE(IsGt(CheckTypes<TBlob, TUuid>()));
  EXPECT_TRUE(IsEq(CheckTypes<TBlob, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<TBlob, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<TBlob, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<TBlob, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<TBlob, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TBlob, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TBlob, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TBlob, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TBlob, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<TBlob, tuple<bool>>()));
}

FIXTURE(String) {
  EXPECT_TRUE(IsGt(CheckTypes<string, TFree<string>>()));
  EXPECT_TRUE(IsGt(CheckTypes<string, TOpt<string>>()));

  EXPECT_TRUE(IsGt(CheckTypes<string, TFree<TBlob>>()));
  EXPECT_TRUE(IsGt(CheckTypes<string, TOpt<TBlob>>()));

  EXPECT_TRUE(IsLt(CheckTypes<string, TFree<TTombstone>>()));
  EXPECT_TRUE(IsLt(CheckTypes<string, TOpt<TTombstone>>()));

  EXPECT_TRUE(IsGt(CheckTypes<string, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<string, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<string, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<string, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<string, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<string, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<string, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<string, uint64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<string, bool>()));
  EXPECT_TRUE(IsGt(CheckTypes<string, char>()));
  EXPECT_TRUE(IsGt(CheckTypes<string, float>()));
  EXPECT_TRUE(IsGt(CheckTypes<string, double>()));
  EXPECT_TRUE(IsGt(CheckTypes<string, TStdDuration>()));
  EXPECT_TRUE(IsGt(CheckTypes<string, TStdTimePoint>()));
  EXPECT_TRUE(IsGt(CheckTypes<string, TUuid>()));
  EXPECT_TRUE(IsGt(CheckTypes<string, TBlob>()));
  EXPECT_TRUE(IsEq(CheckTypes<string, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<string, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<string, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<string, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<string, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<string, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<string, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<string, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<string, tuple<bool>>()));
}

FIXTURE(Tombstone) {
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, TFree<TTombstone>>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, TOpt<TTombstone>>()));

  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, TFree<string>>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, TOpt<string>>()));

  EXPECT_TRUE(IsLt(CheckTypes<TTombstone, TFree<void>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TTombstone, TOpt<void>>()));

  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, uint64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, bool>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, char>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, float>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, double>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, TStdDuration>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, TStdTimePoint>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, TUuid>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, TBlob>()));
  EXPECT_TRUE(IsGt(CheckTypes<TTombstone, string>()));
  EXPECT_TRUE(IsEq(CheckTypes<TTombstone, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<TTombstone, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<TTombstone, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TTombstone, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TTombstone, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TTombstone, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TTombstone, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<TTombstone, tuple<bool>>()));
}

FIXTURE(Void) {
  EXPECT_TRUE(IsGt(CheckTypes<void, TFree<void>>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, TOpt<void>>()));

  EXPECT_TRUE(IsGt(CheckTypes<void, TFree<TTombstone>>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, TOpt<TTombstone>>()));

  EXPECT_TRUE(IsLt(CheckTypes<void, TFree<TDesc<bool>>>()));
  EXPECT_TRUE(IsLt(CheckTypes<void, TOpt<TDesc<bool>>>()));

  EXPECT_TRUE(IsGt(CheckTypes<void, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, uint64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, bool>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, char>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, float>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, double>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, TStdDuration>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, TStdTimePoint>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, TUuid>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, TBlob>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, string>()));
  EXPECT_TRUE(IsGt(CheckTypes<void, TTombstone>()));
  EXPECT_TRUE(IsEq(CheckTypes<void, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<void, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<void, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<void, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<void, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<void, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<void, tuple<bool>>()));
}

FIXTURE(Desc) {
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, TFree<TDesc<bool>>>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, TOpt<TDesc<bool>>>()));

  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, TFree<void>>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, TOpt<void>>()));

  EXPECT_TRUE(IsLt(CheckTypes<TDesc<bool>, TFree<set<bool>>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TDesc<bool>, TOpt<set<bool>>>()));

  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, uint64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, bool>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, char>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, float>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, double>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, TStdDuration>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, TStdTimePoint>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, TUuid>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, TBlob>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, string>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, TTombstone>()));
  EXPECT_TRUE(IsGt(CheckTypes<TDesc<bool>, void>()));
  EXPECT_TRUE(IsEq(CheckTypes<TDesc<bool>, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TDesc<bool>, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TDesc<bool>, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TDesc<bool>, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TDesc<bool>, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<TDesc<bool>, tuple<bool>>()));
}

FIXTURE(Set) {
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, TFree<set<bool>>>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, TOpt<set<bool>>>()));

  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, TFree<TDesc<bool>>>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, TOpt<TDesc<bool>>>()));

  EXPECT_TRUE(IsLt(CheckTypes<set<bool>, TFree<vector<bool>>>()));
  EXPECT_TRUE(IsLt(CheckTypes<set<bool>, TOpt<vector<bool>>>()));

  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, uint64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, bool>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, char>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, float>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, double>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, TStdDuration>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, TStdTimePoint>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, TUuid>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, TBlob>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, string>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, TTombstone>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, void>()));
  EXPECT_TRUE(IsGt(CheckTypes<set<bool>, TDesc<bool>>()));
  EXPECT_TRUE(IsEq(CheckTypes<set<bool>, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<set<bool>, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<set<bool>, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<set<bool>, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<set<bool>, tuple<bool>>()));
}

FIXTURE(Vector) {
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, TFree<vector<bool>>>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, TOpt<vector<bool>>>()));

  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, TFree<set<bool>>>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, TOpt<set<bool>>>()));

  EXPECT_TRUE(IsLt(CheckTypes<vector<bool>, TFree<map<bool, char>>>()));
  EXPECT_TRUE(IsLt(CheckTypes<vector<bool>, TOpt<map<bool, char>>>()));

  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, uint64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, bool>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, char>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, float>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, double>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, TStdDuration>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, TStdTimePoint>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, TUuid>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, TBlob>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, string>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, TTombstone>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, void>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, TDesc<bool>>()));
  EXPECT_TRUE(IsGt(CheckTypes<vector<bool>, set<bool>>()));
  EXPECT_TRUE(IsEq(CheckTypes<vector<bool>, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<vector<bool>, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<vector<bool>, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<vector<bool>, tuple<bool>>()));
}

FIXTURE(Map) {
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, TFree<map<bool, char>>>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, TOpt<map<bool, char>>>()));

  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, TFree<vector<bool>>>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, TOpt<vector<bool>>>()));

  EXPECT_TRUE(IsLt(CheckTypes<map<bool, char>, TFree<TPoint>>()));
  EXPECT_TRUE(IsLt(CheckTypes<map<bool, char>, TOpt<TPoint>>()));

  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, uint64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, bool>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, char>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, float>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, double>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, TStdDuration>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, TStdTimePoint>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, TUuid>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, TBlob>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, string>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, TTombstone>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, void>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, TDesc<bool>>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, set<bool>>()));
  EXPECT_TRUE(IsGt(CheckTypes<map<bool, char>, vector<bool>>()));
  EXPECT_TRUE(IsEq(CheckTypes<map<bool, char>, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<map<bool, char>, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<map<bool, char>, tuple<bool>>()));
}

FIXTURE(Record) {
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, TFree<TPoint>>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, TOpt<TPoint>>()));

  EXPECT_TRUE(IsGt(CheckTypes<TPoint, TFree<map<bool, char>>>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, TOpt<map<bool, char>>>()));

  EXPECT_TRUE(IsLt(CheckTypes<TPoint, TFree<tuple<bool>>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TPoint, TOpt<tuple<bool>>>()));

  EXPECT_TRUE(IsGt(CheckTypes<TPoint, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, uint64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, bool>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, char>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, float>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, double>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, TStdDuration>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, TStdTimePoint>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, TUuid>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, TBlob>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, string>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, TTombstone>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, void>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, TDesc<bool>>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, set<bool>>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, vector<bool>>()));
  EXPECT_TRUE(IsGt(CheckTypes<TPoint, map<bool, char>>()));
  EXPECT_TRUE(IsEq(CheckTypes<TPoint, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<TPoint, tuple<bool>>()));
}

FIXTURE(Tuple) {
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, TFree<tuple<bool>>>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, TOpt<tuple<bool>>>()));

  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, TFree<TPoint>>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, TOpt<TPoint>>()));

  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, uint64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, bool>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, char>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, float>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, double>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, TStdDuration>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, TStdTimePoint>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, TUuid>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, TBlob>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, string>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, TTombstone>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, void>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, TDesc<bool>>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, set<bool>>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, vector<bool>>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, map<bool, char>>()));
  EXPECT_TRUE(IsGt(CheckTypes<tuple<bool>, TPoint>()));
  EXPECT_TRUE(IsEq(CheckTypes<tuple<bool>, tuple<bool>>()));
}

FIXTURE(Free) {
  EXPECT_TRUE(IsEq(CheckTypes<TFree<bool>, TFree<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, TOpt<bool>>()));

  EXPECT_TRUE(IsGt(CheckTypes<TFree<bool>, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TFree<bool>, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TFree<bool>, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TFree<bool>, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TFree<bool>, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TFree<bool>, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TFree<bool>, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TFree<bool>, uint64_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, bool>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, char>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, float>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, double>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, TStdDuration>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, TStdTimePoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, TUuid>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<TFree<bool>, tuple<bool>>()));
}

FIXTURE(Opt) {
  EXPECT_TRUE(IsGt(CheckTypes<TOpt<bool>, TFree<bool>>()));
  EXPECT_TRUE(IsEq(CheckTypes<TOpt<bool>, TOpt<bool>>()));

  EXPECT_TRUE(IsGt(CheckTypes<TOpt<bool>, int8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TOpt<bool>, int16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TOpt<bool>, int32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TOpt<bool>, int64_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TOpt<bool>, uint8_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TOpt<bool>, uint16_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TOpt<bool>, uint32_t>()));
  EXPECT_TRUE(IsGt(CheckTypes<TOpt<bool>, uint64_t>()));
  EXPECT_TRUE(IsLt(CheckTypes<TOpt<bool>, bool>()));
  EXPECT_TRUE(IsLt(CheckTypes<TOpt<bool>, char>()));
  EXPECT_TRUE(IsLt(CheckTypes<TOpt<bool>, float>()));
  EXPECT_TRUE(IsLt(CheckTypes<TOpt<bool>, double>()));
  EXPECT_TRUE(IsLt(CheckTypes<TOpt<bool>, TStdDuration>()));
  EXPECT_TRUE(IsLt(CheckTypes<TOpt<bool>, TStdTimePoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<TOpt<bool>, TUuid>()));
  EXPECT_TRUE(IsLt(CheckTypes<TOpt<bool>, TBlob>()));
  EXPECT_TRUE(IsLt(CheckTypes<TOpt<bool>, string>()));
  EXPECT_TRUE(IsLt(CheckTypes<TOpt<bool>, TTombstone>()));
  EXPECT_TRUE(IsLt(CheckTypes<TOpt<bool>, void>()));
  EXPECT_TRUE(IsLt(CheckTypes<TOpt<bool>, TDesc<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TOpt<bool>, set<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TOpt<bool>, vector<bool>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TOpt<bool>, map<bool, char>>()));
  EXPECT_TRUE(IsLt(CheckTypes<TOpt<bool>, TPoint>()));
  EXPECT_TRUE(IsLt(CheckTypes<TOpt<bool>, tuple<bool>>()));
}