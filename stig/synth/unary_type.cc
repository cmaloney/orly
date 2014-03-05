/* <stig/synth/unary_type.cc>

   Implements <stig/synth/unary_type.h>.

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

#include <stig/synth/unary_type.h>

#include <cassert>

#include <base/assert_true.h>

using namespace Stig;
using namespace Stig::Synth;

TUnaryType::TUnaryType(TType *type, TGet get)
    : Type(Base::AssertTrue(type)), Get(get) {}

TUnaryType::~TUnaryType() {
  assert(this);
  delete Type;
}

void TUnaryType::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  Type->ForEachRef(cb);
}

Type::TType TUnaryType::ComputeSymbolicType() const {
  assert(this);
  return Get(Type->GetSymbolicType());
}
