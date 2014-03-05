/* <stig/symbol/stmt/stmt_block.cc>

   Implements <stig/symbol/stmt/stmt_block.h>

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

#include <stig/symbol/stmt/stmt_block.h>

using namespace Stig;
using namespace Stig::Symbol;
using namespace Stig::Symbol::Stmt;

TStmtBlock::TPtr TStmtBlock::New(const TStmtVec &stmts, const TPosRange &pos_range) {
  return TPtr(new TStmtBlock(stmts, pos_range));
}

TStmtBlock::TStmtBlock(const TStmtVec &stmts, const TPosRange &pos_range)
    : PosRange(pos_range), Stmts(stmts) {
  for (auto stmt : Stmts) {
    stmt->SetStmtBlock(this);
  }
}

TStmtBlock::~TStmtBlock() {
  assert(this);
  for (auto stmt : Stmts) {
    stmt->UnsetStmtBlock(this);
  }
}

const TPosRange &TStmtBlock::GetPosRange() const {
  assert(this);
  return PosRange;
}

const TStmtBlock::TStmtVec &TStmtBlock::GetStmts() const {
  assert(this);
  return Stmts;
}

void TStmtBlock::TypeCheck() const {
  assert(this);
  for (const auto &stmt : Stmts) {
    stmt->TypeCheck();
  }
}
