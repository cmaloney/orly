/* <stig/expr/literal.cc>

   Implements <stig/expr/literal.h>

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

#include <stig/expr/literal.h>

#include <stig/expr/visitor.h>
#include <stig/pos_range.h>

using namespace Stig;
using namespace Stig::Expr;

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

Type::TType TLiteral::GetType() const {
  assert(this);
  return Val.GetType();
}

const Var::TVar &TLiteral::GetVal() const {
  assert(this);
  return Val;
}

