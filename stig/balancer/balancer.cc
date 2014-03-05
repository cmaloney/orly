/* <stig/balancer/balancer.cc>

   Implements <stig/balancer/balancer.h>.

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

#include <stig/balancer/balancer.h>

#include <base/epoll.h>

using namespace std;
using namespace Base;
using namespace Socket;
using namespace Stig::Balancer;

TBalancer::TBalancer(TScheduler *scheduler, const TCmd &cmd)
    : Scheduler(scheduler) {
  /* open the main socket */ {
    TAddress address(TAddress::IPv4Any, cmd.PortNumber);
    MainSocket = TFd(socket(address.GetFamily(), SOCK_STREAM, 0));
    int flag = true;
    IfLt0(setsockopt(MainSocket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)));
    Bind(MainSocket, address);
    IfLt0(listen(MainSocket, cmd.ConnectionBacklog));
  }
  scheduler->Schedule(bind(&TBalancer::AcceptClientConnections, this));
}

TBalancer::~TBalancer() {}

void TBalancer::AcceptClientConnections() {
  assert(this);
  for (;;) {
    TAddress client_address;
    TFd client_socket(Accept(MainSocket, client_address));
    Scheduler->Schedule(bind(&TBalancer::ServeClient, this, move(client_socket), client_address));
  }
}

void TBalancer::ServeClient(TFd &fd, const TAddress &client_address) {
  assert(this);
  assert(&fd);
  assert(&client_address);
  const size_t buf_size = 4096;
  /* Figure out which host to route to. */
  const Socket::TAddress &server_address = ChooseHost();
  TFd new_server_socket(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  try {
    Connect(new_server_socket, server_address);
    char buf[buf_size];
    TEpoll poll;
    poll.Add(fd);
    poll.Add(new_server_socket);
    for (;;) {
      int ready_fd = poll.WaitForOne();
      size_t amt_read = ReadAtMost(ready_fd, &buf, buf_size);
      if (amt_read) {
        WriteExactly(ready_fd == fd ? new_server_socket : fd, &buf, amt_read);
      } else {
        return;
      }
    }
  } catch (const std::exception &ex) {
    OnError(ex);
  }
}