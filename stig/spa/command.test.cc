/* <stig/spa/command.test.cc>

   Unit test for <stig/spa/command.h>.

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

#include <stig/spa/command.h>

#include <stig/type/int.h>
#include <stig/type/real.h>
#include <stig/type/type_czar.h>

#include <test/kit.h>

using namespace std;
using namespace Stig;
using namespace Stig::Spa;
using namespace Stig::Var;

Type::TTypeCzar TypeCzar;

FIXTURE(Int) {
  TVar var;
  EXPECT_TRUE(ParseCommand("5", var));
  EXPECT_TRUE(var == Var::TVar(5));
}

FIXTURE(Real) {
  TVar var;
  EXPECT_TRUE(ParseCommand("2.2", var));
  EXPECT_TRUE(var == Var::TVar(2.2));
}

/* TODO
FIXTURE(Opt) {
  TVar var;
  EXPECT_TRUE(ParseCommand("2.2?", var));
  EXPECT_TRUE(Rt::Match(var, Var::TVar::Opt(Rt::TOpt<double>(2.2))));

  EXPECT_TRUE(ParseCommand("unknown int", var));
  EXPECT_TRUE(Rt::Match(var, Var::TVar::Opt(Rt::TOpt<long int>())));
} */

FIXTURE(Str) {
  TVar var;
  EXPECT_FALSE(ParseCommand("'a\nb'", var));

  EXPECT_TRUE(ParseCommand("\"Hello World\"", var));
  EXPECT_TRUE(var == Var::TVar(string("Hello World")));

  EXPECT_TRUE(ParseCommand("\"Hello\\nWorld2\"", var));
  EXPECT_TRUE(var == Var::TVar(string("Hello\nWorld2")));
}

FIXTURE(Bool) {
  TVar var;
  EXPECT_TRUE(ParseCommand("true", var));
  EXPECT_TRUE(var == Var::TVar(true));

  EXPECT_TRUE(ParseCommand("false", var));
  EXPECT_TRUE(var == Var::TVar(false));
}

FIXTURE(List) {
  TVar var;
  EXPECT_TRUE(ParseCommand("[1, 2, 3]", var));
  EXPECT_TRUE(var == Var::TVar::List(vector<Var::TVar>{Var::TVar(1), Var::TVar(2), Var::TVar(3)}, Type::TInt::Get()));
}

FIXTURE(Set) {
  TVar var;
  EXPECT_TRUE(ParseCommand("{1, 2, 3}", var));
  EXPECT_TRUE(var == Var::TVar::Set(Rt::TSet<Var::TVar>{Var::TVar(1), Var::TVar(2), Var::TVar(3)}, Type::TInt::Get()));
}

FIXTURE(Dict) {
  TVar var;
  EXPECT_TRUE(ParseCommand("{1:2.2, 2:3.3, 3:4.4}", var));
  EXPECT_TRUE(var == Var::TVar::Dict(Rt::TDict<Var::TVar, Var::TVar>{{Var::TVar(1), Var::TVar(2.2)}, {Var::TVar(2), Var::TVar(3.3)}, {Var::TVar(3), Var::TVar(4.4)}}, Type::TInt::Get(), Type::TReal::Get()));
}

FIXTURE(UUID) {
  TVar var;
  EXPECT_TRUE(ParseCommand("{00000000-0000-0000-0000-000000000000}", var));
  EXPECT_TRUE(var == Var::TVar(TUUID("00000000-0000-0000-0000-000000000000")));
}

#if 0 // Addr
FIXTURE(Address) {
  TVar var;
  EXPECT_TRUE(ParseCommand("<[1, desc 2, \"Hello World\"]>", var));
  EXPECT_TRUE(var == Var::TVar(Rt::TAddr<Rt::TAddrElem<TAddrDir::Asc, int64_t>, Rt::TAddrElem<TAddrDir::Desc, int64_t>, Rt::TAddrElem<TAddrDir::Asc, string>>(1, 2, "Hello World")));
  EXPECT_TRUE(ParseCommand("<[]>", var));
  EXPECT_TRUE(var == Var::TVar::Addr(vector<std::pair<TAddrDir, Var::TVar>>{}));
}
#endif

FIXTURE(Object) {
  TVar var;
  EXPECT_TRUE(ParseCommand("<{.a:1, .b:2.2}>", var));
  EXPECT_TRUE(var == Var::TVar::Obj(unordered_map<string, Var::TVar>{{"a", Var::TVar(1)}, {"b", Var::TVar(2.2)}}));
  EXPECT_TRUE(ParseCommand("<{}>", var));
  EXPECT_TRUE(var == Var::TVar::Obj(unordered_map<string, Var::TVar>{}));
}