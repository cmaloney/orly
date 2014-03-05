/* <stig/synth/binary_type.cc>

   Implements <stig/synth/binary_type.h>.

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

#include <stig/synth/binary_type.h>

#include <base/assert_true.h>

using namespace Stig;
using namespace Stig::Synth;

TBinaryType::TBinaryType(TType *lhs, TType *rhs, TGet get)
    : Get(get), Lhs(Base::AssertTrue(lhs)), Rhs(Base::AssertTrue(rhs)) {}

TBinaryType::~TBinaryType() {
  assert(this);
  delete Lhs;
  delete Rhs;
}

void TBinaryType::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Lhs->ForEachRef(cb);
  Rhs->ForEachRef(cb);
}

Type::TType TBinaryType::ComputeSymbolicType() const {
  assert(this);
  return Get(Lhs->GetSymbolicType(), Rhs->GetSymbolicType());
}
