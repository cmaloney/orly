/* <stig/method_request.test.cc>

   Unit test for <stig/method_request.h>.

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

#include <stig/method_request.h>

#include <tuple>

#include <io/binary_input_only_stream.h>
#include <io/binary_output_only_stream.h>
#include <io/recorder_and_player.h>
#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Io;
using namespace Stig;

void CheckMethodRequest(const TMethodRequest &method_request) {
  assert(&method_request);
  EXPECT_TRUE(method_request.GetPovId());
  EXPECT_EQ(method_request.GetPackage().size(), 3u);
  EXPECT_EQ(method_request.GetClosure().GetMethodName(), string("deal"));
  EXPECT_EQ(method_request.GetClosure().GetArgCount(), 1u);
  string arg;
  method_request.GetClosure().GetArg("game", arg);
  EXPECT_EQ(arg, string("hold-em"));
  EXPECT_FALSE(method_request.GetTrackingId());
  EXPECT_EQ(method_request.GetTimeToLive().count(), 0);
}

FIXTURE(Typical) {
  TMethodRequest method_request(TUuid::Best, { "apps", "games", "poker" }, TClosure("deal", string("game"), string("hold-em")));
  CheckMethodRequest(method_request);
  auto recorder = make_shared<TRecorder>();
  /* write */ {
    TBinaryOutputOnlyStream strm(recorder);
    strm << method_request;
  }
  TBinaryInputOnlyStream strm(make_shared<TPlayer>(recorder));
  TMethodRequest copyof_method_request;
  strm >> copyof_method_request;
  CheckMethodRequest(copyof_method_request);
}
