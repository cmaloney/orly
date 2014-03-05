/* <stig/synth/postfix_slice.cc>

   Implements <stig/synth/postfix_slice.h>.

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

#include <stig/synth/postfix_slice.h>

#include <base/assert_true.h>
#include <stig/expr/slice.h>
#include <stig/pos_range.h>
#include <stig/stig.package.cst.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>
#include <stig/synth/new_type.h>

using namespace Stig;
using namespace Stig::Synth;

TPostfixSlice::TPostfixSlice(const TExprFactory *expr_factory, const Package::Syntax::TPostfixSlice *postfix_slice)
    : PostfixSlice(Base::AssertTrue(postfix_slice)), Colon(false), Expr(nullptr), OptLhs(nullptr), OptRhs(nullptr) {
  assert(expr_factory);
  try {
    Expr = expr_factory->NewExpr(PostfixSlice->GetExpr());
    class TSliceVisitor
        : public Package::Syntax::TSlice::TVisitor {
      NO_COPY_SEMANTICS(TSliceVisitor);
      public:
      TSliceVisitor(const TExprFactory *expr_factory, TExpr *&opt_lhs, bool &colon, TExpr *&opt_rhs)
          : ExprFactory(expr_factory), OptLhs(opt_lhs), OptRhs(opt_rhs), Colon(colon) {}
      virtual void operator()(const Package::Syntax::TSingleSlice *that) const {
        OptLhs = ExprFactory->NewExpr(that->GetExpr());
      }
      virtual void operator()(const Package::Syntax::TSliceToEnd *that) const {
        OptLhs = ExprFactory->NewExpr(that->GetExpr());
        Colon = true;
      }
      virtual void operator()(const Package::Syntax::TSliceFromStart *that) const {
        Colon = true;
        OptRhs = ExprFactory->NewExpr(that->GetExpr());
      }
      virtual void operator()(const Package::Syntax::TSliceBetween *that) const {
        OptLhs = ExprFactory->NewExpr(that->GetStart());
        Colon = true;
        OptRhs = ExprFactory->NewExpr(that->GetLimit());
      }
      private:
      const TExprFactory *ExprFactory;
      TExpr *&OptLhs;
      TExpr *&OptRhs;
      bool &Colon;
    };  // TSliceVisitor
    PostfixSlice->GetSlice()->Accept(TSliceVisitor(expr_factory, OptLhs, Colon, OptRhs));
  } catch (...) {
    Cleanup();
    throw;
  }
}

TPostfixSlice::~TPostfixSlice() {
  Cleanup();
}

Expr::TExpr::TPtr TPostfixSlice::Build() const {
  assert(this);
  return Expr::TSlice::New(
           Expr->Build(),
           OptLhs ? OptLhs->Build() : nullptr,
           Colon,
           OptRhs ? OptRhs->Build() : nullptr,
           GetPosRange(PostfixSlice));
}

void TPostfixSlice::Cleanup() {
  assert(this);
  delete Expr;
  delete OptLhs;
  delete OptRhs;
}

void TPostfixSlice::ForEachInnerScope(const std::function<void (TScope *cb)> &cb) {
  assert(this);
  Expr->ForEachInnerScope(cb);
  if (OptLhs) {
    OptLhs->ForEachInnerScope(cb);
  }
  if (OptRhs) {
    OptRhs->ForEachInnerScope(cb);
  }
}

void TPostfixSlice::ForEachRef(const std::function<void (TAnyRef &cb)> &cb) {
  assert(this);
  Expr->ForEachRef(cb);
  if (OptLhs) {
    OptLhs->ForEachRef(cb);
  }
  if (OptRhs) {
    OptRhs->ForEachRef(cb);
  }
}
