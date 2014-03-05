/* <stig/expr/time_obj.cc>

   Implements <stig/expr/time_obj.h>

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

#include <stig/expr/time_obj.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/impl.h>
#include <stig/type/obj.h>
#include <stig/type/unwrap.h>
#include <stig/type/unwrap_visitor.h>
#include <tools/nycr/error.h>
#include <base/time_maps.h>

using namespace Stig;
using namespace Stig::Expr;

TExpr::TPtr TTimeObj::New(const TExpr::TPtr &expr, const TPosRange &pos_range) {
  return TTimeObj::TPtr(new TTimeObj(expr, pos_range));
}

TTimeObj::TTimeObj(const TExpr::TPtr &expr, const TPosRange &pos_range)
    : TUnary(expr, pos_range) {}

void TTimeObj::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TTimeObj::GetType() const {
  class TTimeObjTypeVisitor
      : public Type::TUnwrapVisitor {
    NO_COPY_SEMANTICS(TTimeObjTypeVisitor);
    public:
    TTimeObjTypeVisitor(Type::TType &type, const TPosRange &pos_range)
        : Type::TUnwrapVisitor(type, pos_range) {}
    virtual void operator()(const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSeq      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimeDiff *) const {
      Type = Type::TObj::Get(*Base::Chrono::TimeDiffMap);
    }
    virtual void operator()(const Type::TTimePnt  *) const {
      Type = Type::TObj::Get(*Base::Chrono::TimePntMap);
    }
  };  // TTimeObjTypeVisitor
  assert(this);
  Type::TType type;
  GetExpr()->GetType().Accept(TTimeObjTypeVisitor(type, GetPosRange()));
  return type;
}
