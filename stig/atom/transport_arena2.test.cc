/* <stig/atom/transport_arena2.test.cc>

   Unit test for <stig/atom/transport_arena2.h>.

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

#include <stig/atom/transport_arena2.h>

#include <memory>
#include <sstream>
#include <string>

#include <io/binary_input_only_stream.h>
#include <io/binary_output_only_stream.h>
#include <io/recorder_and_player.h>
#include <stig/atom/kit2.h>
#include <stig/native/all.h>
#include <stig/sabot/state_dumper.h>
#include <test/kit.h>

using namespace std;
using namespace Io;
using namespace Stig;
using namespace Stig::Atom;

template <typename TVal>
static TCore ToCore(TCore::TExtensibleArena *arena, const TVal &val) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Sabot::State::TAny::TWrapper state(Native::State::New(val, state_alloc));
  return TCore(arena, state.get());
}

static string ToString(TCore::TArena *arena, const TCore &core) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Sabot::State::TAny::TWrapper state(core.NewState(arena, state_alloc));
  ostringstream strm;
  state->Accept(Sabot::TStateDumper(strm));
  return strm.str();
}

FIXTURE(Sanity) {
  TSuprena arena;
  TCore core = ToCore(&arena, vector<int>({ 101, 102, 103 }));
  EXPECT_EQ(ToString(&arena, core), "vector(101, 102, 103)");
}

template <typename TVal>
static string RoundTrip(const TVal &in) {
  auto recorder = make_shared<TRecorder>();
  /* write */ {
    TBinaryOutputOnlyStream strm(recorder);
    TSuprena arena;
    TTransportArena::Write(strm, &arena, ToCore(&arena, in));
  }
  TBinaryInputOnlyStream strm(make_shared<TPlayer>(recorder));
  TCore core;
  unique_ptr<TCore::TArena> arena(TTransportArena::Read(strm, core));
  return ToString(arena.get(), core);
}

FIXTURE(RoundTrip) {
  EXPECT_EQ(RoundTrip(vector<int>({ 101, 102, 103 })), "vector(101, 102, 103)");
  EXPECT_EQ(
      RoundTrip(make_tuple(
          vector<int>({ 101, 102, 103 }),
          vector<string>({ "This is a very long string.  It has to be long so it will be interned.  And it should be.  So there.",
                           "This, too, is a very long string.  It will follow its friend into the warm embrace of the interner." })
      )),
      "tuple(vector(101, 102, 103), vector(\"This is a very long string.  It has to be long so it will be interned.  And it should be.  So there.\", \"This, too, is a very long string.  It will follow its friend into the warm embrace of the interner.\"))"
  );
}
