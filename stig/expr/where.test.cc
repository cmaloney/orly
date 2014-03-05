/* <stig/expr/where.test.cc>

   Unit test for <stig/expr/where.h>

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

#include <stig/expr/test_kit.h>

FIXTURE(Where) {
  /* package */
  TPackage::TPtr package = TPackage::New({"foo"}, 1);

  /*
     foo = (bar) where {
       bar = 42;
     };
  */
  TFunction::TPtr foo = TFunction::New(package, "foo", TPosRange());
  TResultDef::TPtr foo_result = TResultDef::New(foo, "foo", TPosRange());
  EXPECT_TRUE(foo_result);
  // Build scope
  TWhere::TPtr where = TWhere::New(TPosRange());
  // function: bar
  TFunction::TPtr bar = TFunction::New(where, "bar", TPosRange());
  TResultDef::TPtr bar_result = TResultDef::New(bar, "bar", TPosRange());
  // Set bar's expr
  bar->SetExpr(TLiteral::New(Var::TVar(42), TPosRange()));
  // TRef to bar's result def.
  TRef::TPtr bar_ref = TRef::New(bar_result, TPosRange());
  // Set where's expr
  where->SetExpr(bar_ref);
  // Set foo's expr
  foo->SetExpr(where);
  EXPECT_EQ(where->GetExpr(), bar_ref);
  EXPECT_EQ(where->GetType(), Type::TInt::Get());
}
