/* <stig/expr/dict.cc>

   Implements <stig/expr/dict.h>

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

#include <stig/expr/dict.h>

#include <base/assert_true.h>
#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/dict.h>
#include <stig/type/seq.h>
#include <stig/type/unwrap.h>
#include <stig/type/util.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Expr;

TDict::TPtr TDict::New(const Type::TType &type, const TPosRange &pos_range) {
  return TDict::TPtr(new TDict(type, pos_range));
}

TDict::TPtr TDict::New(const TMemberMap &members, const TPosRange &pos_range) {
  return TDict::TPtr(new TDict(members, pos_range));
}

/* Empty constructor */
TDict::TDict(const Type::TType &type, const TPosRange &pos_range)
    : TCtor(pos_range), Type(type) {}

/* Non-empty constructor */
TDict::TDict(const TMemberMap &members, const TPosRange &pos_range)
    : TCtor(members, pos_range) {
  assert(!GetMembers().empty());
}

void TDict::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TDict::GetType() const {
  assert(this);
  if (!Type) {
    Type::TType key_type = Type::Unwrap(GetMembers().begin()->first->GetType());
    Type::TType val_type = Type::Unwrap(GetMembers().begin()->second->GetType());
    bool is_sequence = false;
    for (auto member : GetMembers()) {
      if (Type::Unwrap(member.first->GetType()) != key_type) {
        Tools::Nycr::TError::TBuilder((member.first)->GetPosRange())
          << "A dictionary constructor's keys must have the same type.";
      }
      if (Type::Unwrap(member.second->GetType()) != val_type) {
        Tools::Nycr::TError::TBuilder((member.second)->GetPosRange())
          << "A dictionary constructor's vals must have the same type.";
      }
      is_sequence |= member.first->GetType().Is<Type::TSeq>() || member.second->GetType().Is<Type::TSeq>();
    }
    Type = Type::TDict::Get(key_type, val_type);
    if (is_sequence) {
      Type = Type::TSeq::Get(Type);
    }
    assert(Type);
  }
  return Type;
}
