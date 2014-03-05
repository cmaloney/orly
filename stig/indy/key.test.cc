/* <stig/indy/key.test.cc>

   Unit test for <stig/indy/key.h>.

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

#include <stig/indy/key.h>

#include <stig/atom/suprena.h>

#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::Atom;
using namespace Stig::Indy;

FIXTURE(CoreFromKey) {
  TSuprena arena;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  TKey op(10UL, &arena, state_alloc);
  TSuprena new_arena;
  TCore op_copy(&new_arena, Sabot::State::TAny::TWrapper(op.GetCore().NewState(op.GetArena(), state_alloc)).get());
}

FIXTURE(KeyFromKey) {
  TSuprena arena;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  TKey key_1(10UL, &arena, state_alloc);
  TSuprena new_arena;
  TKey key_2(&new_arena, state_alloc, key_1);
}