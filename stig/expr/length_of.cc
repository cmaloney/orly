/* <stig/expr/length_of.cc>

   Implements <stig/expr/length_of.h>

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

#include <stig/expr/length_of.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/impl.h>
#include <stig/type/unwrap.h>
#include <stig/type/unwrap_visitor.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Expr;

TExpr::TPtr TLengthOf::New(const TExpr::TPtr &container, const TPosRange &pos_range) {
  return TLengthOf::TPtr(new TLengthOf(container, pos_range));
}

TLengthOf::TLengthOf(const TExpr::TPtr &container, const TPosRange &pos_range)
    : TUnary(container, pos_range) {}

void TLengthOf::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TLengthOf::GetType() const {
  class TLengthOfTypeVisitor
      : public Type::TUnwrapVisitor {
    NO_COPY_SEMANTICS(TLengthOfTypeVisitor);
    public:
    TLengthOfTypeVisitor(Type::TType &type, const TPosRange &pos_range)
        : Type::TUnwrapVisitor(type, pos_range) {}
    virtual void operator()(const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *) const {
      Type = Type::TInt::Get();
    }
    virtual void operator()(const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *) const {
      Type = Type::TInt::Get();
    }
    virtual void operator()(const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSet      *) const {
      Type = Type::TInt::Get();
    }
    virtual void operator()(const Type::TStr      *) const {
      Type = Type::TInt::Get();
    }
    virtual void operator()(const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
  };  // TLengthOfTypeVisitor
  assert(this);
  Type::TType type;
  GetExpr()->GetType().Accept(TLengthOfTypeVisitor(type, GetPosRange()));
  return type;
}
