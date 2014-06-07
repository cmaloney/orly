/* <starsha/status_line.h>

   Maintains a line of text which states the status

   TODO: Switch to a library someone else has already written for this (linenoise?)

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

#include <iostream>

namespace Starsha {
  /* Returns true if STDOUT is a tty. */
  bool IsRealTty();

  // On construction sets up line, on destruction terminates the line.
  class TStatusLine {
    public:
    static void Cleanup();

    TStatusLine();
    ~TStatusLine();

    // Move only
    TStatusLine(TStatusLine &&) = default;
    TStatusLine(const TStatusLine &) = delete;

    template <typename TVal>
    void Write(const TVal &that) const {
      std::cout << that;
    }
  };

  template <typename TVal>
  const TStatusLine &operator<<(const TStatusLine &out, const TVal &that) {
    out.Write(that);
    return out;
  }
}