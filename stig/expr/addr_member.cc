/* <stig/expr/addr_member.cc>

   Implements <stig/expr/addr_member.h>

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

#include <stig/expr/addr_member.h>

#include <stig/error.h>
#include <stig/type/part.h>
#include <stig/type/unwrap.h>
#include <stig/type/unwrap_visitor.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Expr;

TAddrMember::TPtr TAddrMember::New(const TExpr::TPtr &expr, size_t index, const TPosRange &pos_range) {
  return TAddrMember::TPtr(new TAddrMember(expr, index, pos_range));
}

TAddrMember::TAddrMember(const TExpr::TPtr &expr, size_t index, const TPosRange &pos_range)
    : TUnary(expr, pos_range), Index(index) {}

void TAddrMember::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

size_t TAddrMember::GetIndex() const {
  assert(this);
  return Index;
}

Type::TType TAddrMember::GetType() const {
  class TAddrMemberTypeVisitor
      : public Type::TUnwrapVisitor {
    NO_COPY_SEMANTICS(TAddrMemberTypeVisitor);
    public:
    TAddrMemberTypeVisitor(Type::TType &type, size_t index, const TPosRange &pos_range)
        : TUnwrapVisitor(type, pos_range), Index(index) {}
    virtual void operator()(const Type::TAddr     *that) const {
      const auto &elems = that->GetElems();
      if (Index >= elems.size()) {
        throw TExprError(HERE, PosRange, "No field by such index in address.");
      }
      Type = elems[Index].second;
    }
    virtual void operator()(const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TMutable  *that) const {
      auto parts = that->GetParts();
      parts.push_back(Type::TAddrMember::Get(Index));
      that->GetVal().Accept(*this);
      Type = Type::TMutable::Get(that->GetAddr(), parts, Type, that->GetSrcAtAddr());
    }
    virtual void operator()(const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
    private:
    size_t Index;
  };  // TAddrMemberTypeVisitor
  assert(this);
  Type::TType type;
  GetExpr()->GetType().Accept(TAddrMemberTypeVisitor(type, Index, GetPosRange()));
  return type;
}
