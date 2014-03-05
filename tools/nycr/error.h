/* <tools/nycr/error.h>

   An error in a source file.

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
#include <string>

#include <tools/nycr/pos_range.h>

namespace Tools {

  namespace Nycr {

    /* A error in a source file. */
    class TError {
      public:

      /* TODO */
      class TBuilder {
        public:

        /* TODO */
        TBuilder(const TPosRange &pos_range)
            : PosRange(pos_range) {}

        /* TODO */
        ~TBuilder() {
          assert(this);
          new TError(PosRange, Strm.str());
        }

        /* TODO */
        template <typename TVal>
        void Write(const TVal &val) {
          assert(this);
          Strm << val;
        }

        private:

        /* TODO */
        const TPosRange &PosRange;

        /* TODO */
        std::ostringstream Strm;

      };  // TBuilder

      /* Do-little. */
      TError(const TPosRange &pos_range, const std::string &msg)
          : PosRange(pos_range), Msg(msg) {
        Append();
      }

      /* Do-little. */
      TError(int start_line, int start_col, int limit_line, int limit_col, const char *msg)
          : PosRange(TPos(start_line, start_col), TPos(limit_line, limit_col)), Msg(msg) {
        Append();
      }

      /* The position range which gave rise to the error. */
      const TPosRange &GetPosRange() const {
        assert(this);
        return PosRange;
      }

      /* The error message itself. */
      const std::string &GetMsg() const {
        assert(this);
        return Msg;
      }

      /* The next error, in order of creation. */
      const TError *GetNextError() const {
        assert(this);
        return NextError;
      }

      /* The previous error, in order of creation. */
      const TError *GetPrevError() const {
        assert(this);
        return PrevError;
      }

      /* TODO */
      static void DeleteEach();

      /* The first error, in order of creation. */
      static const TError *GetFirstError() {
        return FirstError;
      }

      /* The last error, in order of creation. */
      static const TError *GetLastError() {
        return LastError;
      }

      static void PrintSortedErrors(std::ostream &out);

      private:

      /* Append to end of list of errors.  Used only during construction. */
      void Append() {
        assert(this);
        PrevError = LastError;
        NextError = 0;
        if(PrevError) {
          PrevError->NextError = this;
        } else {
          FirstError = this;
        }
        LastError = this;
      }

      /* See accessor. */
      TPosRange PosRange;

      /* See accessor. */
      std::string Msg;

      /* See accessor. */
      TError *PrevError, *NextError;

      /* See accessors. */
      static TError *FirstError, *LastError;

    };  // TError

    /* TODO */
    template <typename TVal>
    Tools::Nycr::TError::TBuilder &&operator<<(Tools::Nycr::TError::TBuilder &&that, const TVal &val) {
      that.Write(val);
      return std::move(that);
    }

    template <typename TVal>
    Tools::Nycr::TError::TBuilder &operator<<(Tools::Nycr::TError::TBuilder &that, const TVal &val) {
      that.Write(val);
      return that;
    }

  }  // Nycr

}  // Tools