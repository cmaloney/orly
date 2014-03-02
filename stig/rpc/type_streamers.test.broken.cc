/* <stig/rpc/type_streamers.test.broken.cc> 

   Unit test for <stig/rpc/type_streamers.h>.

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

#include <stig/rpc/type_streamers.h>

#include <cstring>
#include <sstream>
#include <string>

#include <stig/rpc/blob.h>
#include <stig/rpc/buffer_pool.h>
#include <stig/rpc/streamers.h>
#include <test/kit.h>

using namespace std;
using namespace Stig::Rpc;

static string RoundTrip(const char *in) {
  TBufferPool buffer_pool;
  TType::TPtr type;
  /* extra */ {
    auto blob = (TBlob::TWriter(&buffer_pool).Write(in, strlen(in))).DraftBlob();
    TBlob::TReader reader(&blob);
    reader >> type;
    if (reader) {
      return string("<extra stuff>");
    }
  }
  auto blob = (TBlob::TWriter(&buffer_pool) << type.get()).DraftBlob();
  ostringstream strm;
  strm << blob;
  return strm.str();
}

FIXTURE(Typical) {
  // Monomorphs.
  EXPECT_EQ(RoundTrip("b"), "b");
  EXPECT_EQ(RoundTrip("u"), "u");
  EXPECT_EQ(RoundTrip("i"), "i");
  EXPECT_EQ(RoundTrip("r"), "r");
  EXPECT_EQ(RoundTrip("s"), "s");
  EXPECT_EQ(RoundTrip("f"), "f");
  EXPECT_EQ(RoundTrip("p"), "p");
  // Sequences.
  EXPECT_EQ(RoundTrip("li"), "li");
  EXPECT_EQ(RoundTrip("oi"), "oi");
  EXPECT_EQ(RoundTrip("ei"), "ei");
  // And the rest.
  EXPECT_EQ(RoundTrip("dis"), "dis");

  // TODO: addr & obj (revamp round-trip to work with type instances instead of strings because of the counts, or get rid of the counts)

}

