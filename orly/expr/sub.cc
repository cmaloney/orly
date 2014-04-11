/* <orly/expr/sub.cc>

   Implements <orly/expr/sub.h>.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/expr/sub.h>

#include <orly/error.h>
#include <orly/expr/visitor.h>
#include <orly/pos_range.h>
#include <orly/type/sub_visitor.h>

using namespace Orly;
using namespace Orly::Expr;

class TSubTypeVisitor
    : public Type::TSubVisitor {
  public:

  TSubTypeVisitor(Type::TType &type, const TPosRange &pos_range)
      : Type::TSubVisitor(type, pos_range) {}

};  // TSubTypeVisitor

TExpr::TPtr TSub::New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range) {
  return TSub::TPtr(new TSub(lhs, rhs, pos_range));
}

TSub::TSub(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range)
    : TBinary(lhs, rhs, pos_range) {}

void TSub::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TSub::GetType() const {
  assert(this);
  Type::TType type;
  Type::TType::Accept(GetLhs()->GetType(), GetRhs()->GetType(), TSubTypeVisitor(type, GetPosRange()));
  return type;
}
