/* <socket/option.test.cc>

   Unit test for <socket/option.h>.

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

#include <socket/option.h>

#include <sstream>

#include <base/fd.h>
#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Socket;

FIXTURE(LowLevelGetAndSet) {
  TFd sock(socket(AF_INET, SOCK_STREAM, 0));
  int arg;
  GetSockOpt(sock, SO_REUSEADDR, arg);
  EXPECT_EQ(arg, 0);
  SetSockOpt(sock, SO_REUSEADDR, 1);
  GetSockOpt(sock, SO_REUSEADDR, arg);
  EXPECT_EQ(arg, 1);
  SetSockOpt(sock, SO_REUSEADDR, 0);
  GetSockOpt(sock, SO_REUSEADDR, arg);
  EXPECT_EQ(arg, 0);
}

FIXTURE(ConvBool) {
  TFd sock(socket(AF_INET, SOCK_STREAM, 0));
  bool val;
  Conv<bool>::GetSockOpt(sock, SO_REUSEADDR, val);
  EXPECT_FALSE(val);
  Conv<bool>::SetSockOpt(sock, SO_REUSEADDR, true);
  Conv<bool>::GetSockOpt(sock, SO_REUSEADDR, val);
  EXPECT_TRUE(val);
  Conv<bool>::SetSockOpt(sock, SO_REUSEADDR, false);
  Conv<bool>::GetSockOpt(sock, SO_REUSEADDR, val);
  EXPECT_FALSE(val);
}

FIXTURE(ConvInt) {
  TFd sock(socket(AF_INET, SOCK_STREAM, 0));
  int val;
  Conv<int>::GetSockOpt(sock, SO_REUSEADDR, val);
  EXPECT_EQ(val, 0);
  Conv<int>::SetSockOpt(sock, SO_REUSEADDR, 1);
  Conv<int>::GetSockOpt(sock, SO_REUSEADDR, val);
  EXPECT_EQ(val, 1);
  Conv<int>::SetSockOpt(sock, SO_REUSEADDR, 0);
  Conv<int>::GetSockOpt(sock, SO_REUSEADDR, val);
  EXPECT_EQ(val, 0);
}

FIXTURE(ConvLinger) {
  TFd sock(socket(AF_INET, SOCK_STREAM, 0));
  Conv<TLinger>::SetSockOpt(sock, SO_LINGER, TLinger());
  TLinger val;
  Conv<TLinger>::GetSockOpt(sock, SO_LINGER, val);
  EXPECT_FALSE(val.IsKnown());
  Conv<TLinger>::SetSockOpt(sock, SO_LINGER, TLinger(seconds(30)));
  Conv<TLinger>::GetSockOpt(sock, SO_LINGER, val);
  if (EXPECT_TRUE(val.IsKnown())) {
    EXPECT_EQ(val->count(), 30);
  }
}

FIXTURE(ConvTimeout) {
  TFd sock(socket(AF_INET, SOCK_STREAM, 0));
  Conv<TTimeout>::SetSockOpt(sock, SO_RCVTIMEO, TTimeout(2000000));
  TTimeout val;
  Conv<TTimeout>::GetSockOpt(sock, SO_RCVTIMEO, val);
  EXPECT_EQ(val.count(), 2000000);
  Conv<TTimeout>::SetSockOpt(sock, SO_RCVTIMEO, TTimeout(0));
  Conv<TTimeout>::GetSockOpt(sock, SO_RCVTIMEO, val);
  EXPECT_EQ(val.count(), 0);
}

template <typename TVal>
static string ToString(const TVal &val) {
  ostringstream strm;
  Format<TVal>::Dump(strm, val);
  return strm.str();
}

FIXTURE(FormatBool) {
  EXPECT_EQ(ToString(true), "true");
  EXPECT_EQ(ToString(false), "false");
}

FIXTURE(FormatInt) {
  EXPECT_EQ(ToString(0), "0");
  EXPECT_EQ(ToString(101), "101");
  EXPECT_EQ(ToString(-101), "-101");
}

FIXTURE(FormatLinger) {
  EXPECT_EQ(ToString(TLinger()), "off");
  EXPECT_EQ(ToString(TLinger(seconds(0))), "0 sec(s)");
  EXPECT_EQ(ToString(TLinger(seconds(30))), "30 sec(s)");
}

FIXTURE(FormatUcred) {
  ucred val;
  val.pid = 101;
  val.uid = 202;
  val.gid = 303;
  EXPECT_EQ(ToString(val), "{ pid: 101, uid: 202, gid: 303 }");
}

FIXTURE(FormatTimeout) {
  EXPECT_EQ(ToString(TTimeout()), "0 usec(s)");
  EXPECT_EQ(ToString(TTimeout(microseconds(200))), "200 usec(s)");
}

FIXTURE(FormatString) {
  EXPECT_EQ(ToString(string()), "\"\"");
  EXPECT_EQ(ToString(string("eth0")), "\"eth0\"");
}

FIXTURE(StdObject) {
  TFd sock(socket(AF_INET, SOCK_STREAM, 0));
  bool val = ReuseAddr.Get(sock);
  EXPECT_FALSE(val);
  ReuseAddr.Set(sock, true);
  ReuseAddr.Get(sock, val);
  EXPECT_TRUE(val);
  ReuseAddr.Set(sock, false);
  ReuseAddr.Get(sock, val);
  EXPECT_FALSE(val);
  ostringstream strm;
  ReuseAddr.Dump(strm, sock);
  EXPECT_EQ(strm.str(), "reuse_addr: false");
}
