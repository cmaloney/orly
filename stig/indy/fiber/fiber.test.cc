/* <stig/indy/fiber/fiber.test.cc>

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

#include <unistd.h>

#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig::Indy::Fiber;

FIXTURE(Typical) {
  TRunner::TRunnerCons runner_cons(1);
  TRunner runner(runner_cons);
  auto launch_fiber_sched = [&]() {
    runner.Run();
  };
  thread t1(launch_fiber_sched);
  sleep(2);
  runner.ShutDown();
  t1.join();
}

class TCoIterRunnable
    : public TRunnable {
  NO_COPY_SEMANTICS(TCoIterRunnable);
  public:

  TCoIterRunnable(size_t id, size_t num_iter, TRunner *runner, const std::function<void ()> &start_cb, const std::function<void ()> &completion_cb)
      : Id(id), NumIter(num_iter), StartCb(start_cb), CompletionCb(completion_cb) {
    Frame = TFrame::LocalFramePool->Alloc();
    try {
      Frame->Latch(runner, this, static_cast<TRunnable::TFunc>(&TCoIterRunnable::Compute));
    } catch (...) {
      TFrame::LocalFramePool->Free(Frame);
      throw;
    }
  }

  ~TCoIterRunnable() {
    TFrame::LocalFramePool->Free(Frame);
  }

  void Compute() {
    StartCb();
    for (size_t i = 0; i < NumIter; ++i) {
      //std::cout << "TMyRunnable [" << Id << "] Compute [" << i << "]" << std::endl;
      Yield();
    }
    CompletionCb();
  }

  TFrame *GetFrame() const {
    return Frame;
  }

  private:

  size_t Id;

  size_t NumIter;

  TFrame *Frame;

  const std::function<void ()> StartCb;

  const std::function<void ()> CompletionCb;

};

FIXTURE(CoIterate) {
  const size_t num_iter = 10000000UL;
  const size_t num_frames = 4UL;
  const size_t stack_size = 8 * 1024 * 1024;
  TRunner::TRunnerCons runner_cons(1);
  TRunner runner(runner_cons);
  TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *> frame_pool_manager(num_frames, stack_size, &runner);
  TFrame::LocalFramePool = new TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>::TThreadLocalPool(&frame_pool_manager);
  try {
    auto launch_fiber_sched = [&]() {
      runner.Run();
    };
    thread t1(launch_fiber_sched);
    sleep(1);
    std::mutex mut;
    std::condition_variable cond;
    size_t finished_count = 0UL;
    auto completion_cb = [&]() {
      std::lock_guard<std::mutex> lock(mut);
      ++finished_count;
      cond.notify_one();
    };
    TCoIterRunnable runnable1(1UL, num_iter, &runner, [&](){
      Wait();
    }, completion_cb);
    TCoIterRunnable runnable2(2UL, num_iter, &runner, [&](){
      TRunner::Schedule(runnable1.GetFrame());
    }, completion_cb);
    std::unique_lock<std::mutex> lock(mut);
    while (finished_count < 2UL) {
      cond.wait(lock);
    }
    runner.ShutDown();
    t1.join();
  } catch (...) {
    delete TFrame::LocalFramePool;
    TFrame::LocalFramePool = nullptr;
    throw;
  }
  delete TFrame::LocalFramePool;
  TFrame::LocalFramePool = nullptr;
}

class TSpawnRunnable
    : public TRunnable {
  NO_COPY_SEMANTICS(TSpawnRunnable);
  public:

  TSpawnRunnable(TRunner *runner)
      : Finished(0UL), ExpectedSum(0UL), ActualSum(0UL) {
    Frame = TFrame::LocalFramePool->Alloc();
    try {
      Frame->Latch(runner, this, static_cast<TRunnable::TFunc>(&TSpawnRunnable::Spawn));
    } catch (...) {
      TFrame::LocalFramePool->Free(Frame);
      throw;
    }
  }

  ~TSpawnRunnable() {
    TFrame::LocalFramePool->Free(Frame);
  }

  void Spawn() {
    for (size_t i = 0; i < 5; ++i) {
      ExpectedSum += i;
      SubProcVec.emplace_back(new TSubProc(this, i));
    }
    EXPECT_NE(ActualSum, ExpectedSum);
    Wait();
    EXPECT_EQ(ActualSum, ExpectedSum);
    for (auto sub_proc : SubProcVec) {
      delete sub_proc;
    }
  }

  TFrame *GetFrame() const {
    return Frame;
  }

  private:

  void CompleteSubProc() {
    ++Finished;
    if (Finished == SubProcVec.size()) {
      TRunner::Schedule(Frame);
    }
  }

  class TSubProc
      : public TRunnable {
    NO_COPY_SEMANTICS(TSubProc);
    public:

    TSubProc(TSpawnRunnable *spawner, size_t my_val)
        : Spawner(spawner), MyVal(my_val) {
      Frame = TFrame::LocalFramePool->Alloc();
      try {
        Frame->Latch(this, static_cast<TRunnable::TFunc>(&TSubProc::Compute));
      } catch (...) {
        TFrame::LocalFramePool->Free(Frame);
        throw;
      }
    }

    virtual ~TSubProc() {
      TFrame::LocalFramePool->Free(Frame);
    }

    TFrame *GetFrame() const {
      return Frame;
    }

    void Compute() {
      Spawner->ActualSum += MyVal;
      Spawner->CompleteSubProc();
    }

    private:

    TSpawnRunnable *Spawner;

    TFrame *Frame;

    size_t MyVal;

  };

  std::vector<TSubProc *> SubProcVec;

  size_t Finished;

  TFrame *Frame;

  size_t ExpectedSum;

  size_t ActualSum;

};

FIXTURE(SpawnAndSync) {
  const size_t num_frames = 11UL;
  const size_t stack_size = 8 * 1024 * 1024;
  TRunner::TRunnerCons runner_cons(1UL);
  TRunner runner(runner_cons);
  TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *> frame_pool_manager(num_frames, stack_size, &runner);
  TFrame::LocalFramePool = new TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>::TThreadLocalPool(&frame_pool_manager);
  try {
    auto launch_fiber_sched = [&]() {
      TFrame::LocalFramePool = new TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>::TThreadLocalPool(&frame_pool_manager);
      try {
        runner.Run();
      } catch (...) {
        delete TFrame::LocalFramePool;
        TFrame::LocalFramePool = nullptr;
        throw;
      }
      delete TFrame::LocalFramePool;
      TFrame::LocalFramePool = nullptr;
    };
    thread t1(launch_fiber_sched);
    sleep(1);
    TSpawnRunnable runnable(&runner);
    sleep(2);
    runner.ShutDown();
    t1.join();
  } catch (...) {
    delete TFrame::LocalFramePool;
    TFrame::LocalFramePool = nullptr;
    throw;
  }
  delete TFrame::LocalFramePool;
  TFrame::LocalFramePool = nullptr;
}

class TLeakRunnable
    : public TRunnable {
  NO_COPY_SEMANTICS(TLeakRunnable);
  public:

  TLeakRunnable(TRunner *runner, bool &caught_exception) : CaughtException(caught_exception) {
    assert(!caught_exception);
    Frame = TFrame::LocalFramePool->Alloc();
    try {
      Frame->Latch(runner, this, static_cast<TRunnable::TFunc>(&TLeakRunnable::Leak));
    } catch (...) {
      TFrame::LocalFramePool->Free(Frame);
      throw;
    }
  }

  ~TLeakRunnable() {
    try {
      TFrame::LocalFramePool->Free(Frame);
    } catch (const std::logic_error &) {
      CaughtException = true;
    }
  }

  void Leak() {
    Wait();
  }

  TFrame *GetFrame() const {
    return Frame;
  }

  private:

  TFrame *Frame;

  bool &CaughtException;

};

FIXTURE(CatchLeakedFrame) {
  const size_t num_frames = 1UL;
  const size_t stack_size = 8 * 1024 * 1024;
  TRunner::TRunnerCons runner_cons(1);
  TRunner runner(runner_cons);
  TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *> *frame_pool_manager = new TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>(num_frames, stack_size, &runner);
  TFrame::LocalFramePool = new TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>::TThreadLocalPool(frame_pool_manager);
  bool caught_expected_exception = false;
  try {
    auto launch_fiber_sched = [&]() {
      runner.Run();
    };
    thread t1(launch_fiber_sched);
    sleep(1);
    TLeakRunnable runnable(&runner, caught_expected_exception);
    sleep(2);
    runner.ShutDown();
    t1.join();
  } catch (const std::logic_error &ex) {
    delete TFrame::LocalFramePool;
    TFrame::LocalFramePool = nullptr;
    throw;
  }
  delete TFrame::LocalFramePool;
  TFrame::LocalFramePool = nullptr;
  delete frame_pool_manager;
  EXPECT_TRUE(caught_expected_exception);
}

class TSimpleRunnable
    : public TRunnable {
  NO_COPY_SEMANTICS(TSimpleRunnable);
  public:

  TSimpleRunnable(TRunner *runner, TRunner *other_runner, bool &did_run) : OtherRunner(other_runner), DidRun(did_run) {
    Frame = TFrame::LocalFramePool->Alloc();
    try {
      Frame->Latch(runner, this, static_cast<TRunnable::TFunc>(&TSimpleRunnable::RunMe));
    } catch (...) {
      TFrame::LocalFramePool->Free(Frame);
      throw;
    }
  }

  ~TSimpleRunnable() {
    TFrame::LocalFramePool->Free(Frame);
  }

  void RunMe() {
    TRunner *start_runner = TRunner::LocalRunner;
    TRunner *middle_runner = nullptr;
    /* RAII scope */ {
      TSwitchToRunner switcher(OtherRunner);
      middle_runner = TRunner::LocalRunner;
    }
    TRunner *end_runner = TRunner::LocalRunner;
    EXPECT_EQ(start_runner, end_runner);
    EXPECT_NE(start_runner, middle_runner);
    DidRun = true;
  }

  private:

  TFrame *Frame;

  TRunner *OtherRunner;

  bool &DidRun;

};

FIXTURE(SwitchRunnerToRunnerRAII) {
  const size_t num_frames = 2UL;
  const size_t stack_size = 8 * 1024 * 1024;
  TRunner::TRunnerCons runner_cons(2UL);
  bool did_run = false;
  TRunner runner_1(runner_cons);
  TRunner runner_2(runner_cons);
  TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *> *frame_pool_manager = new TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>(num_frames, stack_size, &runner_1);
  try {
    TFrame::LocalFramePool = new TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>::TThreadLocalPool(frame_pool_manager);
    auto launch_fiber_sched = [&](TRunner *runner) {
      runner->Run();
    };
    thread t1(std::bind(launch_fiber_sched, &runner_1));
    thread t2(std::bind(launch_fiber_sched, &runner_2));
    TSimpleRunnable runnable(&runner_1, &runner_2, did_run);
    sleep(2);
    runner_1.ShutDown();
    runner_2.ShutDown();
    t1.join();
    t2.join();
  } catch (...) {
    delete TFrame::LocalFramePool;
    TFrame::LocalFramePool = nullptr;
    throw;
  }
  delete TFrame::LocalFramePool;
  TFrame::LocalFramePool = nullptr;
  delete frame_pool_manager;
  EXPECT_TRUE(did_run);
}

class TActorRunnable
    : public TRunnable {
  NO_COPY_SEMANTICS(TActorRunnable);
  public:

  TActorRunnable(TRunner *runner,
                   TSingleSem &my_push_sem,
                   TSingleSem &my_pop_sem,
                   std::atomic<size_t> &pos_counter,
                   std::atomic<size_t> &accuracy_counter,
                   std::mutex &mut,
                   std::condition_variable &cond,
                   size_t &finish_count)
    : MyPushSem(my_push_sem),
      MyPopSem(my_pop_sem),
      PosCounter(pos_counter),
      AccuracyCounter(accuracy_counter),
      Mut(mut),
      Cond(cond),
      FinishCount(finish_count) {
    Frame = TFrame::LocalFramePool->Alloc();
    try {
      Frame->Latch(runner, this, static_cast<TRunnable::TFunc>(&TActorRunnable::RunMe));
    } catch (...) {
      TFrame::LocalFramePool->Free(Frame);
      throw;
    }
  }

  ~TActorRunnable() {
    TFrame::LocalFramePool->Free(Frame);
  }

  void RunMe() {
    MyPopSem.Pop();
    // 2
    ++PosCounter;
    AccuracyCounter += (PosCounter * 11UL);
    MyPushSem.Push();
    MyPopSem.Pop();
    // 4
    ++PosCounter;
    AccuracyCounter += (PosCounter * 13UL);
    MyPushSem.Push();
    std::lock_guard<std::mutex> lock(Mut);
    ++FinishCount;
    Cond.notify_one();
  }

  private:

  TFrame *Frame;

  TSingleSem &MyPushSem;
  TSingleSem &MyPopSem;

  std::atomic<size_t> &PosCounter;
  std::atomic<size_t> &AccuracyCounter;

  std::mutex &Mut;
  std::condition_variable &Cond;
  size_t &FinishCount;

};

class TTriggerRunnable
    : public TRunnable {
  NO_COPY_SEMANTICS(TTriggerRunnable);
  public:

  TTriggerRunnable(TRunner *runner,
                   TSingleSem &my_push_sem,
                   TSingleSem &my_pop_sem,
                   std::atomic<size_t> &pos_counter,
                   std::atomic<size_t> &accuracy_counter,
                   std::mutex &mut,
                   std::condition_variable &cond,
                   size_t &finish_count)
    : MyPushSem(my_push_sem),
      MyPopSem(my_pop_sem),
      PosCounter(pos_counter),
      AccuracyCounter(accuracy_counter),
      Mut(mut),
      Cond(cond),
      FinishCount(finish_count) {
    Frame = TFrame::LocalFramePool->Alloc();
    try {
      Frame->Latch(runner, this, static_cast<TRunnable::TFunc>(&TTriggerRunnable::RunMe));
    } catch (...) {
      TFrame::LocalFramePool->Free(Frame);
      throw;
    }
  }

  ~TTriggerRunnable() {
    TFrame::LocalFramePool->Free(Frame);
  }

  void RunMe() {
    // 1
    ++PosCounter;
    AccuracyCounter += (PosCounter * 3UL);
    MyPushSem.Push();
    MyPopSem.Pop();
    // 3
    ++PosCounter;
    AccuracyCounter += (PosCounter * 5UL);
    MyPushSem.Push();
    MyPopSem.Pop();
    // 5
    ++PosCounter;
    AccuracyCounter += (PosCounter * 7UL);
    std::lock_guard<std::mutex> lock(Mut);
    ++FinishCount;
    Cond.notify_one();
  }

  private:

  TFrame *Frame;

  TSingleSem &MyPushSem;
  TSingleSem &MyPopSem;

  std::atomic<size_t> &PosCounter;
  std::atomic<size_t> &AccuracyCounter;

  std::mutex &Mut;
  std::condition_variable &Cond;
  size_t &FinishCount;

};

FIXTURE(Sem) {
  const size_t num_frames = 2UL;
  const size_t stack_size = 8 * 1024 * 1024;
  TRunner::TRunnerCons runner_cons(2UL);
  TRunner runner_1(runner_cons);
  TRunner runner_2(runner_cons);
  std::atomic<size_t> pos_counter(0UL);
  std::atomic<size_t> accuracy_counter(0UL);
  TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *> *frame_pool_manager = new TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>(num_frames, stack_size, &runner_1);
  try {
    TFrame::LocalFramePool = new TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>::TThreadLocalPool(frame_pool_manager);
    auto launch_fiber_sched = [&](TRunner *runner) {
      runner->Run();
    };
    thread t1(std::bind(launch_fiber_sched, &runner_1));
    thread t2(std::bind(launch_fiber_sched, &runner_2));
    TSingleSem fiber_sem_1, fiber_sem_2;
    std::mutex mut;
    std::condition_variable cond;
    size_t finish_count = 0UL;
    TActorRunnable actor_runnable(&runner_1, fiber_sem_1, fiber_sem_2, pos_counter, accuracy_counter, mut, cond, finish_count);
    TTriggerRunnable trigger_runnable(&runner_2, fiber_sem_2, fiber_sem_1, pos_counter, accuracy_counter, mut, cond, finish_count);
    /* wait for them to finish using condition variable since we're not frame ourselves... */ {
      std::unique_lock<std::mutex> lock(mut);
      while (finish_count != 2) {
        cond.wait(lock);
      }
    }
    runner_1.ShutDown();
    runner_2.ShutDown();
    t1.join();
    t2.join();
  } catch (...) {
    delete TFrame::LocalFramePool;
    TFrame::LocalFramePool = nullptr;
    throw;
  }
  delete TFrame::LocalFramePool;
  TFrame::LocalFramePool = nullptr;
  delete frame_pool_manager;
  EXPECT_EQ(pos_counter, 5UL);
  EXPECT_EQ(accuracy_counter, 127UL);
}
