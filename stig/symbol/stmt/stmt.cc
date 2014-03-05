/* <stig/symbol/stmt/stmt.cc>

   Implements <stig/symbol/stmt/stmt.h>

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

#include <stig/symbol/stmt/stmt.h>

#include <base/assert_true.h>

using namespace Stig;
using namespace Stig::Symbol;
using namespace Stig::Symbol::Stmt;

TStmt::TStmt(const TPosRange &pos_range)
    : PosRange(pos_range), StmtBlock(nullptr) {}

TStmt::~TStmt() {}

const TPosRange &TStmt::GetPosRange() const {
  assert(this);
  return PosRange;
}

const TStmtBlock *TStmt::GetStmtBlock() const {
  assert(this);
  assert(StmtBlock);
  return StmtBlock;
}

void TStmt::SetStmtBlock(const TStmtBlock *stmt_block) {
  assert(this);
  assert(stmt_block);
  assert(!StmtBlock);
  StmtBlock = stmt_block;
}

void TStmt::UnsetStmtBlock(const TStmtBlock *stmt_block) {
  assert(this);
  assert(stmt_block);
  assert(StmtBlock == stmt_block);
  StmtBlock = nullptr;
}
