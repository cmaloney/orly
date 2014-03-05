/* <stig/synth/if_stmt.cc>

   Implements <stig/synth/if_stmt.h>.

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

#include <stig/synth/if_stmt.h>

#include <cassert>

#include <base/assert_true.h>
#include <base/opt.h>
#include <stig/synth/cst_utils.h>
#include <stig/synth/expr.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/stmt_block.h>

using namespace Stig;
using namespace Stig::Synth;

TIfStmt::TIfClause::~TIfClause() {
  assert(this);
  delete Expr;
  delete StmtBlock;
}

Symbol::Stmt::TIfClause::TPtr TIfStmt::TIfClause::Build() const {
  assert(this);
  return Symbol::Stmt::TIfClause::New(Expr->Build(), StmtBlock->Build());
}

void TIfStmt::TIfClause::ForEachRef(const std::function<void (TAnyRef &)> &cb) const {
  assert(this);
  Expr->ForEachRef(cb);
  StmtBlock->ForEachRef(cb);
}

void TIfStmt::TIfClause::ForEachInnerScope(const std::function<void (TScope *)> &cb) const {
  assert(this);
  Expr->ForEachInnerScope(cb);
  for(auto &stmt : StmtBlock->GetStmts()) {
    stmt->ForEachInnerScope(cb);
  }
}

TIfStmt::TIfStmt(const TExprFactory *expr_factory, const Package::Syntax::TIfStmt *if_stmt)
    : IfStmt(Base::AssertTrue(if_stmt)), OptElseBlock(nullptr) {
  try {
    IfClauses.emplace_back(new TIfClause(expr_factory, IfStmt));
    ForEach<Package::Syntax::TElseIfClause>(IfStmt->GetOptElseIfClauseSeq(),
        [this, expr_factory](const Package::Syntax::TElseIfClause *else_if_clause) -> bool {
          IfClauses.emplace_back(new TIfClause(expr_factory, else_if_clause));
          return true;
        });
    auto else_clause = TryGetNode<Package::Syntax::TElseClause,
                                  Package::Syntax::TNoElseClause>(IfStmt->GetOptElseClause());
    if (else_clause) {
      OptElseBlock = new TStmtBlock(expr_factory, else_clause->GetStmtBlock());
    }
  } catch (...) {
    Cleanup();
    throw;
  }
}

TIfStmt::~TIfStmt() {
  Cleanup();
}

Symbol::Stmt::TStmt::TPtr TIfStmt::Build() const {
  assert(this);
  Symbol::Stmt::TIf::TIfClauseVec if_clauses;
  for (auto if_clause : IfClauses) {
    if_clauses.emplace_back(if_clause->Build());
  }
  return Symbol::Stmt::TIf::New(
             if_clauses,
             OptElseBlock ? Symbol::Stmt::TElseClause::New(OptElseBlock->Build()) : nullptr,
             GetPosRange(IfStmt));
}

void TIfStmt::Cleanup() {
  assert(this);
  for (auto if_clause : IfClauses) {
    delete if_clause;
  }
  delete OptElseBlock;
}

void TIfStmt::ForEachRef(const std::function<void (TAnyRef &)> &cb) const {
  assert(this);
  assert(&cb);
  assert(cb);
  for (auto if_clause : IfClauses) {
    if_clause->ForEachRef(cb);
  }
  if (OptElseBlock) {
    OptElseBlock->ForEachRef(cb);
  }
}

void TIfStmt::ForEachInnerScope(const std::function<void (TScope *)> &cb) const {
  assert(this);
  assert(&cb);
  assert(cb);
  for (auto if_clause : IfClauses) {
    if_clause->ForEachInnerScope(cb);
  }
  if (OptElseBlock) {
    for(auto &stmt : OptElseBlock->GetStmts()) {
      stmt->ForEachInnerScope(cb);
    }
  }
}