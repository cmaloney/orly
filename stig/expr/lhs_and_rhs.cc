/* <stig/expr/lhs_and_rhs.cc>

   Implements <stig/expr/lhs_and_rhs.h>

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

#include <stig/expr/lhs_and_rhs.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/unwrap.h>
#include <stig/type/unwrap_visitor.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Expr;

TLhs::TPtr TLhs::New(const TLhsRhsable::TPtr &lhsrhsable, const TPosRange &pos_range) {
  return TLhs::TPtr(new TLhs(lhsrhsable.get(), pos_range));
}

TLhs::TLhs(const TLhsRhsable *lhsrhsable, const TPosRange &pos_range)
    : TLeaf(pos_range), LhsRhsable(Base::AssertTrue(lhsrhsable)) {}

void TLhs::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TLhs::GetType() const {
  assert(this);
  return LhsRhsable->GetLhsRhsType();
}

const TLhsRhsable *TLhs::GetLhsRhsable() const {
  assert(this);
  return LhsRhsable;
}

TRhs::TPtr TRhs::New(const TLhsRhsable::TPtr &lhsrhsable, const TPosRange &pos_range) {
  return TRhs::TPtr(new TRhs(lhsrhsable.get(), pos_range));
}

TRhs::TRhs(const TLhsRhsable *lhsrhsable, const TPosRange &pos_range)
    : TLeaf(pos_range), LhsRhsable(Base::AssertTrue(lhsrhsable)) {}

void TRhs::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TRhs::GetType() const {
  assert(this);
  return LhsRhsable->GetLhsRhsType();
}

const TLhsRhsable *TRhs::GetLhsRhsable() const {
  assert(this);
  return LhsRhsable;
}
