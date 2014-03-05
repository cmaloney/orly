/* <stig/synth/obj_member_expr.cc>

   Implements <stig/synth/obj_member_expr.h>.

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

#include <stig/synth/obj_member_expr.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/expr/obj_member.h>
#include <stig/pos_range.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>

using namespace Stig;
using namespace Stig::Synth;

TObjMemberExpr::TObjMemberExpr(const TExprFactory *expr_factory, const Package::Syntax::TPostfixObjMember *postfix_obj_member)
    : PostfixObjMember(Base::AssertTrue(postfix_obj_member)),
      Expr(Base::AssertTrue(expr_factory)->NewExpr(PostfixObjMember->GetExpr())),
      Name(PostfixObjMember->GetName()) {}

TObjMemberExpr::~TObjMemberExpr() {
  assert(this);
  delete Expr;
}

Expr::TExpr::TPtr TObjMemberExpr::Build() const {
  assert(this);
  return Expr::TObjMember::New(Expr->Build(), Name.GetText(), GetPosRange(PostfixObjMember));
}

void TObjMemberExpr::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachInnerScope(cb);
}

void TObjMemberExpr::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachRef(cb);
}
