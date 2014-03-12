/* <stig/indy/fiber/fiber.cc>

   Implements <stig/indy/fiber/fiber.h>.

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

#include <stig/indy/fiber/fiber.h>

#include <xmmintrin.h>

using namespace Stig::Indy::Fiber;

__thread TRunner *TRunner::LocalRunner = nullptr;
__thread TFrame *TFrame::LocalFrame = nullptr;
__thread Base::TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>::TThreadLocalPool *TFrame::LocalFramePool = nullptr;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
void TRunner::Run() {
  assert(this);
  LocalRunner = this;
  TFrame *rt_queue = nullptr; /* we use this to push come back right away jobs to the front... */
  TFrame *next_frame = nullptr; /* we use this to loop through a queue... */
  try {
    const timespec lap_wait{0, 10000UL};
    const timespec long_lap_wait{0, 100000UL};
    const size_t laps_before_short_sleep = 100UL;
    const size_t laps_before_long_sleep = 100UL;
    size_t laps_without_work = 0UL;
    for (; likely(KeepRunning.load());) {
      assert(!ReadyToRunQueue);
      /* check for inbound frames */ {
        if (InboundFrameQueue) {
          assert(rt_queue == nullptr);
          TFrame *cur_tail = __sync_lock_test_and_set(&InboundFrameQueue, nullptr);
          for (TFrame *frame = cur_tail; frame; frame = next_frame) {
            assert(frame->InboundQueueNextFrame != frame);
            next_frame = frame->InboundQueueNextFrame;
            if (!frame->ComeBackRightAway) {
              frame->InboundQueueNextFrame = ReadyToRunQueue;
              ReadyToRunQueue = frame;
              //frame->QueueMembership.Insert(&MyFrameQueue, InvCon::Rev);
            } else {
              frame->InboundQueueNextFrame = rt_queue;
              rt_queue = frame;
            }
            //printf("TRunner [%p] push frame [%p]\n", this, frame);
          }
          /* here we have to choose between putting the most recent or least recent "high priority" (come_back_soon) fiber first. We currently
             implement the more recent one (as opposed to fair one) because it's most likely to still have data in the cache... */
          for (TFrame *frame = rt_queue; frame; frame = next_frame) {
            //frame->QueueMembership.Insert(&MyFrameQueue, InvCon::Rev);
            next_frame = frame->InboundQueueNextFrame;
            frame->InboundQueueNextFrame = ReadyToRunQueue;
            ReadyToRunQueue = frame;
          }
          rt_queue = nullptr;
        } else {
          for (size_t i = 0; i < TotalNumRunners; ++i) {
            TRunner *cur_runner = RunnerArray[i];
            if (cur_runner) {
              TFrame *&cur_inbound_queue = cur_runner->QueueArray[RunnerId].Ptr;
              if (cur_inbound_queue) {
                assert(rt_queue == nullptr);
                TFrame *cur_tail = __sync_lock_test_and_set(&cur_inbound_queue, nullptr);
                for (TFrame *frame = cur_tail; frame; frame = next_frame) {
                  assert(frame->InboundQueueNextFrame != frame);
                  next_frame = frame->InboundQueueNextFrame;
                  if (!frame->ComeBackRightAway) {
                    frame->InboundQueueNextFrame = ReadyToRunQueue;
                    ReadyToRunQueue = frame;
                    //frame->QueueMembership.Insert(&MyFrameQueue, InvCon::Rev);
                    //_mm_prefetch(frame->MyFiber.jmp, _MM_HINT_T1);
                  } else {
                    frame->InboundQueueNextFrame = rt_queue;
                    rt_queue = frame;
                  }
                  //printf("TRunner [%p] push frame [%p]\n", this, frame);
                }
                /* here we have to choose between putting the most recent or least recent "high priority" (come_back_soon) fiber first. We currently
                   implement the more recent one (as opposed to fair one) because it's most likely to still have data in the cache... */
                for (TFrame *frame = rt_queue; frame; frame = next_frame) {
                  next_frame = frame->InboundQueueNextFrame;
                  frame->InboundQueueNextFrame = ReadyToRunQueue;
                  ReadyToRunQueue = frame;
                  //frame->QueueMembership.Insert(&MyFrameQueue, InvCon::Rev);
                  #ifdef FAST_SWITCH
                  _mm_prefetch(frame->MyFiber.jmp, _MM_HINT_T1);
                  #endif
                }
                rt_queue = nullptr;
              }
            }
          }
        }
      }
      if (ReadyToRunQueue) {
        laps_without_work = 0UL;
      } else {
        ++laps_without_work;
        if (laps_without_work >= laps_before_long_sleep) {
          nanosleep(&lap_wait, NULL);
        } else if (laps_without_work >= laps_before_short_sleep) {
          nanosleep(&long_lap_wait, NULL);
        }
      }
      for (;;) {
        for (TFrame *frame = ReadyToRunQueue; ReadyToRunQueue; frame = ReadyToRunQueue) {
          ReadyToRunQueue = frame->InboundQueueNextFrame;
          _mm_prefetch(reinterpret_cast<uint8_t *>(ReadyToRunQueue) + offsetof(TFrame, MyFiber), _MM_HINT_T0);
          fiber_t *sched_fib = &frame->GetFiber();
          TFrame::LocalFrame = frame;
          FreeFrame = nullptr;
          FreeFramePool = nullptr;
          //printf("[%p]\tSwitch to Frame\n", this);
          switch_to_fiber(*sched_fib, MainFiber);
          //printf("[%p]\tDone Frame\n", this);
          if (FreeFrame) {
            assert(FreeFrame == frame);
            assert(FreeFramePool);
            FreeFramePool->Free(FreeFrame);
            FreeFrame = nullptr;
            FreeFramePool = nullptr;
          }
          if (ForeignRunnerToMoveFrameTo) {
            assert(FrameToMoveToForeignRunner);
            ScheduleFrameSlow(ForeignRunnerToMoveFrameTo, FrameToMoveToForeignRunner);
            ForeignRunnerToMoveFrameTo = nullptr;
            FrameToMoveToForeignRunner = nullptr;
          }
          TFrame::LocalFrame = nullptr;
        }
        if (NewReadyToRunQueue) {
          assert(!ReadyToRunQueue);
          std::swap(ReadyToRunQueue, NewReadyToRunQueue);
          assert(ReadyToRunQueue);
          assert(!NewReadyToRunQueue);
        } else {
          break;
        }
      }
    }
  } catch (const std::exception &ex) {
    syslog(LOG_INFO, "TRunner [%p] caught exception [%s]", this, ex.what());
  } catch (...) {
    LocalRunner = nullptr;
    throw;
  }
  LocalRunner = nullptr;
}

#pragma GCC diagnostic pop