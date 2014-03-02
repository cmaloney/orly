/* <stig/rpc/safe_transceiver.cc> 

   Implements <stig/rpc/safe_transceiver.h>.

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

#include <stig/rpc/safe_transceiver.h>

#include <unistd.h>
#include <sys/timerfd.h>

#include <base/error_utils.h>
#include <base/zero.h>

using namespace std;
using namespace Base;
using namespace Stig::Rpc;

TSafeTransceiver::TInterrupted::TInterrupted()
    : runtime_error("Stig RPC I/O interrupted") {}

TSafeTransceiver::TTimedOut::TTimedOut()
    : runtime_error("Stig RPC I/O timed out") {}

TSafeTransceiver::TSafeTransceiver(int interrupt_fd)
    : ArmingCount(0), DeadlineInterval(chrono::seconds(2)), ResetIsRequired(false) {
  /* Event 0 is the interrupt event.
     We fill in its fd and the flags indicating we will wait for it to be readable.
     We also indicate it hasn't occurred yet. */
  Poller[0].fd      = interrupt_fd;
  Poller[0].events  = POLLIN;
  Poller[0].revents = 0;
  /* Event 2 is the deadline timer.
     We haven't created it yet, but when we do, we will wait for it to be readable.
     We also indicate it hasn't occurred yet. */
  Poller[2].fd      = -1;
  Poller[2].events  = POLLIN;
  Poller[2].revents = 0;
}

TSafeTransceiver::~TSafeTransceiver() {
  assert(this);
  /* If the deadline timer is open, close it. */
  if (Poller[2].fd >= 0) {
    close(Poller[2].fd);
  }
}

void TSafeTransceiver::ArmDeadline() {
  assert(this);
  /* If we haven't yet created a deadline timer, do so now. */
  if (Poller[2].fd < 0) {
    IfLt0(Poller[2].fd = timerfd_create(CLOCK_MONOTONIC, 0));
  }
  /* If we're the first call to arm, make sure we set the deadline interval later. */
  if (!ArmingCount) {
    ResetIsRequired = true;
  }
  ++ArmingCount;
  assert(ArmingCount);
}

void TSafeTransceiver::DisarmDeadline() {
  assert(this);
  assert(ArmingCount);
  /* Tick the arming count down.  If this actually disarms the deadline, clear any
     pending deadline alarm. */
  --ArmingCount;
  if (!ArmingCount) {
    Poller[2].revents = 0;
  }
}

void TSafeTransceiver::Recv(int sock_fd, int flags) {
  assert(this);
  for (size_t part = 0; Transceiver; Transceiver += part) {
    WaitForSocket(sock_fd, POLLIN);
    part = Transceiver.Recv(sock_fd, flags);
  }
}

void TSafeTransceiver::Send(int sock_fd, int flags) {
  assert(this);
  for (size_t part = 0; Transceiver; Transceiver += part) {
    WaitForSocket(sock_fd, POLLOUT);
    part = Transceiver.Send(sock_fd, flags);
  }
}

void TSafeTransceiver::WaitForSocket(int sock_fd, int flags) {
  assert(this);
  /* Set up event 1 in the poller to wait for the socket. */
  Poller[1].fd      = sock_fd;
  Poller[1].events  = flags;
  Poller[1].revents = 0;
  /* If we need to reset the deadline alarm, do so now. */
  if (ResetIsRequired) {
    /* Set the deadline timer to do a one-shot count-down. */
    auto ms = DeadlineInterval.count();
    itimerspec spec;
    Zero(spec);
    spec.it_value.tv_sec  = ms / 1000;
    spec.it_value.tv_nsec = (ms % 1000) * 1000000;
    IfLt0(timerfd_settime(Poller[2].fd, 0, &spec, nullptr));
    /* Clear the poller of any previous deadline alarm and clear the request to reset. */
    Poller[2].revents = 0;
    ResetIsRequired = false;
  }
  /* Loop until something happens. */
  do {
    /* If the interrupt event has occurred, throw. */
    if (Poller[0].revents) {
      throw TInterrupted();
    }
    /* If the deadline has passed, throw. */
    if (Poller[2].revents) {
      throw TTimedOut();
    }
    /* Wait indefinitely for something to happen. */
    IfLt0(poll(Poller, (ArmingCount > 0) ? 3 : 2, -1));
  } while (!Poller[1].revents);
}

