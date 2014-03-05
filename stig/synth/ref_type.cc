/* <stig/synth/ref_type.cc>

   Implements <stig/synth/ref_type.h>.

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

#include <stig/synth/ref_type.h>

#include <base/assert_true.h>

using namespace Stig;
using namespace Stig::Synth;

TRefType::TRefType(const Package::Syntax::TRefType *ref_type)
    : TypeDef(Base::AssertTrue(ref_type)->GetName()) {}

void TRefType::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  cb(TypeDef);
}

Type::TType TRefType::ComputeSymbolicType() const {
  assert(this);
  return TypeDef->GetSymbolicType();
}
