/* <stig/expr/filter.cc>

   Implements <stig/expr/filter.h>

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

#include <stig/expr/filter.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/type/unwrap.h>
#include <stig/type/unwrap_visitor.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Expr;

TFilter::TPtr TFilter::New(const TExpr::TPtr &lhs, const TPosRange &pos_range) {
  return TFilter::TPtr(new TFilter(lhs, pos_range));
}

TFilter::TFilter(const TExpr::TPtr &lhs, const TPosRange &pos_range)
    : TThatableBinary(lhs, pos_range) {}

void TFilter::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TFilter::GetType() const {
  class TFilterTypeVisitor
      : public Type::TUnwrapVisitor {
    NO_COPY_SEMANTICS(TFilterTypeVisitor);
    public:
    TFilterTypeVisitor(Type::TType &type, const TPosRange &pos_range)
        : Type::TUnwrapVisitor(type, pos_range) {}
    virtual void operator()(const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSeq      *that) const {
      Type = that->AsType();
    }
    virtual void operator()(const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
  };  // TFilterTypeVisitor
  assert(this);
  Type::TType type;
  GetLhs()->GetType().Accept(TFilterTypeVisitor(type, GetPosRange()));
  if (Type::Unwrap(GetRhs()->GetType()) != Type::TBool::Get()) {
    Tools::Nycr::TError::TBuilder(GetPosRange())
      << "Predicate expression for filter must evaluate to a bool";
  }
  return type;
}
