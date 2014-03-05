/* <stig/expr/read.cc>

   Implements <stig/expr/read.h>

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

#include <stig/expr/read.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/mutable.h>
#include <stig/type/unwrap.h>
#include <stig/type/unwrap_visitor.h>

using namespace Stig;
using namespace Stig::Expr;

TRead::TPtr TRead::New(const TExpr::TPtr &expr, const Type::TType &type, const TPosRange &pos_range) {
  return TRead::TPtr(new TRead(expr, type, pos_range));
}

TRead::TRead(const TExpr::TPtr &expr, const Type::TType &type, const TPosRange &pos_range)
    : TUnary(expr, pos_range), Type(type) {}

void TRead::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TRead::GetType() const {
  class TReadTypeVisitor
      : public Type::TUnwrapVisitor {
    NO_COPY_SEMANTICS(TReadTypeVisitor);
    public:
    TReadTypeVisitor(Type::TType &type, const Type::TType &as_type, const TPosRange &pos_range)
        : Type::TUnwrapVisitor(type, pos_range), AsType(as_type) {}
    virtual void operator()(const Type::TAddr     *that) const {
      Type = Type::TMutable::Get(that->AsType(), AsType, AsType);
    }
    virtual void operator()(const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
    private:
    const Type::TType &AsType;
  };  // TReadTypeVisitor
  assert(this);
  Type::TType type;
  GetExpr()->GetType().Accept(TReadTypeVisitor(type, Type, GetPosRange()));
  return type;
}
