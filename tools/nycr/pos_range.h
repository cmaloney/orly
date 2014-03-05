/* <tools/nycr/pos_range.h>

   A range of positions in a source file.

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
#include <sstream>

#include <tools/nycr/pos.h>

namespace Tools {

  namespace Nycr {

    /* A range of positions in a source file. */
    class TPosRange {
      public:

      /* Default to the beginning of the file */
      TPosRange() : Start(1, 1), Limit(1, 1) {}

      /* Cache the given values. */
      TPosRange(const TPos &start, const TPos &limit)
          : Start(start), Limit(limit) {
        assert(Start <= Limit);
      }

      /* Construct a TPosRange whose beginning is 'start'
         and whose limit is 'end'. */
      TPosRange(const TPosRange &start, const TPosRange &end)
          : Start(start.GetStart()), Limit(end.GetLimit()) {
        assert(Start <= Limit);
      }

      /* Equal iff. start and limit are equal. */
      bool operator==(const TPosRange &that) const {
        assert(this);
        assert(&that);
        return Start == that.Start && Limit == that.Limit;
      }

      /* Unequal iff. start or limit are unequal. */
      bool operator!=(const TPosRange &that) const {
        assert(this);
        assert(&that);
        return Start != that.Start || Limit != that.Limit;
      }

      //NOTE: Use the streamer if you can.
      std::string AsStr() const {
        assert(this);
        std::ostringstream out;
        Write(out);
        return out.str();
      }

      /* The limiting position of the range. */
      const TPos &GetLimit() const {
        assert(this);
        return Limit;
      }

      /* The starting position of the range. */
      const TPos &GetStart() const {
        assert(this);
        return Start;
      }

      /* Writes the range as line1:col1-line2:col2. */
      void Write(std::ostream &strm) const {
        assert(this);
        assert(&strm);
        strm << Start << '-' << Limit;
      }

      private:

      /* See accessors. */
      TPos Start, Limit;

    };  // TPosRange

    /* Standard inserter for Tools::Nycr::TPosRange. */
    inline std::ostream &operator<<(std::ostream &strm, const Tools::Nycr::TPosRange &that) {
      assert(&that);
      that.Write(strm);
      return strm;
    }

  }  // Nycr

}  // Tools