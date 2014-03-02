/* <stig/parse_compiled_import.cc> 

   Implements <stig/parse_compiled_import.h>.

   Copyright 2010-2014 Tagged
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   
     http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stig/parse_compiled_import.h>

#include <cassert>

#include <stig/atom/transport_arena2.h>
#include <stig/sabot/to_native.h>
#include <stig/var/sabot_to_var.h>

using namespace std;
using namespace Stig;

bool Stig::ParseCompiledImport(
    Io::TBinaryInputStream &strm, const function<bool (size_t)> &count_cb, const function<bool (const vector<std::pair<Base::TUuid, Var::TVar>> &)> &xact_cb) {
  assert(&strm);
  assert(&count_cb);
  assert(&xact_cb);
  size_t xact_count;
  strm >> xact_count;
  if (!count_cb(xact_count)) {
    return false;
  }
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  for (size_t i = 0; i < xact_count; ++i) {
    vector<std::pair<Base::TUuid, Var::TVar>> xact;
    Atom::TCore core;
    auto arena = Atom::TTransportArena::Read(strm, core);
    try {
      Sabot::ToNative(*Sabot::State::TAny::TWrapper(core.NewState(arena, state_alloc)), xact);
    } catch (...) {
      delete arena;
      throw;
    }
    delete arena;
    if (!xact_cb(xact)) {
      return false;
    }
  }
  return true;
}

