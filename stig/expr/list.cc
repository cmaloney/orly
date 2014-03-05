/* <stig/expr/list.cc>

   Implements <stig/expr/list.h>

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

#include <stig/expr/list.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/list.h>
#include <stig/type/seq.h>
#include <stig/type/unwrap.h>
#include <stig/type/util.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Expr;

TList::TPtr TList::New(const Type::TType &type, const TPosRange &pos_range) {
  return TList::TPtr(new TList(type, pos_range));
}

TList::TPtr TList::New(const TExprVec &exprs, const TPosRange &pos_range) {
  return TList::TPtr(new TList(exprs, pos_range));
}

/* Empty contructor */
TList::TList(const Type::TType &type, const TPosRange &pos_range)
    : TCtor(pos_range), Type(type) {}

/* Non-empty constructor */
TList::TList(const TExprVec &exprs, const TPosRange &pos_range)
    : TCtor(exprs, pos_range) {
  assert(!GetExprs().empty());
}

void TList::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TList::GetType() const {
  assert(this);
  if (!Type) {
    Type::TType elem = Type::Unwrap(GetExprs().front()->GetType());
    bool is_sequence = false;
    for (auto expr : GetExprs()) {
      if (Type::Unwrap(expr->GetType()) != elem) {
        Tools::Nycr::TError::TBuilder(expr->GetPosRange())
          << "A list constructor's elements must be of equal type.";
      }
      is_sequence |= expr->GetType().Is<Type::TSeq>();
    }
    Type = Type::TList::Get(elem);
    if (is_sequence) {
      Type = Type::TSeq::Get(Type);
    }
    assert(Type);
  }
  return Type;
}
