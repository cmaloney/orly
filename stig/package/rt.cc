/* <stig/package/rt.cc>

   Implements <stig/package/rt.h>

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

#include <stig/package/rt.h>

using namespace Stig;
using namespace Stig::Package;

#if 0
void TContext::AddEffect(const Var::TVar &addr, const Var::TPtr<Var::TChange> &change) {
  assert(&addr);
  assert(&change);
  assert(change);

  Spa::FluxCapacitor::TKV kv(addr);

  auto it = Effects.find(kv);

  if (it == Effects.end()) {
    Effects.insert(make_pair(kv, change));
  } else {
    it->second->Augment(change);
  }
}
#endif