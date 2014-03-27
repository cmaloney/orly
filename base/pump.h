/* <base/pump.h>

   A pump for pipes.

   Problems can sometimes arise with pipes when the reader is not responding quickly
   enough.  The writer becomes blocked when the pipe's internal buffer fills and this
   can lead to deadlocks when the reader and writer are synchronizing with each other.

   This pump seeks to alleviate the problem by reading avidly from the pipe and storing
   the results in internal buffers until the actual reader is ready to read them.  The
   pump runs a single background thread which can handle the traffic of many separate
   pipes.

   If writers outstrip readers, then the pump's memory usage will grow without limit.
   However, the pump has a recycling system for its buffers, so, as long as traffic
   remains more or less even, dynamic memory allocations won't happen.

   When the time comes to shut down the pump, you have two choices: shut it down nicely,
   waiting for any pending data to drain through, or shut it down hard, throwing away
   any data which hasn't yet been read.

   For a nice shutdown, use WaitForIdle().  This will block until the pump has delivered
   all pending data.  If other threads continue to create pipes during this time, the
   wait may never be satisfied, so be sure you take steps to prevent this from happening.

   If would like a nice shutdown, but don't have the patience to wait forever, you can
   call WaitForIdleFor(), which takes a timeout, or WaitForIdleUntil(), which takes a
   deadline.  Each of these functions returns true iff. the pump became idle before the
   time limit was reached.

   For a hard shutdown, just destroy the pump.  The destructor will not return until the
   background thread has halted and all unread data and recycled buffers have been thrown
   away.

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
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include <sys/epoll.h>

#include <base/event_semaphore.h>
#include <base/fd.h>
#include <base/no_copy_semantics.h>
#include <base/no_default_case.h>

namespace Base {

  /* A pump for pipes. */
  class TPump final {
    NO_COPY_SEMANTICS(TPump);
    public:

    /* Construct with no pipes. */
    TPump();

    /* Destroy all pipes and lose all unread data. */
    ~TPump();

    /* Create a new pipe and return both ends of it. */
    void NewPipe(TFd &in, TFd &out) {
      assert(this);
      new TPipe(this, in, out);
    }

    /* Wait for the pump to become idle. */
    void WaitForIdle() const {
      assert(this);
      std::unique_lock<std::mutex> lock(Mutex);
      while (FirstPipe) {
        PipeDied.wait(lock);
      }
    }

    /* Wait for the pump to become idle, then return true.
       If the timeout is reached first, return false. */
    template <typename TRep, typename TPeriod>
    bool WaitForIdleFor(const std::chrono::duration<TRep, TPeriod> &timeout) const {
      assert(this);
      std::unique_lock<std::mutex> lock(Mutex);
      while (FirstPipe) {
        if (PipeDied.wait_for(lock, timeout) == std::cv_status::timeout) {
          return false;
        }
      }
      return true;
    }

    /* Wait for the pump to become idle, then return true.
       If the deadline is reached first, return false. */
    template <typename TClock, typename TDuration>
    bool WaitForIdleUntil(const std::chrono::time_point<TClock, TDuration> &deadline) const {
      assert(this);
      std::unique_lock<std::mutex> lock(Mutex);
      while (FirstPipe) {
        if (PipeDied.wait_until(lock, deadline) == std::cv_status::timeout) {
          return false;
        }
      }
      return true;
    }

    private:

    /* Just making the system type look more like a type. */
    using epoll_event_t = epoll_event;

    /* The maximum number of bytes we will pump through a pipe in a single step. */
    static const size_t BufferSize = 65536;

    /* The maximum number of epoll events we will handle in a single step.
       This is NOT the maximum number of events an epoll can handle. */
    static const size_t MaxEventCount = 64;

    /* A pipe, which is really a pair of OS pipes, one inbound and one outbound. */
    class TPipe final {
      NO_COPY_SEMANTICS(TPipe);
      public:

      /* A page of data which is part of a linked list of pages.
         If the page contains data ready to be used, then it is in a pipe's list of committed pages;
         otherwise, it is in the pump's list of recycled pages, waiting to be used again. */
      class TPage final {
        NO_COPY_SEMANTICS(TPage);
        public:

        /* Construct as recycled. */
        TPage(TPump *pump);

        /* Unlink our the list, if any, and go. */
        ~TPage();

        /* Data storage space.  There are at least BufferSize bytes available. */
        char *GetBuffer() const {
          assert(this);
          return Buffer;
        }

        /* The number of bytes in the buffer which contain good data. */
        size_t GetSize() const {
          assert(this);
          return Size;
        }

        /* Append ourself to the given pipe's list of committed pages.
           Set size of available data to the given value. */
        void Commit(TPipe *pipe, size_t size);

        /* Append ourself to the pump's list of recycled pages.
           Set size of available data to zero. */
        void Recycle(TPump *pump);

        private:

        /* The page's status indicates which list of pages it is currently linked to.
           This also determines which field in the union (below) is currently valid. */
        enum TStatus {

          /* The page isn't linked to any list.
             The Void field in the union is null. */
          Unlinked,

          /* The page is linked to the pump's list of recycled pages.
             The Pump field in the union points to the pump. */
          Recycled,

          /* The page is linked to a pipe's list of committed pages.
             The Pipe field in the union points to the pipe. */
          Committed

        };  // TPump::TPipe::TPage::TStatus

        /* A reference to the pointer to the first page in our list.
           It is not legal to call this function when we are unlinked. */
        TPage *&GetFirstConj() const {
          assert(this);
          switch (Status) {
            case Recycled: {
              return Pump->FirstRecycledPage;
            }
            case Committed: {
              return Pipe->FirstCommittedPage;
            }
            NO_DEFAULT_CASE;
          }
        }

        /* A reference to the pointer to the last page in our list.
           It is not legal to call this function when we are unlinked. */
        TPage *&GetLastConj() const {
          assert(this);
          switch (Status) {
            case Recycled: {
              return Pump->LastRecycledPage;
            }
            case Committed: {
              return Pipe->LastCommittedPage;
            }
            NO_DEFAULT_CASE;
          }
        }

        /* A reference to the pointer which points to us from the next page.
           It is not legal to call this function when we are unlinked. */
        TPage *&GetNextConj() const {
          assert(this);
          return NextPage ? NextPage->PrevPage : GetLastConj();
        }

        /* A reference to the pointer which points to us from the previous page.
           It is not legal to call this function when we are unlinked. */
        TPage *&GetPrevConj() const {
          assert(this);
          return PrevPage ? PrevPage->NextPage : GetFirstConj();
        }

        /* Link to the end of the given pipe's list of committed pages.
           If the given pointer is null, instead link to the end of the pump's list of recycled pages. */
        void FixupLinks();

        /* Unlink from whatever list we're currently in. */
        void Unlink();

        /* See TStatus. */
        TStatus Status;

        /* The object whose list we're currently linked to.  See TStatus. */
        union {
          void *Void;
          TPump *Pump;
          TPipe *Pipe;
        };

        /* Our sibling pages in whatever list we're in. */
        TPage *NextPage, *PrevPage;

        /* See accessor. */
        size_t Size;

        /* See accessor. */
        mutable char Buffer[BufferSize];

      };  // TPump::TPipe::TPage

      /* Construct a new pipe in the given pump and return the
         foreground ends of the inbound and outbound half-pipes. */
      TPipe(TPump *pump, TFd &in, TFd &out);

      /* Recycle all remaining committed pages and unlink from the pump. */
      ~TPipe();

      /* Try to read and/or write from/to the half-pipes.
         Return true if we will need further service or false if we're ready to die. */
      bool Service();

      private:

      /* A reference to the pointer which points to us from the next pipe. */
      TPipe *&GetNextConj() const {
        assert(this);
        return NextPipe ? NextPipe->PrevPipe : Pump->LastPipe;
      }

      /* A reference to the pointer which points to us from the previous pipe. */
      TPipe *&GetPrevConj() const {
        assert(this);
        return PrevPipe ? PrevPipe->NextPipe : Pump->FirstPipe;
      }

      /* Join the epoll as a writer. */
      void StartWriting();

      /* Part from the epoll as a writer. */
      void StopWriting();

      /* Part from the epoll as a reader. */
      void StopReading();

      /* The pump to which we belong.  Never null. */
      TPump *Pump;

      /* Out siblings in the pump's list of pipes. */
      TPipe *NextPipe, *PrevPipe;

      /* The first and last pages of committed data,
         waiting to be written to the outbound half-pipe. */
      TPage *FirstCommittedPage, *LastCommittedPage;

      /* Our current position and limit in the first committed page. */
      const char *Start, *Limit;

      /* True iff. we're joined to the epoll as a writer. */
      bool Writing;

      /* The background ends of the in- and outbound half-pipes. */
      TFd In, Out;

    };  // TPump::TPipe

    /* Convenience. */
    using TPage = TPipe::TPage;

    /* The entry point of the background thread. */
    void BackgroundMain();

    /* Returns a page from the recycled list.  If the list
       is empty it makes a new one, so this is never null. */
    TPage *GetPage();

    /* Add the given fd to the epoll with the given events and
       associate the given pipe with it. */
    void JoinEpoll(int fd, int events, TPipe *pipe);

    /* Remove the given fd from the epoll. */
    void PartEpoll(int fd);

    /* Covers FirstPipe & LastPipe. */
    mutable std::mutex Mutex;

    /* Signals when a pipe dies.  This wakes up the WaitForIdle() functions. */
    mutable std::condition_variable PipeDied;

    /* Pushed in the destructor.  It causes the background thread to exit. */
    TEventSemaphore Deleting;

    /* The first and last pipes in the pump. */
    TPipe *FirstPipe, *LastPipe;

    /* The first and last pages in the recycled list. */
    TPage *FirstRecycledPage, *LastRecycledPage;

    /* The epoll against which the background thread blocks. */
    TFd Epoll;

    /* The background thread. */
    std::thread Background;

  };  // TPump

}  // Base
