/* <stig/synth/delete_stmt.cc>

   Implements <stig/synth/delete_stmt.h>.

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

#include <stig/synth/delete_stmt.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/symbol/stmt/new_and_delete.h>
#include <stig/symbol/stmt/stmt_arg.h>
#include <stig/synth/expr.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>
#include <stig/synth/new_type.h>

using namespace Stig;
using namespace Stig::Synth;

TDeleteStmt::TDeleteStmt(
    const TExprFactory *expr_factory,
    const Package::Syntax::TDeleteStmt *delete_stmt)
      : DeleteStmt(Base::AssertTrue(delete_stmt)),
        Expr(Base::AssertTrue(expr_factory)->NewExpr(DeleteStmt->GetExpr())),
        ValueType(NewType(DeleteStmt->GetType())) {}

TDeleteStmt::~TDeleteStmt() {
  assert(this);
  delete Expr;
}

Symbol::Stmt::TStmt::TPtr TDeleteStmt::Build() const {
  assert(this);
  return Symbol::Stmt::TDelete::New(Symbol::Stmt::TStmtArg::New(Expr->Build()),
                                    ValueType->GetSymbolicType(),
                                    GetPosRange(DeleteStmt));
}

void TDeleteStmt::ForEachRef(const std::function<void (TAnyRef &)> &cb) const {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachRef(cb);
  ValueType->ForEachRef(cb);
}

void TDeleteStmt::ForEachInnerScope(const std::function<void (TScope *)> &cb) const {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachInnerScope(cb);
}