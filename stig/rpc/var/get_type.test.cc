/* <stig/rpc/var/get_type.test.cc> 

   Unit test for <stig/rpc/var/get_type.h>.

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

#include <stig/rpc/var/get_type.h>

#include <stig/rpc/all_vars.h>
#include <stig/rpc/all_types.h>
#include <stig/rpc/type/dump.h>
#include <test/kit.h>

using namespace std;
using namespace Stig::Rpc;

static void TestGetType(const TVar &var, const char *expected) {
  EXPECT_EQ(ToString(*GetType(var)), expected);
}

FIXTURE(Typical) {
  TestGetType(Var::TBool    (), "bool"     );
  TestGetType(Var::TId      (), "id"       );
  TestGetType(Var::TInt     (), "int"      );
  TestGetType(Var::TReal    (), "real"     );
  TestGetType(Var::TStr     (), "str"      );
  TestGetType(Var::TTimeDiff(), "time_diff");
  TestGetType(Var::TTimePnt (), "time_pnt" );
  // Poly1.
  TestGetType(Var::TList(TType::TPtr(new Type::TInt)), "list(int)");
  TestGetType(Var::TOpt (TType::TPtr(new Type::TInt)), "opt(int)");
  TestGetType(Var::TSet (TType::TPtr(new Type::TInt)), "set(int)");
  // Poly2.
  TestGetType(Var::TDict(TType::TPtr(new Type::TInt), TType::TPtr(new Type::TStr)), "dict(int, str)");
  /* Addr */ {
    Var::TAddr var;
    var.AddField(TDir::Asc,  TVar::TPtr(new Var::TId     ));
    var.AddField(TDir::Desc, TVar::TPtr(new Var::TTimePnt));
    TestGetType(var, "addr(asc id, desc time_pnt)");
  }
  /* Obj */ {
    Var::TObj var;
    var.AddField("x", TVar::TPtr(new Var::TReal));
    var.AddField("y", TVar::TPtr(new Var::TReal));
    TestGetType(var, "obj(x: real, y: real)");
  }
}

