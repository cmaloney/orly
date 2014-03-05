/* <stig/type/new_sabot.test.cc>

   Unit test for <stig/type/new_sabot.h>.

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

#include <stig/type/new_sabot.h>

#include <cassert>
#include <cstdlib>
#include <sstream>
#include <string>

#include <stig/sabot/type_dumper.h>
#include <stig/native/all.h>
#include <stig/type/type_czar.h>
#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Stig;

static Type::TTypeCzar TypeCzar;

static string ToString(const Type::TType &type) {
  ostringstream strm;
  NewSabot(alloca(1000), type)->Accept(Sabot::TTypeDumper(strm));
  return strm.str();
}

FIXTURE(Typical) {
  EXPECT_EQ(ToString(Type::TBool::Get()), "bool");
  EXPECT_EQ(ToString(Type::TId::Get()), "uuid");
  EXPECT_EQ(ToString(Type::TInt::Get()), "int64");
  EXPECT_EQ(ToString(Type::TReal::Get()), "double");
  EXPECT_EQ(ToString(Type::TStr::Get()), "str");
  EXPECT_EQ(ToString(Type::TTimeDiff::Get()), "duration");
  EXPECT_EQ(ToString(Type::TTimePnt::Get()), "time_point");
  EXPECT_EQ(ToString(Type::TList::Get(Type::TReal::Get())), "vector(double)");
  EXPECT_EQ(ToString(Type::TOpt::Get(Type::TReal::Get())), "opt(double)");
  EXPECT_EQ(ToString(Type::TSet::Get(Type::TReal::Get())), "set(double)");
  EXPECT_EQ(ToString(Type::TDict::Get(Type::TReal::Get(), Type::TStr::Get())), "map(double, str)");
  EXPECT_EQ(ToString(Type::TAddr::Get({ { TAddrDir::Asc, Type::TReal::Get() } })), "tuple(double)");
  EXPECT_EQ(ToString(Type::TAddr::Get({ { TAddrDir::Desc, Type::TReal::Get() } })), "tuple(desc(double))");
  EXPECT_EQ(ToString(Type::TObj::Get({ { "x", Type::TReal::Get() }, { "y", Type::TReal::Get() } })), "record(x: double, y: double)");
}
