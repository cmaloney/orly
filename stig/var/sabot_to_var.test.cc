/* <stig/var/sabot_to_var.test.cc>

   Unit test for <stig/var/sabot_to_var.h>.

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

#include <stig/var/sabot_to_var.h>

#include <sstream>
#include <string>

#include <stig/native/all.h>
#include <stig/native/point.h>
#include <stig/sabot/state_dumper.h>
#include <stig/type/type_czar.h>
#include <stig/var/new_sabot.h>
#include <test/kit.h>

using namespace std;
using namespace Stig;
using namespace Stig::Var;

static Type::TTypeCzar TypeCzar;

template <typename TVal>
void Check(const TVal &val) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  EXPECT_EQ(ToVar(*Sabot::State::TAny::TWrapper(Native::State::New(val, state_alloc))), Var::TVar(val));
}

template <typename TVal>
void Check(const TVal &val, const Var::TVar &expected) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  EXPECT_EQ(ToVar(*Sabot::State::TAny::TWrapper(Native::State::New(val, state_alloc))), expected);
}

FIXTURE(Int) {
  Check(101L);
}

#if 0 // Addr
FIXTURE(Addr) {
  Rt::TAddr<Rt::TAddrElem<TAddrDir::Asc, bool>, Rt::TAddrElem<TAddrDir::Asc, int64_t>, Rt::TAddrElem<TAddrDir::Asc, double>> non_empty_addr(true, 101, 98.6);
  Rt::TAddr<Rt::TAddrElem<TAddrDir::Asc, bool>, Rt::TAddrElem<TAddrDir::Asc, int64_t>, Rt::TAddrElem<TAddrDir::Desc, double>> desc_addr(true, 101, 98.6);
  Check(std::tuple<bool, int64_t, double>(true, 101, 98.6), Var::TVar(non_empty_addr));
  Check(std::tuple<bool, int64_t, TDesc<double>>(true, 101, TDesc<double>(98.6)), Var::TVar(desc_addr));
}
#endif

FIXTURE(Bool) {
  Check(true);
  Check(false);
}

FIXTURE(Dict) {
  Check(std::map<int64_t, double>{{1, 1.1}, {2, 2.2}, {3, 3.3}}, Rt::TDict<int64_t, double>{{1, 1.1}, {2, 2.2}, {3, 3.3}});
}

FIXTURE(Id) {
  Check(Base::TUuid("1b4e28ba-2fa1-11d2-883f-b9a761bde3fb"), Var::TVar(TUUID("1b4e28ba-2fa1-11d2-883f-b9a761bde3fb")));
}

FIXTURE(List) {
  Check(std::vector<int64_t>{7L, 3L, 5L, 4L});
}

FIXTURE(Obj) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  EXPECT_TRUE(ToVar(*Sabot::State::TAny::TWrapper(Native::State::New(TPoint(3.14, 5.78), state_alloc))) == Var::TVar::Obj(std::unordered_map<std::string, TVar>{{"X", Var::TVar(3.14)}, {"Y", Var::TVar(5.78)}}));
}

FIXTURE(Opt) {
  Check(Base::TOpt<int64_t>(7L), Var::TVar(Rt::TOpt<int64_t>(7L)));
  Check(Base::TOpt<int64_t>(), Var::TVar(Rt::TOpt<int64_t>()));
}

FIXTURE(Real) {
  Check(3.14);
}

FIXTURE(Set) {
  Check(std::set<int64_t>{7L, 3L, 5L, 4L}, Rt::TSet<int64_t>{7L, 3L, 5L, 4L});
}

FIXTURE(Str) {
  Check(string("Hello World"));
}

FIXTURE(TimeDiff) {
  Check(Sabot::TStdDuration(0), Var::TVar(Base::Chrono::TTimeDiff(0)));
  Check(Sabot::TStdDuration(1000), Var::TVar(Base::Chrono::TTimeDiff(1000)));
}

FIXTURE(TimePoint) {
  Check(Sabot::TStdTimePoint(Sabot::TStdDuration(1000)), Var::TVar(Base::Chrono::TTimePnt(Base::Chrono::TTimeDiff(1000))));
  Check(Sabot::TStdTimePoint(Sabot::TStdDuration(7000)), Var::TVar(Base::Chrono::TTimePnt(Base::Chrono::TTimeDiff(7000))));
}

class TClosure {
  public:

  using TVarByName = map<string, Var::TVar>;

  TClosure() {}

  TClosure(const string &method_name, const map<string, Var::TVar> &var_by_name)
      : MethodName(method_name), VarByName(var_by_name) {}

  bool operator==(const TClosure &that) const {
    return MethodName == that.MethodName && VarByName == that.VarByName;
  }

  private:

  string MethodName;

  map<string, Var::TVar> VarByName;

};

RECORD_ELEM(TClosure, string, MethodName);
RECORD_ELEM(TClosure, TClosure::TVarByName, VarByName);

FIXTURE(Record) {
  const TClosure expected("sum", { { "a", Var::TVar(101L) }, { "b", Var::TVar(202L) } });
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Sabot::State::TAny::TWrapper state(Native::State::New(expected, state_alloc));
  TClosure actual;
  Sabot::ToNative(*state, actual);
  EXPECT_TRUE(actual == expected);
}
