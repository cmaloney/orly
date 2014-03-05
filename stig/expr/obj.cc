/* <stig/expr/obj.cc>

   Implements <stig/expr/obj.h>

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

#include <stig/expr/obj.h>

#include <stig/error.h>
#include <stig/type/obj.h>
#include <stig/type/seq.h>
#include <stig/type/unwrap.h>
#include <stig/type/util.h>

using namespace Stig;
using namespace Stig::Expr;

TObj::TPtr TObj::New(const TMemberMap &members, const TPosRange &pos_range) {
  return TObj::TPtr(new TObj(members, pos_range));
}

TObj::TObj(const TMemberMap &members, const TPosRange &pos_range)
    : TCtor(members, pos_range) {}

void TObj::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TObj::GetType() const {
  assert(this);
  Type::TObj::TElems elem_map;
  bool is_sequence = false;
  for (auto member : GetMembers()) {
    auto result = elem_map.insert(make_pair(member.first, Type::Unwrap((member.second)->GetType())));
    assert(result.second);
    is_sequence |= member.second->GetType().Is<Type::TSeq>();
  }
  Type::TType type = Type::TObj::Get(elem_map);
  if (is_sequence) {
    type = Type::TSeq::Get(type);
  }
  return type;
}
