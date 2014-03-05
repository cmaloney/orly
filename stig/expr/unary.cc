/* <stig/expr/unary.cc>

   Implements <stig/expr/unary.h>

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

#include <stig/expr/unary.h>

#include <base/assert_true.h>

using namespace Stig;
using namespace Stig::Expr;

TUnary::TUnary(const TPosRange &pos_range)
    : TInterior(pos_range), Expr(nullptr) {}

TUnary::TUnary(const TExpr::TPtr &expr, const TPosRange &pos_range)
    : TInterior(pos_range), Expr(Base::AssertTrue(expr)) {
  Expr->SetExprParent(this);
}

TUnary::~TUnary() {
  assert(this);
  if (Expr) {
    Expr->UnsetExprParent(this);
  }
}

const TExpr::TPtr &TUnary::GetExpr() const {
  assert(this);
  assert(Expr);
  return Expr;
}

void TUnary::SetExpr(const TExpr::TPtr &expr) {
  assert(this);
  assert(expr);
  assert(!Expr);
  Expr = expr;
  Expr->SetExprParent(this);
}
