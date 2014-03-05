/* <stig/expr/ref.cc>

   Implements <stig/expr/ref.h>

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

#include <stig/expr/ref.h>

#include <base/assert_true.h>
#include <base/code_location.h>
#include <stig/error.h>
#include <stig/expr/function_app.h>
#include <stig/expr/interior.h>
#include <stig/expr/util.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/symbol/given_param_def.h>
#include <stig/symbol/result_def.h>
#include <stig/type/util.h>

using namespace Stig;
using namespace Stig::Expr;

TRef::TPtr TRef::New(const Symbol::TDef::TPtr &def, const TPosRange &pos_range) {
  return TRef::TPtr(new TRef(def.get(), pos_range));
}

TRef::TRef(const Symbol::TDef *def, const TPosRange &pos_range)
    : TLeaf(pos_range), Def(Base::AssertTrue(def)) {}

void TRef::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TRef::GetType() const {
  /*
    NOTE: Currently we do a check that the ref points to a zero parameter function
          This works because the only place we allow a ref to point to a non-zero parameter function is
          when the ref is within a function application
  */
  assert(this);
  auto interior = dynamic_cast<const TInterior *>(GetExprParent());
  if (!interior || !interior->Is<Expr::TFunctionApp>()) {
    ImplicitFunctionAppCheck();
  }
  return Def->GetType();
}

const Symbol::TDef *TRef::GetDef() const {
  assert(this);
  return Def;
}

void TRef::ImplicitFunctionAppCheck() const {
  assert(this);
  class TDefVisitor
      : public Symbol::TDef::TVisitor {
    NO_COPY_SEMANTICS(TDefVisitor);
    public:
    TDefVisitor(const TPosRange &pos_range)
        : PosRange(pos_range) {}
    virtual void operator()(const Symbol::TGivenParamDef *that) const {
      auto type = that->GetType();
      auto func = type.TryAs<Type::TFunc>();
      if (func && !func->GetParamObject().As<Type::TObj>()->GetElems().empty()) {
        ThrowError(HERE);
      }
    }
    virtual void operator()(const Symbol::TResultDef *that) const {
      if (!that->GetFunction()->GetParams().empty()) {
        ThrowError(HERE);
      }
    }
    private:
    const TPosRange &PosRange;
    void ThrowError(const Base::TCodeLocation &code_location) const {
      assert(this);
      throw TExprError(code_location, PosRange, "Cannot implicitly call a non-zero parameter function");
    }
  };  // TDefVisitor
  Def->Accept(TDefVisitor(GetPosRange()));
}
