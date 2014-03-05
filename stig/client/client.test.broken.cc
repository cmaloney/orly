/* <stig/client/client.test.broken.cc>

   Unit test for <stig/client/client.h>.

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

// #include <stig/client/client.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <base/fd.h>
#include <socket/address.h>
#include <stig/client/test_server.h>
#include <test/kit.h>

/* Standard namespaces. */
using namespace std;
using namespace chrono;

/* Library namespaces. */
using namespace Base;
using namespace Socket;

/* Stig namespaces. */
using namespace Stig;
using namespace BinProto;
using namespace Client;

FIXTURE(Typical) {
  TTestServer::TCmd cmd(true);
  TTestServer server(cmd);
  TFd client(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  Connect(client, TAddress(TAddress::IPv4Any, cmd.PortNumber));

  char buf[8192];
  char *start = buf;
  TMsgHdr::Encode(start, TMsgHdr::TByteOrder::Network,

  ssize_t expected = send(client, "hello", 5, 0);
  if (EXPECT_GT(expected, 0)) {
    char buf[1024];
    ssize_t actual = recv(client, buf, sizeof(buf), MSG_NOSIGNAL);
    EXPECT_EQ(actual, expected);
  }
}
