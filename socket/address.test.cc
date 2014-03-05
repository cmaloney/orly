/* <socket/address.test.cc>

   Unit test for <socket/address.h>.

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

#include <socket/address.h>

#include <sstream>

#include <test/kit.h>

using namespace std;
using namespace Socket;

FIXTURE(Specials) {
  EXPECT_EQ(TAddress(TAddress::IPv4Any), TAddress(istringstream("0.0.0.0")));
  EXPECT_EQ(TAddress(TAddress::IPv4Loopback), TAddress(istringstream("127.0.0.1")));
  EXPECT_EQ(TAddress(TAddress::IPv6Any), TAddress(istringstream("[::]")));
  EXPECT_EQ(TAddress(TAddress::IPv6Loopback), TAddress(istringstream("[::1]")));
}
