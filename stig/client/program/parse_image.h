/* <stig/client/program/parse_image.h>

   TODO

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

#include <functional>

#include <stig/client/program/program.program.cst.h>
#include <stig/sabot/state.h>

namespace Stig {

  namespace Client {

    namespace Program {

      /* TODO */
      using TForXact = std::function<bool (const TXact *)>;

      /* TODO */
      using TForKv = std::function<bool (const Sabot::State::TAny *lhs, const Sabot::State::TAny *rhs)>;

      /* TODO */
      bool ParseImageFile(const char *path, const TForXact &cb);

      /* TODO */
      bool ParseImageStr(const char *str, const TForXact &cb);

      /* TODO */
      bool TranslateXact(const TXact *xact, const TForKv &cb);

    }  // Program

  }  // Client

}  // Stig
