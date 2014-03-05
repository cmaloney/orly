/* <stig/synth/affix_expr.cc>

   Implements <stig/synth/affix_expr.h>.

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

#include <stig/synth/affix_expr.h>

#include <cassert>

#include <base/assert_true.h>

using namespace Stig;
using namespace Stig::Synth;

TAffixExpr::TAffixExpr(TExpr *expr, TNew new_, const TPosRange &pos_range)
    : Expr(Base::AssertTrue(expr)), New(new_), PosRange(pos_range) {}

TAffixExpr::~TAffixExpr() {
  assert(this);
  delete Expr;
}

Expr::TExpr::TPtr TAffixExpr::Build() const {
  assert(this);
  return New(Expr->Build(), PosRange);
}

void TAffixExpr::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachInnerScope(cb);
}

void TAffixExpr::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachRef(cb);
}
