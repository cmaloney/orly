/* <stig/symbol/stmt/binary.cc>

   Implements <stig/symbol/stmt/binary.h>

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

#include <stig/symbol/stmt/binary.h>

#include <base/assert_true.h>

using namespace Stig;
using namespace Stig::Symbol;
using namespace Stig::Symbol::Stmt;

TBinary::TBinary(
    const TStmtArg::TPtr &lhs,
    const TStmtArg::TPtr &rhs,
    const TPosRange &pos_range)
      : TStmt(pos_range),
        Lhs(Base::AssertTrue(lhs)),
        Rhs(Base::AssertTrue(rhs)) {
  Lhs->SetStmt(this);
  Rhs->SetStmt(this);
}

TBinary::~TBinary() {
  assert(this);
  Lhs->UnsetStmt(this);
  Rhs->UnsetStmt(this);
}

const TStmtArg::TPtr &TBinary::GetLhs() const {
  assert(this);
  return Lhs;
}

const TStmtArg::TPtr &TBinary::GetRhs() const {
  assert(this);
  return Rhs;
}
