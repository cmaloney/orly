/* <base/timer_fd.cc>

   Implements <base/timer_fd.h>.

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

#include <base/timer_fd.h>

#include <algorithm>

#include <util/error.h>

using namespace std;
using namespace std::chrono;
using namespace Base;
using namespace Util;

TTimerFd::TTimerFd(milliseconds ms)
    : Fd(timerfd_create(CLOCK_MONOTONIC, 0)) {
  //seconds = max(seconds, static_cast<uint32_t>(1));
  auto s = duration_cast<seconds>(ms);
  auto ns = duration_cast<nanoseconds>(ms - s);
  itimerspec its = {{s.count(), ns.count()}, {s.count(), ns.count()}};
  IfLt0(timerfd_settime(Fd, 0, &its, nullptr));
}

uint64_t TTimerFd::Pop() {
  assert(this);
  uint64_t count;
  IfLt0(read(Fd, &count, sizeof(count)));
  return count;
}
