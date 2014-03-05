/* <stig/synth/assert_expr.cc>

   Implements <stig/synth/assert_expr.h>.

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

#include <stig/synth/assert_expr.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/synth/cst_utils.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/func_def.h>
#include <stig/synth/new_expr.h>

using namespace Stig;
using namespace Stig::Synth;

TAssertExpr::TAssertCase::TAssertCase(const Base::TOpt<std::string> &opt_name, TExpr *expr)
    : OptName(opt_name), Expr(Base::AssertTrue(expr)) {}

TAssertExpr::TAssertCase::~TAssertCase() {
  assert(this);
  delete Expr;
}

Expr::TAssertCase::TPtr TAssertExpr::TAssertCase::Build(const Expr::TAssert::TPtr &assert) const {
  assert(this);
  return Expr::TAssertCase::New(assert, OptName, Expr->Build());
}

void TAssertExpr::TAssertCase::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachInnerScope(cb);
}

void TAssertExpr::TAssertCase::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachRef(cb);
}

TAssertExpr::TAssertExpr(const TExprFactory *expr_factory, const Package::Syntax::TAssertExpr *assert_expr)
    : AssertExpr(Base::AssertTrue(assert_expr)), Expr(nullptr) {
  class TAssertionVisitor
      : public Package::Syntax::TAssertion::TVisitor {
    NO_COPY_SEMANTICS(TAssertionVisitor);
    public:
    TAssertionVisitor(TAssertCaseVec &assert_cases, const TExprFactory *expr_factory)
        : AssertCases(assert_cases), ExprFactory(expr_factory) {}
    virtual void operator()(const Package::Syntax::TBadAssertion *) const { /* DO NOTHING */ }
    virtual void operator()(const Package::Syntax::TLabeledAssertion *that) const {
      AddAssertCase(TName(that->GetName()).GetText(), that->GetExpr());
    }
    virtual void operator()(const Package::Syntax::TUnlabeledAssertion *that) const {
      AddAssertCase(*Base::TOpt<std::string>::Unknown, that->GetExpr());
    }
    private:
    void AddAssertCase(const Base::TOpt<std::string> &opt_name, const Package::Syntax::TExpr *expr) const {
      AssertCases.emplace_back(new TAssertCase(opt_name, ExprFactory->NewExpr(expr)));
    }
    TAssertCaseVec &AssertCases;
    const TExprFactory *ExprFactory;
  };  // TAssertionVisitor
  assert(expr_factory);
  try {
    Expr = expr_factory->NewExpr(AssertExpr->GetExpr());
    TExprFactory local_expr_factory = *expr_factory;
    local_expr_factory.ThatableExpr = this;
    TAssertionVisitor visitor(AssertCases, &local_expr_factory);
    ForEach<Package::Syntax::TAssertion>(AssertExpr->GetOptAssertionSeq(),
        [&visitor](const Package::Syntax::TAssertion *assertion) -> bool {
          assertion->Accept(visitor);
          return true;
        });
  } catch (...) {
    Cleanup();
    throw;
  }
}

TAssertExpr::~TAssertExpr() {
  Cleanup();
}

Expr::TExpr::TPtr TAssertExpr::Build() const {
  assert(this);
  assert(!Symbol);
  // NOTE: If you're wondering why I build assert cases after the assert itself, take a look at <stig/synth/thatable_expr.h>
  Symbol = Expr::TAssert::New(Expr->Build(), GetPosRange(AssertExpr));
  for (auto &assert_case : AssertCases) {
    assert_case->Build(Symbol);
  }
  return Symbol;
}

void TAssertExpr::Cleanup() {
  assert(this);
  delete Expr;
  for (auto &assert_case : AssertCases) {
    delete assert_case;
  }
}

void TAssertExpr::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachInnerScope(cb);
  for (auto &assert_case : AssertCases) {
    assert_case->ForEachInnerScope(cb);
  }
}

void TAssertExpr::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachRef(cb);
  for (auto &assert_case : AssertCases) {
    assert_case->ForEachRef(cb);
  }
}

const Expr::TAssert::TPtr &TAssertExpr::GetSymbol() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}

Expr::TThatable::TPtr TAssertExpr::GetThatableSymbol() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}
