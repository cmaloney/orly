/* <orly/synth/set_ctor.cc>

   Implements <orly/synth/set_ctor.h>.

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

#include <orly/pos_range.h>
#include <orly/expr/set.h>
#include <orly/synth/cst_utils.h>
#include <orly/synth/get_pos_range.h>
#include <orly/synth/new_expr.h>
#include <orly/synth/set_ctor.h>

using namespace Stig;
using namespace Stig::Synth;

TSetCtor::TSetCtor(const TExprFactory *expr_factory, const Package::Syntax::TSetCtor *set_ctor)
    : SetCtor(Base::AssertTrue(set_ctor)) {
  assert(expr_factory);
  try {
    ForEach<Package::Syntax::TExpr>(SetCtor->GetExprList(),
        [this, expr_factory](const Package::Syntax::TExpr *expr) -> bool {
          auto result = Exprs.emplace(expr_factory->NewExpr(expr));
          assert(result.second);
          return true;
        });
  } catch (...) {
    Cleanup();
    throw;
  }
}

TSetCtor::~TSetCtor() {
  Cleanup();
}

Expr::TExpr::TPtr TSetCtor::Build() const {
  assert(this);
  Expr::TSet::TExprSet exprs;
  for (auto expr : Exprs) {
    auto result = exprs.emplace(expr->Build());
    assert(result.second);
  }
  return Expr::TSet::New(exprs, GetPosRange(SetCtor));
}

void TSetCtor::Cleanup() {
  assert(this);
  for (auto expr : Exprs) {
    delete expr;
  }
}

void TSetCtor::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  for (auto expr : Exprs) {
    expr->ForEachInnerScope(cb);
  }
}

void TSetCtor::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  for (auto expr : Exprs) {
    expr->ForEachRef(cb);
  }
}
