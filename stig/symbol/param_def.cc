/* <stig/symbol/param_def.cc>

   Implements <stig/symbol/param_def.h>

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

#include <stig/symbol/param_def.h>

#include <base/assert_true.h>

using namespace Stig;
using namespace Stig::Symbol;

TParamDef::TParamDef(
    const TFunction::TPtr &function,
    const std::string &name,
    const Type::TType &type,
    const TPosRange &pos_range)
      : TFunction::TParamDef(name, pos_range),
        Function(Base::AssertTrue(function)),
        Type(type) {}

TFunction::TPtr TParamDef::GetFunction() const {
  return Base::AssertTrue(TryGetFunction());
}

Type::TType TParamDef::GetType() const {
  assert(this);
  return Type;
}

TFunction::TPtr TParamDef::TryGetFunction() const {
  assert(this);
  return Function.lock();
}
