/* <orly/synth/new_stmt.cc>

   Implements <orly/synth/new_stmt.h>.

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

#include <orly/synth/new_stmt.h>

#include <cassert>

#include <base/assert_true.h>
#include <orly/symbol/stmt/new_and_delete.h>
#include <orly/symbol/stmt/stmt_arg.h>
#include <orly/synth/expr.h>
#include <orly/synth/get_pos_range.h>
#include <orly/synth/new_expr.h>

using namespace Orly;
using namespace Orly::Synth;

TNewStmt::TNewStmt(const TExprFactory *expr_factory, const Package::Syntax::TNewStmt *new_stmt)
    : NewStmt(Base::AssertTrue(new_stmt)), Lhs(nullptr), Rhs(nullptr) {
  assert(expr_factory);
  try {
    Lhs = expr_factory->NewExpr(NewStmt->GetLhs());
    Rhs = expr_factory->NewExpr(NewStmt->GetRhs());
  } catch (...) {
    delete Lhs;
    delete Rhs;
    throw;
  }
}

TNewStmt::~TNewStmt() {
  assert(this);
  delete Lhs;
  delete Rhs;
}

Symbol::Stmt::TStmt::TPtr TNewStmt::Build() const {
  assert(this);
  return Symbol::Stmt::TNew::New(
             Symbol::Stmt::TStmtArg::New(Lhs->Build()),
             Symbol::Stmt::TStmtArg::New(Rhs->Build()),
             GetPosRange(NewStmt));
}

void TNewStmt::ForEachRef(const std::function<void (TAnyRef &)> &cb) const {
  assert(this);
  assert(&cb);
  assert(cb);
  Lhs->ForEachRef(cb);
  Rhs->ForEachRef(cb);
}

void TNewStmt::ForEachInnerScope(const std::function<void (TScope *)> &cb) const {
  assert(this);
  assert(&cb);
  assert(cb);
  Lhs->ForEachInnerScope(cb);
  Rhs->ForEachInnerScope(cb);
}