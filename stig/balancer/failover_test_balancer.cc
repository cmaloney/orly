/* <stig/balancer/failover_test_balancer.cc>

   Implements <stig/balancer/failover_test_balancer.h>.

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

#include <stig/balancer/failover_test_balancer.h>

#include <base/epoll.h>
#include <base/timer_fd.h>
#include <stig/protocol.h>

using namespace std;
using namespace Base;
using namespace Stig::Handshake;
using namespace Stig::Balancer;

TFailoverTestBalancer::TFailoverTestBalancer(TScheduler *scheduler, const TBalancer::TCmd &cmd, size_t milli_interval)
    : TBalancer(scheduler, cmd), MilliInterval(milli_interval), Running(true) {
  scheduler->Schedule(bind(&TFailoverTestBalancer::CheckHosts, this));
}

TFailoverTestBalancer::~TFailoverTestBalancer() {
  Running = false;
  std::unique_lock<std::mutex> lock(HostMutex);
  HostCond.wait(lock);
}

const Socket::TAddress &TFailoverTestBalancer::ChooseHost() {
  assert(this);
  std::lock_guard<std::mutex> lock(HostMutex);
  if (!MasterHost) {
    throw std::runtime_error("No master host available to connect to.");
  }
  return *MasterHost;
}

void TFailoverTestBalancer::RegisterHost(const Socket::TAddress &address) {
  assert(this);
  std::lock_guard<std::mutex> lock(HostMutex);
  auto ret = HostSet.insert(address);
  if (!ret.second) {
    throw std::runtime_error("Host already exists in HostSet.");
  }
}

void TFailoverTestBalancer::CheckHosts() {
  Base::TTimerFd check_hosts(MilliInterval);
  TEpoll poll;
  poll.Add(check_hosts.GetFd());
  poll.Add(ErrorSem.GetFd());
  for (;Running;) {
    int ready_fd = poll.WaitForOne();
    if (ready_fd == ErrorSem.GetFd()) {
      ErrorSem.Pop();
    } else {
      check_hosts.Pop();
    }
    std::lock_guard<std::mutex> lock(HostMutex);
    MasterHost.Reset();
    for (const auto &addr : HostSet) {
      bool is_master = CheckHost(addr);
      if (MasterHost && is_master) {
        MasterHost.Reset();
        throw std::runtime_error("There is more than 1 master");
      } else if (is_master) {
        MasterHost = addr;
      }
    }
    if (MasterHost) {
      HostCond.notify_all();
    }
  }
  std::lock_guard<std::mutex> lock(HostMutex);
  HostCond.notify_all();
}

bool TFailoverTestBalancer::CheckHost(const Socket::TAddress &addr) const {
  TFd new_server_socket(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  try {
    Connect(new_server_socket, addr);
    THandshake<THealthCheck> handshake(std::chrono::seconds(0));
    WriteExactly(new_server_socket, &handshake, sizeof(handshake));
    THealthCheck::TReply reply;
    ReadExactly(new_server_socket, &reply, sizeof(reply));
    return reply.GetResult() == THealthCheck::TReply::TResult::Ready;
  } catch (const std::exception &ex) {
    return false;
  }
  return false;
}

void TFailoverTestBalancer::OnError(const std::exception &/*ex*/) {
  ErrorSem.Push();
}