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

#include <cassert>
#include <string>

#include <base/chrono.h>
#include <base/thrower.h>
#include <base/uuid.h>
#include <orly/csv_to_bin/level2.h>

namespace Orly {

  namespace CsvToBin {

    /* TODO */
    class TLevel3;

    /* TODO */
    void EndOfField(TLevel3 &that);
    void EndOfFile(TLevel3 &that);
    void EndOfRecord(TLevel3 &that);
    void SkipBytes(TLevel3 &that);
    void StartOfField(TLevel3 &that);
    void StartOfFile(TLevel3 &that);
    void StartOfRecord(TLevel3 &that);

    /* TODO */
    class TLevel3 final {
      NO_COPY(TLevel3);
      public:

      /* TODO */
      DEFINE_ERROR(
          TPastEnd, std::runtime_error,
          "attempt to read past end of field in CSV file");

      /* TODO */
      DEFINE_ERROR(
          TUnexpectedState, std::runtime_error,
          "unexpected state in CSV file");

      /* TODO */
      DEFINE_ERROR(
          TSyntaxError, std::runtime_error,
          "syntax error in field of CSV file");

      /* TODO */
      explicit TLevel3(TLevel2 &level2)
          : Level2(level2), TrueKwd("true"), FalseKwd("false"),
            Cursor(nullptr), Limit(nullptr) {}

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
      TLevel3 &operator>>(void (*helper)(TLevel3 &)) {
        assert(this);
        assert(helper);
        helper(*this);
        return *this;
      }

      /* Be friendly with our helpers. */
      friend void EndOfField(TLevel3 &that);
      friend void EndOfFile(TLevel3 &that);
      friend void EndOfRecord(TLevel3 &that);
      friend void SkipBytes(TLevel3 &that);
      friend void StartOfField(TLevel3 &that);
      friend void StartOfFile(TLevel3 &that);
      friend void StartOfRecord(TLevel3 &that);

      private:

      /* TODO */
      bool CanPeek() const {
        assert(this);
        return RefreshBytes(false);
      }

      /* TODO */
      void MatchState(TLevel2::TState state);

      /* TODO */
      uint8_t Peek() const {
        assert(this);
        RefreshBytes(true);
        return *Cursor;
      }

      /* TODO */
      uint8_t Pop() {
        assert(this);
        RefreshBytes(true);
        return *Cursor++;
      }

      /* TODO */
      bool RefreshBytes(bool required) const;

      /* TODO */
      TLevel2 &Level2;

      /* TODO */
      const char *const TrueKwd, *const FalseKwd;

      /* TODO */
      mutable const uint8_t *Cursor, *Limit;

    };  // TLevel3

  }  // CsvToBin

}  // Orly
