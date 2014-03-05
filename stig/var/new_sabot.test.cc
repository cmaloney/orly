/* <stig/var/new_sabot.test.cc>

   Unit test for <stig/var/new_sabot.h>.

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

#include <stig/var/new_sabot.h>

#include <sstream>
#include <string>

#include <stig/sabot/state_dumper.h>
#include <stig/type/type_czar.h>
#include <test/kit.h>

using namespace std;
using namespace Stig;

static Type::TTypeCzar TypeCzar;

static string ToString(const Var::TVar &var) {
  ostringstream strm;
  auto buf = alloca(1000);
  Sabot::State::TAny::TWrapper(NewSabot(buf, var))->Accept(Sabot::TStateDumper(strm));
  return strm.str();
}

FIXTURE(Bool) {
  EXPECT_EQ(ToString(Var::TVar(true)), "true");
  EXPECT_EQ(ToString(Var::TVar(false)), "false");
}

FIXTURE(Id) {
  const char *str = "1b4e28ba-2fa1-11d2-883f-b9a761bde3fb";
  EXPECT_EQ(ToString(Var::TVar(TUUID(str))), str);
}

FIXTURE(Int) {
  EXPECT_EQ(ToString(Var::TVar(0)), "0");
  EXPECT_EQ(ToString(Var::TVar(101)), "101");
  EXPECT_EQ(ToString(Var::TVar(-101)), "-101");
}

FIXTURE(Real) {
  EXPECT_EQ(ToString(Var::TVar(0.0)), "0");
  EXPECT_EQ(ToString(Var::TVar(98.6)), "98.6");
  EXPECT_EQ(ToString(Var::TVar(-98.6)), "-98.6");
}

FIXTURE(TimeDiff) {
  EXPECT_EQ(ToString(Var::TVar(Base::Chrono::TTimeDiff(0))), "0ms");
  EXPECT_EQ(ToString(Var::TVar(Base::Chrono::TTimeDiff(1))), "1ms");
}

FIXTURE(TTimePnt) {
  EXPECT_EQ(ToString(Var::TVar(Base::Chrono::TTimePnt())), "1970:01:01:00:00:00utc");
}

FIXTURE(Str) {
  EXPECT_EQ(ToString(Var::TVar(string("Mofo the Psychic Gorilla died for your sins."))), "\"Mofo the Psychic Gorilla died for your sins.\"");
}

FIXTURE(List) {
  EXPECT_EQ(ToString(Var::TVar(vector<bool>())), "empty vector(bool)");
  EXPECT_EQ(ToString(Var::TVar(vector<int64_t>({ 101, 102, 103 }))), "vector(101, 102, 103)");
}

FIXTURE(Set) {
  EXPECT_EQ(ToString(Var::TVar(Rt::TSet<bool>())), "empty set(bool)");
  EXPECT_EQ(ToString(Var::TVar(Rt::TSet<int64_t>({ 102, 103, 101 }))), "set(101, 102, 103)");
}

FIXTURE(Opt) {
  EXPECT_EQ(ToString(Var::TVar(Rt::TOpt<bool>())), "unknown opt(bool)");
  EXPECT_EQ(ToString(Var::TVar(Rt::TOpt<int64_t>(101))), "opt(101)");
}

FIXTURE(Dict) {
  EXPECT_EQ(ToString(Var::TVar(Rt::TDict<int64_t, string>())), "empty map(int64, str)");
  EXPECT_EQ(ToString(Var::TVar(Rt::TDict<int64_t, string>({ { 102, "doctor" }, { 103, "name" }, { 101, "hello" } }))), "map(101: \"hello\", 102: \"doctor\", 103: \"name\")");
}

#if 0 // TODO
FIXTURE(Addr) {
  std::tuple<> empty_addr;
  std::tuple<bool, int64_t, double> non_empty_addr(true, 101, 98.6);
  EXPECT_EQ(ToString(Var::TVar(empty_addr)), "tuple()");
  EXPECT_EQ(ToString(Var::TVar(non_empty_addr)), "tuple(true, 101, 98.6)");
}
#endif

FIXTURE(Obj) {
  auto foo = Var::TVar::Obj(std::unordered_map<std::string, Var::TVar>{ { "x", Var::TVar(1.5)}, { "y", Var::TVar(2.5) } });
  EXPECT_EQ(ToString(foo), "record(x: 1.5, y: 2.5)");
}

FIXTURE(Free) {
  EXPECT_EQ(ToString(Var::TVar(Var::TVar::Addr({{Stig::TAddrDir::Asc, Var::TVar(Var::TVar::Free(Stig::Type::TInt::Get()))}}))), "tuple(free(int64))");
}

FIXTURE(FreeDesc) {
  EXPECT_EQ(ToString(Var::TVar(Var::TVar::Addr({{Stig::TAddrDir::Desc, Var::TVar(Var::TVar::Free(Stig::Type::TInt::Get()))}}))), "tuple(free(desc(int64)))");
}

#if 0 // TODO
FIXTURE(AddrWithDesc) {
  std::tuple<TDesc<bool>, int64_t, double> addr1(true, 101, 98.6);
  EXPECT_EQ(ToString(Var::TVar(addr1)), "tuple(desc(true), 101, 98.6)");
  std::tuple<bool, TDesc<int64_t>, double> addr2(true, 101, 98.6);
  EXPECT_EQ(ToString(Var::TVar(addr2)), "tuple(true, desc(101), 98.6)");
  std::tuple<bool, int64_t, TDesc<double>> addr3(true, 101, 98.6);
  EXPECT_EQ(ToString(Var::TVar(addr3)), "tuple(true, 101, desc(98.6))");
}

#endif