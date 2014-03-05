/* <stig/expr/function_app.test.cc>

   Unit test for <stig/expr/function_app.h>.

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

FIXTURE(Typical) {
  /* package scope */
  TPackage::TPtr package = TPackage::New({"foo"}, 1);
  /*
     foo = (x) where {
       x = given::(real);
     }
  */
  // function: foo
  TFunction::TPtr foo = TFunction::New(package, "foo", TPosRange());
  TResultDef::TPtr foo_result = TResultDef::New(foo, "foo", TPosRange());
  // build scope
  TWhere::TPtr where = TWhere::New(TPosRange());
  // function: x
  TFunction::TPtr x = TFunction::New(package, "x", TPosRange());
  TResultDef::TPtr x_result = TResultDef::New(x, "x", TPosRange());
  // pass 3: BuildSecondarySymbol()
  TGivenParamDef::TPtr x_def = TGivenParamDef::New(foo, "x", real_type, TPosRange());
  // pass 4: GetExpr()->Build()
  x->SetExpr(TRef::New(x_def, TPosRange()));
  where->SetExpr(TRef::New(x_result, TPosRange()));
  foo->SetExpr(where);
  /* foo(.x: 0.0); */
  TFunctionAppArg::TPtr x_arg = TFunctionAppArg::New(real0);
  TFunctionApp::TPtr foo_app = TFunctionApp::New(TRef::New(foo_result, TPosRange()),
                                                 TFunctionApp::TFunctionAppArgMap({{"x", x_arg}}),
                                                 TPosRange());
  EXPECT_EQ(foo_app->GetFunction(), foo);
  EXPECT_EQ(foo_app->GetType(), real_type);
}
