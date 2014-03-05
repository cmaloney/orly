/* <stig/sabot/get_depth.test.cc>

   Unit test for <stig/sabot/get_depth.h>.

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

#include <stig/sabot/get_depth.h>

#include <sstream>
#include <string>

#include <stig/native/all.h>
#include <stig/native/point.h>
#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig;

FIXTURE(Nullary) {
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<int8_t>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<int16_t>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<int32_t>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<int64_t>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<uint8_t>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<uint16_t>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<uint32_t>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<uint64_t>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<bool>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<char>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<float>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<double>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<Sabot::TStdDuration>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<Sabot::TStdTimePoint>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<Base::TUuid>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<Native::TBlob>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<std::string>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<Native::TTombstone>::GetType(type_alloc))), 0UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<void>::GetType(type_alloc))), 0UL);
}

FIXTURE(Unary) {
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<TDesc<bool>>::GetType(type_alloc))), 1UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<Native::TFree<bool>>::GetType(type_alloc))), 1UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<TOpt<bool>>::GetType(type_alloc))), 1UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<set<bool>>::GetType(type_alloc))), 1UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<vector<bool>>::GetType(type_alloc))), 1UL);
}

FIXTURE(Binary) {
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<map<bool, char>>::GetType(type_alloc))), 1UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<map<set<bool>, char>>::GetType(type_alloc))), 2UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<map<bool, set<char>>>::GetType(type_alloc))), 2UL);
}

class TObjL {
  private:

  double X;
  set<bool> Y;

};

RECORD_ELEM(TObjL, double, X);
RECORD_ELEM(TObjL, set<bool>, Y);

class TObjG {
  private:

  set<bool> X;
  double Y;

};

RECORD_ELEM(TObjG, set<bool>, X);
RECORD_ELEM(TObjG, double, Y);

FIXTURE(Record) {
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<TPoint>::GetType(type_alloc))), 1UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<TObjL>::GetType(type_alloc))), 2UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<TObjG>::GetType(type_alloc))), 2UL);
}

FIXTURE(Tuple) {
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<tuple<bool>>::GetType(type_alloc))), 1UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<tuple<bool, int64_t>>::GetType(type_alloc))), 1UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<tuple<bool, set<bool>>>::GetType(type_alloc))), 2UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<tuple<bool, set<TOpt<bool>>>>::GetType(type_alloc))), 3UL);
  EXPECT_EQ(Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(Native::Type::For<tuple<set<TOpt<bool>>, bool>>::GetType(type_alloc))), 3UL);
}