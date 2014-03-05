/* <stig/code_gen/package.test.cc>

   Unit test for <stig/code_gen/package.h>

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

#include <stig/code_gen/package.h>

#include <stig/expr/literal.h>
#include <stig/expr/add.h>
#include <stig/symbol/package.h>
#include <stig/symbol/result_def.h>
#include <stig/type/type_czar.h>

#include <test/kit.h>

using namespace Stig;
using namespace Stig::CodeGen;

FIXTURE(Typical) {
  Type::TTypeCzar type_czar;
  //Build up a symbolic package
  Symbol::TPackage::TPtr p_sym = Symbol::TPackage::New({"test"}, 1);
  Symbol::TFunction::TPtr foo = Symbol::TFunction::New(p_sym, "foo", TPosRange());
  Symbol::TResultDef::TPtr foo_result = Symbol::TResultDef::New(foo, "bar", TPosRange());
  foo->SetExpr(Expr::TAdd::New(Expr::TLiteral::New(Var::TVar(10), TPosRange()), Expr::TLiteral::New(Var::TVar(10), TPosRange()), TPosRange()));

  //TODO: Add functions to the package to exercise all the code gen machinery.
  TPackage p(p_sym);

  p.Emit(Jhm::TAbsBase("/tmp/"));

  //GenCdoe
}