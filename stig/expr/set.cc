/* <stig/expr/set.cc>

   Implements <stig/expr/set.h>

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

#include <stig/expr/set.h>

#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/seq.h>
#include <stig/type/set.h>
#include <stig/type/unwrap.h>
#include <stig/type/util.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Expr;

TSet::TPtr TSet::New(const Type::TType &type, const TPosRange &pos_range) {
  return TSet::TPtr(new TSet(type, pos_range));
}

TSet::TPtr TSet::New(const TExprSet &exprs, const TPosRange &pos_range) {
  return TSet::TPtr(new TSet(exprs, pos_range));
}

/* Empty contructor */
TSet::TSet(const Type::TType &type, const TPosRange &pos_range)
    : TCtor(pos_range), Type(type) {}

/* Non-empty constructor */
TSet::TSet(const TExprSet &exprs, const TPosRange &pos_range)
    : TCtor(exprs, pos_range) {
  assert(!GetExprs().empty());
}

void TSet::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TSet::GetType() const {
  assert(this);
  if (!Type) {
    Type::TType elem = Type::Unwrap((*(GetExprs().begin()))->GetType());
    bool is_sequence = false;
    for (auto expr : GetExprs()) {
      if (Type::Unwrap(expr->GetType()) != elem) {
        Tools::Nycr::TError::TBuilder(expr->GetPosRange())
          << "A set constructor's elements must be of equal type.";
      }
      is_sequence |= expr->GetType().Is<Type::TSeq>();
    }
    Type = Type::TSet::Get(elem);
    if (is_sequence) {
      Type = Type::TSeq::Get(Type);
    }
    assert(Type);
  }
  return Type;
}
