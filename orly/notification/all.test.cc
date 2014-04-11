/* <orly/notification/all.test.cc>

   Unit test for <orly/notification/all.h>.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/notification/all.h>

#include <io/binary_input_only_stream.h>
#include <io/binary_output_only_stream.h>
#include <io/recorder_and_player.h>
#include <orly/notification/system_shutdown.h>
#include <orly/notification/pov_failure.h>
#include <orly/notification/update_progress.h>
#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Io;
using namespace Stig::Notification;

static TNotification *Copy(const TNotification *expected) {
  auto recorder = make_shared<TRecorder>();
  /* write */ {
    TBinaryOutputOnlyStream strm(recorder);
    Write(strm, expected);
  }
  TBinaryInputOnlyStream strm(make_shared<TPlayer>(recorder));
  return New(strm);
}

FIXTURE(SystemShutdown) {
  unique_ptr<TNotification>
      expected(TSystemShutdown::New(seconds(600))),
      actual(Copy(expected.get()));
  EXPECT_NE(actual.get(), expected.get());
  EXPECT_TRUE(Matches(*actual, *expected));
}

FIXTURE(PovFailure) {
  unique_ptr<TNotification>
      expected(TPovFailure::New(TUuid::Random)),
      actual(Copy(expected.get()));
  EXPECT_NE(actual.get(), expected.get());
  EXPECT_TRUE(Matches(*actual, *expected));
}

FIXTURE(UpdateProgress) {
  unique_ptr<TNotification>
      expected(TUpdateProgress::New(TUuid::Random, TUuid::Random, TUpdateProgress::Accepted)),
      actual(Copy(expected.get()));
  EXPECT_NE(actual.get(), expected.get());
  EXPECT_TRUE(Matches(*actual, *expected));
}
