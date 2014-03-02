/* <stig/rpc/type/dump.test.cc> 

   Unit test for <stig/rpc/type.h>.

   Copyright 2010-2014 Tagged
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   
     http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stig/rpc/type/dump.h>

#include <stig/rpc/all_types.h>
#include <test/kit.h>

using namespace std;
using namespace Stig::Rpc;

static void TestDump(const TType &type, const char *expected) {
  EXPECT_EQ(ToString(type), expected);
}

FIXTURE(Typical) {
  // Mono.
  TestDump(Type::TBool    (), "bool"     );
  TestDump(Type::TId      (), "id"       );
  TestDump(Type::TInt     (), "int"      );
  TestDump(Type::TReal    (), "real"     );
  TestDump(Type::TStr     (), "str"      );
  TestDump(Type::TTimePnt (), "time_pnt" );
  TestDump(Type::TTimeDiff(), "time_diff");
  // Poly1.
  TestDump(Type::TList(TType::TPtr(new Type::TInt)), "list(int)");
  TestDump(Type::TOpt (TType::TPtr(new Type::TInt)), "opt(int)" );
  TestDump(Type::TSet (TType::TPtr(new Type::TInt)), "set(int)" );
  // Poly2.
  TestDump(Type::TDict(TType::TPtr(new Type::TInt), TType::TPtr(new Type::TStr)), "dict(int, str)" );
  /* Addr */ {
    Type::TAddr type;
    TestDump(type, "addr()");
    type.AddField(TDir::Asc, TType::TPtr(new Type::TInt));
    TestDump(type, "addr(asc int)");
    type.AddField(TDir::Desc, TType::TPtr(new Type::TStr));
    TestDump(type, "addr(asc int, desc str)");
    type.AddField(TDir::Asc, TType::TPtr(new Type::TTimePnt));
    TestDump(type, "addr(asc int, desc str, asc time_pnt)");
  }
  /* Obj */ {
    Type::TObj type;
    TestDump(type, "obj()");
    type.AddField("x", TType::TPtr(new Type::TReal));
    TestDump(type, "obj(x: real)");
    type.AddField("y", TType::TPtr(new Type::TReal));
    TestDump(type, "obj(x: real, y: real)");
    type.AddField("label", TType::TPtr(new Type::TStr));
    TestDump(type, "obj(label: str, x: real, y: real)");
  }
}

