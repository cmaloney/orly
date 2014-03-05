/* <stig/type/mutable.cc>

   Implements <stig/type/mutable.h>

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

#include <stig/type/mutable.h>
#include <stig/type/unwrap.h>
#include <stig/type/util.h>

using namespace std;
using namespace Stig::Type;

IMPL_INTERNED_TYPE(TMutable, TType, TMutable::TParts, TType, TType);

TType TMutable::Get(const TType &addr, const TType &val) {
  return TMutable::Get(addr, {}, val, val);
}

TType TMutable::Get(const TType &addr, const TType &val, const TType &src_at_addr) {
  return TMutable::Get(addr, {}, val, src_at_addr);
}

TType TMutable::Get(const TType &addr, const TParts &parts, const TType &val, const TType &src_at_addr) {
  TType unwrapped_addr = UnwrapOptional(addr);
  assert(!unwrapped_addr.Is<TMutable>());
  assert(unwrapped_addr.Is<TAddr>());
  assert(!val.Is<TMutable>());

  return TInternedType::Get(addr, parts, val, src_at_addr);
}

TType TMutable::GetAddr() const {
  assert(this);
  return get<0>(GetKey());
}

const TMutable::TParts &TMutable::GetParts() const {
  assert(this);
  return get<1>(GetKey());
}

TType TMutable::GetVal() const {
  assert(this);
  return get<2>(GetKey());
}

TType TMutable::GetSrcAtAddr() const {
  assert(this);
  return get<3>(GetKey());
}

void TMutable::Write(ostream &strm) const {
  assert(this);
  strm << "Stig::Rt::TMutable<" << GetAddr() << ", " << GetVal() << '>';
}

TMutable::TMutable(const TType &addr, const TParts &parts, const TType &val, const TType &src_at_addr)
    : TInternedType(addr, parts, val, src_at_addr) {}

TMutable::~TMutable() {}
