/* <stig/method_result.test.cc>

   Unit test for <stig/method_result.h>.

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

#include <tuple>

#include <io/binary_input_only_stream.h>
#include <io/binary_output_only_stream.h>
#include <io/recorder_and_player.h>
#include <stig/atom/suprena.h>
#include <stig/indy/key.h>
#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Io;
using namespace Stig;
using namespace Stig::Atom;

template <typename TValue>
static void RoundTrip(const TValue &value, const TOpt<TTracker> &actual_tracker = *TOpt<TTracker>::Unknown) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  auto recorder = make_shared<TRecorder>();
  /* write */ {
    TSuprena arena;
    TBinaryOutputOnlyStream strm(recorder);
    strm << TMethodResult(&arena, TCore(value, &arena, state_alloc), actual_tracker);
  }
  TBinaryInputOnlyStream strm(make_shared<TPlayer>(recorder));
  TMethodResult method_result;
  strm >> method_result;
  TSuprena arena;
  EXPECT_EQ(Indy::TKey(method_result.GetValue(), method_result.GetArena().get()), Indy::TKey(value, &arena, state_alloc));
  const auto &expected_tracker = method_result.GetTracker();
  if (EXPECT_EQ(actual_tracker.IsKnown(), expected_tracker.IsKnown()) && expected_tracker.IsKnown()) {
    EXPECT_EQ(actual_tracker->Id, expected_tracker->Id);
    EXPECT_EQ(actual_tracker->TimeToLive.count(), expected_tracker->TimeToLive.count());
  }
}

FIXTURE(Typical) {
  RoundTrip(string("Mofo the Psychic Gorilla will now perform delightful feats of mental fancy."));
  RoundTrip(make_tuple(101, true, 98.6, set<int>({ 101, 102, 103 })), TTracker({ TUuid::Best, seconds(300) }));
}
