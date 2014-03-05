/* <tools/nycr/pos.h>

   A position in a source file.

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

    /* A position in a source file. */
    class TPos {
      public:

      /* Cache the given values. */
      TPos(int line_number, int column_number)
          : LineNumber(line_number), ColumnNumber(column_number) {
        assert(line_number > 0);
        assert(column_number > 0);
      }

      /* See Compare(). */
      bool operator==(const TPos &that) const {
        assert(this);
        return Compare(that) == 0;
      }

      /* See Compare(). */
      bool operator!=(const TPos &that) const {
        assert(this);
        return Compare(that) != 0;
      }

      /* See Compare(). */
      bool operator<(const TPos &that) const {
        assert(this);
        return Compare(that) < 0;
      }

      /* See Compare(). */
      bool operator<=(const TPos &that) const {
        assert(this);
        return Compare(that) <= 0;
      }

      /* See Compare(). */
      bool operator>(const TPos &that) const {
        assert(this);
        return Compare(that) > 0;
      }

      /* See Compare(). */
      bool operator>=(const TPos &that) const {
        assert(this);
        return Compare(that) >= 0;
      }

      /* Compares by line, then by column. */
      int Compare(const TPos &that) const {
        assert(this);
        assert(&that);
        int result = LineNumber - that.LineNumber;
        if (!result) {
          result = ColumnNumber - that.ColumnNumber;
        }
        return result;
      }

      /* The 1-based column-number of this position. */
      int GetColumnNumber() const {
        assert(this);
        return ColumnNumber;
      }

      /* The 1-based line-number of this position. */
      int GetLineNumber() const {
        assert(this);
        return LineNumber;
      }

      /* Writes the position as line:col. */
      void Write(std::ostream &strm) const {
        assert(this);
        assert(&strm);
        strm << LineNumber << ':' << ColumnNumber;
      }

      private:

      /* See accessors. */
      int LineNumber, ColumnNumber;

    };  // TPos

    /* Standard inserter for Tools::Nycr::TPos. */
    inline std::ostream &operator<<(std::ostream &strm, const Tools::Nycr::TPos &that) {
      assert(&that);
      that.Write(strm);
      return strm;
    }

  }  // Nycr

}  // Tools