/* <stig/symbol/test/with_clause.cc>

   Implements <stig/symbol/test/with_clause.h>

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

#include <stig/symbol/test/with_clause.h>

using namespace Stig;
using namespace Stig::Symbol;
using namespace Stig::Symbol::Test;

TWithClause::TPtr TWithClause::New(const TNewStmtSet &new_stmts, const TPosRange &pos_range) {
  return TWithClause::TPtr(new TWithClause(new_stmts, pos_range));
}

TWithClause::TWithClause(const TNewStmtSet &new_stmts, const TPosRange &pos_range)
    : NewStmts(new_stmts), PosRange(pos_range) {}

const TWithClause::TNewStmtSet &TWithClause::GetNewStmts() const {
  assert(this);
  return NewStmts;
}

const TPosRange &TWithClause::GetPosRange() const {
  assert(this);
  return PosRange;
}

void TWithClause::TypeCheck() const {
  assert(this);
  for (const auto &new_stmt : NewStmts) {
    new_stmt->TypeCheck();
  }
}
