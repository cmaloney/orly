/* <stig/sabot/match_prefix_state.test.cc>

   Unit test for <stig/sabot/match_prefix_state.h>.

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

#include <stig/sabot/match_prefix_state.h>

#include <sstream>
#include <string>

#include <stig/native/all.h>
#include <stig/native/point.h>
#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::Native;

static const int8_t Int8_A = -1;
static const int8_t Int8_B = 1;
static const TFree<int8_t> FreeInt8 = TFree<int8_t>();

static const int16_t Int16_A = -3;
static const int16_t Int16_B = 3;
static const TFree<int16_t> FreeInt16 = TFree<int16_t>();

static const int32_t Int32_A = -5;
static const int32_t Int32_B = 5;
static const TFree<int32_t> FreeInt32 = TFree<int32_t>();

static const int64_t Int64_A = -7;
static const int64_t Int64_B = 7;
static const TFree<int64_t> FreeInt64 = TFree<int64_t>();

static const uint8_t UInt8_A = 0;
static const uint8_t UInt8_B = 2;
static const TFree<uint8_t> FreeUInt8 = TFree<uint8_t>();

static const uint16_t UInt16_A = 3;
static const uint16_t UInt16_B = 5;
static const TFree<uint16_t> FreeUInt16 = TFree<uint16_t>();

static const uint32_t UInt32_A = 6;
static const uint32_t UInt32_B = 8;
static const TFree<uint32_t> FreeUInt32 = TFree<uint32_t>();

static const uint64_t UInt64_A = 9;
static const uint64_t UInt64_B = 11;
static const TFree<uint64_t> FreeUInt64 = TFree<uint64_t>();

static const bool Bool_A = true;
static const bool Bool_B = false;
static const TFree<bool> FreeBool = TFree<bool>();

static const char Char_A = 'A';
static const char Char_B = 'Z';
static const TFree<char> FreeChar = TFree<char>();

static const float Float_A = 0.1;
static const float Float_B = 0.9;
static const TFree<float> FreeFloat = TFree<float>();

static const double Double_A = 1.1;
static const double Double_B = 9.9;
static const TFree<double> FreeDouble = TFree<double>();

static const Sabot::TStdDuration Duration_A(100);
static const Sabot::TStdDuration Duration_B(10000);
static const TFree<Sabot::TStdDuration> FreeDuration = TFree<Sabot::TStdDuration>();

static const Sabot::TStdTimePoint TimePoint_A(Duration_A);
static const Sabot::TStdTimePoint TimePoint_B(Duration_B);
static const TFree<Sabot::TStdTimePoint> FreeTimePoint = TFree<Sabot::TStdTimePoint>();

static const TUuid Uuid_A("AAAAAAAA-AAAA-AAAA-AAAA-AAAAAAAAAAAA");
static const TUuid Uuid_B("CCCCCCCC-CCCC-CCCC-CCCC-CCCCCCCCCCCC");
static const TFree<TUuid> FreeUuid = TFree<TUuid>();

static const Native::TBlob Blob_A {'A', 'A', 'A', 'A'};
static const Native::TBlob Blob_B {'F', 'F', 'F', 'F'};
static const TFree<Native::TBlob> FreeBlob = TFree<Native::TBlob>();

static const string String_A  = "AAAAAAAA";
static const string String_B  = "CCCCCCCC";
static const TFree<string> FreeString = TFree<string>();

static const TDesc<int32_t> Desc_A(Int32_A);
static const TDesc<int32_t> Desc_B(Int32_B);
static const TFree<TDesc<int32_t>> FreeDesc = TFree<TDesc<int32_t>>();

static const TOpt<int32_t> Opt_A(Int32_A);
static const TOpt<int32_t> Opt_B(Int32_B);
static const TFree<TOpt<int32_t>> FreeOpt = TFree<TOpt<int32_t>>();

static const set<int32_t> Set_A{3, 2, 1};
static const set<int32_t> Set_B{3, 4, 5};
static const TFree<set<int32_t>> FreeSet = TFree<set<int32_t>>();

static const vector<int32_t> Vector_A{1, 2, 3};
static const vector<int32_t> Vector_B{3, 4, 5};
static const TFree<vector<int32_t>> FreeVector = TFree<vector<int32_t>>();

static const map<int32_t, char> Map_A{{1, 'A'}, {2, 'B'}, {3, 'C'}};
static const map<int32_t, char> Map_B{{1, 'B'}, {3, 'A'}, {3, 'C'}};
static const TFree<map<int32_t, char>> FreeMap = TFree<map<int32_t, char>>();

static const TPoint Point_A(1.0, 2.0);
static const TPoint Point_B(3.0, 4.0);
static const TFree<TPoint> FreePoint = TFree<TPoint>();

static const tuple<int32_t> Tuple_A(make_tuple(Int32_A));
static const tuple<int32_t> Tuple_B(make_tuple(Int32_B));
static const TFree<tuple<int32_t>> FreeTuple = TFree<tuple<int32_t>>();

template <typename TLhs, typename TRhs>
Sabot::TMatchResult CheckStates(const TLhs &lhs, const TRhs &rhs) {
  void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
  void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
  Sabot::State::TAny::TWrapper
          lhs_state(Native::State::New(lhs, lhs_state_alloc)),
          rhs_state(Native::State::New(rhs, rhs_state_alloc));
  return Sabot::MatchPrefixState(*lhs_state, *rhs_state);
}

FIXTURE(Typical) {
  EXPECT_TRUE(IsUnifies(CheckStates<tuple<int32_t, TFree<int64_t>>, tuple<int32_t, int64_t>>(tuple<int32_t, TFree<int64_t>>(5, FreeInt64), tuple<int32_t, int64_t>(5, 10))));
  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<int32_t, TFree<int64_t>>, tuple<int32_t>>(tuple<int32_t, TFree<int64_t>>(5, FreeInt64), tuple<int32_t>(5))));
  EXPECT_TRUE(IsUnifies(CheckStates<tuple<int32_t, TFree<int64_t>>, tuple<int32_t, int64_t>>(tuple<int32_t, TFree<int64_t>>(7, FreeInt64), tuple<int32_t, int64_t>(7, 10))));
  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<int32_t, TFree<int64_t>>, tuple<int32_t>>(tuple<int32_t, TFree<int64_t>>(7, FreeInt64), tuple<int32_t>(7))));
}

FIXTURE(Int8) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<int8_t>>,          tuple<int8_t>>         (tuple<TFree<int8_t>>(FreeInt8), tuple<int8_t>(Int8_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<int8_t>>,          tuple<int8_t>>         (tuple<TFree<int8_t>>(FreeInt8), tuple<int8_t>(Int8_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<int8_t>,                 tuple<int8_t>>         (tuple<int8_t>(Int8_A), tuple<int8_t>(Int8_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<int8_t>,                 tuple<int8_t>>         (tuple<int8_t>(Int8_A), tuple<int8_t>(Int8_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<int8_t , TFree<int8_t>>, tuple<int8_t, int8_t>> (tuple<int8_t, TFree<int8_t>>(Int8_A, FreeInt8), tuple<int8_t, int8_t>(Int8_B, Int8_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<int8_t , TFree<int8_t>>, tuple<int8_t, int8_t>> (tuple<int8_t, TFree<int8_t>>(Int8_A, FreeInt8), tuple<int8_t, int8_t>(Int8_A, Int8_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<int8_t , TFree<int8_t>>, tuple<int8_t, int16_t>>(tuple<int8_t, TFree<int8_t>>(Int8_A, FreeInt8), tuple<int8_t, int16_t>(Int8_A, Int16_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<int8_t , TFree<int8_t>>, tuple<int8_t, int16_t>>(tuple<int8_t, TFree<int8_t>>(Int8_A, FreeInt8), tuple<int8_t, int16_t>(Int8_B, Int16_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<int16_t, TFree<int8_t>>, tuple<int8_t, int8_t>> (tuple<int16_t, TFree<int8_t>>(Int16_A, FreeInt8), tuple<int8_t, int8_t>(Int8_B, Int8_A))));
}

FIXTURE(Int16) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<int16_t>>,          tuple<int16_t>>         (tuple<TFree<int16_t>>(FreeInt16), tuple<int16_t>(Int16_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<int16_t>>,          tuple<int16_t>>         (tuple<TFree<int16_t>>(FreeInt16), tuple<int16_t>(Int16_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<int16_t>,                 tuple<int16_t>>         (tuple<int16_t>(Int16_A), tuple<int16_t>(Int16_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<int16_t>,                 tuple<int16_t>>         (tuple<int16_t>(Int16_A), tuple<int16_t>(Int16_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<int16_t, TFree<int16_t>>, tuple<int16_t, int16_t>>(tuple<int16_t, TFree<int16_t>>(Int16_A, FreeInt16), tuple<int16_t, int16_t>(Int16_B, Int16_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<int16_t, TFree<int16_t>>, tuple<int16_t, int16_t>>(tuple<int16_t, TFree<int16_t>>(Int16_A, FreeInt16), tuple<int16_t, int16_t>(Int16_A, Int16_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<int16_t, TFree<int16_t>>, tuple<int16_t, int32_t>>(tuple<int16_t, TFree<int16_t>>(Int16_A, FreeInt16), tuple<int16_t, int32_t>(Int16_A, Int32_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<int16_t, TFree<int16_t>>, tuple<int16_t, int32_t>>(tuple<int16_t, TFree<int16_t>>(Int16_A, FreeInt16), tuple<int16_t, int32_t>(Int16_B, Int32_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<int32_t, TFree<int16_t>>, tuple<int16_t, int16_t>>(tuple<int32_t, TFree<int16_t>>(Int32_A, FreeInt16), tuple<int16_t, int16_t>(Int16_B, Int16_A))));
}

FIXTURE(Int32) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<int32_t>>,          tuple<int32_t>>         (tuple<TFree<int32_t>>(FreeInt32), tuple<int32_t>(Int32_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<int32_t>>,          tuple<int32_t>>         (tuple<TFree<int32_t>>(FreeInt32), tuple<int32_t>(Int32_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<int32_t>,                 tuple<int32_t>>         (tuple<int32_t>(Int32_A), tuple<int32_t>(Int32_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<int32_t>,                 tuple<int32_t>>         (tuple<int32_t>(Int32_A), tuple<int32_t>(Int32_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<int32_t, TFree<int32_t>>, tuple<int32_t, int32_t>>(tuple<int32_t, TFree<int32_t>>(Int32_A, FreeInt32), tuple<int32_t, int32_t>(Int32_B, Int32_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<int32_t, TFree<int32_t>>, tuple<int32_t, int32_t>>(tuple<int32_t, TFree<int32_t>>(Int32_A, FreeInt32), tuple<int32_t, int32_t>(Int32_A, Int32_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<int32_t, TFree<int32_t>>, tuple<int32_t, int64_t>>(tuple<int32_t, TFree<int32_t>>(Int32_A, FreeInt32), tuple<int32_t, int64_t>(Int32_A, Int64_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<int32_t, TFree<int32_t>>, tuple<int32_t, int64_t>>(tuple<int32_t, TFree<int32_t>>(Int32_A, FreeInt32), tuple<int32_t, int64_t>(Int32_B, Int64_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<int64_t, TFree<int32_t>>, tuple<int32_t, int32_t>>(tuple<int64_t, TFree<int32_t>>(Int64_A, FreeInt32), tuple<int32_t, int32_t>(Int32_B, Int32_A))));
}

FIXTURE(Int64) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<int64_t>>,          tuple<int64_t>>         (tuple<TFree<int64_t>>(FreeInt64), tuple<int64_t>(Int64_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<int64_t>>,          tuple<int64_t>>         (tuple<TFree<int64_t>>(FreeInt64), tuple<int64_t>(Int64_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<int64_t>,                 tuple<int64_t>>         (tuple<int64_t>(Int64_A), tuple<int64_t>(Int64_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<int64_t>,                 tuple<int64_t>>         (tuple<int64_t>(Int64_A), tuple<int64_t>(Int64_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<int64_t, TFree<int64_t>>, tuple<int64_t, int64_t>>(tuple<int64_t, TFree<int64_t>>(Int64_A, FreeInt64), tuple<int64_t, int64_t>(Int64_B, Int64_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<int64_t, TFree<int64_t>>, tuple<int64_t, int64_t>>(tuple<int64_t, TFree<int64_t>>(Int64_A, FreeInt64), tuple<int64_t, int64_t>(Int64_A, Int64_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<int64_t, TFree<int64_t>>, tuple<int64_t, uint8_t>>(tuple<int64_t, TFree<int64_t>>(Int64_A, FreeInt64), tuple<int64_t, uint8_t>(Int64_A, UInt8_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<int64_t, TFree<int64_t>>, tuple<int64_t, uint8_t>>(tuple<int64_t, TFree<int64_t>>(Int64_A, FreeInt64), tuple<int64_t, uint8_t>(Int64_B, UInt8_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<uint8_t, TFree<int64_t>>, tuple<int64_t, int64_t>>(tuple<uint8_t, TFree<int64_t>>(UInt8_A, FreeInt64), tuple<int64_t, int64_t>(Int64_B, Int64_A))));
}

FIXTURE(UInt8) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<uint8_t>>,          tuple<uint8_t>>          (tuple<TFree<uint8_t>>(FreeUInt8), tuple<uint8_t>(UInt8_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<uint8_t>>,          tuple<uint8_t>>          (tuple<TFree<uint8_t>>(FreeUInt8), tuple<uint8_t>(UInt8_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<uint8_t>,                 tuple<uint8_t>>          (tuple<uint8_t>(UInt8_A), tuple<uint8_t>(UInt8_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<uint8_t>,                 tuple<uint8_t>>          (tuple<uint8_t>(UInt8_A), tuple<uint8_t>(UInt8_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<uint8_t, TFree<uint8_t>>, tuple<uint8_t, uint8_t>> (tuple<uint8_t, TFree<uint8_t>>(UInt8_A, FreeUInt8), tuple<uint8_t, uint8_t>(UInt8_B, UInt8_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<uint8_t, TFree<uint8_t>>, tuple<uint8_t, uint8_t>> (tuple<uint8_t, TFree<uint8_t>>(UInt8_A, FreeUInt8), tuple<uint8_t, uint8_t>(UInt8_A, UInt8_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<uint8_t, TFree<uint8_t>>, tuple<uint8_t, uint16_t>>(tuple<uint8_t, TFree<uint8_t>>(UInt8_A, FreeUInt8), tuple<uint8_t, uint16_t>(UInt8_A, UInt16_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<uint8_t, TFree<uint8_t>>, tuple<uint8_t, uint16_t>>(tuple<uint8_t, TFree<uint8_t>>(UInt8_A, FreeUInt8), tuple<uint8_t, uint16_t>(UInt8_B, UInt16_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<uint16_t,TFree<uint8_t>>, tuple<uint8_t, uint8_t>> (tuple<uint16_t, TFree<uint8_t>>(UInt16_A, FreeUInt8), tuple<uint8_t, uint8_t>(UInt8_B, UInt8_A))));
}

FIXTURE(UInt16) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<uint16_t>>,           tuple<uint16_t>>          (tuple<TFree<uint16_t>>(FreeUInt16), tuple<uint16_t>(UInt16_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<uint16_t>>,           tuple<uint16_t>>          (tuple<TFree<uint16_t>>(FreeUInt16), tuple<uint16_t>(UInt16_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<uint16_t>,                  tuple<uint16_t>>          (tuple<uint16_t>(UInt16_A), tuple<uint16_t>(UInt16_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<uint16_t>,                  tuple<uint16_t>>          (tuple<uint16_t>(UInt16_A), tuple<uint16_t>(UInt16_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<uint16_t, TFree<uint16_t>>, tuple<uint16_t, uint16_t>>(tuple<uint16_t, TFree<uint16_t>>(UInt16_A, FreeUInt16), tuple<uint16_t, uint16_t>(UInt16_B, UInt16_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<uint16_t, TFree<uint16_t>>, tuple<uint16_t, uint16_t>>(tuple<uint16_t, TFree<uint16_t>>(UInt16_A, FreeUInt16), tuple<uint16_t, uint16_t>(UInt16_A, UInt16_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<uint16_t, TFree<uint16_t>>, tuple<uint16_t, uint32_t>>(tuple<uint16_t, TFree<uint16_t>>(UInt16_A, FreeUInt16), tuple<uint16_t, uint32_t>(UInt16_A, UInt32_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<uint16_t, TFree<uint16_t>>, tuple<uint16_t, uint32_t>>(tuple<uint16_t, TFree<uint16_t>>(UInt16_A, FreeUInt16), tuple<uint16_t, uint32_t>(UInt16_B, UInt32_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<uint32_t, TFree<uint16_t>>, tuple<uint16_t, uint16_t>>(tuple<uint32_t, TFree<uint16_t>>(UInt32_A, FreeUInt16), tuple<uint16_t, uint16_t>(UInt16_B, UInt16_A))));
}

FIXTURE(UInt32) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<uint32_t>>,           tuple<uint32_t>>          (tuple<TFree<uint32_t>>(FreeUInt32), tuple<uint32_t>(UInt32_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<uint32_t>>,           tuple<uint32_t>>          (tuple<TFree<uint32_t>>(FreeUInt32), tuple<uint32_t>(UInt32_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<uint32_t>,                  tuple<uint32_t>>          (tuple<uint32_t>(UInt32_A), tuple<uint32_t>(UInt32_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<uint32_t>,                  tuple<uint32_t>>          (tuple<uint32_t>(UInt32_A), tuple<uint32_t>(UInt32_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<uint32_t, TFree<uint32_t>>, tuple<uint32_t, uint32_t>>(tuple<uint32_t, TFree<uint32_t>>(UInt32_A, FreeUInt32), tuple<uint32_t, uint32_t>(UInt32_B, UInt32_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<uint32_t, TFree<uint32_t>>, tuple<uint32_t, uint32_t>>(tuple<uint32_t, TFree<uint32_t>>(UInt32_A, FreeUInt32), tuple<uint32_t, uint32_t>(UInt32_A, UInt32_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<uint32_t, TFree<uint32_t>>, tuple<uint32_t, uint64_t>>(tuple<uint32_t, TFree<uint32_t>>(UInt32_A, FreeUInt32), tuple<uint32_t, uint64_t>(UInt32_A, UInt64_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<uint32_t, TFree<uint32_t>>, tuple<uint32_t, uint64_t>>(tuple<uint32_t, TFree<uint32_t>>(UInt32_A, FreeUInt32), tuple<uint32_t, uint64_t>(UInt32_B, UInt64_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<uint64_t, TFree<uint32_t>>, tuple<uint32_t, uint32_t>>(tuple<uint64_t, TFree<uint32_t>>(UInt64_A, FreeUInt32), tuple<uint32_t, uint32_t>(UInt32_B, UInt32_A))));
}

FIXTURE(UInt64) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<uint64_t>>,           tuple<uint64_t>>          (tuple<TFree<uint64_t>>(FreeUInt64), tuple<uint64_t>(UInt64_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<uint64_t>>,           tuple<uint64_t>>          (tuple<TFree<uint64_t>>(FreeUInt64), tuple<uint64_t>(UInt64_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<uint64_t>,                  tuple<uint64_t>>          (tuple<uint64_t>(UInt64_A), tuple<uint64_t>(UInt64_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<uint64_t>,                  tuple<uint64_t>>          (tuple<uint64_t>(UInt64_A), tuple<uint64_t>(UInt64_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<uint64_t, TFree<uint64_t>>, tuple<uint64_t, uint64_t>>(tuple<uint64_t, TFree<uint64_t>>(UInt64_A, FreeUInt64), tuple<uint64_t, uint64_t>(UInt64_B, UInt64_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<uint64_t, TFree<uint64_t>>, tuple<uint64_t, uint64_t>>(tuple<uint64_t, TFree<uint64_t>>(UInt64_A, FreeUInt64), tuple<uint64_t, uint64_t>(UInt64_A, UInt64_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<uint64_t, TFree<uint64_t>>, tuple<uint64_t, bool>>(tuple<uint64_t, TFree<uint64_t>>(UInt64_A, FreeUInt64), tuple<uint64_t, bool>(UInt64_A, Bool_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<uint64_t, TFree<uint64_t>>, tuple<uint64_t, bool>>(tuple<uint64_t, TFree<uint64_t>>(UInt64_A, FreeUInt64), tuple<uint64_t, bool>(UInt64_B, Bool_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<bool, TFree<uint64_t>>, tuple<uint64_t, uint64_t>>(tuple<bool, TFree<uint64_t>>(Bool_A, FreeUInt64), tuple<uint64_t, uint64_t>(UInt64_B, UInt64_A))));
}

FIXTURE(Bool) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<bool>>,           tuple<bool>>          (tuple<TFree<bool>>(FreeBool), tuple<bool>(Bool_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<bool>>,           tuple<bool>>          (tuple<TFree<bool>>(FreeBool), tuple<bool>(Bool_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<bool>,                  tuple<bool>>          (tuple<bool>(Bool_A), tuple<bool>(Bool_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<bool>,                  tuple<bool>>          (tuple<bool>(Bool_A), tuple<bool>(Bool_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<bool, TFree<bool>>, tuple<bool, bool>>(tuple<bool, TFree<bool>>(Bool_A, FreeBool), tuple<bool, bool>(Bool_B, Bool_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<bool, TFree<bool>>, tuple<bool, bool>>(tuple<bool, TFree<bool>>(Bool_A, FreeBool), tuple<bool, bool>(Bool_A, Bool_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<bool, TFree<bool>>, tuple<bool, char>>(tuple<bool, TFree<bool>>(Bool_A, FreeBool), tuple<bool, char>(Bool_A, Char_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<bool, TFree<bool>>, tuple<bool, char>>(tuple<bool, TFree<bool>>(Bool_A, FreeBool), tuple<bool, char>(Bool_B, Char_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<char, TFree<bool>>, tuple<bool, bool>>(tuple<char, TFree<bool>>(Char_A, FreeBool), tuple<bool, bool>(Bool_B, Bool_A))));
}

FIXTURE(Char) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<char>>,           tuple<char>>          (tuple<TFree<char>>(FreeChar), tuple<char>(Char_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<char>>,           tuple<char>>          (tuple<TFree<char>>(FreeChar), tuple<char>(Char_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<char>,                  tuple<char>>          (tuple<char>(Char_A), tuple<char>(Char_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<char>,                  tuple<char>>          (tuple<char>(Char_A), tuple<char>(Char_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<char, TFree<char>>, tuple<char, char>>(tuple<char, TFree<char>>(Char_A, FreeChar), tuple<char, char>(Char_B, Char_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<char, TFree<char>>, tuple<char, char>>(tuple<char, TFree<char>>(Char_A, FreeChar), tuple<char, char>(Char_A, Char_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<char, TFree<char>>, tuple<char, float>>(tuple<char, TFree<char>>(Char_A, FreeChar), tuple<char, float>(Char_A, Float_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<char, TFree<char>>, tuple<char, float>>(tuple<char, TFree<char>>(Char_A, FreeChar), tuple<char, float>(Char_B, Float_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<float, TFree<char>>, tuple<char, char>>(tuple<float, TFree<char>>(Float_A, FreeChar), tuple<char, char>(Char_B, Char_A))));
}

FIXTURE(Float) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<float>>,           tuple<float>>          (tuple<TFree<float>>(FreeFloat), tuple<float>(Float_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<float>>,           tuple<float>>          (tuple<TFree<float>>(FreeFloat), tuple<float>(Float_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<float>,                  tuple<float>>          (tuple<float>(Float_A), tuple<float>(Float_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<float>,                  tuple<float>>          (tuple<float>(Float_A), tuple<float>(Float_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<float, TFree<float>>, tuple<float, float>>(tuple<float, TFree<float>>(Float_A, FreeFloat), tuple<float, float>(Float_B, Float_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<float, TFree<float>>, tuple<float, float>>(tuple<float, TFree<float>>(Float_A, FreeFloat), tuple<float, float>(Float_A, Float_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<float, TFree<float>>, tuple<float, double>>(tuple<float, TFree<float>>(Float_A, FreeFloat), tuple<float, double>(Float_A, Double_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<float, TFree<float>>, tuple<float, double>>(tuple<float, TFree<float>>(Float_A, FreeFloat), tuple<float, double>(Float_B, Double_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<double, TFree<float>>, tuple<float, float>>(tuple<double, TFree<float>>(Double_A, FreeFloat), tuple<float, float>(Float_B, Float_A))));
}

FIXTURE(Double) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<double>>,           tuple<double>>          (tuple<TFree<double>>(FreeDouble), tuple<double>(Double_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<double>>,           tuple<double>>          (tuple<TFree<double>>(FreeDouble), tuple<double>(Double_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<double>,                  tuple<double>>          (tuple<double>(Double_A), tuple<double>(Double_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<double>,                  tuple<double>>          (tuple<double>(Double_A), tuple<double>(Double_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<double, TFree<double>>, tuple<double, double>>(tuple<double, TFree<double>>(Double_A, FreeDouble), tuple<double, double>(Double_B, Double_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<double, TFree<double>>, tuple<double, double>>(tuple<double, TFree<double>>(Double_A, FreeDouble), tuple<double, double>(Double_A, Double_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<double, TFree<double>>, tuple<double, Sabot::TStdDuration>>(tuple<double, TFree<double>>(Double_A, FreeDouble), tuple<double, Sabot::TStdDuration>(Double_A, Duration_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<double, TFree<double>>, tuple<double, Sabot::TStdDuration>>(tuple<double, TFree<double>>(Double_A, FreeDouble), tuple<double, Sabot::TStdDuration>(Double_B, Duration_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<Sabot::TStdDuration, TFree<double>>, tuple<double, double>>(tuple<Sabot::TStdDuration, TFree<double>>(Duration_A, FreeDouble), tuple<double, double>(Double_B, Double_A))));
}

FIXTURE(Duration) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<Sabot::TStdDuration>>,           tuple<Sabot::TStdDuration>>          (tuple<TFree<Sabot::TStdDuration>>(FreeDuration), tuple<Sabot::TStdDuration>(Duration_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<Sabot::TStdDuration>>,           tuple<Sabot::TStdDuration>>          (tuple<TFree<Sabot::TStdDuration>>(FreeDuration), tuple<Sabot::TStdDuration>(Duration_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<Sabot::TStdDuration>,                  tuple<Sabot::TStdDuration>>          (tuple<Sabot::TStdDuration>(Duration_A), tuple<Sabot::TStdDuration>(Duration_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<Sabot::TStdDuration>,                  tuple<Sabot::TStdDuration>>          (tuple<Sabot::TStdDuration>(Duration_A), tuple<Sabot::TStdDuration>(Duration_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<Sabot::TStdDuration, TFree<Sabot::TStdDuration>>, tuple<Sabot::TStdDuration, Sabot::TStdDuration>>(tuple<Sabot::TStdDuration, TFree<Sabot::TStdDuration>>(Duration_A, FreeDuration), tuple<Sabot::TStdDuration, Sabot::TStdDuration>(Duration_B, Duration_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<Sabot::TStdDuration, TFree<Sabot::TStdDuration>>, tuple<Sabot::TStdDuration, Sabot::TStdDuration>>(tuple<Sabot::TStdDuration, TFree<Sabot::TStdDuration>>(Duration_A, FreeDuration), tuple<Sabot::TStdDuration, Sabot::TStdDuration>(Duration_A, Duration_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<Sabot::TStdDuration, TFree<Sabot::TStdDuration>>, tuple<Sabot::TStdDuration, Sabot::TStdTimePoint>>(tuple<Sabot::TStdDuration, TFree<Sabot::TStdDuration>>(Duration_A, FreeDuration), tuple<Sabot::TStdDuration, Sabot::TStdTimePoint>(Duration_A, TimePoint_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<Sabot::TStdDuration, TFree<Sabot::TStdDuration>>, tuple<Sabot::TStdDuration, Sabot::TStdTimePoint>>(tuple<Sabot::TStdDuration, TFree<Sabot::TStdDuration>>(Duration_A, FreeDuration), tuple<Sabot::TStdDuration, Sabot::TStdTimePoint>(Duration_B, TimePoint_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<Sabot::TStdTimePoint, TFree<Sabot::TStdDuration>>, tuple<Sabot::TStdDuration, Sabot::TStdDuration>>(tuple<Sabot::TStdTimePoint, TFree<Sabot::TStdDuration>>(TimePoint_A, FreeDuration), tuple<Sabot::TStdDuration, Sabot::TStdDuration>(Duration_B, Duration_A))));
}

FIXTURE(TimePoint) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<Sabot::TStdTimePoint>>,           tuple<Sabot::TStdTimePoint>>          (tuple<TFree<Sabot::TStdTimePoint>>(FreeTimePoint), tuple<Sabot::TStdTimePoint>(TimePoint_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<Sabot::TStdTimePoint>>,           tuple<Sabot::TStdTimePoint>>          (tuple<TFree<Sabot::TStdTimePoint>>(FreeTimePoint), tuple<Sabot::TStdTimePoint>(TimePoint_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<Sabot::TStdTimePoint>,                  tuple<Sabot::TStdTimePoint>>          (tuple<Sabot::TStdTimePoint>(TimePoint_A), tuple<Sabot::TStdTimePoint>(TimePoint_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<Sabot::TStdTimePoint>,                  tuple<Sabot::TStdTimePoint>>          (tuple<Sabot::TStdTimePoint>(TimePoint_A), tuple<Sabot::TStdTimePoint>(TimePoint_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<Sabot::TStdTimePoint, TFree<Sabot::TStdTimePoint>>, tuple<Sabot::TStdTimePoint, Sabot::TStdTimePoint>>(tuple<Sabot::TStdTimePoint, TFree<Sabot::TStdTimePoint>>(TimePoint_A, FreeTimePoint), tuple<Sabot::TStdTimePoint, Sabot::TStdTimePoint>(TimePoint_B, TimePoint_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<Sabot::TStdTimePoint, TFree<Sabot::TStdTimePoint>>, tuple<Sabot::TStdTimePoint, Sabot::TStdTimePoint>>(tuple<Sabot::TStdTimePoint, TFree<Sabot::TStdTimePoint>>(TimePoint_A, FreeTimePoint), tuple<Sabot::TStdTimePoint, Sabot::TStdTimePoint>(TimePoint_A, TimePoint_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<Sabot::TStdTimePoint, TFree<Sabot::TStdTimePoint>>, tuple<Sabot::TStdTimePoint, TUuid>>(tuple<Sabot::TStdTimePoint, TFree<Sabot::TStdTimePoint>>(TimePoint_A, FreeTimePoint), tuple<Sabot::TStdTimePoint, TUuid>(TimePoint_A, Uuid_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<Sabot::TStdTimePoint, TFree<Sabot::TStdTimePoint>>, tuple<Sabot::TStdTimePoint, TUuid>>(tuple<Sabot::TStdTimePoint, TFree<Sabot::TStdTimePoint>>(TimePoint_A, FreeTimePoint), tuple<Sabot::TStdTimePoint, TUuid>(TimePoint_B, Uuid_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TUuid, TFree<Sabot::TStdTimePoint>>, tuple<Sabot::TStdTimePoint, Sabot::TStdTimePoint>>(tuple<TUuid, TFree<Sabot::TStdTimePoint>>(Uuid_A, FreeTimePoint), tuple<Sabot::TStdTimePoint, Sabot::TStdTimePoint>(TimePoint_B, TimePoint_A))));
}

FIXTURE(Uuid) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<TUuid>>,           tuple<TUuid>>          (tuple<TFree<TUuid>>(FreeUuid), tuple<TUuid>(Uuid_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<TUuid>>,           tuple<TUuid>>          (tuple<TFree<TUuid>>(FreeUuid), tuple<TUuid>(Uuid_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TUuid>,                  tuple<TUuid>>          (tuple<TUuid>(Uuid_A), tuple<TUuid>(Uuid_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TUuid>,                  tuple<TUuid>>          (tuple<TUuid>(Uuid_A), tuple<TUuid>(Uuid_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TUuid, TFree<TUuid>>, tuple<TUuid, TUuid>>(tuple<TUuid, TFree<TUuid>>(Uuid_A, FreeUuid), tuple<TUuid, TUuid>(Uuid_B, Uuid_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TUuid, TFree<TUuid>>, tuple<TUuid, TUuid>>(tuple<TUuid, TFree<TUuid>>(Uuid_A, FreeUuid), tuple<TUuid, TUuid>(Uuid_A, Uuid_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<TUuid, TFree<TUuid>>, tuple<TUuid, TBlob>>(tuple<TUuid, TFree<TUuid>>(Uuid_A, FreeUuid), tuple<TUuid, TBlob>(Uuid_A, Blob_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TUuid, TFree<TUuid>>, tuple<TUuid, TBlob>>(tuple<TUuid, TFree<TUuid>>(Uuid_A, FreeUuid), tuple<TUuid, TBlob>(Uuid_B, Blob_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TBlob, TFree<TUuid>>, tuple<TUuid, TUuid>>(tuple<TBlob, TFree<TUuid>>(Blob_A, FreeUuid), tuple<TUuid, TUuid>(Uuid_B, Uuid_A))));
}

FIXTURE(Blob) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<TBlob>>,           tuple<TBlob>>          (tuple<TFree<TBlob>>(FreeBlob), tuple<TBlob>(Blob_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<TBlob>>,           tuple<TBlob>>          (tuple<TFree<TBlob>>(FreeBlob), tuple<TBlob>(Blob_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TBlob>,                  tuple<TBlob>>          (tuple<TBlob>(Blob_A), tuple<TBlob>(Blob_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TBlob>,                  tuple<TBlob>>          (tuple<TBlob>(Blob_A), tuple<TBlob>(Blob_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TBlob, TFree<TBlob>>, tuple<TBlob, TBlob>>(tuple<TBlob, TFree<TBlob>>(Blob_A, FreeBlob), tuple<TBlob, TBlob>(Blob_B, Blob_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TBlob, TFree<TBlob>>, tuple<TBlob, TBlob>>(tuple<TBlob, TFree<TBlob>>(Blob_A, FreeBlob), tuple<TBlob, TBlob>(Blob_A, Blob_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<TBlob, TFree<TBlob>>, tuple<TBlob, string>>(tuple<TBlob, TFree<TBlob>>(Blob_A, FreeBlob), tuple<TBlob, string>(Blob_A, String_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TBlob, TFree<TBlob>>, tuple<TBlob, string>>(tuple<TBlob, TFree<TBlob>>(Blob_A, FreeBlob), tuple<TBlob, string>(Blob_B, String_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<string, TFree<TBlob>>, tuple<TBlob, TBlob>>(tuple<string, TFree<TBlob>>(String_A, FreeBlob), tuple<TBlob, TBlob>(Blob_B, Blob_A))));
}

FIXTURE(String) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<string>>,           tuple<string>>          (tuple<TFree<string>>(FreeString), tuple<string>(String_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<string>>,           tuple<string>>          (tuple<TFree<string>>(FreeString), tuple<string>(String_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<string>,                  tuple<string>>          (tuple<string>(String_A), tuple<string>(String_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<string>,                  tuple<string>>          (tuple<string>(String_A), tuple<string>(String_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<string, TFree<string>>, tuple<string, string>>(tuple<string, TFree<string>>(String_A, FreeString), tuple<string, string>(String_B, String_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<string, TFree<string>>, tuple<string, string>>(tuple<string, TFree<string>>(String_A, FreeString), tuple<string, string>(String_A, String_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<string, TFree<string>>, tuple<string, TDesc<int32_t>>>(tuple<string, TFree<string>>(String_A, FreeString), tuple<string, TDesc<int32_t>>(String_A, Desc_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<string, TFree<string>>, tuple<string, TDesc<int32_t>>>(tuple<string, TFree<string>>(String_A, FreeString), tuple<string, TDesc<int32_t>>(String_B, Desc_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TDesc<int32_t>, TFree<string>>, tuple<string, string>>(tuple<TDesc<int32_t>, TFree<string>>(Desc_A, FreeString), tuple<string, string>(String_B, String_A))));
}

FIXTURE(Desc) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<TDesc<int32_t>>>,           tuple<TDesc<int32_t>>>          (tuple<TFree<TDesc<int32_t>>>(FreeDesc), tuple<TDesc<int32_t>>(Desc_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<TDesc<int32_t>>>,           tuple<TDesc<int32_t>>>          (tuple<TFree<TDesc<int32_t>>>(FreeDesc), tuple<TDesc<int32_t>>(Desc_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TDesc<int32_t>>,                  tuple<TDesc<int32_t>>>          (tuple<TDesc<int32_t>>(Desc_A), tuple<TDesc<int32_t>>(Desc_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TDesc<int32_t>>,                  tuple<TDesc<int32_t>>>          (tuple<TDesc<int32_t>>(Desc_A), tuple<TDesc<int32_t>>(Desc_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TDesc<int32_t>, TFree<TDesc<int32_t>>>, tuple<TDesc<int32_t>, TDesc<int32_t>>>(tuple<TDesc<int32_t>, TFree<TDesc<int32_t>>>(Desc_A, FreeDesc), tuple<TDesc<int32_t>, TDesc<int32_t>>(Desc_B, Desc_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TDesc<int32_t>, TFree<TDesc<int32_t>>>, tuple<TDesc<int32_t>, TDesc<int32_t>>>(tuple<TDesc<int32_t>, TFree<TDesc<int32_t>>>(Desc_A, FreeDesc), tuple<TDesc<int32_t>, TDesc<int32_t>>(Desc_A, Desc_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<TDesc<int32_t>, TFree<TDesc<int32_t>>>, tuple<TDesc<int32_t>, TOpt<int32_t>>>(tuple<TDesc<int32_t>, TFree<TDesc<int32_t>>>(Desc_A, FreeDesc), tuple<TDesc<int32_t>, TOpt<int32_t>>(Desc_A, Opt_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TDesc<int32_t>, TFree<TDesc<int32_t>>>, tuple<TDesc<int32_t>, TOpt<int32_t>>>(tuple<TDesc<int32_t>, TFree<TDesc<int32_t>>>(Desc_A, FreeDesc), tuple<TDesc<int32_t>, TOpt<int32_t>>(Desc_B, Opt_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TOpt<int32_t>, TFree<TDesc<int32_t>>>, tuple<TDesc<int32_t>, TDesc<int32_t>>>(tuple<TOpt<int32_t>, TFree<TDesc<int32_t>>>(Opt_A, FreeDesc), tuple<TDesc<int32_t>, TDesc<int32_t>>(Desc_B, Desc_A))));
}

FIXTURE(Opt) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<TOpt<int32_t>>>,           tuple<TOpt<int32_t>>>          (tuple<TFree<TOpt<int32_t>>>(FreeOpt), tuple<TOpt<int32_t>>(Opt_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<TOpt<int32_t>>>,           tuple<TOpt<int32_t>>>          (tuple<TFree<TOpt<int32_t>>>(FreeOpt), tuple<TOpt<int32_t>>(Opt_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TOpt<int32_t>>,                  tuple<TOpt<int32_t>>>          (tuple<TOpt<int32_t>>(Opt_A), tuple<TOpt<int32_t>>(Opt_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TOpt<int32_t>>,                  tuple<TOpt<int32_t>>>          (tuple<TOpt<int32_t>>(Opt_A), tuple<TOpt<int32_t>>(Opt_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TOpt<int32_t>, TFree<TOpt<int32_t>>>, tuple<TOpt<int32_t>, TOpt<int32_t>>>(tuple<TOpt<int32_t>, TFree<TOpt<int32_t>>>(Opt_A, FreeOpt), tuple<TOpt<int32_t>, TOpt<int32_t>>(Opt_B, Opt_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TOpt<int32_t>, TFree<TOpt<int32_t>>>, tuple<TOpt<int32_t>, TOpt<int32_t>>>(tuple<TOpt<int32_t>, TFree<TOpt<int32_t>>>(Opt_A, FreeOpt), tuple<TOpt<int32_t>, TOpt<int32_t>>(Opt_A, Opt_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<TOpt<int32_t>, TFree<TOpt<int32_t>>>, tuple<TOpt<int32_t>, set<int32_t>>>(tuple<TOpt<int32_t>, TFree<TOpt<int32_t>>>(Opt_A, FreeOpt), tuple<TOpt<int32_t>, set<int32_t>>(Opt_A, Set_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TOpt<int32_t>, TFree<TOpt<int32_t>>>, tuple<TOpt<int32_t>, set<int32_t>>>(tuple<TOpt<int32_t>, TFree<TOpt<int32_t>>>(Opt_A, FreeOpt), tuple<TOpt<int32_t>, set<int32_t>>(Opt_B, Set_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<set<int32_t>, TFree<TOpt<int32_t>>>, tuple<TOpt<int32_t>, TOpt<int32_t>>>(tuple<set<int32_t>, TFree<TOpt<int32_t>>>(Set_A, FreeOpt), tuple<TOpt<int32_t>, TOpt<int32_t>>(Opt_B, Opt_A))));
}

FIXTURE(Set) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<set<int32_t>>>,           tuple<set<int32_t>>>          (tuple<TFree<set<int32_t>>>(FreeSet), tuple<set<int32_t>>(Set_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<set<int32_t>>>,           tuple<set<int32_t>>>          (tuple<TFree<set<int32_t>>>(FreeSet), tuple<set<int32_t>>(Set_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<set<int32_t>>,                  tuple<set<int32_t>>>          (tuple<set<int32_t>>(Set_A), tuple<set<int32_t>>(Set_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<set<int32_t>>,                  tuple<set<int32_t>>>          (tuple<set<int32_t>>(Set_A), tuple<set<int32_t>>(Set_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<set<int32_t>, TFree<set<int32_t>>>, tuple<set<int32_t>, set<int32_t>>>(tuple<set<int32_t>, TFree<set<int32_t>>>(Set_A, FreeSet), tuple<set<int32_t>, set<int32_t>>(Set_B, Set_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<set<int32_t>, TFree<set<int32_t>>>, tuple<set<int32_t>, set<int32_t>>>(tuple<set<int32_t>, TFree<set<int32_t>>>(Set_A, FreeSet), tuple<set<int32_t>, set<int32_t>>(Set_A, Set_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<set<int32_t>, TFree<set<int32_t>>>, tuple<set<int32_t>, vector<int32_t>>>(tuple<set<int32_t>, TFree<set<int32_t>>>(Set_A, FreeSet), tuple<set<int32_t>, vector<int32_t>>(Set_A, Vector_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<set<int32_t>, TFree<set<int32_t>>>, tuple<set<int32_t>, vector<int32_t>>>(tuple<set<int32_t>, TFree<set<int32_t>>>(Set_A, FreeSet), tuple<set<int32_t>, vector<int32_t>>(Set_B, Vector_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<vector<int32_t>, TFree<set<int32_t>>>, tuple<set<int32_t>, set<int32_t>>>(tuple<vector<int32_t>, TFree<set<int32_t>>>(Vector_A, FreeSet), tuple<set<int32_t>, set<int32_t>>(Set_B, Set_A))));
}

FIXTURE(Vector) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<vector<int32_t>>>,           tuple<vector<int32_t>>>          (tuple<TFree<vector<int32_t>>>(FreeVector), tuple<vector<int32_t>>(Vector_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<vector<int32_t>>>,           tuple<vector<int32_t>>>          (tuple<TFree<vector<int32_t>>>(FreeVector), tuple<vector<int32_t>>(Vector_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<vector<int32_t>>,                  tuple<vector<int32_t>>>          (tuple<vector<int32_t>>(Vector_A), tuple<vector<int32_t>>(Vector_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<vector<int32_t>>,                  tuple<vector<int32_t>>>          (tuple<vector<int32_t>>(Vector_A), tuple<vector<int32_t>>(Vector_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<vector<int32_t>, TFree<vector<int32_t>>>, tuple<vector<int32_t>, vector<int32_t>>>(tuple<vector<int32_t>, TFree<vector<int32_t>>>(Vector_A, FreeVector), tuple<vector<int32_t>, vector<int32_t>>(Vector_B, Vector_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<vector<int32_t>, TFree<vector<int32_t>>>, tuple<vector<int32_t>, vector<int32_t>>>(tuple<vector<int32_t>, TFree<vector<int32_t>>>(Vector_A, FreeVector), tuple<vector<int32_t>, vector<int32_t>>(Vector_A, Vector_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<vector<int32_t>, TFree<vector<int32_t>>>, tuple<vector<int32_t>, map<int32_t, char>>>(tuple<vector<int32_t>, TFree<vector<int32_t>>>(Vector_A, FreeVector), tuple<vector<int32_t>, map<int32_t, char>>(Vector_A, Map_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<vector<int32_t>, TFree<vector<int32_t>>>, tuple<vector<int32_t>, map<int32_t, char>>>(tuple<vector<int32_t>, TFree<vector<int32_t>>>(Vector_A, FreeVector), tuple<vector<int32_t>, map<int32_t, char>>(Vector_B, Map_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<map<int32_t, char>, TFree<vector<int32_t>>>, tuple<vector<int32_t>, vector<int32_t>>>(tuple<map<int32_t, char>, TFree<vector<int32_t>>>(Map_A, FreeVector), tuple<vector<int32_t>, vector<int32_t>>(Vector_B, Vector_A))));
}

FIXTURE(Map) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<map<int32_t, char>>>,           tuple<map<int32_t, char>>>          (tuple<TFree<map<int32_t, char>>>(FreeMap), tuple<map<int32_t, char>>(Map_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<map<int32_t, char>>>,           tuple<map<int32_t, char>>>          (tuple<TFree<map<int32_t, char>>>(FreeMap), tuple<map<int32_t, char>>(Map_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<map<int32_t, char>>,                  tuple<map<int32_t, char>>>          (tuple<map<int32_t, char>>(Map_A), tuple<map<int32_t, char>>(Map_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<map<int32_t, char>>,                  tuple<map<int32_t, char>>>          (tuple<map<int32_t, char>>(Map_A), tuple<map<int32_t, char>>(Map_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<map<int32_t, char>, TFree<map<int32_t, char>>>, tuple<map<int32_t, char>, map<int32_t, char>>>(tuple<map<int32_t, char>, TFree<map<int32_t, char>>>(Map_A, FreeMap), tuple<map<int32_t, char>, map<int32_t, char>>(Map_B, Map_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<map<int32_t, char>, TFree<map<int32_t, char>>>, tuple<map<int32_t, char>, map<int32_t, char>>>(tuple<map<int32_t, char>, TFree<map<int32_t, char>>>(Map_A, FreeMap), tuple<map<int32_t, char>, map<int32_t, char>>(Map_A, Map_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<map<int32_t, char>, TFree<map<int32_t, char>>>, tuple<map<int32_t, char>, TPoint>>(tuple<map<int32_t, char>, TFree<map<int32_t, char>>>(Map_A, FreeMap), tuple<map<int32_t, char>, TPoint>(Map_A, Point_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<map<int32_t, char>, TFree<map<int32_t, char>>>, tuple<map<int32_t, char>, TPoint>>(tuple<map<int32_t, char>, TFree<map<int32_t, char>>>(Map_A, FreeMap), tuple<map<int32_t, char>, TPoint>(Map_B, Point_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TPoint, TFree<map<int32_t, char>>>, tuple<map<int32_t, char>, map<int32_t, char>>>(tuple<TPoint, TFree<map<int32_t, char>>>(Point_A, FreeMap), tuple<map<int32_t, char>, map<int32_t, char>>(Map_B, Map_A))));
}

FIXTURE(Record) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<TPoint>>,           tuple<TPoint>>          (tuple<TFree<TPoint>>(FreePoint), tuple<TPoint>(Point_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<TPoint>>,           tuple<TPoint>>          (tuple<TFree<TPoint>>(FreePoint), tuple<TPoint>(Point_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TPoint>,                  tuple<TPoint>>          (tuple<TPoint>(Point_A), tuple<TPoint>(Point_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TPoint>,                  tuple<TPoint>>          (tuple<TPoint>(Point_A), tuple<TPoint>(Point_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TPoint, TFree<TPoint>>, tuple<TPoint, TPoint>>(tuple<TPoint, TFree<TPoint>>(Point_A, FreePoint), tuple<TPoint, TPoint>(Point_B, Point_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TPoint, TFree<TPoint>>, tuple<TPoint, TPoint>>(tuple<TPoint, TFree<TPoint>>(Point_A, FreePoint), tuple<TPoint, TPoint>(Point_A, Point_A))));

  EXPECT_TRUE(IsPrefixMatch(CheckStates<tuple<TPoint, TFree<TPoint>>, tuple<TPoint, tuple<int32_t>>>(tuple<TPoint, TFree<TPoint>>(Point_A, FreePoint), tuple<TPoint, tuple<int32_t>>(Point_A, Tuple_A))));
  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<TPoint, TFree<TPoint>>, tuple<TPoint, tuple<int32_t>>>(tuple<TPoint, TFree<TPoint>>(Point_A, FreePoint), tuple<TPoint, tuple<int32_t>>(Point_B, Tuple_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<tuple<int32_t>, TFree<TPoint>>, tuple<TPoint, TPoint>>(tuple<tuple<int32_t>, TFree<TPoint>>(Tuple_A, FreePoint), tuple<TPoint, TPoint>(Point_B, Point_A))));
}

FIXTURE(Tuple) {
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<tuple<int32_t>>>,           tuple<tuple<int32_t>>>          (tuple<TFree<tuple<int32_t>>>(FreeTuple), tuple<tuple<int32_t>>(Tuple_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<TFree<tuple<int32_t>>>,           tuple<tuple<int32_t>>>          (tuple<TFree<tuple<int32_t>>>(FreeTuple), tuple<tuple<int32_t>>(Tuple_B))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<tuple<int32_t>>,                  tuple<tuple<int32_t>>>          (tuple<tuple<int32_t>>(Tuple_A), tuple<tuple<int32_t>>(Tuple_B))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<tuple<int32_t>>,                  tuple<tuple<int32_t>>>          (tuple<tuple<int32_t>>(Tuple_A), tuple<tuple<int32_t>>(Tuple_A))));

  EXPECT_TRUE(IsNoMatch    (CheckStates<tuple<tuple<int32_t>, TFree<tuple<int32_t>>>, tuple<tuple<int32_t>, tuple<int32_t>>>(tuple<tuple<int32_t>, TFree<tuple<int32_t>>>(Tuple_A, FreeTuple), tuple<tuple<int32_t>, tuple<int32_t>>(Tuple_B, Tuple_A))));
  EXPECT_TRUE(IsUnifies    (CheckStates<tuple<tuple<int32_t>, TFree<tuple<int32_t>>>, tuple<tuple<int32_t>, tuple<int32_t>>>(tuple<tuple<int32_t>, TFree<tuple<int32_t>>>(Tuple_A, FreeTuple), tuple<tuple<int32_t>, tuple<int32_t>>(Tuple_A, Tuple_A))));
}