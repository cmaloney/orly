/* <stig/synth/addr_member_expr.cc>

   Implements <stig/synth/addr_member_expr.h>.

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

#include <stig/synth/addr_member_expr.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/expr/addr_member.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>

using namespace Stig;
using namespace Stig::Synth;

TAddrMemberExpr::TAddrMemberExpr(
    const TExprFactory *expr_factory,
    const Package::Syntax::TPostfixAddrMember *addr_member)
      : AddrMember(Base::AssertTrue(addr_member)),
        Expr(Base::AssertTrue(expr_factory)->NewExpr(AddrMember->GetExpr())),
        Index(AddrMember->GetIntLiteral()->GetLexeme().AsInt()) {}

TAddrMemberExpr::~TAddrMemberExpr() {
  assert(this);
  delete Expr;
}

Expr::TExpr::TPtr TAddrMemberExpr::Build() const {
  assert(this);
  return Expr::TAddrMember::New(Expr->Build(), Index, GetPosRange(AddrMember));
}

void TAddrMemberExpr::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachInnerScope(cb);
}

void TAddrMemberExpr::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachRef(cb);
}
