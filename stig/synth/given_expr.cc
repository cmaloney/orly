/* <stig/synth/given_expr.cc>

   Implements <stig/synth/given_expr.h>.

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

#include <stig/synth/given_expr.h>

#include <stig/expr/ref.h>
#include <stig/synth/func_def.h>
#include <stig/synth/get_pos_range.h>

using namespace Stig;
using namespace Stig::Synth;

TGivenExpr::TGivenExpr(const TParamFuncDef *param_func_def, const Package::Syntax::TGivenExpr *given_expr)
    : GivenExpr(Base::AssertTrue(given_expr)), ParamFuncDef(Base::AssertTrue(param_func_def)) {}

Expr::TExpr::TPtr TGivenExpr::Build() const {
  assert(this);
  return Expr::TRef::New(ParamFuncDef->GetParamDefSymbol(), GetPosRange(GivenExpr));
}

void TGivenExpr::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  ParamFuncDef->GetType()->ForEachRef(cb);
}
