/* <orly/expr/trig.cc>

   Implements <orly/expr/trig.h>

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/expr/trig.h>

#include <orly/error.h>
#include <orly/expr/visitor.h>
#include <orly/pos_range.h>
#include <orly/type/impl.h>
#include <orly/type/unwrap.h>
#include <orly/type/unwrap_visitor.h>

using namespace Orly;
using namespace Orly::Expr;

class TTrigTypeVisitor
    : public Type::TUnwrapVisitor {
  NO_COPY(TTrigTypeVisitor);
  public:
  TTrigTypeVisitor(Type::TType &type, const TPosRange &pos_range)
    : Type::TUnwrapVisitor(type, pos_range) {}

  virtual void operator()(const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId       *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList     *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal     *) const { Type = Type::TReal::Get(); }
  virtual void operator()(const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
};  // TLogTypeVisitor

/* Cos */
TExpr::TPtr TCos::New(const TExpr::TPtr &expr, const TPosRange &pos_range) {
  return TCos::TPtr(new TCos(expr, pos_range));
}

TCos::TCos(const TExpr::TPtr &expr, const TPosRange &pos_range)
    : TUnary(expr, pos_range) {}

void TCos::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TCos::GetTypeImpl() const {
  assert(this);
  Type::TType type;
  GetExpr()->GetType().Accept(TTrigTypeVisitor(type, GetPosRange()));
  return type;
}

/* Sin */
TExpr::TPtr TSin::New(const TExpr::TPtr &expr, const TPosRange &pos_range) {
  return TSin::TPtr(new TSin(expr, pos_range));
}

TSin::TSin(const TExpr::TPtr &expr, const TPosRange &pos_range)
    : TUnary(expr, pos_range) {}

void TSin::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TSin::GetTypeImpl() const {
  assert(this);
  Type::TType type;
  GetExpr()->GetType().Accept(TTrigTypeVisitor(type, GetPosRange()));
  return type;
}

/* Tan */
TExpr::TPtr TTan::New(const TExpr::TPtr &expr, const TPosRange &pos_range) {
  return TTan::TPtr(new TTan(expr, pos_range));
}

TTan::TTan(const TExpr::TPtr &expr, const TPosRange &pos_range)
    : TUnary(expr, pos_range) {}

void TTan::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TTan::GetTypeImpl() const {
  assert(this);
  Type::TType type;
  GetExpr()->GetType().Accept(TTrigTypeVisitor(type, GetPosRange()));
  return type;
}

/* Acos */
TExpr::TPtr TAcos::New(const TExpr::TPtr &expr, const TPosRange &pos_range) {
  return TAcos::TPtr(new TAcos(expr, pos_range));
}

TAcos::TAcos(const TExpr::TPtr &expr, const TPosRange &pos_range)
    : TUnary(expr, pos_range) {}

void TAcos::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TAcos::GetTypeImpl() const {
  assert(this);
  Type::TType type;
  GetExpr()->GetType().Accept(TTrigTypeVisitor(type, GetPosRange()));
  return type;
}

/* Asin */
TExpr::TPtr TAsin::New(const TExpr::TPtr &expr, const TPosRange &pos_range) {
  return TAsin::TPtr(new TAsin(expr, pos_range));
}

TAsin::TAsin(const TExpr::TPtr &expr, const TPosRange &pos_range)
    : TUnary(expr, pos_range) {}

void TAsin::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TAsin::GetTypeImpl() const {
  assert(this);
  Type::TType type;
  GetExpr()->GetType().Accept(TTrigTypeVisitor(type, GetPosRange()));
  return type;
}

/* Atan */
TExpr::TPtr TAtan::New(const TExpr::TPtr &expr, const TPosRange &pos_range) {
  return TAtan::TPtr(new TAtan(expr, pos_range));
}

TAtan::TAtan(const TExpr::TPtr &expr, const TPosRange &pos_range)
    : TUnary(expr, pos_range) {}

void TAtan::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TAtan::GetTypeImpl() const {
  assert(this);
  Type::TType type;
  GetExpr()->GetType().Accept(TTrigTypeVisitor(type, GetPosRange()));
  return type;
}

/* Atan2 */
TExpr::TPtr TAtan2::New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range) {
  return TAtan2::TPtr(new TAtan2(lhs, rhs, pos_range));
}

TAtan2::TAtan2(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range)
    : TBinary(lhs, rhs, pos_range) {}

void TAtan2::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TAtan2::GetTypeImpl() const {
  assert(this);
  Type::TType type_lhs;
  Type::TType type_rhs;
  GetLhs()->GetType().Accept(TTrigTypeVisitor(type_lhs, GetPosRange()));
  GetRhs()->GetType().Accept(TTrigTypeVisitor(type_rhs, GetPosRange()));
  return type_lhs;
}
