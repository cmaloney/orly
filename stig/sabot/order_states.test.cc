/* <stig/sabot/order_states.test.cc>

   Unit test for <stig/sabot/order_states.h>.

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

static const int8_t Int8_L = -1;
static const int8_t Int8_E = 0;
static const int8_t Int8_G = 1;
static const TOpt<int8_t> OptInt8(Int8_L);

static const int16_t Int16_L = -3;
static const int16_t Int16_E = 2;
static const int16_t Int16_G = 3;
static const TOpt<int16_t> OptInt16(Int16_L);

static const int32_t Int32_L = -5;
static const int32_t Int32_E = 4;
static const int32_t Int32_G = 5;
static const TOpt<int32_t> OptInt32(Int32_L);

static const int64_t Int64_L = -7;
static const int64_t Int64_E = 6;
static const int64_t Int64_G = 7;
static const TOpt<int64_t> OptInt64(Int64_L);

static const uint8_t UInt8_L = 0;
static const uint8_t UInt8_E = 1;
static const uint8_t UInt8_G = 2;
static const TOpt<uint8_t> OptUInt8(UInt8_L);

static const uint16_t UInt16_L = 3;
static const uint16_t UInt16_E = 4;
static const uint16_t UInt16_G = 5;
static const TOpt<uint16_t> OptUInt16(UInt16_L);

static const uint32_t UInt32_L = 6;
static const uint32_t UInt32_E = 7;
static const uint32_t UInt32_G = 8;
static const TOpt<uint32_t> OptUInt32(UInt32_L);

static const uint64_t UInt64_L = 9;
static const uint64_t UInt64_E = 10;
static const uint64_t UInt64_G = 11;
static const TOpt<uint64_t> OptUInt64(UInt64_L);

static const bool Bool_F = false;
static const bool Bool_T = true;
static const TOpt<bool> OptBool(Bool_F);

static const char Char_L = 'A';
static const char Char_E = 'M';
static const char Char_G = 'Z';
static const TOpt<char> OptChar(Char_G);

static const float Float_L = 0.1;
static const float Float_E = 0.5;
static const float Float_G = 0.9;
static const TOpt<float> OptFloat(Float_G);

static const double Double_L = 1.1;
static const double Double_E = 5.5;
static const double Double_G = 9.9;
static const TOpt<double> OptDouble(Double_G);

static const Sabot::TStdDuration Duration_L(100);
static const Sabot::TStdDuration Duration_E(1000);
static const Sabot::TStdDuration Duration_G(10000);
static const TOpt<Sabot::TStdDuration> OptDuration(Duration_G);

static const Sabot::TStdTimePoint TimePoint_L(Duration_L);
static const Sabot::TStdTimePoint TimePoint_E(Duration_E);
static const Sabot::TStdTimePoint TimePoint_G(Duration_G);
static const TOpt<Sabot::TStdTimePoint> OptTimePoint(TimePoint_G);

static const TUuid Uuid_L("AAAAAAAA-AAAA-AAAA-AAAA-AAAAAAAAAAAA");
static const TUuid Uuid_E("BBBBBBBB-BBBB-BBBB-BBBB-BBBBBBBBBBBB");
static const TUuid Uuid_G("CCCCCCCC-CCCC-CCCC-CCCC-CCCCCCCCCCCC");
static const TOpt<TUuid> OptUuid(Uuid_G);

static const Native::TBlob Blob_LS{'A', 'A', 'A'};
static const Native::TBlob Blob_L {'A', 'A', 'A', 'A'};
static const Native::TBlob Blob_E {'C', 'C', 'C', 'C'};
static const Native::TBlob Blob_G {'F', 'F', 'F', 'F'};
static const Native::TBlob Blob_GL{'F', 'F', 'F', 'F', 'F'};
static const TOpt<Native::TBlob> OptBlob(Blob_GL);

static const string String_LS = "AAAAAAA";
static const string String_L  = "AAAAAAAA";
static const string String_E  = "BBBBBBBB";
static const string String_G  = "CCCCCCCC";
static const string String_GL = "CCCCCCCCC";
static const TOpt<string> OptString(String_GL);

static const TDesc<int32_t> Desc_L(Int32_G);
static const TDesc<int32_t> Desc_E(Int32_E);
static const TDesc<int32_t> Desc_G(Int32_L);
static const TOpt<TDesc<int32_t>> OptDesc(Desc_G);

static const set<int32_t> Set_LS{1, 2};
static const set<int32_t> Set_L{3, 2, 1};
static const set<int32_t> Set_E{2, 3, 4};
static const set<int32_t> Set_G{3, 4, 5};
static const set<int32_t> Set_GL{3, 4, 5, 6};
static const TOpt<set<int32_t>> OptSet(Set_GL);

static const vector<int32_t> Vector_LS{1, 2};
static const vector<int32_t> Vector_L{1, 2, 3};
static const vector<int32_t> Vector_E{2, 3, 4};
static const vector<int32_t> Vector_G{3, 4, 5};
static const vector<int32_t> Vector_GL{3, 4, 5, 6};
static const TOpt<vector<int32_t>> OptVector(Vector_GL);

static const map<int32_t, char> Map_LS{{1, 'A'}, {2, 'B'}};
static const map<int32_t, char> Map_L{{1, 'A'}, {2, 'B'}, {3, 'C'}};
static const map<int32_t, char> Map_E{{1, 'B'}, {2, 'C'}, {3, 'D'}};
static const map<int32_t, char> Map_G{{1, 'B'}, {3, 'A'}, {3, 'C'}};
static const map<int32_t, char> Map_GL{{1, 'B'}, {3, 'A'}, {3, 'C'}, {4, 'D'}};
static const TOpt<map<int32_t, char>> OptMap(Map_GL);

static const TPoint Point_L(1.0, 2.0);
static const TPoint Point_E(2.0, 3.0);
static const TPoint Point_G(3.0, 4.0);
static const TOpt<TPoint> OptPoint(Point_G);

static const tuple<int32_t> Tuple_L(make_tuple(Int32_L));
static const tuple<int32_t> Tuple_E(make_tuple(Int32_E));
static const tuple<int32_t> Tuple_G(make_tuple(Int32_G));
static const TOpt<tuple<int32_t>> OptTuple(Tuple_G);

template <typename TLhs, typename TRhs>
TComparison CheckStates(const TLhs &lhs, const TRhs &rhs) {
  void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
  void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
  Sabot::State::TAny::TWrapper
    lhs_state(State::New(lhs, lhs_state_alloc)),
    rhs_state(State::New(rhs, rhs_state_alloc));
  return OrderStates(*lhs_state, *rhs_state);
}

FIXTURE(TupleKeySpace) {
  /*
      <[5]>
      <[5, 9]>
      <[7]>
      <[7, 9]>
  */
  EXPECT_TRUE(IsEq(CheckStates<tuple<int8_t>, tuple<int8_t>        > (tuple<int8_t>(5), tuple<int8_t>(5))));
  EXPECT_TRUE(IsLt(CheckStates<tuple<int8_t>, tuple<int8_t, int8_t>> (tuple<int8_t>(5), tuple<int8_t, int8_t>(5, 9))));
  EXPECT_TRUE(IsLt(CheckStates<tuple<int8_t>, tuple<int8_t>        > (tuple<int8_t>(5), tuple<int8_t>(7))));
  EXPECT_TRUE(IsLt(CheckStates<tuple<int8_t>, tuple<int8_t, int8_t>> (tuple<int8_t>(5), tuple<int8_t, int8_t>(7, 9))));

  EXPECT_TRUE(IsGt(CheckStates<tuple<int8_t, int8_t>, tuple<int8_t>        > (tuple<int8_t, int8_t>(5, 9), tuple<int8_t>(5))));
  EXPECT_TRUE(IsEq(CheckStates<tuple<int8_t, int8_t>, tuple<int8_t, int8_t>> (tuple<int8_t, int8_t>(5, 9), tuple<int8_t, int8_t>(5, 9))));
  EXPECT_TRUE(IsLt(CheckStates<tuple<int8_t, int8_t>, tuple<int8_t>        > (tuple<int8_t, int8_t>(5, 9), tuple<int8_t>(7))));
  EXPECT_TRUE(IsLt(CheckStates<tuple<int8_t, int8_t>, tuple<int8_t, int8_t>> (tuple<int8_t, int8_t>(5, 9), tuple<int8_t, int8_t>(7, 9))));

  EXPECT_TRUE(IsGt(CheckStates<tuple<int8_t>, tuple<int8_t>        > (tuple<int8_t>(7), tuple<int8_t>(5))));
  EXPECT_TRUE(IsGt(CheckStates<tuple<int8_t>, tuple<int8_t, int8_t>> (tuple<int8_t>(7), tuple<int8_t, int8_t>(5, 9))));
  EXPECT_TRUE(IsEq(CheckStates<tuple<int8_t>, tuple<int8_t>        > (tuple<int8_t>(7), tuple<int8_t>(7))));
  EXPECT_TRUE(IsLt(CheckStates<tuple<int8_t>, tuple<int8_t, int8_t>> (tuple<int8_t>(7), tuple<int8_t, int8_t>(7, 9))));

  EXPECT_TRUE(IsGt(CheckStates<tuple<int8_t, int8_t>, tuple<int8_t>        > (tuple<int8_t, int8_t>(7, 9), tuple<int8_t>(5))));
  EXPECT_TRUE(IsGt(CheckStates<tuple<int8_t, int8_t>, tuple<int8_t, int8_t>> (tuple<int8_t, int8_t>(7, 9), tuple<int8_t, int8_t>(5, 9))));
  EXPECT_TRUE(IsGt(CheckStates<tuple<int8_t, int8_t>, tuple<int8_t>        > (tuple<int8_t, int8_t>(7, 9), tuple<int8_t>(7))));
  EXPECT_TRUE(IsEq(CheckStates<tuple<int8_t, int8_t>, tuple<int8_t, int8_t>> (tuple<int8_t, int8_t>(7, 9), tuple<int8_t, int8_t>(7, 9))));
}

FIXTURE(MoreTuples) {
  EXPECT_TRUE(IsGt(CheckStates<
                   tuple<int32_t, Sabot::TStdTimePoint>,
                   tuple<int32_t, Base::TUuid>>(
    tuple<int32_t, Sabot::TStdTimePoint>(9872145, Sabot::TStdTimePoint(Sabot::TStdDuration(1))),
    tuple<int32_t, Base::TUuid>(8754321, Base::TUuid(TUuid::Best)))));

  EXPECT_TRUE(IsNe(CheckStates<
                   tuple<int32_t, Sabot::TStdTimePoint>,
                   tuple<int32_t, Base::TUuid>>(
    tuple<int32_t, Sabot::TStdTimePoint>(9872145, Sabot::TStdTimePoint(Sabot::TStdDuration(1))),
    tuple<int32_t, Base::TUuid>(9872145, Base::TUuid(TUuid::Best)))));
}

FIXTURE(Free) {
  EXPECT_TRUE(IsEq(CheckStates<TFree<bool>, TFree<bool>>(Native::TFree<bool>::Free, Native::TFree<bool>::Free)));
}

FIXTURE(Tombstone) {
  EXPECT_TRUE(IsEq(CheckStates<TTombstone, TTombstone>(Native::TTombstone::Tombstone, Native::TTombstone::Tombstone)));
}

FIXTURE(Void) {
  // Note: There is no native void state.
}

FIXTURE(Int8) {
  EXPECT_TRUE(IsLt(CheckStates<TFree<int8_t>,      int8_t> (Native::TFree<int8_t>::Free, Int8_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<int8_t>, TOpt<int8_t>>(Native::TFree<int8_t>::Free, OptInt8)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <int8_t>,      int8_t> (OptInt8, Int8_L)));

  EXPECT_TRUE(IsLt(CheckStates<int8_t, int8_t>(Int8_L, Int8_E)));
  EXPECT_TRUE(IsEq(CheckStates<int8_t, int8_t>(Int8_E, Int8_E)));
  EXPECT_TRUE(IsGt(CheckStates<int8_t, int8_t>(Int8_G, Int8_E)));

  EXPECT_TRUE(IsLt(CheckStates<int8_t, int16_t>(Int8_G, Int16_L)));
}

FIXTURE(Int16) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<int16_t>,      int8_t > (Native::TFree<int16_t>::Free, Int8_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<int16_t>,      int16_t> (Native::TFree<int16_t>::Free, Int16_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<int16_t>, TOpt<int16_t>>(Native::TFree<int16_t>::Free, OptInt16)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <int16_t>,      int16_t> (OptInt16, Int16_L)));

  EXPECT_TRUE(IsLt(CheckStates<int16_t, int16_t>(Int16_L, Int16_E)));
  EXPECT_TRUE(IsEq(CheckStates<int16_t, int16_t>(Int16_E, Int16_E)));
  EXPECT_TRUE(IsGt(CheckStates<int16_t, int16_t>(Int16_G, Int16_E)));

  EXPECT_TRUE(IsGt(CheckStates<int16_t, int8_t >(Int16_G, Int8_G )));
  EXPECT_TRUE(IsLt(CheckStates<int16_t, int32_t>(Int16_G, Int32_L)));
}

FIXTURE(Int32) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<int32_t>,      int16_t >(Native::TFree<int32_t>::Free, Int16_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<int32_t>,      int32_t> (Native::TFree<int32_t>::Free, Int32_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<int32_t>, TOpt<int32_t>>(Native::TFree<int32_t>::Free, OptInt32)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <int32_t>,      int32_t> (OptInt32, Int32_L)));

  EXPECT_TRUE(IsLt(CheckStates<int32_t, int32_t>(Int32_L, Int32_E)));
  EXPECT_TRUE(IsEq(CheckStates<int32_t, int32_t>(Int32_E, Int32_E)));
  EXPECT_TRUE(IsGt(CheckStates<int32_t, int32_t>(Int32_G, Int32_E)));

  EXPECT_TRUE(IsGt(CheckStates<int32_t, int16_t>(Int32_G, Int16_G)));
  EXPECT_TRUE(IsLt(CheckStates<int32_t, int64_t>(Int32_G, Int64_L)));
}

FIXTURE(Int64) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<int64_t>,      int32_t >(Native::TFree<int64_t>::Free, Int32_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<int64_t>,      int64_t> (Native::TFree<int64_t>::Free, Int64_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<int64_t>, TOpt<int64_t>>(Native::TFree<int64_t>::Free, OptInt64)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <int64_t>,      int64_t> (OptInt64, Int64_L)));

  EXPECT_TRUE(IsLt(CheckStates<int64_t, int64_t>(Int64_L, Int64_E)));
  EXPECT_TRUE(IsEq(CheckStates<int64_t, int64_t>(Int64_E, Int64_E)));
  EXPECT_TRUE(IsGt(CheckStates<int64_t, int64_t>(Int64_G, Int64_E)));

  EXPECT_TRUE(IsGt(CheckStates<int64_t, int32_t>(Int64_G, Int32_G)));
  EXPECT_TRUE(IsLt(CheckStates<int64_t, uint8_t>(Int64_G, UInt8_L)));
}

FIXTURE(UInt8) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<uint8_t>,      int64_t >(Native::TFree<uint8_t>::Free, Int64_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<uint8_t>,      uint8_t> (Native::TFree<uint8_t>::Free, UInt8_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<uint8_t>, TOpt<uint8_t>>(Native::TFree<uint8_t>::Free, OptUInt8)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <uint8_t>,      uint8_t> (OptUInt8, UInt8_L)));

  EXPECT_TRUE(IsLt(CheckStates<uint8_t, uint8_t >(UInt8_L, UInt8_E)));
  EXPECT_TRUE(IsEq(CheckStates<uint8_t, uint8_t >(UInt8_E, UInt8_E)));
  EXPECT_TRUE(IsGt(CheckStates<uint8_t, uint8_t >(UInt8_G, UInt8_E)));

  EXPECT_TRUE(IsGt(CheckStates<uint8_t, int64_t >(UInt8_G, Int64_G)));
  EXPECT_TRUE(IsLt(CheckStates<uint8_t, uint16_t>(UInt8_G, UInt16_L)));
}

FIXTURE(UInt16) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<uint16_t>,      uint8_t > (Native::TFree<uint16_t>::Free, UInt8_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<uint16_t>,      uint16_t> (Native::TFree<uint16_t>::Free, UInt16_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<uint16_t>, TOpt<uint16_t>>(Native::TFree<uint16_t>::Free, OptUInt16)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <uint16_t>,      uint16_t> (OptUInt16, UInt16_L)));

  EXPECT_TRUE(IsLt(CheckStates<uint16_t, uint16_t>(UInt16_L, UInt16_E)));
  EXPECT_TRUE(IsEq(CheckStates<uint16_t, uint16_t>(UInt16_E, UInt16_E)));
  EXPECT_TRUE(IsGt(CheckStates<uint16_t, uint16_t>(UInt16_G, UInt16_E)));

  EXPECT_TRUE(IsGt(CheckStates<uint16_t, uint8_t >(UInt16_G, UInt8_G)));
  EXPECT_TRUE(IsLt(CheckStates<uint16_t, uint32_t>(UInt16_G, UInt32_L)));
}

FIXTURE(UInt32) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<uint32_t>,      uint16_t >(Native::TFree<uint32_t>::Free, UInt16_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<uint32_t>,      uint32_t> (Native::TFree<uint32_t>::Free, UInt32_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<uint32_t>, TOpt<uint32_t>>(Native::TFree<uint32_t>::Free, OptUInt32)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <uint32_t>,      uint32_t> (OptUInt32, UInt32_L)));

  EXPECT_TRUE(IsLt(CheckStates<uint32_t, uint32_t>(UInt32_L, UInt32_E)));
  EXPECT_TRUE(IsEq(CheckStates<uint32_t, uint32_t>(UInt32_E, UInt32_E)));
  EXPECT_TRUE(IsGt(CheckStates<uint32_t, uint32_t>(UInt32_G, UInt32_E)));

  EXPECT_TRUE(IsGt(CheckStates<uint32_t, uint16_t>(UInt32_G, UInt16_G)));
  EXPECT_TRUE(IsLt(CheckStates<uint32_t, uint64_t>(UInt32_G, UInt64_L)));
}

FIXTURE(UInt64) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<uint64_t>,      uint32_t >(Native::TFree<uint64_t>::Free, UInt32_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<uint64_t>,      uint64_t> (Native::TFree<uint64_t>::Free, UInt64_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<uint64_t>, TOpt<uint64_t>>(Native::TFree<uint64_t>::Free, OptUInt64)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <uint64_t>,      uint64_t> (OptUInt64, UInt64_L)));

  EXPECT_TRUE(IsLt(CheckStates<uint64_t, uint64_t>(UInt64_L, UInt64_E)));
  EXPECT_TRUE(IsEq(CheckStates<uint64_t, uint64_t>(UInt64_E, UInt64_E)));
  EXPECT_TRUE(IsGt(CheckStates<uint64_t, uint64_t>(UInt64_G, UInt64_E)));

  EXPECT_TRUE(IsGt(CheckStates<uint64_t, uint32_t>(UInt64_G, UInt32_G)));
  EXPECT_TRUE(IsLt(CheckStates<uint64_t, bool    >(UInt64_G, Bool_T  )));
}

FIXTURE(Bool) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<bool>,      uint64_t>(Native::TFree<bool>::Free, UInt64_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<bool>,      bool>    (Native::TFree<bool>::Free, Bool_F)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<bool>, TOpt<bool>>   (Native::TFree<bool>::Free, OptBool)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <bool>,      bool>    (OptBool, Bool_F)));

  EXPECT_TRUE(IsLt(CheckStates<bool, bool>    (Bool_F, Bool_T)));
  EXPECT_TRUE(IsEq(CheckStates<bool, bool>    (Bool_F, Bool_F)));
  EXPECT_TRUE(IsGt(CheckStates<bool, bool>    (Bool_T, Bool_F)));

  EXPECT_TRUE(IsGt(CheckStates<bool, uint64_t>(Bool_F, UInt64_G)));
  EXPECT_TRUE(IsLt(CheckStates<bool, char    >(Bool_F, Char_L  )));
}

FIXTURE(Char) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<char>,      bool >(Native::TFree<char>::Free, Bool_T)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<char>,      char> (Native::TFree<char>::Free, Char_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<char>, TOpt<char>>(Native::TFree<char>::Free, OptChar)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <char>,      char> (OptChar, Char_L)));

  EXPECT_TRUE(IsLt(CheckStates<char, char> (Char_L, Char_E)));
  EXPECT_TRUE(IsEq(CheckStates<char, char> (Char_E, Char_E)));
  EXPECT_TRUE(IsGt(CheckStates<char, char> (Char_G, Char_E)));

  EXPECT_TRUE(IsGt(CheckStates<char, bool >(Char_G, Bool_T)));
  EXPECT_TRUE(IsLt(CheckStates<char, float>(Char_G, Float_L)));
}

FIXTURE(Float) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<float>,      char > (Native::TFree<float>::Free, Char_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<float>,      float> (Native::TFree<float>::Free, Float_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<float>, TOpt<float>>(Native::TFree<float>::Free, OptFloat)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <float>,      float> (OptFloat, Float_L)));

  EXPECT_TRUE(IsLt(CheckStates<float, float> (Float_L, Float_E)));
  EXPECT_TRUE(IsEq(CheckStates<float, float> (Float_E, Float_E)));
  EXPECT_TRUE(IsGt(CheckStates<float, float> (Float_G, Float_E)));

  EXPECT_TRUE(IsGt(CheckStates<float, char  >(Float_G, Char_G)));
  EXPECT_TRUE(IsLt(CheckStates<float, double>(Float_G, Double_L)));
}

FIXTURE(Double) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<double>,      float > (Native::TFree<double>::Free, Float_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<double>,      double> (Native::TFree<double>::Free, Double_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<double>, TOpt<double>>(Native::TFree<double>::Free, OptDouble)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <double>,      double> (OptDouble, Double_L)));

  EXPECT_TRUE(IsLt(CheckStates<double, double>(Double_L, Double_E)));
  EXPECT_TRUE(IsEq(CheckStates<double, double>(Double_E, Double_E)));
  EXPECT_TRUE(IsGt(CheckStates<double, double>(Double_G, Double_E)));

  EXPECT_TRUE(IsGt(CheckStates<double, float >(Double_G, Float_G)));
  EXPECT_TRUE(IsLt(CheckStates<double, Sabot::TStdDuration>(Double_G, Duration_L)));
}

FIXTURE(Duration) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<Sabot::TStdDuration>,      double             > (Native::TFree<Sabot::TStdDuration>::Free, Double_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<Sabot::TStdDuration>,      Sabot::TStdDuration> (Native::TFree<Sabot::TStdDuration>::Free, Duration_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<Sabot::TStdDuration>, TOpt<Sabot::TStdDuration>>(Native::TFree<Sabot::TStdDuration>::Free, OptDuration)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <Sabot::TStdDuration>,      Sabot::TStdDuration> (OptDuration, Duration_L)));

  EXPECT_TRUE(IsLt(CheckStates<Sabot::TStdDuration, Sabot::TStdDuration>(Duration_L, Duration_E)));
  EXPECT_TRUE(IsEq(CheckStates<Sabot::TStdDuration, Sabot::TStdDuration>(Duration_E, Duration_E)));
  EXPECT_TRUE(IsGt(CheckStates<Sabot::TStdDuration, Sabot::TStdDuration>(Duration_G, Duration_E)));

  EXPECT_TRUE(IsGt(CheckStates<Sabot::TStdDuration, double>(Duration_G, Double_G)));
  EXPECT_TRUE(IsLt(CheckStates<Sabot::TStdDuration, Sabot::TStdTimePoint>(Duration_G, TimePoint_L)));
}

FIXTURE(TimePoint) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<Sabot::TStdTimePoint>,      Sabot::TStdDuration > (Native::TFree<Sabot::TStdTimePoint>::Free, Duration_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<Sabot::TStdTimePoint>,      Sabot::TStdTimePoint> (Native::TFree<Sabot::TStdTimePoint>::Free, TimePoint_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<Sabot::TStdTimePoint>, TOpt<Sabot::TStdTimePoint>>(Native::TFree<Sabot::TStdTimePoint>::Free, OptTimePoint)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <Sabot::TStdTimePoint>,      Sabot::TStdTimePoint> (OptTimePoint, TimePoint_L)));

  EXPECT_TRUE(IsLt(CheckStates<TOpt <Sabot::TStdDuration>,      Sabot::TStdDuration> (OptDuration, Duration_L)));
  EXPECT_TRUE(IsLt(CheckStates<Sabot::TStdTimePoint, Sabot::TStdTimePoint>(TimePoint_L, TimePoint_E)));
  EXPECT_TRUE(IsEq(CheckStates<Sabot::TStdTimePoint, Sabot::TStdTimePoint>(TimePoint_E, TimePoint_E)));
  EXPECT_TRUE(IsGt(CheckStates<Sabot::TStdTimePoint, Sabot::TStdTimePoint>(TimePoint_G, TimePoint_E)));

  EXPECT_TRUE(IsGt(CheckStates<Sabot::TStdTimePoint, Sabot::TStdDuration>(TimePoint_G, Duration_G)));
  EXPECT_TRUE(IsLt(CheckStates<Sabot::TStdTimePoint, TUuid>(TimePoint_G, Uuid_L)));
}

FIXTURE(Uuid) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<TUuid>,      Sabot::TStdTimePoint>(Native::TFree<TUuid>::Free, TimePoint_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<TUuid>,      TUuid>               (Native::TFree<TUuid>::Free, Uuid_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<TUuid>, TOpt<TUuid>>              (Native::TFree<TUuid>::Free, OptUuid)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <TUuid>,      TUuid>               (OptUuid, Uuid_L)));

  EXPECT_TRUE(IsLt(CheckStates<TUuid, TUuid>(Uuid_L, Uuid_E)));
  EXPECT_TRUE(IsEq(CheckStates<TUuid, TUuid>(Uuid_E, Uuid_E)));
  EXPECT_TRUE(IsGt(CheckStates<TUuid, TUuid>(Uuid_G, Uuid_E)));

  EXPECT_TRUE(IsGt(CheckStates<TUuid, Sabot::TStdTimePoint>(Uuid_G, TimePoint_G)));
  EXPECT_TRUE(IsLt(CheckStates<TUuid, TBlob>(Uuid_G, Blob_L)));
}

FIXTURE(Blob) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<TBlob>,      TUuid >(Native::TFree<TBlob>::Free, Uuid_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<TBlob>,      TBlob> (Native::TFree<TBlob>::Free, Blob_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<TBlob>, TOpt<TBlob>>(Native::TFree<TBlob>::Free, OptBlob)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <TBlob>,      TBlob> (OptBlob, Blob_L)));

  EXPECT_TRUE(IsLt(CheckStates<TBlob, TBlob >(Blob_LS, Blob_L)));
  EXPECT_TRUE(IsLt(CheckStates<TBlob, TBlob >(Blob_L , Blob_E)));
  EXPECT_TRUE(IsEq(CheckStates<TBlob, TBlob >(Blob_E , Blob_E)));
  EXPECT_TRUE(IsGt(CheckStates<TBlob, TBlob >(Blob_G , Blob_E)));
  EXPECT_TRUE(IsGt(CheckStates<TBlob, TBlob >(Blob_GL, Blob_G)));

  EXPECT_TRUE(IsGt(CheckStates<TBlob, TUuid >(Blob_G, Uuid_G)));
  EXPECT_TRUE(IsLt(CheckStates<TBlob, string>(Blob_G, String_L)));
}

FIXTURE(String) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<string>,      TBlob > (Native::TFree<string>::Free, Blob_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<string>,      string> (Native::TFree<string>::Free, String_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<string>, TOpt<string>>(Native::TFree<string>::Free, OptString)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <string>,      string> (OptString, String_L)));

  EXPECT_TRUE(IsLt(CheckStates<string, string>(String_LS, String_L)));
  EXPECT_TRUE(IsLt(CheckStates<string, string>(String_L , String_E)));
  EXPECT_TRUE(IsEq(CheckStates<string, string>(String_E , String_E)));
  EXPECT_TRUE(IsGt(CheckStates<string, string>(String_G , String_E)));
  EXPECT_TRUE(IsGt(CheckStates<string, string>(String_GL, String_G)));

  EXPECT_TRUE(IsGt(CheckStates<string, TBlob >(String_G, Blob_G)));
  EXPECT_TRUE(IsLt(CheckStates<string, TDesc<int32_t>>(String_G, Desc_L)));
}

FIXTURE(Desc) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<TDesc<int32_t>>,      string        > (Native::TFree<TDesc<int32_t>>::Free, String_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<TDesc<int32_t>>,      TDesc<int32_t>> (Native::TFree<TDesc<int32_t>>::Free, Desc_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<TDesc<int32_t>>, TOpt<TDesc<int32_t>>>(Native::TFree<TDesc<int32_t>>::Free, OptDesc)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <TDesc<int32_t>>,      TDesc<int32_t>> (OptDesc, Desc_L)));

  EXPECT_TRUE(IsLt(CheckStates<TDesc<int32_t>, TDesc<int32_t>>(Desc_L, Desc_E)));
  EXPECT_TRUE(IsEq(CheckStates<TDesc<int32_t>, TDesc<int32_t>>(Desc_E, Desc_E)));
  EXPECT_TRUE(IsGt(CheckStates<TDesc<int32_t>, TDesc<int32_t>>(Desc_G, Desc_E)));

  EXPECT_TRUE(IsGt(CheckStates<TDesc<int32_t>, string>(Desc_G, String_G)));
  EXPECT_TRUE(IsLt(CheckStates<TDesc<int32_t>, set<int32_t>>(Desc_G, Set_L)));
}

FIXTURE(Set) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<set<int32_t>>,      TDesc<int32_t>>(Native::TFree<set<int32_t>>::Free, Desc_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<set<int32_t>>,      set<int32_t>>  (Native::TFree<set<int32_t>>::Free, Set_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<set<int32_t>>, TOpt<set<int32_t>>> (Native::TFree<set<int32_t>>::Free, OptSet)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <set<int32_t>>,      set<int32_t>>  (OptSet, Set_L)));

  EXPECT_TRUE(IsLt(CheckStates<set<int32_t>, set<int32_t>>   (Set_LS, Set_L)));
  EXPECT_TRUE(IsLt(CheckStates<set<int32_t>, set<int32_t>>   (Set_L , Set_E)));
  EXPECT_TRUE(IsEq(CheckStates<set<int32_t>, set<int32_t>>   (Set_E , Set_E)));
  EXPECT_TRUE(IsGt(CheckStates<set<int32_t>, set<int32_t>>   (Set_G , Set_E)));
  EXPECT_TRUE(IsGt(CheckStates<set<int32_t>, set<int32_t>>   (Set_GL, Set_G)));

  EXPECT_TRUE(IsGt(CheckStates<set<int32_t>, TDesc<int32_t>> (Set_G, Desc_G)));
  EXPECT_TRUE(IsLt(CheckStates<set<int32_t>, vector<int32_t>>(Set_G, Vector_L)));
}

FIXTURE(Vector) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<vector<int32_t>>,      set<int32_t>   > (Native::TFree<vector<int32_t>>::Free, Set_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<vector<int32_t>>,      vector<int32_t>> (Native::TFree<vector<int32_t>>::Free, Vector_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<vector<int32_t>>, TOpt<vector<int32_t>>>(Native::TFree<vector<int32_t>>::Free, OptVector)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <vector<int32_t>>,      vector<int32_t>> (OptVector, Vector_L)));

  EXPECT_TRUE(IsLt(CheckStates<vector<int32_t>, vector<int32_t>>(Vector_LS, Vector_L)));
  EXPECT_TRUE(IsLt(CheckStates<vector<int32_t>, vector<int32_t>>(Vector_L , Vector_E)));
  EXPECT_TRUE(IsEq(CheckStates<vector<int32_t>, vector<int32_t>>(Vector_E , Vector_E)));
  EXPECT_TRUE(IsGt(CheckStates<vector<int32_t>, vector<int32_t>>(Vector_G , Vector_E)));
  EXPECT_TRUE(IsGt(CheckStates<vector<int32_t>, vector<int32_t>>(Vector_GL, Vector_G)));

  EXPECT_TRUE(IsGt(CheckStates<vector<int32_t>, set<int32_t>>   (Vector_G, Set_G)));
  EXPECT_TRUE(IsLt(CheckStates<vector<int32_t>, map<int32_t, char>>(Vector_G, Map_L)));
}

FIXTURE(Map) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<map<int32_t, char>>,      vector<int32_t>   > (Native::TFree<map<int32_t, char>>::Free, Vector_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<map<int32_t, char>>,      map<int32_t, char>> (Native::TFree<map<int32_t, char>>::Free, Map_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<map<int32_t, char>>, TOpt<map<int32_t, char>>>(Native::TFree<map<int32_t, char>>::Free, OptMap)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <map<int32_t, char>>,      map<int32_t, char>> (OptMap, Map_L)));

  EXPECT_TRUE(IsLt(CheckStates<map<int32_t, char>, map<int32_t, char>>(Map_LS, Map_L)));
  EXPECT_TRUE(IsLt(CheckStates<map<int32_t, char>, map<int32_t, char>>(Map_L , Map_E)));
  EXPECT_TRUE(IsEq(CheckStates<map<int32_t, char>, map<int32_t, char>>(Map_E , Map_E)));
  EXPECT_TRUE(IsGt(CheckStates<map<int32_t, char>, map<int32_t, char>>(Map_G , Map_E)));
  EXPECT_TRUE(IsGt(CheckStates<map<int32_t, char>, map<int32_t, char>>(Map_GL, Map_G)));

  EXPECT_TRUE(IsGt(CheckStates<map<int32_t, char>, vector<int32_t>>   (Map_G , Vector_G)));
  EXPECT_TRUE(IsLt(CheckStates<map<int32_t, char>, TPoint>            (Map_G , Point_L)));
}

FIXTURE(Record) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<TPoint>,      map<int32_t, char>>(Native::TFree<TPoint>::Free, Map_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<TPoint>,      TPoint>            (Native::TFree<TPoint>::Free, Point_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<TPoint>, TOpt<TPoint>>           (Native::TFree<TPoint>::Free, OptPoint)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <TPoint>,      TPoint>            (OptPoint, Point_L)));

  EXPECT_TRUE(IsLt(CheckStates<TPoint, TPoint>            (Point_L, Point_E)));
  EXPECT_TRUE(IsEq(CheckStates<TPoint, TPoint>            (Point_E, Point_E)));
  EXPECT_TRUE(IsGt(CheckStates<TPoint, TPoint>            (Point_G, Point_E)));

  EXPECT_TRUE(IsGt(CheckStates<TPoint, map<int32_t, char>>(Point_G, Map_G)));
  EXPECT_TRUE(IsLt(CheckStates<TPoint, tuple<int32_t>>    (Point_G, Tuple_L)));
}

FIXTURE(Tuple) {
  EXPECT_TRUE(IsGt(CheckStates<TFree<tuple<int32_t>>,      TPoint        > (Native::TFree<tuple<int32_t>>::Free, Point_G)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<tuple<int32_t>>,      tuple<int32_t>> (Native::TFree<tuple<int32_t>>::Free, Tuple_L)));
  EXPECT_TRUE(IsLt(CheckStates<TFree<tuple<int32_t>>, TOpt<tuple<int32_t>>>(Native::TFree<tuple<int32_t>>::Free, OptTuple)));
  EXPECT_TRUE(IsLt(CheckStates<TOpt <tuple<int32_t>>,      tuple<int32_t>> (OptTuple, Tuple_L)));

  EXPECT_TRUE(IsLt(CheckStates<tuple<int32_t>, tuple<int32_t>>(Tuple_L, Tuple_E)));
  EXPECT_TRUE(IsEq(CheckStates<tuple<int32_t>, tuple<int32_t>>(Tuple_E, Tuple_E)));
  EXPECT_TRUE(IsGt(CheckStates<tuple<int32_t>, tuple<int32_t>>(Tuple_G, Tuple_E)));

  EXPECT_TRUE(IsGt(CheckStates<tuple<int32_t>, TPoint>        (Tuple_G, Point_G)));
}