/* <stig/rpc/transceiver.test.cc> 

   Unit test for <stig/rpc/transceiver.h>.

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

#include <stig/rpc/transceiver.h>

#include <string>

#include <base/fd.h>
#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig::Rpc;

FIXTURE(Typical) {
  /* Construct a local socket pair and a tranceiver which will use them. */
  TFd sock_a, sock_b;
  TFd::SocketPair(sock_a, sock_b, AF_UNIX, SOCK_STREAM, 0);
  TTransceiver xver;
  /* Send a message in two pieces. */
  string expected("Hello, world!");
  size_t
      size = expected.size(),
      half = size / 2;
  char *data = const_cast<char *>(expected.data());
  auto *vecs = xver.GetIoVecs(2);
  vecs[0].iov_base = data;
  vecs[0].iov_len  = half;
  vecs[1].iov_base = data + half;
  vecs[1].iov_len  = size - half;
  for (size_t part = 0; xver; xver += part) {
    part = xver.Send(sock_a);
  }
  /* Receive the message in one piece. */
  char actual[size];
  vecs = xver.GetIoVecs(1);
  vecs[0].iov_base = actual;
  vecs[0].iov_len  = size;
  for (size_t part = 0; xver; xver += part) {
    part = xver.Recv(sock_b);
  }
  /* Did we get it? */
  EXPECT_EQ(string(actual, size), expected);
}

