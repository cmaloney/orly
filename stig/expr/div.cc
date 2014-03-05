/* <stig/expr/div.cc>

   Implements <stig/expr/div.h>.

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

#include <stig/expr/div.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/div_visitor.h>
#include <stig/type/unwrap.h>

using namespace Stig;
using namespace Stig::Expr;

class TDivTypeVisitor
    : public Type::TDivVisitor {
  public:

  TDivTypeVisitor(Type::TType &type, const TPosRange &pos_range)
      : TDivVisitor(type, pos_range) {}

};  // TDivTypeVisitor

TExpr::TPtr TDiv::New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range) {
  return TDiv::TPtr(new TDiv(lhs, rhs, pos_range));
}

TDiv::TDiv(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range)
    : TBinary(lhs, rhs, pos_range) {}

void TDiv::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TDiv::GetType() const {
  assert(this);
  Type::TType type;
  Type::TType::Accept(GetLhs()->GetType(), GetRhs()->GetType(), TDivTypeVisitor(type, GetPosRange()));
  return type;
}
