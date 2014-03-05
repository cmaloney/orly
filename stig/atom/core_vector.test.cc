/* <stig/atom/core_vector.test.cc>

   Unit test for <stig/atom/core_vector.h> and <stig/atom/core_vector_builder.h>.

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

#include <stig/atom/core_vector.h>
#include <stig/atom/core_vector_builder.h>

#include <memory>
#include <sstream>
#include <string>

#include <io/binary_input_only_stream.h>
#include <io/binary_output_only_stream.h>
#include <io/recorder_and_player.h>
#include <stig/sabot/state_dumper.h>
#include <test/kit.h>

using namespace std;
using namespace Io;
using namespace Stig;
using namespace Stig::Atom;

static const char *LongStr = "This is a very long string.  It has to be long so it will be interned.  And it should be.  So there.";

static string ToString(TCore::TArena *arena, const TCore &core) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Sabot::State::TAny::TWrapper state(core.NewState(arena, state_alloc));
  ostringstream strm;
  state->Accept(Sabot::TStateDumper(strm));
  return strm.str();
}

FIXTURE(RoundTrip) {
  TCoreVectorBuilder cv_builder;
  /* Push values into the builder. */ {
    for (int i = 101; i <= 110; ++i) {
      cv_builder.Push(i);
    }
    cv_builder.Push(LongStr);
    cv_builder.Push(make_tuple(LongStr, 98.6, true));
  }
  auto recorder = make_shared<TRecorder>();
  /* Stream the vector out. */ {
    TBinaryOutputOnlyStream strm(recorder);
    cv_builder.Write(strm);
  }
  TBinaryInputOnlyStream strm(make_shared<TPlayer>(recorder));
  TCoreVector cv(strm);
  const vector<TCore>
      &cores_written = cv_builder.GetCores(),
      &cores_read = cv.GetCores();
  if (EXPECT_EQ(cores_read.size(), cores_written.size())) {
    size_t size = cores_written.size();
    for (size_t i = 0; i < size; ++i) {
      EXPECT_EQ(ToString(cv.GetArena(), cores_read[i]), ToString(cv_builder.GetArena(), cores_written[i]));
    }
  }
}
