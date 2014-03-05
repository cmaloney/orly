/* <stig/expr/unknown.cc>

   Implements <stig/expr/unknown.h>

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

#include <stig/expr/unknown.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/opt.h>
#include <stig/type/unwrap_visitor.h>

using namespace Stig;
using namespace Stig::Expr;

TUnknown::TPtr TUnknown::New(const Type::TType &type, const TPosRange &pos_range) {
  return TUnknown::TPtr(new TUnknown(type, pos_range));
}

TUnknown::TUnknown(const Type::TType &type, const TPosRange &pos_range)
    : TLeaf(pos_range), Type(type) {}

void TUnknown::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

const Type::TType &TUnknown::GetCoreType() const {
  assert(this);
  return Type;
}

Type::TType TUnknown::GetType() const {
  class TUnknownVisitor
      : public Type::TUnwrapVisitor {
    NO_COPY_SEMANTICS(TUnknownVisitor);
    public:
    TUnknownVisitor(Type::TType &type, const TPosRange &pos_range)
        : Type::TUnwrapVisitor(type, pos_range) {}
    virtual void operator()(const Type::TAddr     *that) const {
      Type = Type::TOpt::Get(that->AsType());
    }
    virtual void operator()(const Type::TAny      *) const {
      throw TImpossibleError(HERE, PosRange);
    }
    virtual void operator()(const Type::TBool     *) const {
      Type = Type::TOpt::Get(Type::TBool::Get());
    }
    virtual void operator()(const Type::TDict     *that) const {
      Type = Type::TOpt::Get(that->AsType());
    }
    virtual void operator()(const Type::TId       *) const {
      Type = Type::TOpt::Get(Type::TId::Get());
    }
    virtual void operator()(const Type::TInt      *) const {
      Type = Type::TOpt::Get(Type::TInt::Get());
    }
    virtual void operator()(const Type::TList     *that) const {
      Type = Type::TOpt::Get(that->AsType());
    }
    virtual void operator()(const Type::TMutable  *) const {
      throw TExprError(HERE, PosRange, "Unknown mutable is not allowed.");
    }
    virtual void operator()(const Type::TObj      *that) const {
      Type = Type::TOpt::Get(that->AsType());
    }
    virtual void operator()(const Type::TOpt      *) const {
      throw TExprError(HERE, PosRange, "Unknown optional is not allowed.");
    }
    virtual void operator()(const Type::TReal     *) const {
      Type = Type::TOpt::Get(Type::TReal::Get());
    }
    virtual void operator()(const Type::TSeq      *) const {
      throw TExprError(HERE, PosRange, "Unknown sequence is not allowed.");
    }
    virtual void operator()(const Type::TSet      *that) const {
      Type = Type::TOpt::Get(that->AsType());
    }
    virtual void operator()(const Type::TStr      *) const {
      Type = Type::TOpt::Get(Type::TStr::Get());
    }
    virtual void operator()(const Type::TTimeDiff *) const {
      Type = Type::TOpt::Get(Type::TTimeDiff::Get());
    }
    virtual void operator()(const Type::TTimePnt  *) const {
      Type = Type::TOpt::Get(Type::TTimePnt::Get());
    }
  };  // TUnknownVisitor
  assert(this);
  Type::TType type;
  Type.Accept(TUnknownVisitor(type, GetPosRange()));
  return type;
}
