/* <stig/synth/stmt_block.cc>

   Implements <stig/synth/stmt_block.h>.

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

#include <stig/synth/stmt_block.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/synth/cst_utils.h>
#include <stig/synth/delete_stmt.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/if_stmt.h>
#include <stig/synth/mutate_stmt.h>
#include <stig/synth/new_stmt.h>
#include <stig/synth/stmt.h>

using namespace Stig;
using namespace Stig::Synth;

TStmtBlock::TStmtBlock(const TExprFactory *expr_factory, const Package::Syntax::TStmtBlock *stmt_block)
    : StmtBlock(Base::AssertTrue(stmt_block)) {
  class TStmtVisitor
      : public Package::Syntax::TStmt::TVisitor {
    public:
    TStmtVisitor(const TExprFactory *expr_factory, TStmtVec &stmts)
        : ExprFactory(expr_factory), Stmts(stmts) {}
    virtual void operator()(const Package::Syntax::TBadStmt    *) const { /* DO NOTHING */ }
    virtual void operator()(const Package::Syntax::TNewStmt    *that) const { Stmts.emplace_back(new TNewStmt(ExprFactory, that));    }
    virtual void operator()(const Package::Syntax::TDeleteStmt *that) const { Stmts.emplace_back(new TDeleteStmt(ExprFactory, that)); }
    virtual void operator()(const Package::Syntax::TIfStmt     *that) const { Stmts.emplace_back(new TIfStmt(ExprFactory, that));     }
    virtual void operator()(const Package::Syntax::TMutateStmt *that) const { Stmts.emplace_back(new TMutateStmt(ExprFactory, that)); }
    private:
    const TExprFactory *ExprFactory;
    TStmtVec &Stmts;
  };
  assert(expr_factory);
  try {
    TStmtVisitor visitor(expr_factory, Stmts);
    ForEach<Package::Syntax::TStmt>(StmtBlock->GetOptStmtSeq(),
        [&visitor](const Package::Syntax::TStmt *stmt) -> bool {
          stmt->Accept(visitor);
          return true;
        });
  } catch (...) {
    Cleanup();
    throw;
  }
}

TStmtBlock::~TStmtBlock() {
  Cleanup();
}

Symbol::Stmt::TStmtBlock::TPtr TStmtBlock::Build() const {
  assert(this);
  Symbol::Stmt::TStmtBlock::TStmtVec stmts;
  for (auto stmt : Stmts) {
    stmts.emplace_back(stmt->Build());
  }
  return Symbol::Stmt::TStmtBlock::New(stmts, GetPosRange(StmtBlock));
}

void TStmtBlock::Cleanup() {
  assert(this);
  for (auto stmt : Stmts) {
    delete stmt;
  }
}

void TStmtBlock::ForEachRef(const std::function<void (TAnyRef &)> &cb) const {
  assert(this);
  for (auto stmt : Stmts) {
    stmt->ForEachRef(cb);
  }
}