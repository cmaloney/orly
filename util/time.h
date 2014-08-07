/* <util/time.h>

   Various time related helper functions (And timestamp related).

   NOTE: We use nanosecond durations as file timestamps because we have no clue what clock they are tied to.

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

#include <time.h>

#include <chrono>
#include <string>
#include <thread>

#include <base/opt.h>

namespace Util {

  using TTimestamp = std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration>;
  using TOptTimestamp = Base::TOpt<TTimestamp>;

  /* Returns true iff the time point has passed / now is after it. */
  template <typename TClock>
  bool HasPassed(std::chrono::time_point<TClock> time) {
    return time <= TClock::now();
  }

  /* Sleep until the given time is reached. Does a comparison first because that provides a noticable speed boost in
     testing done (a while ago at this point), which I don't want to remove / test at the moment. */
  template <typename TClock>
  void SleepUntil(std::chrono::time_point<TClock> time) {
    if (time < std::chrono::steady_clock::now()) {
      std::this_thread::sleep_until(time);
    }
  }

  /* Convert a duration in nanoseconds into a timespec struct, useful for timerfd calls. */
  timespec ToTimespec(std::chrono::nanoseconds time);

  /* Convert a C timespec struct into a C++ chrono time_point */
  TTimestamp ToTimestamp(timespec time);

  /* Convert a C timespec struct into a C++ chrono time_point for the specified chrono clock type. */
  template <typename TClock>
  std::chrono::time_point<TClock, typename TClock::duration> ToTimestampClock(timespec time) {
    return std::chrono::time_point<TClock, typename TClock::duration>(
        std::chrono::duration_cast<typename TClock::duration>(std::chrono::seconds(time.tv_sec) +
                                                              std::chrono::nanoseconds(time.tv_nsec)));
  }


  /* Tries to get the timestamp for the given file. Returns unknown if the file doesn't exist / stat fails. */
  TOptTimestamp TryGetTimestamp(const std::string &name);

  /* Get the timestamp for the given filename or throw an exception */
  TTimestamp GetTimestamp(const std::string &name);

  /* Get the timestamp for the given filename searching PATH or throw an exception */
  TTimestamp GetTimestampSearchingPath(const std::string &name);

  /* It's either this or making operator overloads all work on Base::TOpt, for which we really don't know what the
     "right" behavior is. */
  bool IsNewer(TTimestamp lhs, TOptTimestamp rhs);

  /* Helper functions for getting combining timestamps to get the newest / oldest. */
  TTimestamp Newest(TTimestamp lhs, TOptTimestamp rhs);
  TOptTimestamp Newest(TOptTimestamp lhs, TOptTimestamp rhs);
  TOptTimestamp Oldest(TOptTimestamp lhs, TOptTimestamp rhs);

  std::string ToStr(const TTimestamp &ts);
}
