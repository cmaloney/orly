/* <base/epoll.h>

   A wrapper around the operating system epoll.

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

#pragma once

#include <cassert>
#include <cstddef>

#include <sys/epoll.h>

#include <base/fd.h>
#include <base/no_copy_semantics.h>

namespace Base {

  /* A wrapper around the operating system epoll. */
  class TEpoll {
    NO_COPY_SEMANTICS(TEpoll);
    public:

    /* Starts with no fds and no cached events. */
    TEpoll();

    /* Frees internal storage. */
    ~TEpoll();

    /* Add the given fd to the epoll.  The epoll will trigger for the given flags, as well as for the mandatory flags. */
    void Add(int fd, int flags = EPOLLIN) {
      assert(this);
      Control(fd, flags, EPOLL_CTL_ADD);
      ++FdCount;
    }

    /* Returns the nth cached event. */
    void GetEvent(size_t n, int &fd, int &flags) const {
      assert(this);
      assert(n < EventCount);
      assert(&fd);
      assert(&flags);
      const epoll_event &event = Events[n];
      fd = event.data.fd;
      flags = event.events;
    }

    /* The number of events currently cached. */
    size_t GetEventCount() const {
      assert(this);
      return EventCount;
    }

    /* An fd on which one can wait for this epoll to be triggered. */
    const TFd &GetFd() const {
      assert(this);
      return Fd;
    }

    /* TODO */
    size_t GetFdCount() const {
      assert(this);
      return FdCount;
    }

    /* Equvalent to removing the fd and re-adding it with the given flags. */
    void Modify(int fd, int flags) {
      assert(this);
      Control(fd, flags, EPOLL_CTL_MOD);
    }

    /* Remove the fd from the epoll.  The epoll will no longer be triggered by it. */
    void Remove(int fd) {
      assert(this);
      Control(fd, 0, EPOLL_CTL_DEL);
      --FdCount;
    }

    /* Wait for the epoll to be triggered or for a timeout.  The timeout is in milliseconds.
       Return the number of events that occur, up to the maximum requested.  If we timed out, return zero.
       The events are cached and can be accessed by GetEvent().  Calling Wait() again will overwrite previously cached events. */
    size_t Wait(size_t max_event_count, int timeout = -1);

    /* Wait forever for a single event, then return that event's fd. */
    int WaitForOne() {
      assert(this);
      int fd, flags;
      Wait(1);
      GetEvent(0, fd, flags);
      return fd;
    }

    private:

    /* TODO */
    void Control(int fd, int flags, int op);

    /* TODO */
    TFd Fd;

    /* TODO */
    epoll_event *Events;

    /* TODO */
    size_t MaxEventCount, EventCount, FdCount;

  };  // TEpoll

}  // Base
