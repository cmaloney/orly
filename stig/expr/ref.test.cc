/* <stig/expr/ref.test.cc>

   Unit test for <stig/expr/ref.h>

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

FIXTURE(Ref) {
  /* package #1; */
  TPackage::TPtr package = TPackage::New({"foo"}, 1);
  /*
     foo = (x) where {
       x = given::(int);
     };
  */
  // function: foo
  TFunction::TPtr foo = TFunction::New(package, "foo", TPosRange());
  TResultDef::TPtr foo_result = TResultDef::New(foo, "foo", TPosRange());
  EXPECT_TRUE(foo_result);
  // Build scope
  TWhere::TPtr where = TWhere::New(TPosRange());
  // function: x
  TFunction::TPtr x = TFunction::New(where, "x", TPosRange());
  TResultDef::TPtr x_result = TResultDef::New(x, "x", TPosRange());
  // pass 3: BuildSecondarySymbol()
  TGivenParamDef::TPtr x_def = TGivenParamDef::New(foo, "x", int_type, TPosRange());
  // pass 4: GetExpr()->Build()
  // func_expr: given::(int)
  TRef::TPtr x_ref = TRef::New(x_def, TPosRange());
  x->SetExpr(x_ref);
  // where_expr: x
  where->SetExpr(TRef::New(x_result, TPosRange()));
  foo->SetExpr(where);
  EXPECT_EQ(x_ref->GetDef(), x_def.get());
  EXPECT_EQ(x_ref->GetExprParent(), x.get());
  EXPECT_EQ(x_ref->GetType(), int_type);
}
