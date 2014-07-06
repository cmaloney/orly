/* <orly/synth/empty_ctor.cc>

   Implements <orly/synth/empty_ctor.h>.

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

#include <orly/synth/empty_ctor.h>

#include <cassert>
#include <unordered_map>
#include <vector>

#include <base/assert_true.h>
#include <orly/expr/dict.h>
#include <orly/expr/list.h>
#include <orly/expr/set.h>
#include <orly/synth/context.h>
#include <orly/synth/get_pos_range.h>
#include <orly/synth/new_type.h>
#include <orly/pos_range.h>
#include <orly/type/impl.h>
#include <orly/type/dict.h>
#include <orly/type/list.h>
#include <orly/type/set.h>

using namespace Orly;
using namespace Orly::Synth;

TEmptyCtor::TEmptyCtor(const Package::Syntax::TEmptyCtor *empty_ctor)
    : EmptyCtor(Base::AssertTrue(empty_ctor)),
      Type(nullptr) {
  class TSyntaxTypeVisitor
      : public Package::Syntax::TType::TVisitor {
    NO_COPY(TSyntaxTypeVisitor);
    public:
    TSyntaxTypeVisitor(TType *&type, const TPosRange &pos_range)
        : Type(type), PosRange(pos_range) {}
    virtual void operator()(const Package::Syntax::TAddrType     *) const { BuildError(); }
    virtual void operator()(const Package::Syntax::TBoolType     *) const { BuildError(); }
    virtual void operator()(const Package::Syntax::TDictType     *that) const {
      Type = NewType(that);
    }
    virtual void operator()(const Package::Syntax::TErrType      *) const { BuildError(); }
    virtual void operator()(const Package::Syntax::TFuncType     *) const { BuildError(); }
    virtual void operator()(const Package::Syntax::TIdType       *) const { BuildError(); }
    virtual void operator()(const Package::Syntax::TIntType      *) const { BuildError(); }
    virtual void operator()(const Package::Syntax::TListType     *that) const {
      Type = NewType(that);
    }
    virtual void operator()(const Package::Syntax::TMutableType  *) const { BuildError(); }
    virtual void operator()(const Package::Syntax::TObjType      *) const { BuildError(); }
    virtual void operator()(const Package::Syntax::TOptType      *) const { BuildError(); }
    virtual void operator()(const Package::Syntax::TParenType    *that) const {
      Type = NewType(that);
    }
    virtual void operator()(const Package::Syntax::TRealType     *) const { BuildError(); }
    virtual void operator()(const Package::Syntax::TSeqType      *) const { BuildError(); }
    virtual void operator()(const Package::Syntax::TSetType      *that) const {
      Type = NewType(that);
    }
    virtual void operator()(const Package::Syntax::TStrType      *) const { BuildError(); }
    virtual void operator()(const Package::Syntax::TTimePntType  *) const { BuildError(); }
    virtual void operator()(const Package::Syntax::TTimeDiffType *) const { BuildError(); }
    virtual void operator()(const Package::Syntax::TRefType      *that) const {
      Type = NewType(that);
    }
    private:
    void BuildError() const {
      GetContext().AddError(PosRange, "empty constructor's type is not list, set, or dictionary");
    }
    TType *&Type;
    const TPosRange &PosRange;
  };  // TSyntaxTypeVisitor
  EmptyCtor->GetType()->Accept(TSyntaxTypeVisitor(Type, GetPosRange(EmptyCtor)));
}

TEmptyCtor::~TEmptyCtor() {
  assert(this);
  delete Type;
}

Expr::TExpr::TPtr TEmptyCtor::Build() const {
  class TTypeVisitor
      : public Type::TType::TVisitor {
    public:
    TTypeVisitor(Expr::TExpr::TPtr &expr, const TPosRange &pos_range)
        : Expr(expr), PosRange(pos_range) {}
    virtual void operator()(const Type::TAddr     *) const { BuildError(); }
    virtual void operator()(const Type::TAny      *) const { BuildError(); }
    virtual void operator()(const Type::TBool     *) const { BuildError(); }
    virtual void operator()(const Type::TDict     *that) const {
      Expr = Expr::TDict::New(that->AsType(), PosRange);
    }
    virtual void operator()(const Type::TErr      *) const { BuildError(); }
    virtual void operator()(const Type::TFunc     *) const { BuildError(); }
    virtual void operator()(const Type::TId       *) const { BuildError(); }
    virtual void operator()(const Type::TInt      *) const { BuildError(); }
    virtual void operator()(const Type::TList     *that) const {
      Expr = Expr::TList::New(that->AsType(), PosRange);
    }
    virtual void operator()(const Type::TMutable  *) const { BuildError(); }
    virtual void operator()(const Type::TObj      *) const { BuildError(); }
    virtual void operator()(const Type::TOpt      *) const { BuildError(); }
    virtual void operator()(const Type::TReal     *) const { BuildError(); }
    virtual void operator()(const Type::TSeq      *) const { BuildError(); }
    virtual void operator()(const Type::TSet      *that) const {
      Expr = Expr::TSet::New(that->AsType(), PosRange);
    }
    virtual void operator()(const Type::TStr      *) const { BuildError(); }
    virtual void operator()(const Type::TTimeDiff *) const { BuildError(); }
    virtual void operator()(const Type::TTimePnt  *) const { BuildError(); }
    private:
    void BuildError() const {
      GetContext().AddError(PosRange, "empty constructor's type is not list, set, or dictionary");
    }
    Expr::TExpr::TPtr &Expr;
    const TPosRange &PosRange;
  };  // TTypeVisitor
  assert(this);
  Expr::TExpr::TPtr expr;
  Type->GetSymbolicType().Accept(TTypeVisitor(expr, GetPosRange(EmptyCtor)));
  return expr;
}

void TEmptyCtor::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  assert(Type);
  Type->ForEachRef(cb);
}
