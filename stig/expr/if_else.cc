/* <stig/expr/if_else.cc>

   Implements <stig/expr/if_else.h>

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

#include <stig/expr/if_else.h>

#include <base/assert_true.h>
#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/type/impl.h>
#include <stig/type/equal_visitor.h>
#include <stig/type/unwrap.h>

using namespace Stig;
using namespace Stig::Expr;

TIfElse::TPtr TIfElse::New(
    const TExpr::TPtr &true_case,
    const TExpr::TPtr &predicate,
    const TExpr::TPtr &false_case,
    const TPosRange &pos_range) {
  return TIfElse::TPtr(new TIfElse(true_case, predicate, false_case, pos_range));
}

TIfElse::TIfElse(
    const TExpr::TPtr &true_case,
    const TExpr::TPtr &predicate,
    const TExpr::TPtr &false_case,
    const TPosRange &pos_range)
      : TNAry(TExprArray {{Base::AssertTrue(true_case),
                           Base::AssertTrue(predicate),
                           Base::AssertTrue(false_case)}}, pos_range) {}

void TIfElse::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

const TExpr::TPtr &TIfElse::GetTrue() const {
  assert(this);
  return GetExprs()[0];
}

const TExpr::TPtr &TIfElse::GetPredicate() const {
  assert(this);
  return GetExprs()[1];
}

const TExpr::TPtr &TIfElse::GetFalse() const {
  assert(this);
  return GetExprs()[2];
}

Type::TType TIfElse::GetType() const {
  class TIfElseVisitor : public Type::TEqualVisitor {
    NO_COPY_SEMANTICS(TIfElseVisitor);
    public:
    TIfElseVisitor(Type::TType &type, const TPosRange &pos_range) : TEqualVisitor(type, pos_range) {}
    virtual void operator()(const Type::TAny *, const Type::TAny *) const {
      throw TExprError(
          HERE,
          PosRange,
          "Unable to resolve return type for if else expression. Are you missing a base case for a recursive function?");
    }
    virtual void operator()(const Type::TAny      *, const Type::TAddr     *rhs) const { Type = rhs->AsType(); }
    virtual void operator()(const Type::TAny      *, const Type::TBool     *rhs) const { Type = rhs->AsType(); }
    virtual void operator()(const Type::TAny      *, const Type::TDict     *rhs) const { Type = rhs->AsType(); }
    virtual void operator()(const Type::TAny      *, const Type::TId       *rhs) const { Type = rhs->AsType(); }
    virtual void operator()(const Type::TAny      *, const Type::TInt      *rhs) const { Type = rhs->AsType(); }
    virtual void operator()(const Type::TAny      *, const Type::TList     *rhs) const { Type = rhs->AsType(); }
    virtual void operator()(const Type::TAny      *, const Type::TMutable  *rhs) const { Type = rhs->AsType(); }
    virtual void operator()(const Type::TAny      *, const Type::TObj      *rhs) const { Type = rhs->AsType(); }
    virtual void operator()(const Type::TAny      *, const Type::TReal     *rhs) const { Type = rhs->AsType(); }
    virtual void operator()(const Type::TAny      *, const Type::TSet      *rhs) const { Type = rhs->AsType(); }
    virtual void operator()(const Type::TAny      *, const Type::TSeq      *rhs) const { Type = rhs->AsType(); }
    virtual void operator()(const Type::TAny      *, const Type::TStr      *rhs) const { Type = rhs->AsType(); }
    virtual void operator()(const Type::TAny      *, const Type::TTimeDiff *rhs) const { Type = rhs->AsType(); }
    virtual void operator()(const Type::TAny      *, const Type::TTimePnt  *rhs) const { Type = rhs->AsType(); }
    virtual void operator()(const Type::TAddr     *lhs, const Type::TAny      *) const final { Type = lhs->AsType(); }
    virtual void operator()(const Type::TBool     *lhs, const Type::TAny      *) const final { Type = lhs->AsType(); }
    virtual void operator()(const Type::TDict     *lhs, const Type::TAny      *) const final { Type = lhs->AsType(); }
    virtual void operator()(const Type::TId       *lhs, const Type::TAny      *) const final { Type = lhs->AsType(); }
    virtual void operator()(const Type::TInt      *lhs, const Type::TAny      *) const final { Type = lhs->AsType(); }
    virtual void operator()(const Type::TList     *lhs, const Type::TAny      *) const final { Type = lhs->AsType(); }
    virtual void operator()(const Type::TMutable  *lhs, const Type::TAny      *) const final { Type = lhs->AsType(); }
    virtual void operator()(const Type::TObj      *lhs, const Type::TAny      *) const final { Type = lhs->AsType(); }
    virtual void operator()(const Type::TReal     *lhs, const Type::TAny      *) const final { Type = lhs->AsType(); }
    virtual void operator()(const Type::TSet      *lhs, const Type::TAny      *) const final { Type = lhs->AsType(); }
    virtual void operator()(const Type::TSeq      *lhs, const Type::TAny      *) const final { Type = lhs->AsType(); }
    virtual void operator()(const Type::TStr      *lhs, const Type::TAny      *) const final { Type = lhs->AsType(); }
    virtual void operator()(const Type::TTimeDiff *lhs, const Type::TAny      *) const final { Type = lhs->AsType(); }
    virtual void operator()(const Type::TTimePnt  *lhs, const Type::TAny      *) const final { Type = lhs->AsType(); }
    virtual void operator()(const Type::TAddr     *lhs, const Type::TAddr     *) const { Type = lhs->AsType(); }
    virtual void operator()(const Type::TBool     *lhs, const Type::TBool     *) const { Type = lhs->AsType(); }
    virtual void operator()(const Type::TDict     *lhs, const Type::TDict     *) const { Type = lhs->AsType(); }
    virtual void operator()(const Type::TId       *lhs, const Type::TId       *) const { Type = lhs->AsType(); }
    virtual void operator()(const Type::TInt      *lhs, const Type::TInt      *) const { Type = lhs->AsType(); }
    virtual void operator()(const Type::TList     *lhs, const Type::TList     *) const { Type = lhs->AsType(); }
    virtual void operator()(const Type::TObj      *lhs, const Type::TObj      *) const { Type = lhs->AsType(); }
    virtual void operator()(const Type::TReal     *lhs, const Type::TReal     *) const { Type = lhs->AsType(); }
    virtual void operator()(const Type::TSet      *lhs, const Type::TSet      *) const { Type = lhs->AsType(); }
    virtual void operator()(const Type::TStr      *lhs, const Type::TStr      *) const { Type = lhs->AsType(); }
    virtual void operator()(const Type::TTimeDiff *lhs, const Type::TTimeDiff *) const { Type = lhs->AsType(); }
    virtual void operator()(const Type::TTimePnt  *lhs, const Type::TTimePnt  *) const { Type = lhs->AsType(); }
  };  // TIfElseVisitor
  assert(this);
  if (Type::Unwrap(GetPredicate()->GetType()) != Type::TBool::Get()) {
    throw TExprError(HERE, GetPosRange(), "if else expression's predicate must evaluate to a bool.");
  }
  Type::TType type;
  Type::TType::Accept(GetTrue()->GetType(), GetFalse()->GetType(), TIfElseVisitor(type, GetPosRange()));
  return type;
}
