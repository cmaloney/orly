/* <stig/spa/flux_capacitor/sync.perf.cc>

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

/*
./sync.perf 100 1000000
min = 19.992, max = 20.7677, mean = 20.4054, dev = 0.131617, total = 2040.54
*/

#include <stig/spa/flux_capacitor/sync.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;
using namespace Stig::Spa::FluxCapacitor;

class TActor {
  NO_COPY_SEMANTICS(TActor);
  public:

  TActor(size_t spin_count)
      : Timer(0) {
    Thread = thread(&TActor::Main, this, spin_count);
  }

  double GetTimer() const {
    assert(this);
    return Timer;
  }

  void Join() {
    assert(this);
    Thread.join();
  }

  static void Go() {
    unique_lock<mutex> lock(Mutex);
    GoFlag = true;
    Cond.notify_all();
  }

  private:

  void Main(size_t spin_count) {
    assert(this);
    /* extra */ {
      unique_lock<mutex> lock(Mutex);
      Cond.wait(lock, []{ return GoFlag; });
    }
    for (size_t i = 0; i < spin_count; ++i) {
      TSync::TSharedLock lock(Sync);
    }
    Timer = TSync::TSharedLock::GetTimerSL();
  }

  double Timer;

  thread Thread;

  static TSync Sync;

  static mutex Mutex;

  static condition_variable Cond;

  static bool GoFlag;

};  // TActor

TSync TActor::Sync;

mutex TActor::Mutex;

condition_variable TActor::Cond;

bool TActor::GoFlag = false;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cerr << "usage: " << argv[0] << " <actor_count> <spin_count>" << endl;
    return EXIT_FAILURE;
  }
  size_t
    actor_count = atoi(argv[1]),
    spin_count  = atoi(argv[2]);
  if (actor_count < 1 || actor_count > 100 || spin_count < 0 || spin_count > 10000000) {
    cerr << "error: value(s) out of range" << endl;
    return EXIT_FAILURE;
  }
  vector<TActor *> actors(actor_count);
  for (size_t i = 0; i < actor_count; ++i) {
    actors[i] = new TActor(spin_count);
  }
  TActor::Go();
  double timers[actor_count];
  double total = 0;
  for (size_t i = 0; i < actor_count; ++i) {
    TActor *actor = actors[i];
    double &timer = timers[i];
    actor->Join();
    timer = actor->GetTimer();
    delete actor;
    total += timer;
  }
  double mean = total / static_cast<double>(actor_count);
  double min_timer = timers[0], max_timer = timers[0], dev = 0;
  for (size_t i = 0; i < actor_count; ++i) {
    const double &timer = timers[i];
    min_timer = min(min_timer, timer);
    max_timer = max(max_timer, timer);
    double diff = timer - mean;
    dev += diff * diff;
  }
  dev = sqrt(dev / static_cast<double>(actor_count));
  cout << "min = " << min_timer
       << ", max = " << max_timer
       << ", mean = " << mean
       << ", dev = " << dev
       << ", total = " << total << endl;
  return EXIT_SUCCESS;
}
