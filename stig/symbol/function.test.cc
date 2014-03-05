/* <stig/symbol/function.test.cc>

   Unit test for <stig/symbol/function.h>.

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

FIXTURE(Function) {
  /* package scope; */
  TPackage::TPtr package = TPackage::New({"foo"}, 1);
  EXPECT_EQ(package->GetVersion(), 1U);

  /* foo = 10; */
  TFunction::TPtr foo = TFunction::New(package, "foo", TPosRange());
  TResultDef::TPtr foo_result = TResultDef::New(foo, "foo", TPosRange());
  foo->SetExpr(TLiteral::New(Var::TVar(10), TPosRange()));
  /* Name */
  EXPECT_EQ(foo->GetName(), "foo");
  /* ParamDefs */
  const auto &foo_param_defs = foo->GetParamDefs();
  EXPECT_EQ(foo_param_defs.size(), 0U);
  /* ResultDefs */
  const auto &foo_result_defs = foo->GetResultDefs();
  EXPECT_EQ(foo_result_defs.size(), 1U);
  EXPECT_EQ(foo_result_defs[0], foo_result);
  /* GetScope */
  EXPECT_EQ(foo->GetScope(), package);
  /* GetReturnType */
  EXPECT_EQ(foo->GetReturnType(), int_type);
  /* GetType */
  EXPECT_TRUE(foo->GetType() == Type::TFunc::Get(Type::TObj::Get(Type::TObj::TElems({})), int_type));

  /*
     bar = (x + y) where {
       x = given::(int);
       y = given::(int);
     };
  */
  // pass 1: continue
  // pass 2: Build FunctionSymbol and ResultDefSymbol
  // function: bar
  TFunction::TPtr bar = TFunction::New(package, "bar", TPosRange());
  TResultDef::TPtr bar_result = TResultDef::New(bar, "bar", TPosRange());
  // Build scope
  TWhere::TPtr where = TWhere::New(TPosRange());
  // function: x
  TFunction::TPtr x = TFunction::New(where, "x", TPosRange());
  TResultDef::TPtr x_result = TResultDef::New(x, "x", TPosRange());
  // function: y
  TFunction::TPtr y = TFunction::New(where, "y", TPosRange());
  TResultDef::TPtr y_result = TResultDef::New(y, "y", TPosRange());
  // pass 3: BuildSecondarySymbol()
  TGivenParamDef::TPtr x_def = TGivenParamDef::New(bar, "x", int_type, TPosRange());
  TGivenParamDef::TPtr y_def = TGivenParamDef::New(bar, "y", int_type, TPosRange());
  // pass 4: GetExpr()->Build()
  // where_expr: x + y
  where->SetExpr(TAdd::New(TRef::New(x_result, TPosRange()), TRef::New(y_result, TPosRange()), TPosRange()));
  // func_expr: given::(int)
  x->SetExpr(TRef::New(x_def, TPosRange()));
  y->SetExpr(TRef::New(y_def, TPosRange()));
  bar->SetExpr(where);
  /* Name */
  EXPECT_EQ(bar->GetName(), "bar");
  EXPECT_EQ(x_def->GetName(), "x");
  EXPECT_EQ(y_def->GetName(), "y");
  /* ResultDefs */
  const auto &bar_result_defs = bar->GetResultDefs();
  EXPECT_EQ(bar_result_defs.size(), 1U);
  EXPECT_EQ(bar_result_defs[0], bar_result);
  /* ParamDefs */
  const auto &bar_param_defs = bar->GetParamDefs();
  EXPECT_EQ(bar_param_defs.size(), 2U);
  EXPECT_TRUE(Base::Contains(bar_param_defs, x_def));
  EXPECT_TRUE(Base::Contains(bar_param_defs, y_def));
  /* GetScope */
  EXPECT_EQ(bar->GetScope(), package);
  /* GetReturnType */
  EXPECT_EQ(bar->GetReturnType(), int_type);
  /* GetType */
  EXPECT_TRUE(
    bar->GetType() == /* lhs */
    Type::TFunc::Get( /* rhs */
      Type::TObj::Get(Type::TObj::TElems({{"x", int_type}, {"y", int_type}})), /* param_obj */
      int_type /* return_type */
    )
  );
}
