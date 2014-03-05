/* <stig/synth/effecting_expr.cc>

   Implements <stig/synth/effecting_expr.h>.

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

#include <stig/synth/effecting_expr.h>

#include <cassert>
#include <vector>

#include <base/assert_true.h>
#include <stig/expr/effect.h>
#include <stig/synth/cst_utils.h>
#include <stig/synth/func_def.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>

using namespace Stig;
using namespace Stig::Synth;

TEffectingExpr::TEffectingExpr(
    const TExprFactory *expr_factory,
    const Package::Syntax::TEffectingExpr *effecting_expr)
      : EffectingExpr(Base::AssertTrue(effecting_expr)),
        Expr(nullptr),
        StmtBlock(nullptr) {
  assert(expr_factory);
  TExprFactory local_expr_factory = *expr_factory;
  local_expr_factory.ThatableExpr = this;
  try {
    Expr = local_expr_factory.NewExpr(EffectingExpr->GetExpr());
    StmtBlock = new TStmtBlock(&local_expr_factory, EffectingExpr->GetStmtBlock());
  } catch (...) {
    delete Expr;
    delete StmtBlock;
    throw;
  }
}

TEffectingExpr::~TEffectingExpr() {
  assert(this);
  delete Expr;
  delete StmtBlock;
}

Expr::TExpr::TPtr TEffectingExpr::Build() const {
  assert(this);
  assert(!Symbol);
  // NOTE: If you're wondering why I build stmt block seperately, take a look at <stig/synth/thatable_expr.h>
  Symbol = Expr::TEffect::New(Expr->Build(), GetPosRange(EffectingExpr));
  Symbol->SetStmtBlock(StmtBlock->Build());
  return Symbol;
}

void TEffectingExpr::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachInnerScope(cb);
  for(auto &stmt : StmtBlock->GetStmts()) {
    stmt->ForEachInnerScope(cb);
  }
}

void TEffectingExpr::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachRef(cb);
  StmtBlock->ForEachRef(cb);
}

const Expr::TEffect::TPtr &TEffectingExpr::GetSymbol() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}

Expr::TThatable::TPtr TEffectingExpr::GetThatableSymbol() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}
