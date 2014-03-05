/* <socket/factory.test.cc>

   Unit test for <socket/factory.h>.

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

#include <socket/factory.h>

#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Socket;

FIXTURE(Typical) {
  static const string expected = "Mofo the Psychic Gorilla.";
  /* Make a pair of UDP sockets with system-assigned ports. */
  TFactory f;
  f.Type = SOCK_DGRAM;
  TAddress addr_a, addr_b;
  TFd
      a = f.New(addr_a),
      b = f.New(addr_b);
  /* We should get two different addresses. */
  EXPECT_NE(addr_a, addr_b);
  /* Send some data from a to b and have b receive it. */
  SendTo(a, expected.data(), expected.size(), 0, addr_b);
  TAddress sender;
  char actual[1024];
  auto size = RecvFrom(b, actual, sizeof(actual) - 1, 0, sender);
  actual[size] = '\0';
  /* We should have gotten the message from a. */
  EXPECT_EQ(expected, actual);
  EXPECT_EQ(sender, addr_a);
}

