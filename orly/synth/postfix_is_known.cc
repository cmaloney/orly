/* <orly/synth/postfix_is_known.cc>

   Implements <orly/synth/postfix_is_known.h>.

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

#include <orly/synth/postfix_is_known.h>

#include <base/assert_true.h>
#include <orly/expr/known.h>
#include <orly/pos_range.h>
#include <orly/orly.package.cst.h>
#include <orly/synth/get_pos_range.h>
#include <orly/synth/new_expr.h>
#include <orly/synth/new_type.h>

using namespace Orly;
using namespace Orly::Synth;

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
