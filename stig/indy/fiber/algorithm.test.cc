/* <stig/indy/fiber/algorithm.test.cc>

   Unit test for <stig/indy/fiber/algorithm.h>.

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

#include <stig/indy/fiber/algorithm.h>

#include <algorithm>
#include <thread>

#include <unistd.h>

#include <base/timer.h>
#include <base/usage_meter.h>
#include <test/kit.h>

using namespace Base;
using namespace Stig::Indy::Fiber;

template <typename TVal, size_t ParallelThresh>
class TSortRunnable
    : public TRunnable {
  NO_COPY_SEMANTICS(TSortRunnable);
  public:

  TSortRunnable(TRunner *runner,
                std::vector<TVal> &data,
                TRunnerPool &work_pool,
                std::mutex &mut,
                std::condition_variable &cond,
                bool &fin)
    : Data(data),
      WorkPool(work_pool),
      Mut(mut),
      Cond(cond),
      Fin(fin) {
    Frame = TFrame::LocalFramePool->Alloc();
    try {
      Frame->Latch(runner, this, static_cast<TRunnable::TFunc>(&TSortRunnable::RunMe));
    } catch (...) {
      TFrame::LocalFramePool->Free(Frame);
      throw;
    }
  }

  ~TSortRunnable() {
    TFrame::LocalFramePool->Free(Frame);
  }

  void RunMe() {
    Base::TUsageMeter usage_meter(RUSAGE_SELF);
    Base::TTimer parallel_timer;
    usage_meter.Start();
    parallel_timer.Start();
    Sort<ParallelThresh>(WorkPool, Data.begin(), Data.end());
    parallel_timer.Stop();
    usage_meter.Stop();
    printf("parallel sort =[%f], usr=[%f], sys=[%f]\n", parallel_timer.Total(), usage_meter.GetTotalUserCPU(), usage_meter.GetTotalSystemCPU());
    std::lock_guard<std::mutex> lock(Mut);
    Fin = true;
    Cond.notify_one();
  }

  private:

  TFrame *Frame;

  std::vector<TVal> &Data;

  TRunnerPool &WorkPool;

  std::mutex &Mut;
  std::condition_variable &Cond;
  bool &Fin;

};

template <typename TVal>
void TestSort(const std::vector<TVal> &input_data, const size_t num_workers) {
  const size_t num_frames = 1UL;
  const size_t num_sub_frames = (num_workers * 2) - 1;
  const size_t stack_size = 8 * 1024 * 1024;
  std::vector<TVal> expected_data(input_data.begin(), input_data.end());
  std::vector<TVal> fiber_data(input_data.begin(), input_data.end());
  Base::TUsageMeter usage_meter(RUSAGE_SELF);
  usage_meter.Start();
  Base::TTimer expected_timer;
  expected_timer.Start();
  std::sort(expected_data.begin(), expected_data.end());
  expected_timer.Stop();
  usage_meter.Stop();
  printf("single threaded sort =[%f], usr=[%f], sys=[%f]\n", expected_timer.Total(), usage_meter.GetTotalUserCPU(), usage_meter.GetTotalSystemCPU());
  EXPECT_TRUE(expected_data != input_data);
  EXPECT_TRUE(fiber_data != expected_data);
  TRunner::TRunnerCons runner_cons(1UL + num_workers);
  TRunner runner(runner_cons);
  TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *> *frame_pool_manager = new TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>(num_frames + num_sub_frames, stack_size, &runner);
  TFrame::LocalFramePool = new TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>::TThreadLocalPool(frame_pool_manager);
  TRunnerPool work_pool(runner_cons, num_workers);
  try {
    auto launch_fiber_sched = [&]() {
      if (!TFrame::LocalFramePool) {
        TFrame::LocalFramePool = new TThreadLocalGlobalPoolManager<TFrame, size_t, TRunner *>::TThreadLocalPool(frame_pool_manager);
      }
      runner.Run();
      if (TFrame::LocalFramePool) {
        delete TFrame::LocalFramePool;
      }
    };
    std::thread t1(launch_fiber_sched);
    std::mutex mut;
    std::condition_variable cond;
    bool fin = false;
    TSortRunnable<TVal, 8> runnable(&runner, fiber_data, work_pool, mut, cond, fin);
    std::unique_lock<std::mutex> lock(mut);
    while (!fin) {
      cond.wait(lock);
    }
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
  EXPECT_TRUE(fiber_data == expected_data);
}

class TSortedObj {
    public:

    /* TODO */
    TSortedObj(size_t old_key, size_t new_key) : OldKey(old_key), NewKey(new_key) {}

    /* TODO */
    bool operator<(const TSortedObj &that) const {
      return OldKey < that.OldKey;
    }

    /* TODO */
    bool operator==(const TSortedObj &that) const {
      return OldKey == that.OldKey && NewKey == that.NewKey;
    }

    /* TODO */
    size_t OldKey;

    /* TODO */
    size_t NewKey;

  };  // TSortedObj

FIXTURE(SmallParallelSort) {
  const size_t num_workers = 5UL;
  const std::vector<size_t> input_data{9, 8, 7, 6, 5, 4, 3, 2, 1};
  TestSort(input_data, num_workers);
}

FIXTURE(LargeParallelSort) {
  const size_t num_elem = 500000UL;
  const size_t num_workers = 8UL;
  std::vector<TSortedObj> input_data;
  input_data.reserve(num_elem);
  for (size_t i = num_elem; i > 0; --i) {
    input_data.emplace_back(i, i * 10);
  }
  printf("DataSetSize = [%fMB]\n", static_cast<double>(num_elem * sizeof(TSortedObj)) / (1024 * 1024));
  random_shuffle(input_data.begin(), input_data.end());
  TestSort(input_data, num_workers);
}