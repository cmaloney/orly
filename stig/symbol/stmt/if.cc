/* <stig/symbol/stmt/if.cc>

   Implements <stig/symbol/stmt/if.h>

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

#include <stig/symbol/stmt/if.h>

#include <stig/error.h>
#include <stig/type/unwrap.h>
#include <stig/type/util.h>

using namespace Stig;
using namespace Stig::Symbol;
using namespace Stig::Symbol::Stmt;

TIfClause::TPtr TIfClause::New(const Expr::TExpr::TPtr &expr, const TStmtBlock::TPtr &stmt_block) {
  return TIfClause::TPtr(new TIfClause(expr, stmt_block));
}

TIfClause::TIfClause(
    const Expr::TExpr::TPtr &expr,
    const TStmtBlock::TPtr &stmt_block)
      : TRoot(expr),
        StmtBlock(Base::AssertTrue(stmt_block)) {}

const TStmtBlock::TPtr &TIfClause::GetStmtBlock() const {
  assert(this);
  return StmtBlock;
}

void TIfClause::TypeCheck() const {
  assert(this);
  if (!Type::UnwrapMutable(GetExpr()->GetType()).Is<Type::TBool>()) {
    throw TExprError(HERE, GetExpr()->GetPosRange(), "if predicates must evaluate to bool.");
  }
  StmtBlock->TypeCheck();
}

TElseClause::TPtr TElseClause::New(const TStmtBlock::TPtr &stmt_block) {
  return TElseClause::TPtr(new TElseClause(stmt_block));
}

TElseClause::TElseClause(const TStmtBlock::TPtr &stmt_block)
    : StmtBlock(Base::AssertTrue(stmt_block)) {}

const TStmtBlock::TPtr &TElseClause::GetStmtBlock() const {
  assert(this);
  return StmtBlock;
}

void TElseClause::TypeCheck() const {
  assert(this);
  StmtBlock->TypeCheck();
}

TIf::TPtr TIf::New(const TIfClauseVec &if_cases, const TElseClause::TPtr &opt_else_case, const TPosRange &pos_range) {
  return TIf::TPtr(new TIf(if_cases, opt_else_case, pos_range));
}

void TIf::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

const TIf::TIfClauseVec &TIf::GetIfClauses() const {
  assert(this);
  return IfClauses;
}

const TElseClause::TPtr &TIf::GetOptElseClause() const {
  assert(this);
  return OptElseClause;
}

void TIf::TypeCheck() const {
  assert(this);
  for (const auto &if_clause : IfClauses) {
    if_clause->TypeCheck();
  }
  if (OptElseClause) {
    OptElseClause->TypeCheck();
  }
}

TIf::TIf(const TIfClauseVec &if_cases, const TElseClause::TPtr &opt_else_case, const TPosRange &pos_range)
    : TStmt(pos_range), IfClauses(if_cases), OptElseClause(opt_else_case) {}
