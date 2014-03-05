/* <stig/synth/collected_by_expr.cc>

   Implements <stig/synth/collected_by_expr.h>.

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

#include <stig/synth/collected_by_expr.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>

using namespace Stig;
using namespace Stig::Synth;

TCollectedByExpr::TCollectedByExpr(const TExprFactory *expr_factory, const Package::Syntax::TCollectedByExpr *infix_collected_by)
    : CollectedByExpr(Base::AssertTrue(infix_collected_by)), Lhs(nullptr), Rhs(nullptr) {
  assert(expr_factory);
  try {
    Lhs = expr_factory->NewExpr(CollectedByExpr->GetSeq());
    TExprFactory local_expr_factory = *expr_factory;
    local_expr_factory.LhsRhsableExpr = this;
    Rhs = local_expr_factory.NewExpr(CollectedByExpr->GetCollect());
  } catch (...) {
    delete Lhs;
    delete Rhs;
    throw;
  }
}

TCollectedByExpr::~TCollectedByExpr() {
  assert(this);
  delete Lhs;
  delete Rhs;
}

Expr::TExpr::TPtr TCollectedByExpr::Build() const {
  assert(this);
  assert(!Symbol);
  // NOTE: If you're wondering why I set rhs seperately, take a look at <stig/synth/lhsrhsable_expr.h>
  Symbol = Expr::TCollectedBy::New(Lhs->Build(), GetPosRange(CollectedByExpr));
  Symbol->SetRhs(Rhs->Build());
  return Symbol;
}

void TCollectedByExpr::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Lhs->ForEachInnerScope(cb);
  Rhs->ForEachInnerScope(cb);
}

void TCollectedByExpr::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Lhs->ForEachRef(cb);
  Rhs->ForEachRef(cb);
}

const Expr::TCollectedBy::TPtr &TCollectedByExpr::GetSymbol() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}

Expr::TLhsRhsable::TPtr TCollectedByExpr::GetLhsRhsableSymbol() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}
