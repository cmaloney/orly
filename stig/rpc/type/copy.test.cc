/* <stig/rpc/type/copy.test.cc> 

   Unit test for <stig/rpc/type/copy.h>.

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

#include <stig/rpc/type/copy.h>

#include <stig/rpc/all_types.h>
#include <stig/rpc/type/dump.h>
#include <test/kit.h>

using namespace std;
using namespace Stig::Rpc;

static void TestCopy(const TType &type) {
  auto copy = Copy(type);
  EXPECT_EQ(ToString(type), ToString(*Copy(type)));
}

FIXTURE(Typical) {
  // Mono.
  TestCopy(Type::TBool    ());
  TestCopy(Type::TId      ());
  TestCopy(Type::TInt     ());
  TestCopy(Type::TReal    ());
  TestCopy(Type::TStr     ());
  TestCopy(Type::TTimeDiff());
  TestCopy(Type::TTimePnt ());
  // Poly1.
  TestCopy(Type::TList(TType::TPtr(new Type::TInt)));
  TestCopy(Type::TOpt (TType::TPtr(new Type::TInt)));
  TestCopy(Type::TSet (TType::TPtr(new Type::TInt)));
  // Poly2.
  TestCopy(Type::TDict(TType::TPtr(new Type::TInt), TType::TPtr(new Type::TStr)));
  /* Addr */ {
    Type::TAddr type;
    type.AddField(TDir::Asc,  TType::TPtr(new Type::TId     ));
    type.AddField(TDir::Desc, TType::TPtr(new Type::TTimePnt));
    TestCopy(type);
  }
  /* Obj */ {
    Type::TObj type;
    type.AddField("x", TType::TPtr(new Type::TReal));
    type.AddField("y", TType::TPtr(new Type::TReal));
    TestCopy(type);
  }
}

