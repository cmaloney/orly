/* <stig/indy/fiber/fiber.h>

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

#include <atomic>
#include <cassert>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <unordered_set>


#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <ucontext.h>

#include <base/assert_true.h>
#include <base/likely.h>
#include <base/no_copy_semantics.h>
#include <base/spin_lock.h>
#include <base/thread_local_global_pool.h>
#include <base/zero.h>
#include <inv_con/unordered_list.h>

namespace Stig {

  namespace Indy {

    namespace Fiber {

      #define FAST_SWITCH

      #ifdef FAST_SWITCH
      /* TODO */
      struct fiber_t {
        ucontext_t fib;
        jmp_buf jmp;
      };

      /* TODO */
      struct fiber_ctx_t {
        void (*fnc)(void *);
        void *ctx;
        jmp_buf *cur;
        ucontext_t *prv;
      };

      /* TODO */
      static void fiber_start_fnc(void *p) {
        fiber_ctx_t *ctx = reinterpret_cast<fiber_ctx_t *>(p);
        void (*ufnc)(void *) = ctx->fnc;
        void *uctx = ctx->ctx;
        if (_setjmp(*ctx->cur) == 0) {
          ucontext_t tmp;
          swapcontext(&tmp, ctx->prv);
        }
        ufnc(uctx);
      }

      /* TODO */
      inline void create_fiber(fiber_t &fib, void(*ufnc)(void *), void *uctx, size_t stack_size) {
        getcontext(&fib.fib);
        fib.fib.uc_stack.ss_sp = malloc(stack_size);
        //printf("ss_sp=[%p], [%p]\n", fib.fib.uc_stack.ss_sp, reinterpret_cast<uint8_t *>(fib.fib.uc_stack.ss_sp) + stack_size);
        fib.fib.uc_stack.ss_size = stack_size;
        fib.fib.uc_link = 0;
        ucontext_t tmp;
        fiber_ctx_t ctx = {ufnc, uctx, &fib.jmp, &tmp};
        makecontext(&fib.fib, reinterpret_cast<void(*)()>(fiber_start_fnc), 1, &ctx);
        swapcontext(&tmp, &fib.fib);
      }

      /* TODO */
      inline size_t get_stack_size(fiber_t &fib) {
        return fib.fib.uc_stack.ss_size;
      }

      /* TODO */
      inline void free_fiber(fiber_t &fib) {
        assert(&fib);
        assert(fib.fib.uc_stack.ss_sp);
        free(fib.fib.uc_stack.ss_sp);
      }

      /* TODO */
      inline void switch_to_fiber(fiber_t &fib, fiber_t &prv) {
        if (_setjmp(prv.jmp) == 0) {
          _longjmp(fib.jmp, 1);
        }
      }

      #else

      /* TODO */
      typedef ucontext_t fiber_t;

      /* TODO */
      inline void create_fiber(fiber_t &fib, void(*ufnc)(void *), void *uctx, size_t stack_size) {
        getcontext(&fib);
        fib.uc_stack.ss_sp = malloc(stack_size);
        fib.uc_stack.ss_size = stack_size;
        fib.uc_link = 0;
        makecontext(&fib, reinterpret_cast<void (*)()>(ufnc), 1, uctx);
      }

      /* TODO */
      inline size_t get_stack_size(fiber_t &fib) {
        return fib.uc_stack.ss_size;
      }

      /* TODO */
      inline void free_fiber(fiber_t &fib) {
        assert(&fib);
        assert(fib.uc_stack.ss_sp);
        free(fib.uc_stack.ss_sp);
      }

      /* TODO */
      inline void switch_to_fiber(fiber_t &fib, fiber_t &prev) {
        swapcontext(&prev, &fib);
      }
      #endif

      /* Forward Declaration */
      class TFrame;

      /* TODO */
      class __attribute__((aligned(64))) TRunner {
        NO_COPY_SEMANTICS(TRunner);
        public:

        /* TODO */
        //typedef InvCon::UnorderedList::TCollection<TRunner, TFrame> TFrameQueue;

        /* TODO */
        class TRunnerCons {
          NO_COPY_SEMANTICS(TRunnerCons);
          public:

          /* TODO */
          TRunnerCons(size_t num_runners)
              : NumRunners(num_runners), NextId(0UL) {
            syslog(LOG_INFO, "TRunnerCons [%ld]", num_runners);
            RunnerArray = new TRunner *[num_runners];
            syslog(LOG_INFO, "TRunnerCons [%ld] A", num_runners);
            for (size_t i = 0; i < num_runners; ++i) {
              RunnerArray[i] = nullptr;
            }
            syslog(LOG_INFO, "TRunnerCons [%ld] B", num_runners);
          }

          /* TODO */
          ~TRunnerCons() {
            assert(this);
            delete[] RunnerArray;
          }

          private:

          /* TODO */
          size_t GetNewId() {
            syslog(LOG_INFO, "TRunnerCons [%ld] GetNewId()", NextId);
            assert(this);
            size_t ret = NextId;
            if (ret < NumRunners) {
              ++NextId;
              return ret;
            } else {
              throw std::logic_error("Adjust logic that computes num_runners for TRunnerCons so that we have enough communication channels.");
            }
          }

          /* TODO */
          const size_t NumRunners;

          /* TODO */
          size_t NextId;

          /* TODO */
          TRunner **RunnerArray;

          friend class TRunner;

        };  // TRunnerCons

        TRunner(TRunnerCons &runner_cons) : TRunner(runner_cons.NumRunners, runner_cons.GetNewId(), runner_cons.RunnerArray) {
          runner_cons.RunnerArray[RunnerId] = this;
        }

        /* TODO */
        TRunner(size_t total_num_runners, size_t runner_id, TRunner **runner_array)
            : FreeFrame(nullptr),
              FreeFramePool(nullptr),
              //MyFrameQueue(this),
              ReadyToRunQueue(nullptr),
              NewReadyToRunQueue(nullptr),
              KeepRunning(true),
              InboundFrameQueue(nullptr),
              ForeignRunnerToMoveFrameTo(nullptr),
              FrameToMoveToForeignRunner(nullptr),
              TotalNumRunners(total_num_runners),
              RunnerId(runner_id),
              RunnerArray(runner_array) {
          assert(runner_id < total_num_runners);
          #ifdef FAST_SWITCH
          Base::Zero(MainFiber.fib);
          #else
          Base::Zero(MainFiber);
          #endif
          QueueArray = new TOutboundQueue[total_num_runners];
          for (size_t i = 0; i < total_num_runners; ++i) {
            QueueArray[i].Ptr = nullptr;
          }
        }

        /* TODO */
        ~TRunner() {
          assert(this);
          RunnerArray[RunnerId] = nullptr;
          delete[] QueueArray;
        }

        /* TODO */
        void Run();

        /* TODO */
        void ShutDown() {
          assert(this);
          KeepRunning.store(false);
        }

        /* TODO */
        static inline void Yield(fiber_t &fiber) {
          assert(LocalRunner);
          switch_to_fiber(LocalRunner->MainFiber, fiber);
        }

        /* TODO */
        static inline void Schedule(TFrame *frame) {
          assert(LocalRunner);
          LocalRunner->ScheduleFrame(frame);
        }

        /* TODO */
        inline void ScheduleFrame(TFrame *frame);

        /* TODO */
        fiber_t MainFiber;

        /* TODO */
        static __thread TRunner *LocalRunner;

        /* TODO */
        TFrame *FreeFrame;
        Base::TThreadLocalGlobalPoolManager<Indy::Fiber::TFrame, size_t, Indy::Fiber::TRunner *>::TThreadLocalPool *FreeFramePool;

        private:

        /* Schedule this frame using the CAS queue, so that we can exit the runner loop for the local queue. An example of somewhere to use this is
           when you are waiting on an event in a spin loop, and the event can only be triggered by also being scheduled on this runner using the CAS
           queue. */
        inline void ScheduleFrameSlow(TFrame *frame);

        inline void ScheduleFrameSlow(TRunner *other_runner, TFrame *frame);

        /* TODO */
        //mutable TFrameQueue::TImpl MyFrameQueue;
        TFrame *ReadyToRunQueue;
        TFrame *NewReadyToRunQueue;

        /* TODO */
        std::atomic<bool> KeepRunning;

        /* TODO */
        TFrame *InboundFrameQueue;
        struct __attribute__((aligned(64))) TOutboundQueue {
          TFrame *Ptr;
        };
        TOutboundQueue *QueueArray;

        /* TODO */
        TRunner *ForeignRunnerToMoveFrameTo;
        TFrame *FrameToMoveToForeignRunner;

        /* TODO */
        size_t __attribute__((aligned(64))) TotalNumRunners;
        size_t __attribute__((aligned(64))) RunnerId;
        size_t blank_buf[7];

        TRunner **RunnerArray;

        /* Access to ComeBackSoon */
        friend class TFrame;
        friend class TFramePool;

      };  // TRunner

      /* TODO */
      class TRunnable {
        NO_COPY_SEMANTICS(TRunnable);
        public:

        /* TODO */
        typedef void (TRunnable::*TFunc)();

        protected:

        /* TODO */
        TRunnable() {}

      };  // TRunnable

      /* TODO */
      class __attribute__((aligned(64))) TRunnerPool {
        NO_COPY_SEMANTICS(TRunnerPool);
        public:

        /* TODO */
        TRunnerPool(TRunner::TRunnerCons &runner_cons,
                    size_t num_worker)
            : WorkerCount(num_worker), AssignPos(0UL) {
          for (size_t i = 0; i < num_worker; ++i) {
            RunnerVec.emplace_back(new TRunner(runner_cons));
            ThreadVec.emplace_back(new std::thread(std::bind([](TRunner *runner) {
              runner->Run();
            }, RunnerVec.back().get())));
          }
        }

        /* TODO */
        ~TRunnerPool() {
          for (auto &runner : RunnerVec) {
            runner->ShutDown();
          }
          for (auto &t : ThreadVec) {
            t->join();
          }
        }

        /* TODO */
        inline size_t GetWorkerCount() const {
          assert(this);
          return WorkerCount;
        }

        /* TODO */
        inline void Schedule(TFrame *frame, TRunnable *runnable, const TRunnable::TFunc &func);

        private:

        /* TODO */
        const size_t WorkerCount;

        /* TODO: use better data structure */
        std::vector<std::unique_ptr<TRunner>> RunnerVec;
        std::vector<std::unique_ptr<std::thread>> ThreadVec;

        /* TODO: we can do runner assignment more effectively than iterating through the vector */
        std::atomic<size_t> AssignPos;

      };  // TRunnerPool

      static void StartFrame(void *void_frame);

      /* TODO */
      class TFrame
          : public Base::TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>::TObjBase {
        NO_COPY_SEMANTICS(TFrame);
        public:

        /* TODO */
        TFrame(size_t stack_size, TRunner */*runner*/)
            :
              #ifndef NDEBUG
              DebugIsRunning(false),
              #endif
              RunnableFunc(nullptr),
              Runnable(nullptr),
              //QueueMembership(this),
              InboundQueueNextFrame(nullptr),
              ComeBackRightAway(false) {
          create_fiber(MyFiber, StartFrame, this, stack_size);
        }

        virtual ~TFrame() {
          /* this is where we make sure that our frame's stack unwound properly... */
          CheckFrameUnwound();
          assert(Runnable == nullptr);
          assert(RunnableFunc == nullptr);
          free_fiber(MyFiber);
        }

        /* TODO */
        inline void Latch(TRunner *runner, TRunnable *runnable, TRunnable::TFunc runnable_func) {
          //printf("TFrame [%p] Latch runnable [%p]\n", this, runnable);
          CheckFrameUnwound();
          assert(Runnable == nullptr);
          assert(RunnableFunc == nullptr);
          Runnable = runnable;
          RunnableFunc = runnable_func;
          runner->ScheduleFrame(this);
        }

        /* TODO */
        inline void Latch(TRunnable *runnable, TRunnable::TFunc runnable_func) {
          //printf("TFrame [%p] Latch runnable [%p]\n", this, runnable);
          CheckFrameUnwound();
          assert(Runnable == nullptr);
          assert(RunnableFunc == nullptr);
          Runnable = runnable;
          RunnableFunc = runnable_func;
          TRunner::Schedule(this);
        }

        /* TODO */
        inline void Yield() {
          assert(this);
          ComeBackRightAway = false;
          TRunner::Schedule(this);
          TRunner::Yield(MyFiber);
        }

        /* TODO */
        inline void YieldSlow() {
          assert(this);
          ComeBackRightAway = false;
          TRunner::LocalRunner->ScheduleFrameSlow(this);
          TRunner::Yield(MyFiber);
        }

        /* TODO */
        inline void Wait(bool come_back_right_away) {
          assert(this);
          ComeBackRightAway = come_back_right_away;
          TRunner::Yield(MyFiber);
        }

        /* TODO */
        inline void SwitchTo(TRunner *runner) {
          assert(this);
          ComeBackRightAway = false;
          runner->ScheduleFrame(this);
          TRunner::Yield(MyFiber);
        }

        /* TODO */
        void Run() {
          //printf("TFrame [%p] Run()\n", this);
          assert(this);
          /* wait to get scheduled (after a Latch). */
          for (;;) {
            assert(Runnable);
            assert(RunnableFunc);
            //printf("TFrame [%p] calling func\n", this);

            #ifndef NDEBUG
            DebugIsRunning = true;
            #endif
            TRunnable::TFunc runnable_func = RunnableFunc;
            TRunnable *const runnable = Runnable;
            Runnable = nullptr;
            RunnableFunc = nullptr;
            try {
              ((*runnable).*runnable_func)();
            } catch (const std::exception &ex) {
              syslog(LOG_EMERG, "FATAL ERROR: Fiber Runner caught exception. These must be handled within the fiber.");
              //abort();
            }
            #ifndef NDEBUG
            DebugIsRunning = false;
            #endif
            //printf("TFrame [%p] FINISH calling func\n", this);

            /* wait to get scheduled again. */
            switch_to_fiber(TRunner::LocalRunner->MainFiber, MyFiber);
          }
        }

        #ifndef NDEBUG
        void AssertCanFree() {
          if (DebugIsRunning) {
            throw std::logic_error("Cannot Free Frame that is still running");
          }
          assert(!DebugIsRunning);
        }
        #endif

        /* TODO */
        inline fiber_t &GetFiber() {
          assert(this);
          return MyFiber;
        }

        /* TODO */
        static __thread TFrame *LocalFrame;

        static __thread Base::TThreadLocalGlobalPoolManager<Fiber::TFrame, size_t, Fiber::TRunner *>::TThreadLocalPool *LocalFramePool;

        private:

        /* TODO */
        inline void CheckFrameUnwound() {
          /* this is where we make sure that our frame's stack unwound properly... */
          #ifndef NDEBUG
          if (Runnable != nullptr || RunnableFunc != nullptr) {
            throw std::logic_error("Stack frame was not unwound properly!");
            /* this exception is thrown when a function scheduled against this stack switched back to the main scheduler, and was never queued again.
               This indicated a logical error that can leak resources. We should always switch back to each frame so that is can clean up. */
          }
          #endif
        }

        #ifndef NDEBUG
        bool DebugIsRunning;
        #endif

        /* TODO */
        fiber_t MyFiber;

        /* TODO */
        TRunnable::TFunc RunnableFunc;

        /* TODO */
        TRunnable *Runnable;

        /* TODO */
        //TQueueMembership::TImpl QueueMembership;

        /* TODO */
        TFrame *InboundQueueNextFrame;

        /* TODO */
        bool ComeBackRightAway;

        /* MyFiber */
        friend class TFramePool;
        friend class TRunner;

      };  // TFrame

      /* TODO */
      static void StartFrame(void *void_frame) {
        TFrame *frame = reinterpret_cast<TFrame*>(void_frame);
        assert(frame);
        frame->Run();
        TRunner::Yield(frame->GetFiber());
      }

      /* This should only be used within the same scheduler. It is not safe to use between schedulers. */
      class TSync {
        NO_COPY_SEMANTICS(TSync);
        public:

        TSync(size_t waiting_for = 0UL) : Frame(nullptr), WaitingFor(waiting_for), Finished(0UL) {
          #ifndef NDEBUG
          RunnerSafety = TRunner::LocalRunner;
          #endif
        }

        /* TODO */
        inline void Sync(bool come_back_right_away = true);

        /* TODO */
        inline void Complete();

        /* TODO */
        inline void WaitForMore(size_t num);

        private:

        /* TODO */
        TFrame *Frame;

        /* TODO */
        size_t WaitingFor;
        size_t Finished;

        #ifndef NDEBUG
        TRunner *RunnerSafety;
        #endif

      };  // TSync

      /* This is a multi-scheduler safe version of TSync. There is more synchronization involved, so use this when you are actually parallelizing
         tasks as opposed to scheduling multiple fibers on the same scheduler. */
      class TSafeSync {
        NO_COPY_SEMANTICS(TSafeSync);
        public:

        TSafeSync(size_t waiting_for = 0UL)
            : WaitingFor(waiting_for),
              Finished(0UL),
              FrameWaiting(nullptr),
              RunnerToReactivateOn(nullptr) {}

        /* TODO */
        inline void Sync(bool come_back_right_away = true);

        /* TODO */
        inline void Complete();

        /* TODO */
        inline void WaitForMore(size_t num);

        private:

        /* TODO */
        std::atomic<size_t> WaitingFor;
        std::atomic<size_t> Finished;

        /* TODO */
        Fiber::TFrame *FrameWaiting;
        Fiber::TRunner *RunnerToReactivateOn;

        /* TODO */
        Base::TSpinLock SpinLock;

      };  // TSafeSync

      /* TODO */
      class TSingleSem {
        NO_COPY_SEMANTICS(TSingleSem);
        public:

        /* TODO */
        TSingleSem() : FlagOn(false), FrameWaiting(nullptr), RunnerToReactivateOn(nullptr) {}

        /* TODO */
        ~TSingleSem() {}

        /* TODO */
        inline void Push();

        /* TODO */
        inline void Pop();

        private:

        /* TODO */
        Base::TSpinLock SpinLock;
        bool FlagOn;

        Fiber::TFrame *FrameWaiting;
        Fiber::TRunner *RunnerToReactivateOn;

      };  // TSingleSem


      /* Use this locking mechanism when you want to lock and run the critical section on your current scheduler (core). This is usefull if you intend
         to use thread local or fiber local storage. It is also beneficial if you want to stay with the same cpu cache. The total throughput using
         this lock is less than a Queued lock as the lock bounces between schedulers. */
      class __attribute__((aligned(64))) TFiberLock {
        NO_COPY_SEMANTICS(TFiberLock);
        public:

        /* TODO */
        inline TFiberLock()
            : Taken(false), RootLock(nullptr) {}

        /* TODO */
        class __attribute__((aligned(64))) TLock {
          NO_COPY_SEMANTICS(TLock);
          public:

          /* TODO */
          inline TLock(TFiberLock &lock)
              : Lock(lock) {
            #ifndef NDEBUG
            DebugRunner = TRunner::LocalRunner;
            #endif
            assert(TRunner::LocalRunner);
            assert(TFrame::LocalFrame); /* we have to be fiber scheduled in order to use this locking mechanism. */
            /* Grab the spin lock to see if we can take control, or if we have to queue. */ {
              Base::TSpinLock::TLock my_lock(Lock.SpinLock);
              if (!lock.Taken) {
                lock.Taken = true;
                return;
              } else {
                NextLock = lock.RootLock;
                Runner = TRunner::LocalRunner;
                Frame = TFrame::LocalFrame;
                lock.RootLock = this;
                /* wait to get re-scheduled... once we're re-scheduled we have the lock. */
              }
            }  // release spin lock
            TFrame::LocalFrame->Wait(true);
            assert(lock.Taken);
          }

          /* TODO */
          inline ~TLock() {
            assert(this);
            /* Grab the spin lock to release control, and enqueue anyone who is waiting. */ {
              Base::TSpinLock::TLock lock(Lock.SpinLock);
              TLock *next_to_release;
              if (!(next_to_release = Lock.RootLock)) {
                Lock.Taken = false;
              } else {
                Lock.RootLock = next_to_release->NextLock;
                /* schedule next guy... */
                next_to_release->Runner->ScheduleFrame(next_to_release->Frame);
                //TRunner::LocalRunner->ScheduleFrame(next_to_release->Frame);
              }
            }
            assert(DebugRunner == TRunner::LocalRunner);
          }

          private:

          /* TODO */
          TFiberLock &Lock;

          /* TODO */
          TLock *NextLock;

          /* TODO */
          TRunner *Runner;
          TFrame *Frame;

          #ifndef NDEBUG
          TRunner *DebugRunner;
          #endif

        };  // TLock

        private:

        /* TODO */
        Base::TSpinLock SpinLock;

        /* TODO */
        bool Taken;

        /* TODO */
        TLock *RootLock;

      };  // TFiberLock

      /* Use this lock if you don't care about thread or fiber local storage. Your critical section will be run on the scheduler (core) that the lock
         is tied to. The benefit of this is that lots of frames can queue to use this lock, meaning the raw bandwidth of this lock is larger. The
         downside is that your critical section is run on a different scheduler (core) so you may have a cold cpu cache. This lock performs better
         when tied to the scheduler that uses it the most. (the fewest moves to a different scheduler). The major difference with this lock versus the
         fiber lock is that the critical sections move to a single lock scheduler, as opposed to the critical sections determining which scheduler
         should run them next. */
      class __attribute__((aligned(64))) TLockedQueue {
        NO_COPY_SEMANTICS(TLockedQueue);
        public:

        /* TODO */
        inline TLockedQueue(TRunner *runner)
            : Runner(runner) {}

        /* TODO */
        class __attribute__((aligned(64))) TLock {
          NO_COPY_SEMANTICS(TLock);
          public:

          /* TODO */
          inline TLock(TLockedQueue &lock)
              : Lock(lock) {
            assert(TRunner::LocalRunner);
            assert(TFrame::LocalFrame); /* we have to be fiber scheduled in order to use this locking mechanism. */
            Runner = TRunner::LocalRunner;
            if (TRunner::LocalRunner != lock.Runner) {
              TFrame::LocalFrame->SwitchTo(lock.Runner);
            }
          }

          /* TODO */
          inline ~TLock() {
            assert(this);
            if (Runner != Lock.Runner) {
              TFrame::LocalFrame->SwitchTo(Runner);
            }
          }

          private:

          /* TODO */
          TLockedQueue &Lock;

          /* TODO */
          TRunner *Runner;

        };  // TLock

        private:

        /* TODO */
        TRunner *Runner;

      };  // TLockedQueue

      /***************
        *** Inline ***
        *************/

      inline void TRunnerPool::Schedule(TFrame *frame, TRunnable *runnable, const TRunnable::TFunc &func) {
        size_t prev_assignment_count = std::atomic_fetch_add(&AssignPos, 1UL);
        TRunner *const chosen_runner = RunnerVec[prev_assignment_count % WorkerCount].get();
        frame->Latch(chosen_runner, runnable, func);
      }

      static inline void Yield() {
        assert(TFrame::LocalFrame);
        TFrame::LocalFrame->Yield();
      }

      static inline void YieldSlow() {
        assert(TFrame::LocalFrame);
        TFrame::LocalFrame->YieldSlow();
      }

      static inline void Wait(bool come_back_right_away = false) {
        assert(TFrame::LocalFrame);
        TFrame::LocalFrame->Wait(come_back_right_away);
      }

      static inline void SwitchTo(TRunner *runner) {
        assert(runner);
        assert(TFrame::LocalFrame);
        TFrame::LocalFrame->SwitchTo(runner);
      }

      static inline void FreeMyFrame(Base::TThreadLocalGlobalPoolManager<Indy::Fiber::TFrame, size_t, Indy::Fiber::TRunner *>::TThreadLocalPool *pool) {
        assert(TFrame::LocalFrame);
        assert(TRunner::LocalRunner);
        TRunner::LocalRunner->FreeFrame = TFrame::LocalFrame;
        TRunner::LocalRunner->FreeFramePool = pool;
      }

      /* TODO */
      class TSwitchToRunner {
        NO_COPY_SEMANTICS(TSwitchToRunner);
        public:

        /* TODO */
        TSwitchToRunner(TRunner *runner_to_switch_to)
            : ComeFromRunner(Base::AssertTrue(TRunner::LocalRunner)) {
          assert(runner_to_switch_to);
          SwitchTo(runner_to_switch_to);
        }

        /* TODO */
        ~TSwitchToRunner() {
          assert(this);
          assert(ComeFromRunner);
          SwitchTo(ComeFromRunner);
        }

        private:

        /* TODO */
        TRunner *ComeFromRunner;

      };  // TSwitchToRunner

      inline void TRunner::ScheduleFrameSlow(TFrame *frame) {
        assert(this);
        assert(frame);
        if (LocalRunner) {
          TFrame *&outbound_queue = LocalRunner->QueueArray[RunnerId].Ptr;
          do {
            frame->InboundQueueNextFrame = outbound_queue;
          } while (!__sync_bool_compare_and_swap(&outbound_queue, frame->InboundQueueNextFrame, frame));
        } else {
          do {
            frame->InboundQueueNextFrame = InboundFrameQueue;
          } while (!__sync_bool_compare_and_swap(&InboundFrameQueue, frame->InboundQueueNextFrame, frame));
        }
      }

      inline void TRunner::ScheduleFrameSlow(TRunner *other_runner, TFrame *frame) {
        assert(this);
        assert(LocalRunner);
        assert(other_runner);
        assert(frame);
        TFrame *&outbound_queue = QueueArray[other_runner->RunnerId].Ptr;
        do {
          frame->InboundQueueNextFrame = outbound_queue;
        } while (!__sync_bool_compare_and_swap(&outbound_queue, frame->InboundQueueNextFrame, frame));
      }

      inline void TRunner::ScheduleFrame(TFrame *frame) {
        assert(this);
        assert(frame);
        if (this == LocalRunner) {
          //printf("ScheduleFrame local\n");
          if (!frame->ComeBackRightAway) {
            frame->InboundQueueNextFrame = NewReadyToRunQueue;
            NewReadyToRunQueue = frame;
          } else {
            frame->InboundQueueNextFrame = ReadyToRunQueue;
            ReadyToRunQueue = frame;
          }
        } else if (LocalRunner && TFrame::LocalFrame == frame) {
          /* if we're rescheduling the local frame, then delay the schedule operation till after the frame returns to the scheduler so there's no race. */
          //printf("ScheduleFrame foreign from inside a scheduler [%p]\n", frame);
          assert(LocalRunner->ForeignRunnerToMoveFrameTo == nullptr);
          assert(LocalRunner->FrameToMoveToForeignRunner == nullptr);
          LocalRunner->ForeignRunnerToMoveFrameTo = this;
          LocalRunner->FrameToMoveToForeignRunner = frame;
        } else {
          /* if we're scheduling a different frame, then we can CAS right onto the scheduler of interest. */
          //printf("ScheduleFrame foreign from outside a scheduler [%p]\n", frame);
          ScheduleFrameSlow(frame);
        }
      }

      inline void TSync::Sync(bool come_back_right_away) {
        assert(RunnerSafety == TRunner::LocalRunner);
        //printf("TSync::Sync()\n");
        if (Finished < WaitingFor) {
          Frame = TFrame::LocalFrame;
          Wait(come_back_right_away);
          //printf("TSync::Sync completed()\n");
        }
        /* fall through */
        assert(RunnerSafety == TRunner::LocalRunner);
      }

      inline void TSync::Complete() {
        assert(RunnerSafety == TRunner::LocalRunner);
        ++Finished;
        //printf("TSync::Complete [%ld] of [%ld]\n", Finished, WaitingFor);
        if (Finished >= WaitingFor && Frame) {
          assert(Finished == WaitingFor);
          assert(Frame);
          TRunner::Schedule(Frame);
        }
        assert(RunnerSafety == TRunner::LocalRunner);
      }

      inline void TSync::WaitForMore(size_t num) {
        assert(RunnerSafety == TRunner::LocalRunner);
        WaitingFor += num;
      }

      /********************************************************/
      inline void TSafeSync::Sync(bool come_back_right_away) {
        assert(this);
        #ifndef NDEBUG
        TRunner *safety_runner = TRunner::LocalRunner;
        #endif
        assert(Fiber::TFrame::LocalFrame);
        assert(Fiber::TRunner::LocalRunner);
        bool should_wait = false;
        /* set frame waiting */ {
          Base::TSpinLock::TLock lock(SpinLock);
          if (Finished.load() != WaitingFor.load()) {
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
        assert(Finished.load() <= WaitingFor.load());
        //Base::TSpinLock::TLock lock(SpinLock);
        /* fall through */
        assert(safety_runner == TRunner::LocalRunner);
      }

      inline void TSafeSync::Complete() {
        assert(this);
        size_t prev = std::atomic_fetch_add(&Finished, 1UL);
        if ((prev + 1UL) == WaitingFor.load()) {
          Base::TSpinLock::TLock lock(SpinLock);
          if (FrameWaiting) {
            /* there's a frame waiting for us... let's activate him. */
            assert(RunnerToReactivateOn);
            Fiber::TFrame *frame = FrameWaiting;
            Fiber::TRunner *runner = RunnerToReactivateOn;
            FrameWaiting = nullptr;
            RunnerToReactivateOn = nullptr;
            runner->ScheduleFrame(frame);
          }
        }
      }

      inline void TSafeSync::WaitForMore(size_t num) {
        assert(this);
        WaitingFor += num;
      }

      inline void TSingleSem::Push() {
        Base::TSpinLock::TLock lock(SpinLock);
        if (FrameWaiting) {
          assert(!FlagOn);
          assert(RunnerToReactivateOn);
          Fiber::TFrame *frame = FrameWaiting;
          Fiber::TRunner *runner = RunnerToReactivateOn;
          FrameWaiting = nullptr;
          RunnerToReactivateOn = nullptr;
          runner->ScheduleFrame(frame);
        } else {
          FlagOn = true;
        }
      }

      inline void TSingleSem::Pop() {
        assert(TRunner::LocalRunner);
        bool do_wait = false;
        assert(!FrameWaiting);
        assert(!RunnerToReactivateOn);
        /* spinlock scope */ {
          Base::TSpinLock::TLock lock(SpinLock);
          if (FlagOn) {
            FlagOn = false;
          } else {
            do_wait = true;
            FrameWaiting = TFrame::LocalFrame;
            RunnerToReactivateOn = TRunner::LocalRunner;
          }
        } // end spinlock scope
        if (do_wait) {
          Fiber::Wait();
        }
      }

      /********************************************************/

      /* We use this as a lambda to pass to a thread that is going run a fiber scheduler with just 1 task assigned. This is temporary till we convert
         some background threads to properly fiber schedule... */
      static inline void LaunchSlowFiberSched(TRunner *runner, Base::TThreadLocalGlobalPoolManager<Indy::Fiber::TFrame, size_t, Indy::Fiber::TRunner *> *frame_pool_manager) {
        if (!Fiber::TFrame::LocalFramePool) {
          Fiber::TFrame::LocalFramePool = new Base::TThreadLocalGlobalPoolManager<Fiber::TFrame, size_t, Fiber::TRunner *>::TThreadLocalPool(frame_pool_manager);
        }
        runner->Run();
        if (Fiber::TFrame::LocalFramePool) {
          delete Fiber::TFrame::LocalFramePool;
          Fiber::TFrame::LocalFramePool = nullptr;
        }
      };

    }  // Fiber

  }  // Indy

}  // Stig
