/* <stig/expr/binary.cc>

   Implements <stig/expr/binary.h>

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

#include <stig/expr/binary.h>

#include <stig/pos_range.h>

using namespace Stig;
using namespace Stig::Expr;

// NOTE: Should only be used by TThatableBinary (TReduce and TFilter) and TSort
TBinary::TBinary(const TExpr::TPtr &lhs, const TPosRange &pos_range)
    : TInterior(pos_range), Lhs(Base::AssertTrue(lhs)), Rhs(nullptr) {
  Lhs->SetExprParent(this);
}

TBinary::TBinary(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range)
    : TInterior(pos_range), Lhs(Base::AssertTrue(lhs)), Rhs(Base::AssertTrue(rhs)) {
  Lhs->SetExprParent(this);
  Rhs->SetExprParent(this);
}

TBinary::~TBinary() {
  assert(this);
  if (Lhs) {
    Lhs->UnsetExprParent(this);
  }
  if (Rhs) {
    Rhs->UnsetExprParent(this);
  }
}

const TExpr::TPtr &TBinary::GetLhs() const {
  assert(this);
  return Lhs;
}

const TExpr::TPtr &TBinary::GetRhs() const {
  assert(this);
  assert(Rhs);
  return Rhs;
}

void TBinary::SetRhs(const TExpr::TPtr &rhs) {
  assert(this);
  assert(rhs);
  assert(!Rhs);
  Rhs = rhs;
  Rhs->SetExprParent(this);
}

