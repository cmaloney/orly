/* <stig/symbol/any_function.cc>

   Implements <stig/symbol/any_function.h>

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

#include <stig/symbol/any_function.h>

#include <algorithm>

#include <stig/symbol/param_def.h>

using namespace Stig;
using namespace Stig::Symbol;

TAnyFunction::TAnyFunction(const std::string &name)
    : Name(name) {}

void TAnyFunction::Add(const TResultDef::TPtr &result_def) {
  assert(this);
  assert(result_def);
  ResultDefs.push_back(result_def);
}

const std::string &TAnyFunction::GetName() const {
  assert(this);
  return Name;
}

const TAnyFunction::TResultDefVec &TAnyFunction::GetResultDefs() const {
  assert(this);
  return ResultDefs;
}

Type::TType TAnyFunction::GetType() const {
  assert(this);
  return Type::TFunc::Get(Type::TObj::Get(GetParams()), GetReturnType());
}

void TAnyFunction::Remove(const TResultDef::TPtr &result_def) {
  assert(this);
  assert(result_def);
  auto result = std::find(ResultDefs.begin(), ResultDefs.end(), result_def);
  if (result != ResultDefs.end()) {
    ResultDefs.erase(result);
  }
}
