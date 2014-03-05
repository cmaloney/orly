/* <stig/indy/disk/result.h>

   The result of a disk controller action.

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

#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>

#include <base/spin_lock.h>
#include <base/event_counter.h>

#include <stig/indy/fiber/fiber.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      /* An error thrown when an I/O request fails due to a runtime / logic error. */
      class TDiskError
          : public std::runtime_error {
        public:

        /* Do-little. */
        TDiskError(const char *err_str)
            : runtime_error(err_str) {}

      };  // TDiskError

      /* An error thrown when an I/O request fails due to disk failure. */
      class TDiskFailure
          : public TDiskError {
        public:

        /* Do-little. */
        TDiskFailure()
            : TDiskError("Disk Failure") {}

      };  // TDiskFailure

      /* An error thrown when an I/O request fails due to server shutdown. */
      class TDiskServiceShutdown
          : public std::runtime_error {
        public:

        /* Do-little. */
        TDiskServiceShutdown()
            : runtime_error("Disk Error: Service Shutdown") {}

      };  // TDiskServiceShutdown

      /* The status of a disk controller action. Should be used to continue or halt
         progress of threads blocked on I/O. */
      enum TDiskResult {

        /* The I/O was successful. */
        Success,

        /* An error Occured */
        Error,

        /* This I/O failed. This could mean it was mal-formed or the disk device is
           dead / gone. We should unwind as our action was not successful. */
        DiskFailure,

        /* The server is shutting down. The I/O was interrupted by the thread manager.
           We should unwind as our action may not have been successful. */
        ServerShutdown

      };  // TResult

      /* TODO */
      template <typename TVal>
      static void WaitForDiskSpin(TVal &trigger, TVal wait_for_this, std::condition_variable &cond, std::unique_lock<std::mutex> &lock, TDiskResult &result, const char *err_str) {
        while (trigger != wait_for_this) {
          switch (result) {
            case Success : {
              cond.wait(lock);
              break;
            }
            case Error : {
              throw TDiskError(err_str);
              break;
            }
            case DiskFailure : {
              throw TDiskFailure();
              break;
            }
            case ServerShutdown : {
              throw TDiskServiceShutdown();
              break;
            }
          }
        }
      }

      /* TODO */
      class TCompletionTrigger {
        NO_COPY_SEMANTICS(TCompletionTrigger);
        public:

        /* TODO */
        inline TCompletionTrigger();

        /* TODO */
        inline ~TCompletionTrigger();

        /* TODO */
        inline void WaitForMore(size_t num);

        /* TODO */
        inline void WaitForOneMore();

        /* TODO */
        inline void Callback(TDiskResult disk_result, const char *err_str);

        /* TODO */
        inline void Wait(bool come_back_right_away = false);

        private:

        /* TODO */
        std::atomic<size_t> WaitFor;

        /* TODO */
        std::atomic<size_t> NumFinished;

        /* TODO */
        Fiber::TFrame *FrameWaiting;
        Fiber::TRunner *RunnerToReactivateOn;

        /* TODO */
        Base::TSpinLock SpinLock;

        /* TODO */
        TDiskResult Result;

        /* TODO */
        const char *ErrStr;

      };  // TCompletionTrigger

      /***************
        *** inline ***
        *************/

      inline TCompletionTrigger::TCompletionTrigger()
          : WaitFor(0UL),
          NumFinished(0UL),
          FrameWaiting(nullptr),
          RunnerToReactivateOn(nullptr),
          Result(Success),
          ErrStr(nullptr) {}

      inline TCompletionTrigger::~TCompletionTrigger() {}

      inline void TCompletionTrigger::WaitForMore(size_t num) {
        assert(this);
        WaitFor += num;
      }

      inline void TCompletionTrigger::WaitForOneMore() {
        assert(this);
        ++WaitFor;
      }

      inline void TCompletionTrigger::Callback(TDiskResult disk_result, const char *err_str) {
        assert(this);
        const size_t prev = std::atomic_fetch_add(&NumFinished, 1UL);
        switch (Result) {
          case Success : {
            switch (disk_result) {
              case Success : {
                if ((prev + 1UL) == WaitFor.load()) {
                  Base::TSpinLock::TLock lock(SpinLock);
                  Result = disk_result;
                  if (FrameWaiting) {
                    /* there's a frame waiting for us... let's activate him. */
                    assert(RunnerToReactivateOn);
                    Fiber::TFrame *frame = FrameWaiting;
                    Fiber::TRunner *runner = RunnerToReactivateOn;
                    FrameWaiting = nullptr;
                    RunnerToReactivateOn = nullptr;
                    runner->ScheduleFrame(frame);
                  }
                  //EventFd.Push();
                }
                break;
              }
              case Error : {
                Base::TSpinLock::TLock lock(SpinLock);
                Result = disk_result;
                ErrStr = err_str;
                if (FrameWaiting) {
                  /* there's a frame waiting for us... let's activate him. */
                  assert(RunnerToReactivateOn);
                  Fiber::TFrame *frame = FrameWaiting;
                  Fiber::TRunner *runner = RunnerToReactivateOn;
                  FrameWaiting = nullptr;
                  RunnerToReactivateOn = nullptr;
                  runner->ScheduleFrame(frame);
                }
                //EventFd.Push();
                break;
              }
              case DiskFailure : {
                Base::TSpinLock::TLock lock(SpinLock);
                Result = disk_result;
                if (FrameWaiting) {
                  /* there's a frame waiting for us... let's activate him. */
                  assert(RunnerToReactivateOn);
                  Fiber::TFrame *frame = FrameWaiting;
                  Fiber::TRunner *runner = RunnerToReactivateOn;
                  FrameWaiting = nullptr;
                  RunnerToReactivateOn = nullptr;
                  runner->ScheduleFrame(frame);
                }
                //EventFd.Push();
                break;
              }
              case ServerShutdown : {
                Base::TSpinLock::TLock lock(SpinLock);
                Result = disk_result;
                if (FrameWaiting) {
                  /* there's a frame waiting for us... let's activate him. */
                  assert(RunnerToReactivateOn);
                  Fiber::TFrame *frame = FrameWaiting;
                  Fiber::TRunner *runner = RunnerToReactivateOn;
                  FrameWaiting = nullptr;
                  RunnerToReactivateOn = nullptr;
                  runner->ScheduleFrame(frame);
                }
                //EventFd.Push();
                break;
              }
            }
            break;
          }
          default : {
            /* if we're already in an error state, stay in it. */
            break;
          }
        }
      }

      inline void TCompletionTrigger::Wait(bool come_back_right_away) {
        assert(this);
        assert(Fiber::TFrame::LocalFrame);
        assert(Fiber::TRunner::LocalRunner);
        bool should_wait = false;
        /* set frame waiting */ {
          Base::TSpinLock::TLock lock(SpinLock);
          if (NumFinished.load() != WaitFor.load()) {
            /* we are going to wait... */
            should_wait = true;
            FrameWaiting = Fiber::TFrame::LocalFrame;
            RunnerToReactivateOn = Fiber::TRunner::LocalRunner;
          }
        }
        if (should_wait) {
          Fiber::Wait(come_back_right_away);
        }
        /* fall through means we are ready... */
        assert(NumFinished.load() <= WaitFor.load());
        Base::TSpinLock::TLock lock(SpinLock);
        switch (Result) {
          case Success : {
            break;
          }
          case Error : {
            throw TDiskError(ErrStr);
            break;
          }
          case DiskFailure : {
            throw TDiskFailure();
            break;
          }
          case ServerShutdown : {
            throw TDiskServiceShutdown();
            break;
          }
        }
      }

    }  // Disk

  }  // Indy

}  // Stig

