/* <base/timer.h>

   Provide a timer to allow efficiency analysis of code.

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

#pragma once

#include <base/class_traits.h>
#include <base/cpu_clock.h>

namespace Base {

  /* Simple timer class which wraps a std::chrono clock.

     NOTE: Timers implicitly start on construction. */
  template <typename TClock>
  class TGenericTimer {
    NO_COPY(TGenericTimer);
    public:

    using duration = typename TClock::duration;
    using time_point = typename TClock::time_point;

    TGenericTimer() { Start(); }

    /* Updates the start time, resetting lap and total time. */
    void Start() {
      StartTime = TClock::now();
      Total = duration::zero();
      Lap = duration::zero();
    }

    /* Completes a lap, updating the total time and lap time.
       NOTE: Also updates the starttime to be now. */
    void Stop() {
      auto now = TClock::now();
      Lap = now - StartTime;
      StartTime = now;
      Total += Lap;
    }

    /* Returns the time elapsed since the start */
    duration GetElapsed() const {
      return TClock::now() - StartTime;
    }

    /* Returns the time between the last two stop calls. */
    duration GetLap() const {
      return Lap;
    }

    //TODO: Should probably be a helper converter
    double GetLapSeconds() const {
      return std::chrono::duration_cast<std::chrono::duration<double>>(Lap).count();
    }

    /* Returns the total time elapsed across all Stop() calls. */
    duration GetTotal() const {
      return Total;
    }

    //TODO: Should probably be a helper converter
    double GetTotalSeconds() const {
      return std::chrono::duration_cast<std::chrono::duration<double>>(Total).count();
    }

    private:
    time_point StartTime;
    duration Lap;
    duration Total;
  };

  //TODO:  this should actually be a monotonic clock. For now keeping the old behavior / semantics.
  /* Easy-access generic timer. */
  using TTimer = TGenericTimer<std::chrono::system_clock>;

  /* Easy-access timer which gives the cpu time for the given thread. */
  using TCPUTimer = TGenericTimer<Base::cpu_clock>;
}