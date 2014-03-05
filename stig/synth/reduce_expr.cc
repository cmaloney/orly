/* <stig/synth/reduce_expr.cc>

   Implements <stig/synth/reduce_expr.h>.

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

#include <stig/synth/reduce_expr.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>

using namespace Stig;
using namespace Stig::Synth;

TReduceExpr::TReduceExpr(const TExprFactory *expr_factory, const Package::Syntax::TInfixReduce *infix_reduce)
    : InfixReduce(Base::AssertTrue(infix_reduce)), Lhs(nullptr), Rhs(nullptr) {
  assert(expr_factory);
  try {
    Lhs = expr_factory->NewExpr(InfixReduce->GetLhs());
    TExprFactory local_expr_factory = *expr_factory;
    local_expr_factory.StartableExpr = this;
    local_expr_factory.ThatableExpr = this;
    Rhs = local_expr_factory.NewExpr(InfixReduce->GetRhs());
  } catch (...) {
    delete Lhs;
    delete Rhs;
    throw;
  }
}

TReduceExpr::~TReduceExpr() {
  assert(this);
  delete Lhs;
  delete Rhs;
}

Expr::TExpr::TPtr TReduceExpr::Build() const {
  assert(this);
  assert(!Symbol);
  // NOTE: If you're wondering why I set rhs seperately, take a look at <stig/synth/thatable_expr.h>
  Symbol = Expr::TReduce::New(Lhs->Build(), GetPosRange(InfixReduce));
  Symbol->SetRhs(Rhs->Build());
  return Symbol;
}

void TReduceExpr::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Lhs->ForEachInnerScope(cb);
  Rhs->ForEachInnerScope(cb);
}

void TReduceExpr::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Lhs->ForEachRef(cb);
  Rhs->ForEachRef(cb);
}

const Expr::TReduce::TPtr &TReduceExpr::GetSymbol() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}

Expr::TStartable::TPtr TReduceExpr::GetStartableSymbol() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}

Expr::TThatable::TPtr TReduceExpr::GetThatableSymbol() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}
