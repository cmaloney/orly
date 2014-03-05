/* <stig/synth/lhs_expr.cc>

   Implements <stig/synth/lhs_expr.h>.

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

#include <stig/synth/lhs_expr.h>

#include <base/assert_true.h>
#include <stig/expr/lhs_and_rhs.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/infix_expr.h>
#include <stig/synth/new_expr.h>
#include <stig/synth/sort_expr.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Synth;

TLhsExpr::TLhsExpr(const TExprFactory *expr_factory, const Package::Syntax::TLhsExpr *lhs_expr)
    : LhsExpr(Base::AssertTrue(lhs_expr)), LhsRhsableExpr(Base::AssertTrue(expr_factory)->LhsRhsableExpr) {
  if (!LhsRhsableExpr) {
    Tools::Nycr::TError::TBuilder(GetPosRange(LhsExpr)) << "'lhs' outside of 'sort'";
  }
}

Expr::TExpr::TPtr TLhsExpr::Build() const {
  assert(this);
  return Expr::TLhs::New(LhsRhsableExpr->GetLhsRhsableSymbol(), GetPosRange(LhsExpr));
}
