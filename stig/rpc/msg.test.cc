/* <stig/rpc/msg.test.cc> 

   Unit test <stig/rpc/msg.h>.

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

#include <stig/rpc/msg.h>

#include <sstream>
#include <string>

#include <base/event_semaphore.h>
#include <base/fd.h>
#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig::Rpc;

static string ToString(const TBlob &blob) {
  ostringstream strm;
  strm << blob;
  return strm.str();
}

FIXTURE(Typical) {
  /* Construct a buffer pool, a semaphore, and a local socket pair and a tranceiver which will use them. */
  TBufferPool buffer_pool;
  TEventSemaphore sem;
  TFd sock_a, sock_b;
  TFd::SocketPair(sock_a, sock_b, AF_UNIX, SOCK_STREAM, 0);
  TSafeTransceiver xver(sem.GetFd());
  /* Make up a message to send, then send it. */
  TMsg::TKind expected_kind = TMsg::TKind::NormalReply;
  TMsg::TReqId expected_req_id = 1001;
  const string expected_str = "Mofo the Psychic Gorilla!";
  /* extra */ {
    TBlob::TWriter writer(&buffer_pool);
    writer.Write(expected_str.data(), expected_str.size());
    TMsg(expected_kind, expected_req_id, writer.DraftBlob()).Send(&xver, sock_a);
  }
  /* Receive the message and make sure we got what we sent. */
  TMsg msg = TMsg::Recv(&buffer_pool, &xver, sock_b);
  EXPECT_TRUE(msg.GetKind() == expected_kind);
  EXPECT_EQ(msg.GetReqId(), expected_req_id);
  EXPECT_EQ(ToString(msg.GetBlob()), expected_str);
}

