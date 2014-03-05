/* <stig/synth/filter_expr.cc>

   Implements <stig/synth/filter_expr.h>.

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

#include <stig/synth/filter_expr.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>

using namespace Stig;
using namespace Stig::Synth;

TFilterExpr::TFilterExpr(const TExprFactory *expr_factory, const Package::Syntax::TInfixFilter *infix_filter)
    : InfixFilter(Base::AssertTrue(infix_filter)), Lhs(nullptr), Rhs(nullptr) {
  assert(expr_factory);
  try {
    Lhs = expr_factory->NewExpr(InfixFilter->GetLhs());
    TExprFactory local_expr_factory = *expr_factory;
    local_expr_factory.ThatableExpr = this;
    Rhs = local_expr_factory.NewExpr(InfixFilter->GetRhs());
  } catch (...) {
    delete Lhs;
    delete Rhs;
    throw;
  }
}

TFilterExpr::~TFilterExpr() {
  assert(this);
  delete Lhs;
  delete Rhs;
}

Expr::TExpr::TPtr TFilterExpr::Build() const {
  assert(this);
  assert(!Symbol);
  // NOTE: If you're wondering why I set rhs seperately, take a look at <stig/synth/thatable_expr.h>
  Symbol = Expr::TFilter::New(Lhs->Build(), GetPosRange(InfixFilter));
  Symbol->SetRhs(Rhs->Build());
  return Symbol;
}

void TFilterExpr::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Lhs->ForEachInnerScope(cb);
  Rhs->ForEachInnerScope(cb);
}

void TFilterExpr::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Lhs->ForEachRef(cb);
  Rhs->ForEachRef(cb);
}

const Expr::TFilter::TPtr &TFilterExpr::GetSymbol() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}

Expr::TThatable::TPtr TFilterExpr::GetThatableSymbol() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}
