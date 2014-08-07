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

#include <base/opt.h>

namespace Util {

  using TTimestamp = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;
  using TOptTimestamp = Base::TOpt<TTimestamp>;

  timespec ToTimespec(std::chrono::nanoseconds time);

  TTimestamp ToTimestamp(timespec time);

  /* Tries to get the timestamp for the given file. Returns unknown if the file doesn't exist / stat fails. */
  TOptTimestamp TryGetTimestamp(const std::string &name);

  TTimestamp GetTimestamp(const std::string &name);

  TTimestamp GetTimestampSearchingPath(const std::string &name);

  /* It's either this or making operator overloads all work on Base::TOpt, for which we really don't know what the "right" behavior is. */
  bool IsNewer(TTimestamp lhs, TOptTimestamp rhs);

  TTimestamp Newer(TTimestamp lhs, TOptTimestamp rhs);

  TOptTimestamp Newer(TOptTimestamp lhs, TOptTimestamp rhs);

  TOptTimestamp Older(TOptTimestamp lhs, TOptTimestamp rhs);

  std::string ToStr(const TTimestamp &ts);
}
