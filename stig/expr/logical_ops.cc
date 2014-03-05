/* <stig/expr/logical_ops.cc>

   Implements <stig/expr/logical_ops.h>.

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

#include <stig/expr/logical_ops.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/logical_ops_visitor.h>
#include <stig/type/unwrap.h>
#include <stig/type/unwrap_visitor.h>

using namespace Stig;
using namespace Stig::Expr;

class TLogicalOpsTypeVisitor
    : public Type::TLogicalOpsVisitor {
  public:

  TLogicalOpsTypeVisitor(Type::TType &type, const TPosRange &pos_range)
      : Type::TLogicalOpsVisitor(type, pos_range) {}

};  // TLogicalOpsTypeVisitor

TExpr::TPtr TAnd::New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range) {
  return TAnd::TPtr(new TAnd(lhs, rhs, pos_range));
}

TAnd::TAnd(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range)
    : TBinary(lhs, rhs, pos_range) {}

void TAnd::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TAnd::GetType() const {
  assert(this);
  Type::TType type;
  Type::TType::Accept(GetLhs()->GetType(), GetRhs()->GetType(), TLogicalOpsTypeVisitor(type, GetPosRange()));
  return type;
}

TExpr::TPtr TAndThen::New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range) {
  return TAndThen::TPtr(new TAndThen(lhs, rhs, pos_range));
}

TAndThen::TAndThen(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range)
    : TBinary(lhs, rhs, pos_range) {}

void TAndThen::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TAndThen::GetType() const {
  assert(this);
  Type::TType type;
  Type::TType::Accept(GetLhs()->GetType(), GetRhs()->GetType(), TLogicalOpsTypeVisitor(type, GetPosRange()));
  return type;
}

TExpr::TPtr TNot::New(const TExpr::TPtr &expr, const TPosRange &pos_range) {
  return TNot::TPtr(new TNot(expr, pos_range));
}

TNot::TNot(const TExpr::TPtr &expr, const TPosRange &pos_range)
    : TUnary(expr, pos_range) {}

void TNot::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TNot::GetType() const {
  class TNotTypeVisitor
      : public Type::TUnwrapVisitor {
    NO_COPY_SEMANTICS(TNotTypeVisitor);
    public:
    TNotTypeVisitor(Type::TType &type, const TPosRange &pos_range)
        : Type::TUnwrapVisitor(type, pos_range) {}
    virtual void operator()(const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TBool     *) const {
      Type = Type::TBool::Get();
    }
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
  };  // TNotTypeVisitor
  assert(this);
  Type::TType type;
  GetExpr()->GetType().Accept(TNotTypeVisitor(type, GetPosRange()));
  return type;
}

TExpr::TPtr TOr::New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range) {
  return TOr::TPtr(new TOr(lhs, rhs, pos_range));
}

TOr::TOr(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range)
    : TBinary(lhs, rhs, pos_range) {}

void TOr::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TOr::GetType() const {
  assert(this);
  Type::TType type;
  Type::TType::Accept(GetLhs()->GetType(), GetRhs()->GetType(), TLogicalOpsTypeVisitor(type, GetPosRange()));
  return type;
}

TExpr::TPtr TOrElse::New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range) {
  return TOrElse::TPtr(new TOrElse(lhs, rhs, pos_range));
}

TOrElse::TOrElse(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range)
    : TBinary(lhs, rhs, pos_range) {}

void TOrElse::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TOrElse::GetType() const {
  assert(this);
  Type::TType type;
  Type::TType::Accept(GetLhs()->GetType(), GetRhs()->GetType(), TLogicalOpsTypeVisitor(type, GetPosRange()));
  return type;
}

TExpr::TPtr TXor::New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range) {
  return TXor::TPtr(new TXor(lhs, rhs, pos_range));
}

TXor::TXor(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range)
    : TBinary(lhs, rhs, pos_range) {}

void TXor::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TXor::GetType() const {
  assert(this);
  Type::TType type;
  Type::TType::Accept(GetLhs()->GetType(), GetRhs()->GetType(), TLogicalOpsTypeVisitor(type, GetPosRange()));
  return type;
}
