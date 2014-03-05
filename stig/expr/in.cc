/* <stig/expr/in.cc>

   Implements <stig/expr/in.h>.

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

#include <stig/expr/in.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/infix_visitor.h>
#include <stig/type/unwrap.h>

using namespace Stig;
using namespace Stig::Expr;

TExpr::TPtr TIn::New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range) {
  return TIn::TPtr(new TIn(lhs, rhs, pos_range));
}

TIn::TIn(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range)
    : TBinary(lhs, rhs, pos_range) {}

void TIn::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TIn::GetType() const {
  class TInTypeVisitor
      : public Type::TInfixVisitor {
    NO_COPY_SEMANTICS(TInTypeVisitor);
    public:
    TInTypeVisitor(Type::TType &type, const TPosRange &pos_range)
        : Type::TInfixVisitor(type, pos_range) {}
    virtual void operator()(const Type::TAddr     *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TAddr     *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TAddr     *lhs, const Type::TDict     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TAddr     *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TAddr     *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TAddr     *lhs, const Type::TList     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TAddr     *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TAddr     *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TAddr     *lhs, const Type::TSet      *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TAddr     *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TAddr     *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TAddr     *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TBool     *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TBool     *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TBool     *lhs, const Type::TDict     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TBool     *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TBool     *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TBool     *lhs, const Type::TList     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TBool     *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TBool     *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TBool     *lhs, const Type::TSet      *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TBool     *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TBool     *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TBool     *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *lhs, const Type::TDict     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TDict     *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *lhs, const Type::TList     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TDict     *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *lhs, const Type::TSet      *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TDict     *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TId       *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TId       *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TId       *lhs, const Type::TDict     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TId       *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TId       *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TId       *lhs, const Type::TList     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TId       *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TId       *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TId       *lhs, const Type::TSet      *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TId       *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TId       *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TId       *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *lhs, const Type::TDict     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TInt      *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *lhs, const Type::TList     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TInt      *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *lhs, const Type::TSet      *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TInt      *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *lhs, const Type::TDict     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TList     *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *lhs, const Type::TList     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TList     *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *lhs, const Type::TSet      *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TList     *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TObj      *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TObj      *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TObj      *lhs, const Type::TDict     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TObj      *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TObj      *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TObj      *lhs, const Type::TList     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TObj      *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TObj      *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TObj      *lhs, const Type::TSet      *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TObj      *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TObj      *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TObj      *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TReal     *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TReal     *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TReal     *lhs, const Type::TDict     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TReal     *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TReal     *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TReal     *lhs, const Type::TList     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TReal     *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TReal     *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TReal     *lhs, const Type::TSet      *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TReal     *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TReal     *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TReal     *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSet      *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSet      *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSet      *lhs, const Type::TDict     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TSet      *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSet      *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSet      *lhs, const Type::TList     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TSet      *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSet      *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSet      *lhs, const Type::TSet      *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TSet      *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSet      *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSet      *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TStr      *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TStr      *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TStr      *lhs, const Type::TDict     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TStr      *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TStr      *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TStr      *lhs, const Type::TList     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TStr      *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TStr      *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TStr      *lhs, const Type::TSet      *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TStr      *, const Type::TStr      *) const {
      Type = Type::TBool::Get();
    }
    virtual void operator()(const Type::TStr      *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TStr      *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimeDiff *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimeDiff *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimeDiff *lhs, const Type::TDict     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TTimeDiff *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimeDiff *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimeDiff *lhs, const Type::TList     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TTimeDiff *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimeDiff *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimeDiff *lhs, const Type::TSet      *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TTimeDiff *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimeDiff *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimeDiff *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimePnt  *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimePnt  *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimePnt  *lhs, const Type::TDict     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TTimePnt  *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimePnt  *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimePnt  *lhs, const Type::TList     *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TTimePnt  *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimePnt  *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimePnt  *lhs, const Type::TSet      *rhs) const {
      GetType(lhs->AsType(), rhs);
    }
    virtual void operator()(const Type::TTimePnt  *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimePnt  *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimePnt  *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
    private:
    void GetType(const Type::TType &lhs, const Type::TDict *rhs) const {
      if (lhs != Type::UnwrapOptional(Type::Unwrap(rhs->GetKey()))) {
        throw TExprError(HERE, PosRange,
            "Type of the expression on the left does not match the type of the keys in the dictionary");
      }
      Type = Type::TBool::Get();
    }
    void GetType(const Type::TType &lhs, const Type::TList *rhs) const {
      if (lhs != Type::UnwrapOptional(Type::Unwrap(rhs->GetElem()))) {
        throw TExprError(HERE, PosRange,
            "Type of the expression on the left does not match the type of the elements in the list");
      }
      Type = Type::TBool::Get();
    }
    void GetType(const Type::TType &lhs, const Type::TSet *rhs) const {
      if (lhs != Type::UnwrapOptional(Type::Unwrap(rhs->GetElem()))) {
        throw TExprError(HERE, PosRange,
            "Type of the expression on the left does not match the type of the elements in the set");
      }
      Type = Type::TBool::Get();
    }
  };  // TInTypeVisitor
  assert(this);
  Type::TType type;
  Type::TType::Accept(GetLhs()->GetType(), GetRhs()->GetType(), TInTypeVisitor(type, GetPosRange()));
  return type;
}
