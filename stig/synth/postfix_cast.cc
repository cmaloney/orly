/* <stig/synth/postfix_cast.cc>

   Implements <stig/synth/postfix_cast.h>.

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

#include <stig/synth/postfix_cast.h>

#include <base/assert_true.h>
#include <stig/expr/as.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>
#include <stig/synth/new_type.h>

using namespace Stig;
using namespace Stig::Synth;

TPostfixCast::TPostfixCast(const TExprFactory *expr_factory, const Package::Syntax::TPostfixCast *postfix_cast)
    : PostfixCast(Base::AssertTrue(postfix_cast)), Lhs(nullptr), Rhs(nullptr) {
  assert(expr_factory);
  try {
    Lhs = expr_factory->NewExpr(PostfixCast->GetExpr());
    Rhs = NewType(PostfixCast->GetType());
  } catch (...) {
    delete Lhs;
    delete Rhs;
    throw;
  }
}

TPostfixCast::~TPostfixCast() {
  assert(this);
  delete Lhs;
  delete Rhs;
}

Expr::TExpr::TPtr TPostfixCast::Build() const {
  assert(this);
  return Expr::TAs::New(Lhs->Build(), Rhs->GetSymbolicType(), GetPosRange(PostfixCast));
}

void TPostfixCast::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Lhs->ForEachInnerScope(cb);
}

void TPostfixCast::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Lhs->ForEachRef(cb);
  Rhs->ForEachRef(cb);
}
