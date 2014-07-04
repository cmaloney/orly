/* <orly/csv_to_bin/level3.h>

   TODO

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

#include <string>

#include <base/chrono.h>
#include <base/thrower.h>
#include <base/uuid.h>
#include <orly/csv_to_bin/level2.h>

namespace Orly {

  namespace CsvToBin {

    /* TODO */
    class TLevel3 final {
      NO_COPY(TLevel3);
      public:

      /* TODO */
      DEFINE_ERROR(TUnexpectedState, std::runtime_error, "unexpected state");

      /* TODO */
      explicit TLevel3(TLevel2 &level2)
          : Level2(level2) {}

      /* TODO */
      TLevel3 &operator>>(bool &that);

      /* TODO */
      TLevel3 &operator>>(Base::TUuid &that);

      /* TODO */
      TLevel3 &operator>>(int64_t &that);

      /* TODO */
      TLevel3 &operator>>(double &that);

      /* TODO */
      TLevel3 &operator>>(std::string &that);

      /* TODO */
      TLevel3 &operator>>(Base::Chrono::TTimePnt &that);

      /* TODO */
      void MatchState(TLevel2::TState state);

      private:

      /* TODO */
      TLevel2 &Level2;

      /* TODO */
      const uint8_t *Cursor, *Limit;

    };  // TLevel3

  }  // CsvToBin

}  // Orly
