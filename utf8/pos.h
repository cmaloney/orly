/* <utf8/pos.h>

   A line/col position in a UTF-8 piece.

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
#include <ostream>

namespace Utf8 {

  /* A line/col position in a UTF-8 piece. */
  class TPos {
    public:

    /* Default position is line 1, col 1. */
    TPos()
        : Line(1), Col(1) {}

    /* Cache the line/col, neither of which may be zero. */
    TPos(size_t line, size_t col)
        : Line(line), Col(col) {
      assert(line > 0);
      assert(col > 0);
    }

    /* The column number.  Never zero. */
    size_t GetCol() const {
      assert(this);
      return Col;
    }

    /* The row number.  Never zero. */
    size_t GetLine() const {
      assert(this);
      return Line;
    }

    private:

    /* See accessors */
    size_t Line, Col;

  };  // TPos

  /* A std stream inserter for Utf8::TPiece. */
  inline std::ostream &operator<<(std::ostream &strm, const TPos &that) {
    assert(&strm);
    assert(&that);
    return strm << that.GetLine() << ':' << that.GetCol();
  }

}  // Utf8
