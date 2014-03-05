/* <stig/symbol/stmt/unary.cc>

   Implements <stig/symbol/stmt/unary.h>

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

#include <stig/symbol/stmt/unary.h>

#include <base/assert_true.h>

using namespace Stig;
using namespace Stig::Symbol;
using namespace Stig::Symbol::Stmt;

TUnary::TUnary(const TStmtArg::TPtr &stmt_arg, const TPosRange &pos_range)
    : TStmt(pos_range), StmtArg(Base::AssertTrue(stmt_arg)) {
  StmtArg->SetStmt(this);
}

TUnary::~TUnary() {
  assert(this);
  StmtArg->UnsetStmt(this);
}

const TStmtArg::TPtr &TUnary::GetStmtArg() const {
  assert(this);
  return StmtArg;
}
