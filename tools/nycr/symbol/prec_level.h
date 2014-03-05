/* <tools/nycr/symbol/prec_level.h>

   A precedence level, used for describing the precedence of operators.

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

#include <cassert>
#include <cstddef>
#include <vector>

#include <base/no_copy_semantics.h>
#include <tools/nycr/symbol/name.h>

namespace Tools {

  namespace Nycr {

    namespace Symbol {

      /* TODO */
      class TPrecLevel {
        NO_COPY_SEMANTICS(TPrecLevel);
        public:

        /* TODO */
        typedef std::vector<TPrecLevel *> TPrecLevels;

        /* TODO */
        TPrecLevel(const TName &name, size_t idx);

        /* TODO */
        virtual ~TPrecLevel();

        /* TODO */
        size_t GetIdx() const {
          assert(this);
          return Idx;
        }

        /* TODO */
        const TName &GetName() const {
          assert(this);
          return Name;
        }

        /* TODO */
        static const TPrecLevels &GetPrecLevels() {
          return PrecLevels;
        }

        private:

        /* TODO */
        TName Name;

        /* TODO */
        size_t Idx;

        /* TODO */
        static TPrecLevels PrecLevels;

      };  // TPrecLevel

    }  // Symbol

  }  // Nycr

}  // Tools
