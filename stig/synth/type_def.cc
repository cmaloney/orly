/* <stig/synth/type_def.cc>

   Implements <stig/synth/type_def.h>.

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

#include <stig/synth/type_def.h>

#include <base/assert_true.h>
#include <base/no_default_case.h>
#include <stig/synth/new_type.h>

using namespace Stig;
using namespace Stig::Synth;

TTypeDef::TTypeDef(TScope *scope, const Package::Syntax::TTypeDef *type_def)
    : TDef(scope, Base::AssertTrue(type_def)->GetName()),
      Type(NewType(type_def->GetType())) {}

TTypeDef::~TTypeDef() {
  assert(this);
  delete Type;
}

TAction TTypeDef::Build(int pass) {
  assert(this);
  switch (pass) {
    case 1: {
      Type->GetSymbolicType();
      break;
    }
    NO_DEFAULT_CASE;
  }
  return Finish;
}

void TTypeDef::ForEachPred(int pass, const std::function<bool (TDef *)> &cb) {
  assert(this);
  switch (pass) {
    case 1: {
      Type->ForEachRef([cb](TAnyRef &ref) -> bool {
        cb(ref.GetDef());
        return true;
      });
      break;
    }
    NO_DEFAULT_CASE;
  }
}

void TTypeDef::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  Type->ForEachRef(cb);
}

const Type::TType &TTypeDef::GetSymbolicType() const {
  assert(this);
  return Type->GetSymbolicType();
}

const char *TDef::TInfo<TTypeDef>::Name = "a type defintion";
