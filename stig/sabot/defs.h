/* <stig/sabot/defs.h>

   Basic types used in sabots.

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

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

#include <base/opt.h>
#include <base/uuid.h>

namespace Stig {

  namespace Sabot {

    /* Types used for time. */
    using TStdDuration = std::chrono::nanoseconds;
    using TStdTimePoint = std::chrono::time_point<std::chrono::system_clock, TStdDuration>;

    /* Thrown by the subscripting functions when the requested index is larger than the number of elements. */
    class TIdxTooBig
        : public std::runtime_error {
      public:

      /* Do-little. */
      TIdxTooBig();

    };  // TIdxTooBig

    /* Thrown when we reach a supposedly unreachable state.  Note that this is a program logic error. */
    class TUnreachable
        : public std::logic_error {
      public:

      /* Do-little. */
      TUnreachable();

    };  // TUnreachable

    /* Thrown when we reach a state that has not (yet?) been implemented by the development team. */
    class TNotImplemented
        : public std::runtime_error {
      public:

      /* Do-little. */
      TNotImplemented();

    };  // TNotImplemented

  }  // Sabot

}  // Stig
