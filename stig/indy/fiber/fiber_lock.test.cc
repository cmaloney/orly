/* <stig/indy/fiber/fiber_lock.test.cc>

   Unit test for <stig/indy/fiber/fiber.h>.

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

#include <condition_variable>
#include <thread>
#include <sys/ioctl.h>
#include <sys/syscall.h>

#include <unistd.h>

#include <base/error_utils.h>
#include <base/timer.h>

#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig::Indy::Fiber;

std::mutex Mutex;
Base::TSpinLock SpinLock;

class TMyRunnable
    : public TRunnable {
  NO_COPY_SEMANTICS(TMyRunnable);
  public:

  TMyRunnable(TRunner *runner, /*TFiberLock &fiber_lock*/ TLockedQueue &locked_queue, size_t num_iter, std::mutex &mut, std::condition_variable &cond, size_t &num_finished)
      : FramePool(TFrame::LocalFramePool), /*FiberLock(fiber_lock), */LockedQueue(locked_queue), NumIter(num_iter), Mut(mut), Cond(cond), NumFinished(num_finished) {
    Frame = FramePool->Alloc();
    try {
      Frame->Latch(runner, this, static_cast<TRunnable::TFunc>(&TMyRunnable::Compute));
    } catch (...) {
      FramePool->Free(Frame);
      throw;
    }
  }

  ~TMyRunnable() {
    assert(this);
    FreeMyFrame(FramePool);
  }

  void Compute() {
    size_t sum = 0UL;
    for (size_t i = 0; i < NumIter; ++i) {
      //Base::TSpinLock::TLock lock(SpinLock);
      //TFiberLock::TLock lock(FiberLock);
      TLockedQueue::TLock lock(LockedQueue);
      //std::lock_guard<std::mutex> lock(Mutex);
      sum += i;
    }
    //printf("Sum = [%ld]\n", sum);
    std::lock_guard<std::mutex> lock(Mut);
    ++NumFinished;
    Cond.notify_one();
    delete this;
  }

  private:

  Base::TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>::TThreadLocalPool *FramePool;

  TFrame *Frame;

  //TFiberLock &FiberLock;
  TLockedQueue &LockedQueue;

  size_t NumIter;
  std::mutex &Mut;
  std::condition_variable &Cond;
  size_t &NumFinished;

};

FIXTURE(Typical) {
  printf("Size of TFrame [%ld]\n", sizeof(TFrame));
  printf("Size of TRuner [%ld]\n", sizeof(TRunner));
  //const size_t num_iter = 10000UL;
  //const size_t num_iter = 100000UL;
  const size_t num_iter = 1000000UL;
  const size_t num_runnable_per_thread = 256UL;
  size_t num_threads = 1UL;
  std::vector<TRunner *> runner_vec;
  TRunner *runner_array[num_threads];
  for (size_t i = 0; i < num_threads; ++i) {
    runner_array[i] = nullptr;
  }
  for (size_t i = 0; i < num_threads; ++i) {
    TRunner *runner = new TRunner(num_threads, i, runner_array);
    runner_array[i] = runner;
    runner_vec.emplace_back(runner);
  }
  Base::TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *> frame_pool_manager(num_threads * num_runnable_per_thread, 64 * 1024, runner_vec[0]);
  //TFiberLock lock;
  TLockedQueue lock(runner_vec[0]);
  if (!TFrame::LocalFramePool) {
    TFrame::LocalFramePool = new TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>::TThreadLocalPool(&frame_pool_manager);
  }
  std::mutex mut;
  std::condition_variable cond;
  bool can_start = false;
  size_t num_finished = 0UL;
  auto launch_fiber_sched = [&](TRunner *runner, size_t i) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(i, &mask);
    Base::IfLt0(sched_setaffinity(syscall(SYS_gettid), sizeof(cpu_set_t), &mask));
    assert(runner);
    /* wait for start */ {
      std::unique_lock<std::mutex> lock(mut);
      while (!can_start) {
        cond.wait(lock);
      }
    }
    runner->Run();
  };
  std::vector<std::thread *> thread_vec;
  size_t pos = 0UL;
  for (auto r : runner_vec) {
    //if (pos != 0) {
      for (size_t i = 0; i < num_runnable_per_thread; ++i) {
        new TMyRunnable(r, lock, num_iter, mut, cond, num_finished);
      }
    //}
    thread_vec.emplace_back(new thread(std::bind(launch_fiber_sched, r, pos)));
    ++pos;
  }
  Base::TTimer timer;
  timer.Start();
  /* can start */ {
    std::lock_guard<std::mutex> lock(mut);
    can_start = true;
    cond.notify_all();
  }
  /* TODO: temp get rid of numthreads -- */
  //--num_threads;
  /* wait for finished */ {
    std::unique_lock<std::mutex> lock(mut);
    while (num_finished != num_threads * num_runnable_per_thread) {
      cond.wait(lock);
    }
  }
  timer.Stop();
  printf("Total time = [%f], [%f] per, [%ld] / s\n", timer.Total(), timer.Total() / (num_iter * num_threads), static_cast<size_t>((num_iter * num_threads * num_runnable_per_thread) / timer.Total()));
  for (auto r : runner_vec) {
    r->ShutDown();
  }
  for (auto t : thread_vec) {
    t->join();
    delete t;
  }
  for (auto r : runner_vec) {
    delete r;
  }
  delete TFrame::LocalFramePool;
}