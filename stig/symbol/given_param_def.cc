/* <stig/symbol/given_param_def.cc>

   Implements <stig/symbol/given_param_def.h>

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

#include <stig/symbol/given_param_def.h>

using namespace Stig;
using namespace Stig::Symbol;

TGivenParamDef::TPtr TGivenParamDef::New(
    const TFunction::TPtr &function,
    const std::string &name,
    const Type::TType &type,
    const TPosRange &pos_range) {
  assert(function);
  auto given_param_def = TGivenParamDef::TPtr(new TGivenParamDef(function, name, type, pos_range));
  function->Add(given_param_def);
  return given_param_def;
}

TGivenParamDef::TGivenParamDef(
    const TFunction::TPtr &function,
    const std::string &name,
    const Type::TType &type,
    const TPosRange &pos_range)
      : TParamDef(function, name, type, pos_range) {}

TGivenParamDef::~TGivenParamDef() {
  assert(this);
  auto function = TryGetFunction();
  if (function) {
    function->Remove(shared_from_this());
  }
}

void TGivenParamDef::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}
