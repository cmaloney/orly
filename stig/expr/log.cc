/* <stig/expr/log.cc>

   Implements <stig/expr/log.h>

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

#include <stig/expr/log.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/impl.h>
#include <stig/type/unwrap.h>
#include <stig/type/unwrap_visitor.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Expr;

class TLogTypeVisitor
    : public Type::TUnwrapVisitor {
  NO_COPY_SEMANTICS(TLogTypeVisitor);
  public:
  TLogTypeVisitor(Type::TType &type, const TPosRange &pos_range)
    : Type::TUnwrapVisitor(type, pos_range) {}

  virtual void operator()(const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId       *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt      *) const { Type = Type::TReal::Get(); }
  virtual void operator()(const Type::TList     *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal     *) const { Type = Type::TReal::Get(); }
  virtual void operator()(const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
};  // TLogTypeVisitor

TExpr::TPtr TLog::New(const TExpr::TPtr &expr, const TPosRange &pos_range) {
  return TLog::TPtr(new TLog(expr, pos_range));
}

TLog::TLog(const TExpr::TPtr &expr, const TPosRange &pos_range)
    : TUnary(expr, pos_range) {}

void TLog::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TLog::GetType() const {
  assert(this);
  Type::TType type;
  GetExpr()->GetType().Accept(TLogTypeVisitor(type, GetPosRange()));
  return type;
}

TExpr::TPtr TLog2::New(const TExpr::TPtr &expr, const TPosRange &pos_range) {
  return TLog2::TPtr(new TLog2(expr, pos_range));
}

TLog2::TLog2(const TExpr::TPtr &expr, const TPosRange &pos_range)
    : TUnary(expr, pos_range) {}

void TLog2::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TLog2::GetType() const {
  assert(this);
  Type::TType type;
  GetExpr()->GetType().Accept(TLogTypeVisitor(type, GetPosRange()));
  return type;
}

TExpr::TPtr TLog10::New(const TExpr::TPtr &expr, const TPosRange &pos_range) {
  return TLog10::TPtr(new TLog10(expr, pos_range));
}

TLog10::TLog10(const TExpr::TPtr &expr, const TPosRange &pos_range)
    : TUnary(expr, pos_range) {}

void TLog10::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TLog10::GetType() const {
  assert(this);
  Type::TType type;
  GetExpr()->GetType().Accept(TLogTypeVisitor(type, GetPosRange()));
  return type;
}