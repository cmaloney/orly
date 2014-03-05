/* <stig/synth/infix_expr.cc>

   Implements <stig/synth/infix_expr.h>

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

#include <stig/synth/infix_expr.h>

#include <base/assert_true.h>

using namespace Stig;
using namespace Stig::Synth;

TInfixExpr::TInfixExpr(TExpr *lhs, TExpr *rhs, TNew new_, const TPosRange &pos_range)
    : Lhs(Base::AssertTrue(lhs)), New(new_), Rhs(Base::AssertTrue(rhs)), PosRange(pos_range) {}

/* TODO */
TInfixExpr::~TInfixExpr() {
  assert(this);
  delete Lhs;
  delete Rhs;
}

/* TODO */
Expr::TExpr::TPtr TInfixExpr::Build() const {
  assert(this);
  assert(Lhs);
  assert(Rhs);
  return New(Lhs->Build(), Rhs->Build(), PosRange);
}

/* TODO */
void TInfixExpr::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  Lhs->ForEachInnerScope(cb);
  Rhs->ForEachInnerScope(cb);
}

/* TODO */
void TInfixExpr::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  Lhs->ForEachRef(cb);
  Rhs->ForEachRef(cb);
}
