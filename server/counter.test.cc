/* <server/counter.test.cc>

   Unit test for <server/counter.h>.

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

#include <server/counter.h>

#include <cstdio>
#include <cstring>
#include <thread>
#include <unistd.h>
#include <sys/epoll.h>

#include <base/fd.h>
#include <base/zero.h>
#include <socket/address.h>
#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Server;
using namespace Socket;
using namespace Util;

SERVER_COUNTER(Connections);
SERVER_COUNTER(Requests);

static const size_t BufSize = 1024;

static void ServerMain(int die, int sock, bool &success) {
  try {
    success = true;
    TFd ep(epoll_create1(0));
    epoll_event event;
    Zero(event);
    event.data.fd = die;
    event.events = EPOLLIN;
    IfLt0(epoll_ctl(ep, EPOLL_CTL_ADD, die, &event));
    event.data.fd = sock;
    event.events = EPOLLIN;
    IfLt0(epoll_ctl(ep, EPOLL_CTL_ADD, sock, &event));
    for (;;) {
      IfLt0(epoll_wait(ep, &event, 1, -1));
      if (event.data.fd == die) {
        break;
      }
      if (event.data.fd == sock) {
        Connections.Increment();
        TFd cli(accept(sock, 0, 0));
        for (;;) {
          char buf[BufSize];
          ssize_t size;
          IfLt0(size = read(cli, buf, BufSize));
          if (!size) {
            break;
          }
          Requests.Increment();
          IfLt0(write(cli, buf, size));
        }
      }
    }
  } catch (...) {
    success = false;
  }
}

static void ClientMain(int id, const TAddress &address, uint32_t request_count, bool &success) {
  try {
    success = true;
    TFd my_socket(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    Connect(my_socket, address);
    for (uint32_t i = 0; i < request_count; ++i) {
      char request[BufSize];
      sprintf(request, "%d %d", id, i);
      ssize_t request_size = strlen(request);
      ssize_t size;
      IfLt0(size = write(my_socket, request, request_size));
      if (size != request_size) {
        throw 0;
      }
      char reply[BufSize];
      IfLt0(size = read(my_socket, reply, BufSize));
      if (size != request_size || strncmp(request, reply, request_size) != 0) {
        throw 0;
      }
    }
  } catch (...) {
    success = false;
  }
}

FIXTURE(Typical) {
  int fds[2];
  IfLt0(pipe(fds));
  TFd recv_die(fds[0]), send_die(fds[1]);
  TFd listening_socket(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  Bind(listening_socket, TAddress(TAddress::IPv4Loopback));
  IfLt0(listen(listening_socket, 5));
  TAddress address = GetSockName(listening_socket);
  bool server_success = false, client_1_success = false, client_2_success = false, client_3_success = false;
  static const uint32_t request_count = 5;
  thread
    server(ServerMain, static_cast<int>(recv_die), static_cast<int>(listening_socket), ref(server_success)),
    client_1(ClientMain, 101, cref(address), request_count, ref(client_1_success)),
    client_2(ClientMain, 102, cref(address), request_count, ref(client_2_success)),
    client_3(ClientMain, 103, cref(address), request_count, ref(client_3_success));
  client_1.join();
  client_2.join();
  client_3.join();
  IfLt0(write(send_die, "x", 1));
  server.join();
  EXPECT_TRUE(server_success);
  EXPECT_TRUE(client_1_success);
  EXPECT_TRUE(client_2_success);
  EXPECT_TRUE(client_3_success);
  TCounter::Sample();
  EXPECT_EQ(Connections.GetCount(), 3U);
  EXPECT_EQ(Requests.GetCount(), request_count * 3);
  TCounter::Reset();
  EXPECT_FALSE(Connections.GetCount());
  EXPECT_FALSE(Requests.GetCount());
}
