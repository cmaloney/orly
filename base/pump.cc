/* <base/pump.cc>

   Implements <base/pump.h>.

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

#include <base/pump.h>

#include <utility>

#include <base/error_utils.h>
#include <base/io_utils.h>

using namespace std;
using namespace Base;

TPump::TPump()
    : FirstPipe(nullptr), LastPipe(nullptr),
      FirstRecycledPage(nullptr), LastRecycledPage(nullptr),
      Epoll(epoll_create1(EPOLL_CLOEXEC)) {
  /* Add the 'deleting' semaphore's fd to the epoll.
     It'll be the only one with a null pointer associated with it. */
  JoinEpoll(Deleting.GetFd(), EPOLLIN, nullptr);
  /* Launch the background thread. */
  Background = thread(&TPump::BackgroundMain, this);
}

TPump::~TPump() {
  assert(this);
  /* Signal the background thread to exit, then wait for it to do so. */
  Deleting.Push();
  Background.join();
  /* Destroy all pipes. */
  while (FirstPipe) {
    delete FirstPipe;
  }
  /* Destroy all recycled pages. */
  while (FirstRecycledPage) {
    delete FirstRecycledPage;
  }
}

TPump::TPipe::TPage::TPage(TPump *pump)
    : Status(Unlinked), Void(nullptr), NextPage(nullptr), PrevPage(nullptr), Size(0) {
  Recycle(pump);
}

TPump::TPipe::TPage::~TPage() {
  assert(this);
  Unlink();
}

void TPump::TPipe::TPage::Commit(TPipe *pipe, size_t size) {
  assert(this);
  assert(pipe);
  Unlink();
  Status = Committed;
  Pipe = pipe;
  Size = size;
  FixupLinks();
}

void TPump::TPipe::TPage::Recycle(TPump *pump) {
  assert(this);
  assert(pump);
  Unlink();
  Status = Recycled;
  Pump = pump;
  Size = 0;
  FixupLinks();
}

void TPump::TPipe::TPage::FixupLinks() {
  assert(this);
  assert(Status != Unlinked);
  assert(!NextPage);
  PrevPage = GetLastConj();
  GetPrevConj() = this;
  GetNextConj() = this;
}

void TPump::TPipe::TPage::Unlink() {
  assert(this);
  if (Status != Unlinked) {
    GetPrevConj() = NextPage;
    GetNextConj() = PrevPage;
    Status = Unlinked;
    Void = nullptr;
    NextPage = nullptr;
    PrevPage = nullptr;
  }
}

TPump::TPipe::TPipe(TPump *pump, TFd &in, TFd &out)
    : Pump(nullptr), NextPipe(nullptr), PrevPipe(nullptr),
      FirstCommittedPage(nullptr), LastCommittedPage(nullptr),
      Start(nullptr), Limit(nullptr), Writing(false) {
  assert(pump);
  assert(&in);
  assert(&out);
  /* Construct the half pipes and set their background ends to non-blocking I/O. */
  TFd temp_in, temp_out;
  TFd::Pipe(temp_in, Out);
  TFd::Pipe(In, temp_out);
  SetNonBlocking(In);
  SetNonBlocking(Out);
  /* Add the background end of the inbound half to the epoll. */
  pump->JoinEpoll(In, EPOLLIN, this);
  /* Link into the pump's list of pipes. */ {
    lock_guard<mutex> lock(pump->Mutex);
    Pump = pump;
    PrevPipe = pump->LastPipe;
    GetPrevConj() = this;
    GetNextConj() = this;
  }
  /* Return the foreground ends. */
  in = move(temp_in);
  out = move(temp_out);
}

TPump::TPipe::~TPipe() {
  assert(this);
  /* Destroy all unused pages of data. */
  while (FirstCommittedPage) {
    FirstCommittedPage->Recycle(Pump);
  }
  /* Unlink from the pump. */ {
    lock_guard<mutex> lock(Pump->Mutex);
    GetPrevConj() = NextPipe;
    GetNextConj() = PrevPipe;
    Pump->PipeDied.notify_all();
  }
}

bool TPump::TPipe::Service() {
  assert(this);
  if (In.IsOpen()) {
    /* Get a page to read into and try to read into it from the inbound half. */
    TPage *page = Pump->GetPage();
    ssize_t size = read(In, page->GetBuffer(), BufferSize);
    if (size > 0) {
      /* We got data, so append the page to our list of committed pages
         and start trying to write it to the outbound half. */
      page->Commit(this, size);
      StartWriting();
    } else if (!size || errno != EAGAIN) {
      /* The foreground end of the inbound half has closed or has sent us an error.
         Either way, we're done trying to read from the inbound half. */
      StopReading();
    }
  }
  if (Writing && Start >= Limit) {
    /* We're out of data to write to the outbound half. */
    if (Start) {
      /* The page we were writing from is all used up.
         Send it back to the recycle list. */
      FirstCommittedPage->Recycle(Pump);
    }
    if (FirstCommittedPage) {
      /* There's more data to write, so point at it. */
      Start = FirstCommittedPage->GetBuffer();
      Limit = Start + FirstCommittedPage->GetSize();
    } else {
      /* There's no more data to write right now. */
      Start = nullptr;
      Limit = nullptr;
      StopWriting();
    }
  }
  if (Writing) {
    /* Try to write some data to the outbound half. */
    ssize_t size = write(Out, Start, Limit - Start);
    if (size > 0) {
      /* We wrote some data, so bump our data pointer. */
      Start += size;
    } else if (!size || errno != EAGAIN) {
      /* The foreground end of the outbound half has closed or has sent us an error.
         Either way, this pipe is doomed. */
      StopWriting();
      StopReading();
    }
  }
  /* We should stay alive if we're writing or might write;
     otherwise, its time for this pipe to go >poof<. */
  return Writing || In.IsOpen();
}

void TPump::TPipe::StartWriting() {
  assert(this);
  if (!Writing) {
    Pump->JoinEpoll(Out, EPOLLOUT, this);
    Writing = true;
  }
}

void TPump::TPipe::StopWriting() {
  assert(this);
  if (Writing) {
    Pump->PartEpoll(Out);
    Writing = false;
  }
}

void TPump::TPipe::StopReading() {
  assert(this);
  assert(In.IsOpen());
  Pump->PartEpoll(In);
  In.Reset();
}

void TPump::BackgroundMain() {
  assert(this);
  /* Loop forever, servicing pipes, until the foreground tells us to stop. */
  for (;;) {
    /* Wait for some events. */
    static const int max_event_count = 64;
    epoll_event_t events[max_event_count];
    int event_count;
    IfLt0(event_count = epoll_wait(Epoll, events, max_event_count, -1));
    /* Handle the events. */
    TPipe
        *dead_pipes[max_event_count],
        **dead_pipe_limit = dead_pipes;
    for (int i = 0; i < event_count; ++i) {
      TPipe *pipe = static_cast<TPipe *>(events[i].data.ptr);
      if (pipe) {
        /* A pipe needs service.  Check to see if an earlier service this round left the pipe dead. */
        bool is_alive = true;
        for (TPipe **dead_pipe = dead_pipes; dead_pipe < dead_pipe_limit; ++dead_pipe) {
          if (pipe == *dead_pipe) {
            is_alive = false;
            break;
          }
        }
        if (is_alive && !pipe->Service()) {
          /* The pipe wasn't dead but now it is.
             Destroy it and leave its address as a warning to others. */
          delete pipe;
          *dead_pipe_limit++ = pipe;
        }
      } else {
        /* The 'deleting' semaphore has woken us.  Gotta go. */
        return;
      }
    }  // for each event
  }  // forever
}

TPump::TPage *TPump::GetPage() {
  assert(this);
  return FirstRecycledPage ? FirstRecycledPage : new TPage(this);
}

void TPump::JoinEpoll(int fd, int events, TPipe *pipe) {
  assert(this);
  epoll_event_t epoll_event;
  epoll_event.events = events;
  epoll_event.data.ptr = pipe;
  IfLt0(epoll_ctl(Epoll, EPOLLIN, fd, &epoll_event));
}

void TPump::PartEpoll(int fd) {
  assert(this);
  IfLt0(epoll_ctl(Epoll, EPOLL_CTL_DEL, fd, nullptr));
}
