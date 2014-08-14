/* <base/cpu_clock.cc>

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <base/cpu_clock.h>

#include <time.h>

#include <util/error.h>
#include <util/time.h>

using namespace Util;

Base::cpu_clock::time_point Base::cpu_clock::now() noexcept {
  timespec ts;
  //NOTE: The IfLt0 is just a safety precaution. Could be removed for perf
  //      None of the error conditions should apply here
  IfLt0(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts));
  return ToTimestampClock<Base::thread_clock>(ts);
}

Base::thread_clock::thread_clock(pthread_t thread_id) {
  //NOTE: The IfLt0 is just a safety precaution. Could be removed for perf
  //      None of the error conditions should apply here
  IfLt0(pthread_getcpuclockid(thread_id, &ClockId));
}

Base::thread_clock::time_point Base::thread_clock::now() {
  timespec ts;

  IfLt0(clock_gettime(ClockId, &ts));
  return ToTimestampClock<Base::thread_clock>(ts);
}