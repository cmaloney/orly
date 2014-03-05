/* <stig/expr/slice.cc>

   Implements <stig/expr/slice.h>

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

#include <stig/expr/slice.h>

#include <base/opt.h>
#include <stig/error.h>
#include <stig/pos_range.h>
#include <stig/type/part.h>
#include <stig/type/unwrap.h>
#include <stig/type/unwrap_visitor.h>

using namespace Stig;
using namespace Stig::Expr;

TSlice::TPtr TSlice::New(
    const TExpr::TPtr &container,
    const TExpr::TPtr &opt_lhs,
    bool colon,
    const TExpr::TPtr &opt_rhs,
    const TPosRange &pos_range) {
  return TSlice::TPtr(new TSlice(container, opt_lhs, colon, opt_rhs, pos_range));
}

TSlice::TSlice(
    const TExpr::TPtr &container,
    const TExpr::TPtr &opt_lhs,
    bool colon,
    const TExpr::TPtr &opt_rhs,
    const TPosRange &pos_range)
      : TNAry(TExprArray {{Base::AssertTrue(container), opt_lhs, opt_rhs}}, pos_range),
        Colon(colon) {}

void TSlice::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TSlice::GetType() const {
  class TSliceTypeVisitor
      : public Type::TUnwrapVisitor {
    NO_COPY_SEMANTICS(TSliceTypeVisitor);
    public:
    TSliceTypeVisitor(
        Type::TType &type,
        const Expr::TExpr::TPtr &opt_lhs,
        bool colon,
        const Expr::TExpr::TPtr &opt_rhs,
        const TPosRange &pos_range)
          : Type::TUnwrapVisitor(type, pos_range),
            Colon(colon),
            OptLhs(opt_lhs),
            OptRhs(opt_rhs) {}
    virtual void operator()(const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *that) const {
      Parts.push_back(Type::TDictMember::Get(that->GetKey()));
      if (Colon) {
        throw TExprError(HERE, PosRange, "Dict can only be sliced with a single value.");
      }
      assert(OptLhs);
      assert(!OptRhs);
      Type = that->GetVal();
      if (Type::Unwrap(OptLhs->GetType()) != that->GetKey()) {
        throw TExprError(HERE, PosRange, "Dict can only be sliced with an index of the key type");
      }
    }
    virtual void operator()(const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *that) const {
      Parts.push_back(*Type::TListIndex::ListIndex);
      if ((OptLhs && Type::Unwrap(OptLhs->GetType()) != Type::TInt::Get()) ||
          (OptRhs && Type::Unwrap(OptRhs->GetType()) != Type::TInt::Get())) {
        throw TExprError(HERE, PosRange, "List can only be sliced with an index of type int");
      }
      Type = Colon ? that->AsType() : that->GetElem();
    }
    virtual void operator()(const Type::TMutable  *that) const {
      Parts = that->GetParts();
      that->GetVal().Accept(*this);
      Type = Type::TMutable::Get(that->GetAddr(), Parts, Type, that->GetSrcAtAddr());
    }
    virtual void operator()(const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TOpt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TStr      *) const {
      Type = Type::TStr::Get();
    }
    virtual void operator()(const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
    private:
    bool Colon;
    const Expr::TExpr::TPtr &OptLhs;
    const Expr::TExpr::TPtr &OptRhs;
    mutable Type::TMutable::TParts Parts;
  };  // TSliceTypeVisitor
  assert(this);
  Type::TType type;
  TSliceTypeVisitor visitor(type, GetOptLhs(), HasColon(), GetOptRhs(), GetPosRange());
  GetContainer()->GetType().Accept(visitor);
  return type;
}

/* Return the pointer to the container */
const TExpr::TPtr &TSlice::GetContainer() const {
  assert(this);
  return GetExprs()[0];
}

/* Return the pointer to an optional lhs. This means the pointer can be null */
const TExpr::TPtr &TSlice::GetOptLhs() const {
  assert(this);
  return GetExprs()[1];
}

bool TSlice::HasColon() const {
  assert(this);
  return Colon;
}

/* Return the pointer to an optional rhs. This means the pointer can be null */
const TExpr::TPtr &TSlice::GetOptRhs() const {
  assert(this);
  return GetExprs()[2];
}
