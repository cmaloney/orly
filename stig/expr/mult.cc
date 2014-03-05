/* <stig/expr/mult.cc>

   Implements <stig/expr/mult.h>.

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

#include <stig/expr/mult.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/mult_visitor.h>
#include <stig/type/unwrap.h>

using namespace Stig;
using namespace Stig::Expr;

class TMultTypeVisitor
    : public Type::TMultVisitor {
  public:

  TMultTypeVisitor(Type::TType &type, const TPosRange &pos_range)
      : Type::TMultVisitor(type, pos_range) {}

};  // TMultTypeVisitor

TExpr::TPtr TMult::New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range) {
  return TMult::TPtr(new TMult(lhs, rhs, pos_range));
}

TMult::TMult(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range)
    : TBinary(lhs, rhs, pos_range) {}

void TMult::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TMult::GetType() const {
  assert(this);
  Type::TType type;
  Type::TType::Accept(GetLhs()->GetType(), GetRhs()->GetType(), TMultTypeVisitor(type, GetPosRange()));
  return type;
}
