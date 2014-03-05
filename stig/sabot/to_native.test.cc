/* <stig/sabot/to_native.test.cc>

   Unit test for <stig/sabot/to_native.h>.

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

#include <stig/sabot/to_native.h>

#include <sstream>
#include <string>

#include <stig/native/all.h>
#include <stig/native/point.h>
#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig;

FIXTURE(Int8) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  int8_t expected = -5;
  int8_t out;
  ToNative(*Sabot::State::TAny::TWrapper(Native::State::New(expected, state_alloc)), out);
  EXPECT_EQ(expected, out);
}

FIXTURE(Int16) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  int8_t expected = -7;
  int8_t out;
  ToNative(*Sabot::State::TAny::TWrapper(Native::State::New(expected, state_alloc)), out);
  EXPECT_EQ(expected, out);
}

FIXTURE(Int32) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  int32_t expected = -9;
  int32_t out;
  ToNative(*Sabot::State::TAny::TWrapper(Native::State::New(expected, state_alloc)), out);
  EXPECT_EQ(expected, out);
}

FIXTURE(Int64) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  int64_t expected = -101;
  int64_t out;
  ToNative(*Sabot::State::TAny::TWrapper(Native::State::New(expected, state_alloc)), out);
  EXPECT_EQ(expected, out);
}

FIXTURE(UInt8) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  uint8_t expected = 5;
  uint8_t out;
  ToNative(*Sabot::State::TAny::TWrapper(Native::State::New(expected, state_alloc)), out);
  EXPECT_EQ(expected, out);
}

FIXTURE(UInt16) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  uint8_t expected = 7;
  uint8_t out;
  ToNative(*Sabot::State::TAny::TWrapper(Native::State::New(expected, state_alloc)), out);
  EXPECT_EQ(expected, out);
}

FIXTURE(UInt32) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  uint32_t expected = 9;
  uint32_t out;
  ToNative(*Sabot::State::TAny::TWrapper(Native::State::New(expected, state_alloc)), out);
  EXPECT_EQ(expected, out);
}

FIXTURE(UInt64) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  uint64_t expected = 101;
  uint64_t out;
  ToNative(*Sabot::State::TAny::TWrapper(Native::State::New(expected, state_alloc)), out);
  EXPECT_EQ(expected, out);
}

FIXTURE(Bool) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  bool expected = true;
  bool out;
  ToNative(*Sabot::State::TAny::TWrapper(Native::State::New(expected, state_alloc)), out);
  EXPECT_EQ(expected, out);
}

FIXTURE(Tuple) {
  typedef std::tuple<int64_t, uint8_t, bool> tuple_type;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  tuple_type expected(-101, 7, true);
  tuple_type out;
  ToNative(*Sabot::State::TAny::TWrapper(Native::State::New(expected, state_alloc)), out);
  EXPECT_TRUE(expected == out);
}

FIXTURE(Record) {
  const TPoint expected(1.5, 2.5);
  TPoint out;
  EXPECT_NE(out.GetX(), expected.GetX());
  EXPECT_NE(out.GetY(), expected.GetY());
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  ToNative(*Sabot::State::TAny::TWrapper(Native::State::New(expected, state_alloc)), out);
  EXPECT_EQ(out.GetX(), expected.GetX());
  EXPECT_EQ(out.GetY(), expected.GetY());
}
