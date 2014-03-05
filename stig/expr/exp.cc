/* <stig/expr/exp.cc>

   Implements <stig/expr/exp.h>.

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

#include <stig/expr/exp.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/exp_visitor.h>
#include <stig/type/unwrap.h>

using namespace Stig;
using namespace Stig::Expr;

class TExpTypeVisitor
    : public Type::TExpVisitor {
  public:

  TExpTypeVisitor(Type::TType &type, const TPosRange &pos_range)
      : Type::TExpVisitor(type, pos_range) {}

};  // TExpTypeVisitor

TExpr::TPtr TExp::New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range) {
  return TExp::TPtr(new TExp(lhs, rhs, pos_range));
}

TExp::TExp(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range)
    : TBinary(lhs, rhs, pos_range) {}

void TExp::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TExp::GetType() const {
  assert(this);
  Type::TType type;
  Type::TType::Accept(GetLhs()->GetType(), GetRhs()->GetType(), TExpTypeVisitor(type, GetPosRange()));
  return type;
}
