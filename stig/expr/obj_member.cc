/* <stig/expr/obj_member.cc>

   Implements <stig/expr/obj_member.h>

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

#include <stig/expr/obj_member.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/impl.h>
#include <stig/type/part.h>
#include <stig/type/unwrap.h>
#include <stig/type/unwrap_visitor.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Expr;

TObjMember::TPtr TObjMember::New(const TExpr::TPtr &expr, const std::string &name, const TPosRange &pos_range) {
  return TObjMember::TPtr(new TObjMember(expr, name, pos_range));
}

TObjMember::TObjMember(const TExpr::TPtr &expr, const std::string &name, const TPosRange &pos_range)
    : TUnary(expr, pos_range), Name(name) {}

void TObjMember::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TObjMember::GetType() const {
  class TObjMemberTypeVisitor
      : public Type::TUnwrapVisitor {
    NO_COPY_SEMANTICS(TObjMemberTypeVisitor);
    public:
    TObjMemberTypeVisitor(Type::TType &type, const std::string &name, const TPosRange &pos_range)
        : Type::TUnwrapVisitor(type, pos_range), Name(name) {}
    virtual void operator()(const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TId       *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TList     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TMutable  *that) const {
      auto parts = that->GetParts();
      parts.push_back(Type::TObjMember::Get(Name));
      that->GetVal().Accept(*this);
      Type = Type::TMutable::Get(that->GetAddr(), parts, Type, that->GetSrcAtAddr());
    }
    virtual void operator()(const Type::TObj      *that) const {
      auto &elem_map = that->GetElems();
      auto iter = elem_map.find(Name);
      if (iter == elem_map.end()) {
        throw TExprError(HERE, PosRange, "No such field in object");
      }
      Type = iter->second;
    }
    virtual void operator()(const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
    virtual void operator()(const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
    private:
    const std::string &Name;
  };  // TObjMemberTypeVisitor
  assert(this);
  Type::TType type;
  GetExpr()->GetType().Accept(TObjMemberTypeVisitor(type, Name, GetPosRange()));
  return type;
}

const std::string &TObjMember::GetName() const {
  assert(this);
  return Name;
}
