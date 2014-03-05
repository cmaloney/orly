/* <starsha/pipe.cc>

   Implements <starsha/pipe.h>.

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

#include <starsha/pipe.h>

#include <unistd.h>

#include <base/os_error.h>
#include <base/zero.h>
#include <base/thrower.h>

using namespace std;
using namespace Base;
using namespace Starsha;

TPipe::TPoller::TPoller(int flags)
    : PipeCount(0) {
  TOsError::IfLt0(HERE, Fd = epoll_create1(flags));
}

TPipe::TPoller::~TPoller() {
  assert(this);
  close(Fd);
}

void TPipe::TPoller::Add(TPipe *pipe) {
  assert(this);
  assert(pipe);
  epoll_event event;
  Zero(event);
  event.events = EPOLLIN;
  event.data.ptr = pipe;
  TOsError::IfLt0(HERE, epoll_ctl(Fd, EPOLL_CTL_ADD, pipe->Fds[In], &event));
  ++PipeCount;
}

void TPipe::TPoller::Remove(TPipe *pipe) {
  assert(this);
  assert(pipe);
  epoll_event event;
  TOsError::IfLt0(HERE, epoll_ctl(Fd, EPOLL_CTL_DEL, pipe->Fds[In], &event));
  --PipeCount;
}

TPipe *TPipe::TPoller::Wait() {
  assert(this);
  TPipe *pipe;
  if (PipeCount) {
    /* Wait (forever) until any pipe becomes readable or has an error. */
    epoll_event event;
    for(;;) {
      int res = epoll_wait(Fd, &event, 1, -1);
      if (res == -1) {
        if(errno != EINTR) {
          TOsError(HERE);
        }
      } else {
        break;
      }
    }
    /* Get the pipe that woke us. */
    pipe = static_cast<TPipe *>(event.data.ptr);
    if (!(event.events & EPOLLIN)) {
      /* The pipe that isn't readable, so we asume it is reporting an error.
         This means it must leave the poller. */
      Remove(pipe);
    }
  } else {
    /* We're out of pipes, so leave empty-handed. */
    pipe = 0;
  }
  return pipe;
}

TPipe::TPipe()
    : IsOpen { true, true } {
  TOsError::IfLt0(HERE, pipe(Fds));
}

TPipe::~TPipe() {
  assert(this);
  for (size_t i = 0; i < 2; ++i) {
    if (IsOpen[i]) {
      close(Fds[i]);
    }
  }
}

void TPipe::Close(int idx) {
  assert(this);
  assert(IsOpen[idx]);
  close(Fds[idx]);
  IsOpen[idx] = false;
}

void TPipe::MoveFd(int idx, int as_fd) {
  assert(this);
  assert(idx == 0 || idx == 1);
  assert(IsOpen[idx]);
  TOsError::IfLt0(HERE, dup2(Fds[idx], as_fd));
  Close(idx);
}

size_t TPipe::Read(void *buf, size_t max_size) {
  assert(this);
  ssize_t size;
  TOsError::IfLt0(HERE, size = read(Fds[In], buf, max_size));
  return size;
}

size_t TPipe::Write(const void *buf, size_t max_size) {
  assert(this);
  ssize_t size;
  TOsError::IfLt0(HERE, size = write(Fds[Out], buf, max_size));
  return size;
}
