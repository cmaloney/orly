/* <stig/synth/collated_by_expr.cc>

   Implements <stig/synth/collated_by_expr.h>.

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

#include <stig/synth/collated_by_expr.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>

using namespace Stig;
using namespace Stig::Synth;

TCollatedByExpr::TCollatedByExpr(const TExprFactory *expr_factory, const Package::Syntax::TCollatedByExpr *collated_by_expr)
    : CollatedByExpr(Base::AssertTrue(collated_by_expr)),
      Seq(nullptr),
      Reduce(nullptr),
      Having(nullptr) {
  assert(expr_factory);
  try {
    Seq = expr_factory->NewExpr(CollatedByExpr->GetSeq());
    TExprFactory local_expr_factory = *expr_factory;
    local_expr_factory.StartableExpr = this;
    local_expr_factory.ThatableExpr = this;
    Reduce = local_expr_factory.NewExpr(CollatedByExpr->GetReduce());
    Having = local_expr_factory.NewExpr(CollatedByExpr->GetHaving());
  } catch (...) {
    delete Seq;
    delete Reduce;
    delete Having;
    throw;
  }
}

TCollatedByExpr::~TCollatedByExpr() {
  assert(this);
  delete Seq;
  delete Reduce;
  delete Having;
}

Expr::TExpr::TPtr TCollatedByExpr::Build() const {
  assert(this);
  assert(!Symbol);
  // NOTE: If you're wondering why I set rhs seperately, take a look at <stig/synth/thatable_expr.h>
  Symbol = Expr::TCollatedBy::New(Seq->Build(), GetPosRange(CollatedByExpr));
  Symbol->SetReduce(Reduce->Build());
  Symbol->SetHaving(Having->Build());
  return Symbol;
}

void TCollatedByExpr::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Seq->ForEachInnerScope(cb);
  Reduce->ForEachInnerScope(cb);
  Having->ForEachInnerScope(cb);
}

void TCollatedByExpr::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Seq->ForEachRef(cb);
  Reduce->ForEachRef(cb);
  Having->ForEachRef(cb);
}

const Expr::TCollatedBy::TPtr &TCollatedByExpr::GetSymbol() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}

Expr::TStartable::TPtr TCollatedByExpr::GetStartableSymbol() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}

Expr::TThatable::TPtr TCollatedByExpr::GetThatableSymbol() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}
