/* <stig/symbol/result_def.cc>

   Implements <stig/symbol/result_def.h>

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

#include <stig/symbol/result_def.h>

#include <base/assert_true.h>
#include <stig/symbol/any_function.h>

using namespace Stig;
using namespace Stig::Symbol;

TResultDef::TPtr TResultDef::New(const TAnyFunction::TPtr &function, const std::string &name, const TPosRange &pos_range) {
  assert(function);
  auto result_def = TResultDef::TPtr(new TResultDef(function, name, pos_range));
  function->Add(result_def);
  return result_def;
}

TResultDef::TResultDef(const TAnyFunction::TPtr &function, const std::string &name, const TPosRange &pos_range)
    : TDef(name, pos_range), Function(Base::AssertTrue(function)) {}

TResultDef::~TResultDef() {
  assert(this);
  auto function = TryGetFunction();
  if (function) {
    function->Remove(shared_from_this());
  }
}

void TResultDef::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

TAnyFunction::TPtr TResultDef::GetFunction() const {
  return Base::AssertTrue(TryGetFunction());
}

Type::TType TResultDef::GetType() const {
  assert(this);
  return GetFunction()->GetReturnType();
}

TAnyFunction::TPtr TResultDef::TryGetFunction() const {
  assert(this);
  return Function.lock();
}