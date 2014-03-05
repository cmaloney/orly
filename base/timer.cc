/* <base/timer.cc>

   Implements <base/timer.h>.

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

#include <base/timer.h>

using namespace Base;

struct timespec Base::TsAdd(const struct timespec &lhs, const struct timespec &rhs) {
  struct timespec ret;
  ret.tv_sec = lhs.tv_sec + rhs.tv_sec;
  ret.tv_nsec = lhs.tv_nsec + rhs.tv_nsec;
  if (ret.tv_nsec >= 1000000000L) {
    ++ret.tv_sec;
    ret.tv_nsec-= 1000000000L;
  }
  return ret;
}

struct timespec Base::TsAdd(const struct timespec &lhs, size_t msec) {
  struct timespec timeout;
  timeout.tv_sec = msec / 1000;
  timeout.tv_nsec = (msec % 1000) * 1000000;
  return TsAdd(lhs, timeout);
}

size_t Base::TsTimeLeftMsec(const struct timespec &val) {
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  if (val.tv_sec < t.tv_sec || (val.tv_sec == t.tv_sec && val.tv_nsec <= t.tv_nsec)) {
    return 0;
  }
  size_t ret = (val.tv_sec - t.tv_sec) * 1000 + (val.tv_nsec - t.tv_nsec) / 1000000 + 1;
  return ret;
}

int Base::TsCompare(const struct timespec &lhs, const struct timespec &rhs) {
  if (lhs.tv_sec < rhs.tv_sec) {
    return -1;
  } else if (lhs.tv_sec > rhs.tv_sec) {
    return 1;
  } else if (lhs.tv_nsec < rhs.tv_nsec) {
    return -1;
  } else if (lhs.tv_nsec > rhs.tv_nsec) {
    return 1;
  }
  return 0;
}
