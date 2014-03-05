/* <base/epoll.cc>

   Implements <base/epoll.h>.

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

#include <base/epoll.h>

#include <cstdlib>
#include <new>

#include <base/error_utils.h>
#include <base/shutting_down.h>
#include <base/zero.h>

using namespace std;
using namespace Base;

TEpoll::TEpoll()
    : Fd(epoll_create1(0)), Events(nullptr), MaxEventCount(0), EventCount(0), FdCount(0) {}

TEpoll::~TEpoll() {
  assert(this);
  free(Events);
}

size_t TEpoll::Wait(size_t max_event_count, int timeout) {
  assert(this);
  assert(max_event_count);
  if (max_event_count > MaxEventCount) {
    Events = static_cast<epoll_event *>(realloc(Events, sizeof(epoll_event) * max_event_count));
    if (!Events) {
      throw bad_alloc();
    }
    MaxEventCount = max_event_count;
  }
  int result;
  for (;;) {
    result = epoll_wait(Fd, Events, max_event_count, timeout);
    if (result >= 0) {
      break;
    }
    if (errno != EINTR || ShuttingDown) {
      ThrowSystemError(errno);
    }
  }
  return EventCount = result;
}

void TEpoll::Control(int fd, int flags, int op) {
  assert(this);
  epoll_event event;
  Zero(event);
  event.events = flags;
  event.data.fd = fd;
  IfLt0(epoll_ctl(Fd, op, fd, &event));
}
