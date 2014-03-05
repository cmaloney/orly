/* <stig/method_result.cc>

   Implements <stig/method_result.h>.

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

#include <stig/method_result.h>

#include <stig/atom/suprena.h>
#include <stig/atom/transport_arena2.h>

using namespace std;
using namespace Base;
using namespace Io;
using namespace Stig;
using namespace Stig::Atom;

TMethodResult::TMethodResult(const shared_ptr<TArena> &arena, const TCore &value, const TOpt<TTracker> &tracker, bool is_error)
    : Arena(arena), Value(value), Tracker(tracker), Error(is_error) {}

TMethodResult::TMethodResult(TArena *arena, const Atom::TCore &value, const Base::TOpt<TTracker> &tracker, bool is_error)
    : Tracker(tracker), Error(is_error) {
  auto new_arena = make_shared<TSuprena>();
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Value = TCore(new_arena.get(), state_alloc, arena, value);
  Arena = new_arena;
}

void TMethodResult::Read(TBinaryInputStream &strm) {
  assert(this);
  assert(&strm);
  Reset();
  Arena.reset(TTransportArena::Read(strm, Value));
  strm >> Tracker >> Error;
}

void TMethodResult::Reset() {
  assert(this);
  Arena.reset();
  Value = TCore();
  Tracker.Reset();
}

void TMethodResult::Write(TBinaryOutputStream &strm) const {
  assert(this);
  assert(&strm);
  auto temp = dynamic_pointer_cast<TSuprena>(Arena);
  assert(temp);
  TTransportArena::Write(strm, temp.get(), Value);
  strm << Tracker << Error;
}
