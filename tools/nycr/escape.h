/* <tools/nycr/escape.h>

   An escaper for pretty-printing CSTs.

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
#include <string>

namespace Tools {

  namespace Nycr {

    /* TODO */
    class TEscape {
      public:

      /* TODO */
      enum TEscapeStyle { CStyle, XmlStyle };

      /* TODO */
      TEscape(const std::string &text, TEscapeStyle escape_style = CStyle)
          : Text(text), EscapeStyle(escape_style) {}

      /* TODO  */
      void Write(std::ostream &strm) const;

      private:

      /* TODO  */
      const std::string &Text;

      /* TODO  */
      const TEscapeStyle EscapeStyle;

    };  // TEscape

    /* TODO */
    inline std::ostream &operator<<(std::ostream &strm, const Tools::Nycr::TEscape &that) {
      assert(&that);
      that.Write(strm);
      return strm;
    }

  }  // Nycr

}  // Tools
