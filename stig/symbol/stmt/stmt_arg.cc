/* <stig/symbol/stmt/stmt_arg.cc>

   Implements <stig/symbol/stmt/stmt_arg.h>

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

#include <stig/symbol/stmt/stmt_arg.h>

using namespace Stig;
using namespace Stig::Symbol;
using namespace Stig::Symbol::Stmt;

TStmtArg::TPtr TStmtArg::New(const Expr::TExpr::TPtr &expr) {
  return TStmtArg::TPtr(new TStmtArg(expr));
}

TStmtArg::TStmtArg(const Expr::TExpr::TPtr &expr)
    : TRoot(expr), Stmt(nullptr) {}

const TStmt *TStmtArg::GetStmt() const {
  assert(this);
  assert(Stmt);
  return Stmt;
}

void TStmtArg::SetStmt(const TStmt *stmt) {
  assert(this);
  assert(stmt);
  assert(!Stmt);
  Stmt = stmt;
}

void TStmtArg::UnsetStmt(const TStmt *stmt) {
  assert(this);
  assert(stmt);
  assert(Stmt == stmt);
  Stmt = nullptr;
}
