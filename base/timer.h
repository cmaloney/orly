/* <base/timer.h>

   Provide a timer to allow efficiency analysis of code.


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

#pragma once

#include <time.h>

#include <base/no_copy_semantics.h>

namespace Base {

  /* TODO */
  class TTimer {
    NO_COPY_SEMANTICS(TTimer);
    public:

    /* TODO */
    TTimer(clockid_t type = CLOCK_REALTIME) : LapTime(0L), TotalTime(0L), Type(type) {
      clock_gettime(Type, &StartTime);
    }

    /* TODO */
    void Start() {
      clock_gettime(Type, &StartTime);
    }

    /* TODO */
    void Stop() {
      clock_gettime(Type, &Now);
      LapTime = ((Now.tv_sec - StartTime.tv_sec) * 1000000000L) + (Now.tv_nsec - StartTime.tv_nsec);
      StartTime = Now;
      TotalTime += LapTime;
    }

    /* TODO */
    double Elapsed() const {
      clock_gettime(Type, &Now);
      return (((Now.tv_sec - StartTime.tv_sec) * 1000000000L) + (Now.tv_nsec - StartTime.tv_nsec)) / 1000000000.0;
    }

    /* TODO */
    double Lap() const {
      return LapTime / 1000000000.0;
    }

    /* TODO */
    double Total() const {
      return TotalTime / 1000000000.0;
    }

    /* TODO */
    void Reset() {
      LapTime = 0L;
      TotalTime = 0L;
    }

    private:

    /* TODO */
    timespec StartTime;

    /* TODO */
    mutable timespec Now;

    /* TODO */
    long LapTime;

    /* TODO */
    long TotalTime;

    /* TODO */
    clockid_t Type;
  };

  /* TODO */
  class TCPUTimer : public TTimer {
    NO_COPY_SEMANTICS(TCPUTimer);
    public:

    /* TODO */
    TCPUTimer() : TTimer(CLOCK_THREAD_CPUTIME_ID) {}

  };

  /* TODO */
  struct timespec TsAdd(const struct timespec &lhs, const struct timespec &rhs);

  /* TODO */
  struct timespec TsAdd(const struct timespec &lhs, size_t msec);

  /* The amount of time left in milliseconds before val; returns 0 if val has already passed */
  size_t TsTimeLeftMsec(const struct timespec &val);

  /* TODO */
  int TsCompare(const struct timespec &lhs, const struct timespec &rhs);
}