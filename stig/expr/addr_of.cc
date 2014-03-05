/* <stig/expr/addr_of.cc>

   Implements <stig/expr/addr_of.h>

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

#include <stig/expr/addr_of.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/impl.h>
#include <stig/type/unwrap.h>
#include <stig/type/unwrap_visitor.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Expr;

TExpr::TPtr TAddrOf::New(const TExpr::TPtr &expr, const TPosRange &pos_range) {
  return TAddrOf::TPtr(new TAddrOf(expr, pos_range));
}

TAddrOf::TAddrOf(const TExpr::TPtr &expr, const TPosRange &pos_range)
  : TUnary(expr, pos_range) {}

void TAddrOf::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TAddrOf::GetType() const {
  class TAddrOfTypeVisitor
      : public Type::TUnwrapVisitor {
    NO_COPY_SEMANTICS(TAddrOfTypeVisitor);
    public:
    TAddrOfTypeVisitor(Type::TType &type, const TPosRange &pos_range)
        : Type::TUnwrapVisitor(type, pos_range) {}
    virtual void operator()(const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TMutable  *that) const {
      Type = Type::TOpt::Get(that->GetAddr());
    }
    virtual void operator()(const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TOpt      *that) const {
      that->GetElem().Accept(*this);
      /* Don't rewrap optional here since if we have an optional mutable,
         it would result in a optional of optional which is disallowed */
    }
    virtual void operator()(const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
  };  // TAddrOfTypeVisitor
  assert(this);
  Type::TType type;
  GetExpr()->GetType().Accept(TAddrOfTypeVisitor(type, GetPosRange()));
  return type;
}
