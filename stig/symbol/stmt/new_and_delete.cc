/* <stig/symbol/stmt/new_and_delete.cc>

   Implements <stig/symbol/stmt/new_and_delete.h>

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

#include <stig/symbol/stmt/new_and_delete.h>

#include <stig/error.h>
#include <stig/type/unwrap_visitor.h>

using namespace Stig;
using namespace Stig::Symbol;
using namespace Stig::Symbol::Stmt;

class TAddressTypeVisitor
    : public Type::TUnwrapVisitor {
  NO_COPY_SEMANTICS(TAddressTypeVisitor);
  public:
  TAddressTypeVisitor(Type::TType &type, const TPosRange &pos_range)
      : Type::TUnwrapVisitor(type, pos_range) {}
  virtual void operator()(const Type::TAddr     *) const { /* DO NOTHING */ }
  virtual void operator()(const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId       *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList     *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable  *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
};  // TAddressTypeVisitor

TDelete::TPtr TDelete::New(const TStmtArg::TPtr &stmt_arg, Type::TType value_type, const TPosRange &pos_range) {
  return TDelete::TPtr(new TDelete(stmt_arg, value_type, pos_range));
}

void TDelete::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TDelete::TypeCheck() const {
  assert(this);
  Type::TType dummy;
  GetStmtArg()->GetExpr()->GetType().Accept(TAddressTypeVisitor(dummy, GetPosRange()));
}

TDelete::TDelete(const TStmtArg::TPtr &stmt_arg, Type::TType value_type, const TPosRange &pos_range)
    : TUnary(stmt_arg, pos_range), ValueType(value_type) {}

TNew::TPtr TNew::New(
    const TStmtArg::TPtr &lhs,
    const TStmtArg::TPtr &rhs,
    const TPosRange &pos_range) {
  return TNew::TPtr(new TNew(lhs, rhs, pos_range));
}

void TNew::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TNew::TypeCheck() const {
  assert(this);
  Type::TType dummy;
  GetLhs()->GetExpr()->GetType().Accept(TAddressTypeVisitor(dummy, GetPosRange()));
  /* NOTE: Maybe this should be type check rather than get type. But for now,
           GetType() calls ComputeType() which does the type check. */
  GetRhs()->GetExpr()->GetType();
}

TNew::TNew(
    const TStmtArg::TPtr &lhs,
    const TStmtArg::TPtr &rhs,
    const TPosRange &pos_range)
      : TBinary(lhs, rhs, pos_range) {}
