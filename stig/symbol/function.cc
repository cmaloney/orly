/* <stig/symbol/function.cc>

   Implements <stig/symbol/function.h>

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

#include <stig/symbol/function.h>

#include <base/assert_true.h>
#include <stig/symbol/scope.h>
#include <stig/type/any.h>

using namespace Stig;
using namespace Stig::Symbol;

TFunction::TParamDef::TParamDef(const std::string &name, const TPosRange &pos_range)
    : TDef(name, pos_range) {}

TFunction::TPtr TFunction::New(const TScope::TPtr &scope, const std::string &name, const TPosRange &pos_range) {
  assert(scope);
  auto function = TFunction::TPtr(new TFunction(scope, name, pos_range));
  scope->Add(function);
  return function;
}

TFunction::TFunction(const TScope::TPtr &scope, const std::string &name, const TPosRange &pos_range)
    : TAnyFunction(name), IsRecursive(false), Scope(Base::AssertTrue(scope)), PosRange(pos_range) {}

TFunction::~TFunction() {
  assert(this);
  auto scope = TryGetScope();
  if (scope) {
    scope->Remove(shared_from_this());
  }
}

void TFunction::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TFunction::Add(const TParamDef::TPtr &param_def) {
  assert(this);
  assert(param_def);
  auto result = ParamDefs.insert(param_def);
  assert(result.second);
}

const TFunction::TParamDefSet &TFunction::GetParamDefs() const {
  assert(this);
  return ParamDefs;
}

Type::TObj::TElems TFunction::GetParams() const {
  assert(this);
  Type::TObj::TElems elems;
  for (auto param_def : ParamDefs) {
    auto result = elems.insert(std::make_pair(param_def->GetName(), param_def->GetType()));
    /* If this assertion fails, we have multiple parameters with the same name.
       Fix the synth layer to catch this error */
    assert(result.second);
  }
  return elems;
}

const TPosRange &TFunction::GetPosRange() const {
  assert(this);
  return PosRange;
}

Type::TType TFunction::GetReturnType() const {
  assert(this);
  Type::TType type;
  if (IsRecursive) {
    type = Type::TAny::Get();
  } else {
    IsRecursive = true;
    type = GetExpr()->GetType();
    IsRecursive = false;
  }
  return type;
}

TScope::TPtr TFunction::GetScope() const {
  return Base::AssertTrue(TryGetScope());
}

void TFunction::Remove(const TParamDef::TPtr &param_def) {
  assert(this);
  assert(param_def);
  ParamDefs.erase(param_def);
}

TScope::TPtr TFunction::TryGetScope() const {
  assert(this);
  return Scope.lock();
}
