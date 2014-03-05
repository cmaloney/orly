/* <stig/synth/if_else_expr.cc>

   Implements <stig/synth/if_else_expr.h>.

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

#include <stig/synth/if_else_expr.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/expr/if_else.h>
#include <stig/synth/get_pos_range.h>

using namespace Stig;
using namespace Stig::Synth;

TIfElseExpr::TIfElseExpr(
    TExpr *true_case,
    TExpr *predicate,
    TExpr *false_case,
    const Package::Syntax::TIfExpr *if_expr)
      : IfExpr(Base::AssertTrue(if_expr)),
        FalseCase(Base::AssertTrue(false_case)),
        Predicate(Base::AssertTrue(predicate)),
        TrueCase(Base::AssertTrue(true_case)) {}

TIfElseExpr::~TIfElseExpr() {
  assert(this);
  delete FalseCase;
  delete Predicate;
  delete TrueCase;
}

Expr::TExpr::TPtr TIfElseExpr::Build() const {
  assert(this);
  return Expr::TIfElse::New(
             TrueCase->Build(),
             Predicate->Build(),
             FalseCase->Build(),
             GetPosRange(IfExpr));
}

void TIfElseExpr::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  FalseCase->ForEachInnerScope(cb);
  Predicate->ForEachInnerScope(cb);
  TrueCase->ForEachInnerScope(cb);
}

void TIfElseExpr::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  FalseCase->ForEachRef(cb);
  Predicate->ForEachRef(cb);
  TrueCase->ForEachRef(cb);
}
