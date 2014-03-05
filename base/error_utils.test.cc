/* <base/error_utils.test.cc>

   Unit test for <base/error_utils.h>.

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

#include <base/error_utils.h>

#include <condition_variable>
#include <mutex>
#include <thread>

#include <signal.h>
#include <unistd.h>

#include <base/zero.h>
#include <test/kit.h>

using namespace std;
using namespace Base;

FIXTURE(LibraryGenerated) {
  bool caught = false;
  try {
    thread().detach();
  } catch (const system_error &error) {
    caught = true;
  } catch (...) {}
  EXPECT_TRUE(caught);
}

FIXTURE(UtilsGenerated) {
  bool caught = false;
  try {
    IfLt0(read(-1, 0, 0));
  } catch (const system_error &error) {
    caught = true;
  } catch (...) {}
  EXPECT_TRUE(caught);
}

FIXTURE(Interruption) {
  struct sigaction action;
  Zero(action);
  action.sa_handler = [](int) {};
  sigaction(SIGUSR1, &action, 0);
  mutex mx;
  condition_variable cv;
  bool running = false, was_interrupted = false;
  thread t([&mx, &cv, &running, &was_interrupted] {
    /* lock */ {
      unique_lock<mutex> lock(mx);
      running = true;
      cv.notify_one();
    }
    try {
      IfLt0(pause());
    } catch (system_error &error) {
      was_interrupted = WasInterrupted(error);
    } catch (...) {}
  });
  /* lock */ {
    unique_lock<mutex> lock(mx);
    while (!running) {
      cv.wait(lock);
    }
  }
  IfNe0(pthread_kill(t.native_handle(), SIGUSR1));
  t.join();
  EXPECT_TRUE(was_interrupted);
}
