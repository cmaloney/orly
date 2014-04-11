/* <orly/synth/rhs_expr.cc>

   Implements <orly/synth/rhs_expr.h>.

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

#include <orly/synth/rhs_expr.h>

#include <base/assert_true.h>
#include <orly/expr/lhs_and_rhs.h>
#include <orly/synth/get_pos_range.h>
#include <orly/synth/infix_expr.h>
#include <orly/synth/new_expr.h>
#include <orly/synth/sort_expr.h>
#include <tools/nycr/error.h>

using namespace Orly;
using namespace Orly::Synth;

TRhsExpr::TRhsExpr(const TExprFactory *expr_factory, const Package::Syntax::TRhsExpr *rhs_expr)
    : RhsExpr(Base::AssertTrue(rhs_expr)), LhsRhsableExpr(Base::AssertTrue(expr_factory)->LhsRhsableExpr) {
  if (!LhsRhsableExpr) {
    Tools::Nycr::TError::TBuilder(GetPosRange(RhsExpr)) << "'rhs' outside of 'sort'";
  }
}

Expr::TExpr::TPtr TRhsExpr::Build() const {
  assert(this);
  return Expr::TRhs::New(LhsRhsableExpr->GetLhsRhsableSymbol(), GetPosRange(RhsExpr));
}
