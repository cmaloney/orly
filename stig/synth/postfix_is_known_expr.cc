/* <stig/synth/postfix_is_known_expr.cc>

   Implements <stig/synth/postfix_is_known_expr.h>.

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

#include <stig/synth/postfix_is_known_expr.h>

#include <base/assert_true.h>
#include <stig/expr/known.h>
#include <stig/pos_range.h>
#include <stig/stig.package.cst.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>
#include <stig/synth/new_type.h>

using namespace Stig;
using namespace Stig::Synth;

TPostfixIsKnownExpr::TPostfixIsKnownExpr(const TExprFactory *expr_factory, const Package::Syntax::TPostfixIsKnownExpr *postfix_is_known_expr)
    : PostfixIsKnownExpr(Base::AssertTrue(postfix_is_known_expr)), Lhs(nullptr), Rhs(nullptr) {
  assert(expr_factory);
  try {
    Lhs = expr_factory->NewExpr(PostfixIsKnownExpr->GetLhs());
    Rhs = expr_factory->NewExpr(PostfixIsKnownExpr->GetRhs());
  } catch (...) {
    delete Lhs;
    delete Rhs;
    throw;
  }
}

TPostfixIsKnownExpr::~TPostfixIsKnownExpr() {
  assert(this);
  delete Lhs;
  delete Rhs;
}

Expr::TExpr::TPtr TPostfixIsKnownExpr::Build() const {
  assert(this);
  return Expr::TIsKnownExpr::New(Lhs->Build(), Rhs->Build(), GetPosRange(PostfixIsKnownExpr));
}

void TPostfixIsKnownExpr::ForEachInnerScope(const std::function<void (TScope *cb)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Lhs->ForEachInnerScope(cb);
  Rhs->ForEachInnerScope(cb);
}

void TPostfixIsKnownExpr::ForEachRef(const std::function<void (TAnyRef &cb)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Lhs->ForEachRef(cb);
  Rhs->ForEachRef(cb);
}
