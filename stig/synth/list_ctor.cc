/* <stig/synth/list_ctor.cc>

   Implements <stig/synth/list_ctor.h>.

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

#include <stig/synth/list_ctor.h>

#include <base/assert_true.h>
#include <stig/error.h>
#include <stig/expr/list.h>
#include <stig/synth/cst_utils.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>

using namespace Stig;
using namespace Stig::Synth;

TListCtor::TListCtor(const TExprFactory *expr_factory, const Package::Syntax::TListCtor *list_ctor)
    : ListCtor(Base::AssertTrue(list_ctor)) {
  assert(expr_factory);
  try {
    ForEach<Package::Syntax::TExpr>(ListCtor->GetExprList(),
        [this, expr_factory](const Package::Syntax::TExpr *expr) -> bool {
          Exprs.emplace_back(expr_factory->NewExpr(expr));
          return true;
        });
  } catch (...) {
    Cleanup();
    throw;
  }
  if (Exprs.empty()) {
    throw TCompileError(HERE, GetPosRange(ListCtor), "List is empty. Use empty constructor instead.");
  }
}

TListCtor::~TListCtor() {
  Cleanup();
}

Expr::TExpr::TPtr TListCtor::Build() const {
  assert(this);
  std::vector<Expr::TExpr::TPtr> exprs;
  for (auto expr : Exprs) {
    exprs.emplace_back(expr->Build());
  }
  return Expr::TList::New(exprs, GetPosRange(ListCtor));
}

void TListCtor::Cleanup() {
  assert(this);
  for (auto expr : Exprs) {
    delete expr;
  }
}

void TListCtor::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  for (auto expr : Exprs) {
    expr->ForEachInnerScope(cb);
  }
}

void TListCtor::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  for (auto expr : Exprs) {
    expr->ForEachRef(cb);
  }
}