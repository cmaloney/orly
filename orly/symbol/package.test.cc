/* <orly/symbol/package.test.cc>

   Unit test for <orly/symbol/package.h>.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/expr/test_kit.h>

namespace std {
  ostream &operator<<(ostream &out, const vector<string> &ns) {
    return WriteNamespace(out, ns);
  }
}

FIXTURE(Package) {
  /* package scope */
  TPackage::TPtr package = TPackage::New({{"foo"}}, 1);
  EXPECT_EQ(package->GetVersion(), 1U);
  EXPECT_EQ(package->GetName(), Package::TName{{"foo"}});

  /* foo = 24;
     bar = 42; */
  TFunction::TPtr foo = TFunction::New(package, "foo", TPosRange());
  TResultDef::TPtr foo_result = TResultDef::New(foo, "foo", TPosRange());
  assert(foo_result);
  foo->SetExpr(TLiteral::New(Var::TVar(24), TPosRange()));
  TFunction::TPtr bar = TFunction::New(package, "bar", TPosRange());
  TResultDef::TPtr bar_result = TResultDef::New(bar, "bar", TPosRange());
  assert(bar_result);
  bar->SetExpr(TLiteral::New(Var::TVar(42), TPosRange()));

  /* GetFunctions */
  const auto &functions = package->GetFunctions();
  EXPECT_EQ(functions.size(), 2U);
  EXPECT_TRUE(Util::Contains(functions, foo));
  EXPECT_TRUE(Util::Contains(functions, bar));
}
