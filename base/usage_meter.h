/* <base/usage_meter.h>

   Provide a usage metric.

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

#include <sys/resource.h>
#include <sys/time.h>

#include <base/no_copy_semantics.h>

namespace Base {

  /* TODO */
  class TUsageMeter {
    NO_COPY_SEMANTICS(TUsageMeter);
    public:

    /* who = (RUSAGE_SELF | RUSAGE_CHILDREN | RUSAGE_THREAD) */
    TUsageMeter(int who = RUSAGE_THREAD) : Who(who) {
      getrusage(Who, &StartU);
    }

    /* TODO */
    void Start() {
      getrusage(Who, &StartU);
    }

    /* TODO */
    void Stop() {
      getrusage(Who, &StopU);
    }

    /* TODO */
    double GetTotalUserCPU() const {
      return (((StopU.ru_utime.tv_sec - StartU.ru_utime.tv_sec) * 1000000L) + (StopU.ru_utime.tv_usec - StartU.ru_utime.tv_usec)) / 1000000.0;
    }

    /* TODO */
    double GetTotalSystemCPU() const {
      return (((StopU.ru_stime.tv_sec - StartU.ru_stime.tv_sec) * 1000000L) + (StopU.ru_stime.tv_usec - StartU.ru_stime.tv_usec)) / 1000000.0;
    }

    /* TODO */
    size_t GetVoluntary() const {
      return StopU.ru_nvcsw - StartU.ru_nvcsw;
    }

    size_t GetInvoluntary() const {
      return StopU.ru_nivcsw - StartU.ru_nivcsw;
    }

    private:

    /* TODO */
    int Who;

    /* TODO */
    struct rusage StartU;

    /* TODO */
    struct rusage StopU;

  };  // TUsageMeter

} // Base