/* <stig/expr/range.cc>

   Implements <stig/expr/range.h>

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

#include <stig/expr/range.h>

#include <base/assert_true.h>
#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/unwrap_visitor.h>

using namespace Stig;
using namespace Stig::Expr;

TRange::TPtr TRange::New(
    const TExpr::TPtr &start,
    const TExpr::TPtr &opt_stride,
    const TExpr::TPtr &opt_end,
    bool end_included,
    const TPosRange &pos_range) {
  return TRange::TPtr(new TRange(start, opt_stride, opt_end, end_included, pos_range));
}

TRange::TRange(
    const TExpr::TPtr &start,
    const TExpr::TPtr &opt_stride,
    const TExpr::TPtr &opt_end,
    bool end_included,
    const TPosRange &pos_range)
      : TNAry(TExprArray {{Base::AssertTrue(start), opt_stride, opt_end}}, pos_range),
        EndIncluded(end_included) {}


void TRange::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TRange::GetType() const {
  class TRangeTypeVisitor
      : public Type::TUnwrapVisitor {
    NO_COPY_SEMANTICS(TRangeTypeVisitor);
    public:
    TRangeTypeVisitor(Type::TType &type, const TPosRange &pos_range)
        : Type::TUnwrapVisitor(type, pos_range) {}
    virtual void operator()(const Type::TAddr     *) const { ThrowExprError(HERE); }
    virtual void operator()(const Type::TBool     *) const { ThrowExprError(HERE); }
    virtual void operator()(const Type::TDict     *) const { ThrowExprError(HERE); }
    virtual void operator()(const Type::TId       *) const { ThrowExprError(HERE); }
    virtual void operator()(const Type::TInt      *) const {
      Type = Type::TInt::Get();
    }
    virtual void operator()(const Type::TList     *) const { ThrowExprError(HERE); }
    virtual void operator()(const Type::TObj      *) const { ThrowExprError(HERE); }
    virtual void operator()(const Type::TOpt      *) const { ThrowExprError(HERE); }
    virtual void operator()(const Type::TReal     *) const { ThrowExprError(HERE); }
    virtual void operator()(const Type::TSet      *) const { ThrowExprError(HERE); }
    virtual void operator()(const Type::TStr      *) const { ThrowExprError(HERE); }
    virtual void operator()(const Type::TTimeDiff *) const { ThrowExprError(HERE); }
    virtual void operator()(const Type::TTimePnt  *) const { ThrowExprError(HERE); }
    private:
    void ThrowExprError(const Base::TCodeLocation &code_location) const {
      throw TExprError(code_location, PosRange, "Range constructor elements must be evaluated to int");
    }
  };  // TRangeTypeVisitor
  assert(this);
  Type::TType start_type;
  GetStart()->GetType().Accept(TRangeTypeVisitor(start_type, GetStart()->GetPosRange()));
  if (GetOptStride()) {
    Type::TType stride_type;
    GetOptStride()->GetType().Accept(TRangeTypeVisitor(stride_type, GetOptStride()->GetPosRange()));
    if (start_type != stride_type) {
      throw TExprError(HERE, GetPosRange(),
          "Range constructor's start and stride must evaluate to the same type");
    }
  }
  if (GetOptEnd()) {
    Type::TType end_type;
    GetOptEnd()->GetType().Accept(TRangeTypeVisitor(end_type, GetOptEnd()->GetPosRange()));
    if (start_type != end_type) {
      throw TExprError(HERE, GetPosRange(),
          "Range constructor's start and end must evaluate to the same type");
    }
  }
  return Type::TSeq::Get(start_type);
}

/* Return the pointer to the start */
const TExpr::TPtr &TRange::GetStart() const {
  assert(this);
  return GetExprs()[0];
}

/* Return the pointer to an optional end. This means the pointer can be null */
const TExpr::TPtr &TRange::GetOptStride() const {
  assert(this);
  return GetExprs()[1];
}

/* Return the pointer to an optional end. This means the pointer can be null */
const TExpr::TPtr &TRange::GetOptEnd() const {
  assert(this);
  return GetExprs()[2];
}

bool TRange::HasEndIncluded() const {
  assert(this);
  return EndIncluded;
}

