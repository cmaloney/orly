/* <stig/sabot/get_hash.test.cc>

   Unit test for <stig/sabot/get_hash.h>.

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

#include <stig/sabot/get_hash.h>

#include <sstream>
#include <string>

#include <stig/native/all.h>
#include <stig/native/point.h>
#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig;

/* SCALAR */
static const int8_t Int8Val = -8;
static const int16_t Int16Val = -16;
static const int32_t Int32Val = -32;
static const int64_t Int64Val = -64;
static const uint8_t UInt8Val = 8U;
static const uint16_t UInt16Val = 16U;
static const uint32_t UInt32Val = 32U;
static const uint64_t UInt64Val = 64U;
static const bool BoolVal = true;
static const char CharVal = 'C';
static const float FloatVal = 0.123;
static const double DoubleVal = 3.14;
static const Sabot::TStdDuration DurationVal(7);
static const Sabot::TStdTimePoint TimePointVal(DurationVal);
static const TUuid UuidVal("1b4e28ba-2fa1-11d2-883f-b9a761bde3fb");

/* Array of Scalar */
static const string StringVal("Hello World");
static const Native::TBlob BlobVal{'A', '1', '3', 'F'};

/* Arrays of single states. */
static const TDesc<int32_t> DescVal(BoolVal);
static const TOpt<float> OptVal(FloatVal);
static const set<int32_t> SetVal{8, 7, 6, 5, 4, 3, 2, 1};
static const vector<int64_t> VectorVal{8, 7, 6, 5, 4, 3, 2, 1};

/* Arrays of pair states. */
static const map<bool, char> MapVal{{true, 'A'}, {false, 'B'}};

/* array of static single states. */
static const TPoint PointVal(3.14, 7.89);
static const tuple<bool, int32_t> TupleVal(true, 72);

template <typename TVal>
bool CheckHash(const TVal &val) {
  size_t hash_val;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Sabot::State::TAny::TWrapper(Native::State::New<TVal>(val, state_alloc))->Accept( Sabot::THashVisitor(hash_val) );
  return hash_val == std::hash<TVal>()(val);
}

template <typename TVal>
size_t GetHash(const TVal &val) {
  size_t hash_val;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Sabot::State::TAny::TWrapper(Native::State::New<TVal>(val, state_alloc))->Accept( Sabot::THashVisitor(hash_val) );
  return hash_val;
}

FIXTURE(Empty) {
  EXPECT_EQ(GetHash<Native::TFree<bool>>(Native::TFree<bool>::Free), 0UL);
  EXPECT_EQ(GetHash<Native::TTombstone>(Native::TTombstone::Tombstone), 0UL);
  // Note: There is no native void state.
}

FIXTURE(Scalar) {
  EXPECT_EQ(GetHash<int8_t>(Int8Val), 11817417496758063002UL);
  EXPECT_EQ(GetHash<int16_t>(Int16Val), 13719135514804538670UL);
  EXPECT_EQ(GetHash<int32_t>(Int32Val), 3203370286708038931UL);
  EXPECT_EQ(GetHash<int64_t>(Int64Val), 12952019401161042793UL);
  EXPECT_EQ(GetHash<uint8_t>(UInt8Val), 2258353126044249582UL);
  EXPECT_EQ(GetHash<uint16_t>(UInt16Val), 1682059383145895688UL);
  EXPECT_EQ(GetHash<uint32_t>(UInt32Val), 17522780044756955965UL);
  EXPECT_EQ(GetHash<uint64_t>(UInt64Val), 7978597833130992815UL);
  EXPECT_TRUE(CheckHash<bool>(BoolVal));
  EXPECT_TRUE(CheckHash<char>(CharVal));
  EXPECT_TRUE(CheckHash<float>(FloatVal));
  EXPECT_TRUE(CheckHash<double>(DoubleVal));
  EXPECT_EQ  (GetHash  <Sabot::TStdDuration>(DurationVal), std::hash<Sabot::TStdDuration::rep>()(DurationVal.count()));
  EXPECT_EQ  (GetHash  <Sabot::TStdTimePoint>(TimePointVal), std::hash<Sabot::TStdDuration::rep>()(TimePointVal.time_since_epoch().count()));
  EXPECT_TRUE(CheckHash<TUuid>(UuidVal));
}

FIXTURE(ArrayOfScalar) {
  EXPECT_TRUE(CheckHash<string>(StringVal));
  EXPECT_EQ  (GetHash  <Native::TBlob>(BlobVal), 2193830995326974444UL);
}

FIXTURE(ArrayOfSingleStates) {
  size_t expected_desc_hash = GetHash(*DescVal);
  size_t expected_opt_hash = GetHash(*OptVal);
  size_t expected_set_hash = 0U;
  size_t pos = 0UL;
  for (const auto &iter : SetVal) {
    expected_set_hash ^= Base::RotatedRight(GetHash(iter), pos * 5);
    ++pos;
  }
  size_t expected_vec_hash = 0U;
  pos = 0UL;
  for (const auto &iter : VectorVal) {
    expected_vec_hash ^= Base::RotatedRight(GetHash(iter), pos * 5);
    ++pos;
  }
  EXPECT_EQ(GetHash<TDesc<int32_t>>(DescVal), expected_desc_hash);
  EXPECT_EQ(GetHash<Base::TOpt<float>>(OptVal), expected_opt_hash);
  EXPECT_EQ(GetHash<std::set<int32_t>>(SetVal), expected_set_hash);
  EXPECT_EQ(GetHash<std::vector<int64_t>>(VectorVal), expected_vec_hash);
}

FIXTURE(ArrayOfPairStates) {
  size_t expected_map_hash = 0U;
  size_t pos = 0U;
  for (const auto &iter : MapVal) {
    expected_map_hash ^= Base::RotatedRight(GetHash(iter.first), pos * 5);
    expected_map_hash ^= Base::RotatedRight(GetHash(iter.second), pos * 7);
    ++pos;
  }
  EXPECT_EQ((GetHash<map<bool, char>>(MapVal)), expected_map_hash);
}

FIXTURE(ArrayOfStaticStates) {
  size_t expected_record_hash = 0U;
  expected_record_hash = GetHash(PointVal.GetX());
  expected_record_hash ^= Base::RotatedRight(GetHash(PointVal.GetY()), 5);
  EXPECT_EQ((GetHash<TPoint>(PointVal)), expected_record_hash);
  size_t expected_tuple_hash = 0U;
  expected_tuple_hash = GetHash(get<0>(TupleVal));
  expected_tuple_hash ^= Base::RotatedRight(GetHash(get<1>(TupleVal)), 5);
  EXPECT_EQ((GetHash<tuple<bool, int32_t>>(TupleVal)), expected_tuple_hash);
}

FIXTURE(CheckFreeTuple) {
  const tuple<bool, int32_t> prefix_tuple(true, 72);
  const tuple<bool, int32_t, Native::TFree<int32_t>> free_tuple(true, 72, Native::TFree<int32_t>());
  EXPECT_EQ((GetHash<tuple<bool, int32_t>>(TupleVal)), (GetHash<tuple<bool, int32_t, Native::TFree<int32_t>>>(free_tuple)));
}