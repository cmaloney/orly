/* <base/booster.test.manual.cc>

   Implements <base/booster.h>.

   NOTE: This test must run as root.  See the comment at the top of <base/booster.h> for
   more information.

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

#include <base/booster.h>

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include <unistd.h>

#include <base/error_utils.h>
#include <test/kit.h>

using namespace std;
using namespace Base;

/* A synchronized counter.  We'll use this to manage the thundering herd of threads we'll use for tests. */
class TCounter {
  NO_COPY_SEMANTICS(TCounter);
  public:

  /* Start at zero. */
  TCounter()
      : Count(0) {}

  /* Increment the counter, possibly unblocking the threads blocked on Wait(). */
  void Incr() {
    assert(this);
    unique_lock<mutex> lock(Mutex);
    ++Count;
    Changed.notify_all();
  }

  /* Block until the count equals the given value. */
  void Wait(size_t count) {
    assert(this);
    unique_lock<mutex> lock(Mutex);
    while (Count != count) {
      Changed.wait(lock);
    }
  }

  private:

  /* Covers 'Count'. */
  mutex Mutex;

  /* Notifies all waiters when 'Count' changes. */
  condition_variable Changed;

  /* Our actual count. */
  size_t Count;

};  // TCounter

/* A rig for performing a test involving a thundering herd of worker threads.
   The workers will all vie to push values into a queue.  If priority is honored, then
   the queue should get the values in order of the workers' priorities; otherwise, the
   values in the queue could be in any arbitrary order. */
class TTester {
  NO_COPY_SEMANTICS(TTester);
  public:

  /* Run the test with the given number of workers, each pushing to the queue the given number of times.
     If we're testing with boosting turned on, then each of the workers will run at a different priority;
     otherwise, they'll all run at the same priority.
     This function returns true iff. the resulting queue is perfectly ordered, as though the workers
     took turns according to priority, each pushing to completion before allowing the next to run.  This
     should be unlikely to happen if we're running without boosting. */
  static bool Test(int worker_count, int push_count, bool is_boosted) {
    return TTester(worker_count, push_count, is_boosted).Run();
  }

  private:

  /* Cache the args. */
  TTester(int worker_count, int push_count, bool is_boosted)
      : WorkerCount(worker_count), PushCount(push_count), Boosted(is_boosted) {}

  /* Run the workers, let them build the queue, then check to see if the queue is in order. */
  bool Run() {
    assert(this);
    /* Launch the required number of workers. */
    vector<thread> workers;
    for (int priority_backoff = 0; priority_backoff < WorkerCount; ++priority_backoff) {
      workers.push_back(thread(&TTester::ThreadMain, this, priority_backoff));
    }
    /* Wait for all the workers to block, waiting for our signal to go. */
    WorkerCounter.Wait(WorkerCount);
    /* Let 'em go!  Then wait for them all to finish. */
    GoCounter.Incr();
    for (auto &worker: workers) {
      worker.join();
    }
    /* See if the queue is in order. */
    int push_count = PushCount, priority_backoff = 0;
    bool success = true;
    for (; success && !Queue.empty(); Queue.pop()) {
      success = (Queue.front() == priority_backoff);
      --push_count;
      if (!push_count) {
        ++priority_backoff;
        push_count = PushCount;
      }
    }
    return success;
  }

  /* Workers start here. */
  void ThreadMain(int priority_backoff) {
    assert(this);
    /* Boost, if we're supposed to. */
    unique_ptr<TBooster> booster;
    if (Boosted) {
      booster.reset(new TBooster(SCHED_FIFO, priority_backoff));
    }
    /* Let Run() know we're ready. */
    WorkerCounter.Incr();
    /* Wait for Run() to say go. */
    GoCounter.Wait(1);
    /* Push to the queue as fast as we can. */
    for (int i = 0; i < PushCount; ++i) {
      lock_guard<mutex> lock(Mutex);
      Queue.push(priority_backoff);
    }
  }

  /* Cached at construction. */
  int WorkerCount, PushCount;
  bool Boosted;

  /* Coordinates the workers. */
  TCounter WorkerCounter, GoCounter;

  /* Collects the workers' results. */
  mutex Mutex;
  queue<int> Queue;

};  // TTester

/* Run the test a bunch of times, boosted and unboosted.
   The boosted version should generally succeed more often than the unboosted version,
   but it's not perfect.  The ratio should be large but probably won't be 1.0. */
static void GetBoostedSuccessRatio(size_t &boosted_successes, size_t &unboosted_successes) {
  assert(&boosted_successes);
  assert(&unboosted_successes);
  boosted_successes = 0;
  unboosted_successes = 0;
  int worker_count = 5, push_count = 10;
  for (size_t i = 0; i < 10000; ++i) {
    if (TTester::Test(worker_count, push_count, true)) {
      ++boosted_successes;
    }
    if (TTester::Test(worker_count, push_count, false)) {
      ++unboosted_successes;
    }
  }
}

FIXTURE(Typical) {
  size_t boosted_successes, unboosted_successes;
  GetBoostedSuccessRatio(boosted_successes, unboosted_successes);
  EXPECT_GT(boosted_successes, unboosted_successes);
  auto ratio = static_cast<double>(boosted_successes) / static_cast<double>(boosted_successes + unboosted_successes);
  EXPECT_GE(ratio, 0.80);
}
