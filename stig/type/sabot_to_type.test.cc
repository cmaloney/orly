/* <stig/type/sabot_to_type.test.cc>

   Unit test for <stig/type/sabot_to_type.h>.

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

#include <stig/type/sabot_to_type.h>

#include <sstream>
#include <string>

#include <stig/native/all.h>
#include <stig/native/point.h>
#include <stig/sabot/state_dumper.h>
#include <stig/type/type_czar.h>
#include <test/kit.h>

using namespace std;
using namespace Stig;
using namespace Stig::Type;

static Type::TTypeCzar TypeCzar;

template <typename TVal>
void Check(const Type::TType &expected) {
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  EXPECT_EQ(ToType(*Sabot::Type::TAny::TWrapper(Native::Type::For<TVal>::GetType(type_alloc))), expected);
}

FIXTURE(Int) {
  Check<int64_t>(TInt::Get());
}

FIXTURE(Addr) {
  Check<tuple<double, int64_t>>(Type::TAddr::Get({ { TAddrDir::Asc, Type::TReal::Get() }, { TAddrDir::Asc, Type::TInt::Get() } }));
  Check<tuple<TDesc<double>, int64_t>>(Type::TAddr::Get({ { TAddrDir::Desc, Type::TReal::Get() }, { TAddrDir::Asc, Type::TInt::Get() } }));
}

FIXTURE(Bool) {
  Check<bool>(TBool::Get());
}

FIXTURE(Dict) {
  Check<std::map<int64_t, double>>(TDict::Get(TInt::Get(), TReal::Get()));
}

FIXTURE(Id) {
  Check<Base::TUuid>(TId::Get());
}

FIXTURE(List) {
  Check<std::vector<int64_t>>(TList::Get(TInt::Get()));
}

FIXTURE(Obj) {
  Check<TPoint>(Type::TObj::Get({ { "X", Type::TReal::Get() }, { "Y", Type::TReal::Get() } }));
}

FIXTURE(Opt) {
  Check<Base::TOpt<int64_t>>(TOpt::Get(TInt::Get()));
}

FIXTURE(Real) {
  Check<double>(TReal::Get());
}

FIXTURE(Set) {
  Check<std::set<int64_t>>(TSet::Get(TInt::Get()));
}

FIXTURE(Str) {
  Check<std::string>(TStr::Get());
}

FIXTURE(TimeDiff) {
  Check<Sabot::TStdDuration>(TTimeDiff::Get());
}

FIXTURE(TimePoint) {
  Check<Sabot::TStdTimePoint>(TTimePnt::Get());
}
