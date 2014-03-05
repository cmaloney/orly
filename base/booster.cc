/* <base/booster.cc>

   Implements <base/booster.h>.

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

#include <algorithm>
#include <cassert>

#include <pthread.h>

#include <base/error_utils.h>
#include <base/zero.h>

using namespace std;
using namespace Base;

TBooster::TIneffective::TIneffective()
    : runtime_error("priority boost ineffective; probably not running with sufficient privilege") {}

TBooster::TBooster(int policy, int priority_backoff) {
  assert(priority_backoff >= 0);
  /* Nab the min/max priority levels for the given policy. */
  int min_priority, max_priority;
  IfLt0(min_priority = sched_get_priority_min(policy));
  IfLt0(max_priority = sched_get_priority_max(policy));
  /* Cache this thread's current policy and priority. */
  auto self = pthread_self();
  IfNe0(pthread_getschedparam(self, &OldPolicy, &OldSchedParam));
  /* Boost to the new policy and priority. */
  sched_param new_sched_param;
  Zero(new_sched_param);
  new_sched_param.sched_priority = max(min_priority, max_priority - priority_backoff);
  IfNe0(pthread_setschedparam(self, policy, &new_sched_param));
  int test_policy;
  sched_param test_sched_param;
  IfNe0(pthread_getschedparam(self, &test_policy, &test_sched_param));
  if (test_policy != policy || test_sched_param.sched_priority != new_sched_param.sched_priority) {
    this->~TBooster();
    throw TIneffective();
  }
}

TBooster::~TBooster() {
  assert(this);
  pthread_setschedparam(pthread_self(), OldPolicy, &OldSchedParam);
}
