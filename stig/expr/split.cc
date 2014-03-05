/* <stig/expr/split.cc>

   Implements <stig/expr/split.h>

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

#include <stig/expr/split.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/unwrap.h>
#include <stig/type/unwrap_visitor.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Expr;
using namespace Stig::Type;
using namespace std;

TExpr::TPtr TSplit::New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range) {
  return TSplit::TPtr(new TSplit(lhs, rhs, pos_range));
}

TSplit::TSplit(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range)
    : TBinary(lhs, rhs, pos_range) {}

void TSplit::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TSplit::GetType() const {
  class TSplitTypeVisitor
      : public Type::TUnwrapVisitor {
    NO_COPY_SEMANTICS(TSplitTypeVisitor);
    public:
    TSplitTypeVisitor(Type::TType &type, const TPosRange &pos_range)
        : Type::TUnwrapVisitor(type, pos_range) {}
    virtual void operator()(const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSeq      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TStr      *) const {
      /* sequence of <[str, str?]> tuples */
      Type = TSeq::Get(Stig::Type::TAddr::Get(vector<pair<TAddrDir, TType>>{{TAddrDir::Asc, TStr::Get()}, {TAddrDir::Asc, TOpt::Get(TStr::Get())}}));
    }
    virtual void operator()(const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
  };  // TSplitTypeVisitor
  assert(this);
  Type::TType type;
  GetLhs()->GetType().Accept(TSplitTypeVisitor(type, GetPosRange()));
  if (Type::Unwrap(GetRhs()->GetType()) != Type::TStr::Get()) {
    Tools::Nycr::TError::TBuilder(GetPosRange())
      << "Regex to split on must be specified as a string";
  }
  return type;
}
