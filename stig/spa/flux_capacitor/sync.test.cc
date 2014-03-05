/* <stig/spa/flux_capacitor/sync.test.cc>

   Unit test for <stig/spa/flux_capacitor/sync.h>.

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

#include <stig/spa/flux_capacitor/sync.h>

#include <condition_variable>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <vector>

#include <test/kit.h>

using namespace Stig::Spa::FluxCapacitor;

static TSync VecSync;
static std::vector<int> Vec;
static std::mutex Mutex;
static std::condition_variable Cond1; // see Step1
static std::condition_variable Cond2; // see Step2
static std::condition_variable Cond3; // see Step3
static std::condition_variable Cond4; // see Step4
static std::condition_variable Cond5; // see Step5
static std::condition_variable Cond6; // see Step6
static bool Step1 = false; // synchronize starting sum = 1
static bool Step2 = false; // appender gets exclusive lock; testing exclusivity
static bool Step3 = false; // getter has verified shared lock waited for appender's exclusive lock to exit
static bool Step4 = false; // appender has shared lock; testing multiple threads holding shared locks
static bool Step5 = false; // getter has shared lock; following Step4: appender still holding shared lock
static bool Step6 = false; // appender has acknowledged getter's shared lock; finished testing multiple threads holding shared locks

/* Sum the total value of the vector while holding a shared lock */
int GetTotal() {
  int total = 0;
  TSync::TSharedLock lock(VecSync);
  for (auto iter = Vec.begin(); iter != Vec.end(); ++iter) {
    total += *iter;
  }
  return total;
}

/* Primarily used to append values to the vector. We walk in lock step with the getter and force race conditions as well as lock promotion */
class TAppender {
  NO_COPY_SEMANTICS(TAppender);
  public:

  /* Launch a thread to represent us */
  TAppender() {
    TSync::TExclusiveLock lock(VecSync);
    Vec.push_back(1);
    Thread = std::thread(std::bind(&TAppender::Go, this));
  }

  /* Join the thread so our test case finishes */
  ~TAppender() {
    Thread.join();
  }

  /* This is where we actually walk in lock step with the getter to test TSync */
  void Go() {
    // We should both start at sum = 1
    EXPECT_EQ(GetTotal(), 1);
    /* wait for the getter to check our sum is 1 */ {
      std::unique_lock<std::mutex> std_lock(Mutex);
      while (!Step1) {
        Cond1.wait(std_lock);
      }
    }
    /* Get an exclusive lock; let the getter know; wait 1 second, push value 3; then wait on getter
       *** This tests that a shared lock after an exclusive lock will wait till the exclusive lock is finished ***
    */ {
      std::unique_lock<std::mutex> std_lock(Mutex);
      {
        // get exclusive lock
        TSync::TExclusiveLock lock(VecSync);
        // notify getter
        Step2 = true;
        Cond2.notify_all();
        // sleep 1 second
        sleep(1);
        // push 3
        Vec.push_back(3);
      }
      // wait for getter
      while (!Step3) {
        Cond3.wait(std_lock);
      }
    }
    /* get an exclusive lock and push 5; sum = 9 */ {
      TSync::TExclusiveLock lock(VecSync);
      Vec.push_back(5);
    }
    /* get a shared lock and notify the getter; ensure sum = 9; wait for getter; notify getter
       *** This tests that we can hold 2 shared locks from seperate threads simultaneously and read the same value ***
       The (wait for getter; notify getter) seems strange but it is done to ensure both threads are holding the shared lock
    */ {
      std::unique_lock<std::mutex> std_lock(Mutex);
      // get shared lock
      TSync::TSharedLock lock(VecSync);
      // notify getter
      Step4 = true;
      Cond4.notify_all();
      // ensure the sum = 9
      EXPECT_EQ(GetTotal(), 9);
      // wait on getter
      while (!Step5) {
        Cond5.wait(std_lock);
      }
      // notify appender
      Step6 = true;
      Cond6.notify_all();
    }
    /* push value 7; sum = 16 */ {
      TSync::TExclusiveLock lock(VecSync);
      Vec.push_back(7);
    }
    /* we are no longer in lock step; the following tests are for recursive locks and lock promotion: */
    /* aquire a recursive locks and ensure we see the same value */ {
      // get lock 1
      TSync::TSharedLock lock(VecSync);
      EXPECT_EQ(GetTotal(), 16);
      {
        // get recursive lock 1
        TSync::TSharedLock recursive_lock(VecSync);
        EXPECT_EQ(GetTotal(), 16);
        {
          // get recursive lock 2
          TSync::TSharedLock recursive_lock2(VecSync);
          EXPECT_EQ(GetTotal(), 16);
        }
      }
    }
    /* test that we can still get a recursive shared lock even if we have an exclusive lock already. TSync is supposed to accept this and treat the recursive shared lock as an exclusive as exclusive has a greater shadow */ {
      // get the exclusive lock
      TSync::TExclusiveLock exclusive_lock(VecSync);
      // push 9; sum = 25
      Vec.push_back(9);
      EXPECT_EQ(GetTotal(), 25);
      {
        // get a shared recurisve lock (treated as recursive exlusive)
        TSync::TSharedLock shared_lock(VecSync);
        // ensure sum = 25
        EXPECT_EQ(GetTotal(), 25);
        // push 11; sum = 36
        Vec.push_back(11);
        // ensure sum = 36;
        EXPECT_EQ(GetTotal(), 36);
      }
    }
    /* Push 13; sum = 49 */ {
      TSync::TExclusiveLock exclusive_lock(VecSync);
      Vec.push_back(13);
    }
    /* Test lock promotion failure. TSync should not allow promotion of shared locks to exclusive locks */ {
      // get shared lock
      TSync::TSharedLock shared_lock(VecSync);
      EXPECT_EQ(GetTotal(), 49);
      {
        bool caught_error = false;
        // try to get a recursive exclusive lock (lock promotion)
        try {
          // THIS SHOULD FAIL
          TSync::TExclusiveLock exclusive_lock(VecSync);
        } catch (TSyncException &ex) {
          caught_error = true;
        }
        // we should have caught the exception
        EXPECT_TRUE(caught_error);
      }
    }
  }

  private:

  /* the thread in which we run; launch at constructor; join on destructor */
  std::thread Thread;

};

/* Primarily used to walk in lock step with the appender and force race conditions and test the recursive nature of TSync */
class TGetter {
  NO_COPY_SEMANTICS(TGetter);

  public:

  /* Launch a thread to represent us */
  TGetter() {
    EXPECT_EQ(GetTotal(), 1);
    Thread = std::thread(std::bind(&TGetter::Go, this));
  }

  /* Join the thread to ensure we finish our test case */
  ~TGetter() {
    Thread.join();
  }

  /* This is where we actually walk in lock step with the appender */
  void Go() {
    // we should both start at sum = 1
    EXPECT_EQ(GetTotal(), 1);
    /* notify the appender that we're ready for more */ {
      std::lock_guard<std::mutex> std_lock(Mutex);
      Step1 = true;
      Cond1.notify_all();
    }
    /* wait on appender; get a shared lock (this is while the appender has an exclusive lock); make sure the sum is 4; notify getter that we're done
       *** This tests that a shared lock after an exclusive lock will wait till the exclusive lock is finished ***
    */ {
      std::unique_lock<std::mutex> std_lock(Mutex);
      // wait on appender
      while (!Step2) {
        Cond2.wait(std_lock);
      }
      // get shared lock; this should block till the appender's exclusive lock is finished
      TSync::TSharedLock lock(VecSync);
      // ensure the sum is 4; proving this happened after the exclusive lock
      EXPECT_EQ(GetTotal(), 4);
      // notify the appender we're done
      Step3 = true;
      Cond3.notify_all();
    }
    /* wait for appender; get a shared lock (this is after appender already has a shared lock); ensure sum = 9; notify appender; wait on appender
       *** This tests that we can hold 2 shared locks from seperate threads simultaneously and read the same value ***
       The (notify appender; wait on appender) seems strange but it is done to ensure both threads are holding the shared lock
    */ {
      std::unique_lock<std::mutex> std_lock(Mutex);
      // wait on the appender
      while (!Step4) {
        Cond4.wait(std_lock);
      }
      // get the shared lock
      TSync::TSharedLock lock(VecSync);
      // ensure the sum = 9
      EXPECT_EQ(GetTotal(), 9);
      // notify the the appender
      Step5 = true;
      Cond5.notify_all();
      // wait on the appender
      while (!Step6) {
        Cond6.wait(std_lock);
      }
    }
  }

  private:

  /* the thread in which we run; launch at constructor; join on destructor */
  std::thread Thread;

};

FIXTURE(Typical) {
  TAppender appender;
  TGetter getter;
}