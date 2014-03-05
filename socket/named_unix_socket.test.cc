/* <socket/named_unix_socket.test.cc>

   Unit test for <socket/named_unix_socket.h>.

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

#include <socket/named_unix_socket.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <socket/address.h>
#include <test/kit.h>

using namespace Socket;

FIXTURE(NamedUnixSocketTest) {
  TNamedUnixSocket sock(SOCK_DGRAM, 0);
  EXPECT_TRUE(sock.IsOpen());
  EXPECT_FALSE(sock.IsBound());
  EXPECT_TRUE(sock.GetPath().empty());
  int fd = sock;
  int fd2 = sock.GetFd();
  EXPECT_EQ(fd, fd2);
  TAddress address;
  address.SetFamily(AF_LOCAL);
  const char path[] = "/tmp/named_unix_socket_test";
  address.SetPath(path);
  Bind(sock, address);
  EXPECT_TRUE(sock.IsOpen());
  EXPECT_TRUE(sock.IsBound());
  struct stat buf;
  int ret = stat(path, &buf);
  EXPECT_EQ(ret, 0);
  EXPECT_TRUE(S_ISSOCK(buf.st_mode));
  sock.Reset();
  EXPECT_FALSE(sock.IsOpen());
  EXPECT_FALSE(sock.IsBound());
  ret = stat(path, &buf);
  EXPECT_EQ(ret, -1);

  {
    TNamedUnixSocket sock2(SOCK_DGRAM, 0);
    TAddress address2;
    address2.SetFamily(AF_LOCAL);
    address2.SetPath(path);
    Bind(sock2, address2);
    EXPECT_TRUE(sock2.IsOpen());
    EXPECT_TRUE(sock2.IsBound());
    ret = stat(path, &buf);
    EXPECT_EQ(ret, 0);
    EXPECT_TRUE(S_ISSOCK(buf.st_mode));
  }

  ret = stat(path, &buf);
  EXPECT_EQ(ret, -1);
}