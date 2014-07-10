/* <orly/expr/addr.cc>

   Implements <orly/expr/addr.h>

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

#include <orly/expr/addr.h>

#include <base/assert_true.h>
#include <orly/error.h>
#include <orly/expr/visitor.h>
#include <orly/pos_range.h>
#include <orly/type/addr.h>
#include <orly/type/util.h>
#include <orly/type/unwrap.h>

using namespace Orly;
using namespace Orly::Expr;

TAddr::TPtr TAddr::New(const TMemberVec &members, const TPosRange &pos_range) {
  return TAddr::TPtr(new TAddr(members, pos_range));
}

TAddr::TAddr(const TMemberVec &members, const TPosRange &pos_range)
    : TCtor(members, pos_range) {}

void TAddr::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TAddr::GetType() const {
  assert(this);
  Type::TAddr::TElems elems;
  bool is_sequence = false;
  for (auto member : GetMembers()) {
    elems.push_back(std::make_pair(member.first, Type::Unwrap(member.second->GetType())));
    is_sequence |= member.second->GetType().Is<Type::TSeq>();
  }
  Type::TType type = Type::TAddr::Get(elems);
  if (is_sequence) {
    type = Type::TSeq::Get(type);
  }
  return type;
}
