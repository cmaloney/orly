/* <stig/expr/effect.cc>

   Implements <stig/expr/effect.h>

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

#include <stig/expr/effect.h>

using namespace Stig;
using namespace Stig::Expr;

TEffect::TPtr TEffect::New(const TExpr::TPtr &expr, const TPosRange &pos_range) {
  return TEffect::TPtr(new TEffect(expr, pos_range));
}

TEffect::TEffect(const TExpr::TPtr &expr, const TPosRange &pos_range)
    : TThatableUnary(expr, pos_range), StmtBlock(nullptr) {}

void TEffect::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

const Symbol::Stmt::TStmtBlock::TPtr &TEffect::GetStmtBlock() const {
  assert(this);
  assert(StmtBlock);
  return StmtBlock;
}

Type::TType TEffect::GetType() const {
  assert(this);
  assert(StmtBlock);
  StmtBlock->TypeCheck();
  return GetExpr()->GetType();
}

void TEffect::SetStmtBlock(const Symbol::Stmt::TStmtBlock::TPtr &stmt_block) {
  assert(this);
  assert(stmt_block);
  assert(!StmtBlock);
  StmtBlock = stmt_block;
}
