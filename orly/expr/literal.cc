/* <orly/expr/literal.cc>

   Implements <orly/expr/literal.h>

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

#include <orly/expr/literal.h>

#include <orly/expr/visitor.h>
#include <orly/pos_range.h>

using namespace Orly;
using namespace Orly::Expr;

TLiteral::TPtr TLiteral::New(const Var::TVar &val, const TPosRange &pos_range) {
  return TLiteral::TPtr(new TLiteral(val, pos_range));
}

TLiteral::TLiteral(const Var::TVar &val, const TPosRange &pos_range)
    : TLeaf(pos_range), Val(val) {}

void TLiteral::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TLiteral::GetTypeImpl() const {
  assert(this);
  return Val.GetType();
}

const Var::TVar &TLiteral::GetVal() const {
  assert(this);
  return Val;
}

