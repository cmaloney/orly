/* <tools/nycr/indent.h>

   An indenter for pretty-printing CSTs.

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
#include <ostream>

namespace Tools {

  namespace Nycr {

    /* TODO */
    class TIndent {
      public:

      /* TODO */
      TIndent(size_t depth)
          : Depth(depth) {}

      /* TODO  */
      void Write(std::ostream &strm) const;

      private:

      /* TODO  */
      size_t Depth;

    };  // TIndent

    /* TODO */
    inline std::ostream &operator<<(std::ostream &strm, const Tools::Nycr::TIndent &that) {
      assert(&that);
      that.Write(strm);
      return strm;
    }

  }  // Nycr

}  // Tools