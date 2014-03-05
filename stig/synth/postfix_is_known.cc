/* <stig/synth/postfix_is_known.cc>

   Implements <stig/synth/postfix_is_known.h>.

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

#include <stig/synth/postfix_is_known.h>

#include <base/assert_true.h>
#include <stig/expr/known.h>
#include <stig/pos_range.h>
#include <stig/stig.package.cst.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>
#include <stig/synth/new_type.h>

using namespace Stig;
using namespace Stig::Synth;

TPostfixIsKnown::TPostfixIsKnown(const TExprFactory *expr_factory, const Package::Syntax::TPostfixIsKnown *postfix_is_known)
    : PostfixIsKnown(Base::AssertTrue(postfix_is_known)),
      Expr(Base::AssertTrue(expr_factory)->NewExpr(PostfixIsKnown->GetExpr())) {}

TPostfixIsKnown::~TPostfixIsKnown() {
  assert(this);
  delete Expr;
}

Expr::TExpr::TPtr TPostfixIsKnown::Build() const {
  assert(this);
  return Expr::TIsKnown::New(Expr->Build(), GetPosRange(PostfixIsKnown));
}

void TPostfixIsKnown::ForEachInnerScope(const std::function<void (TScope *cb)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachInnerScope(cb);
}

void TPostfixIsKnown::ForEachRef(const std::function<void (TAnyRef &cb)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachRef(cb);
}
