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

      /* True at StartOfField, false at EndOfRecord, and throws for all other
         states.  Use this in the predicate of a loop to tell when there's
         another field to consume for the current record.  This doesn't
         consume the StartOfField state automatically. */
      bool AtField() const;

      /* True at StartOfRecord, false at EndOfFile, and throws for all other
         states.  Use this in the predicate of a loop to tell when there's
         another record to consume.  This doesn't consume the StartOfRecord
         state automatically. */
      bool AtRecord() const;

      /* Our current level-2 parser state.  See TLevel2 for a description of
         these states.  Use this to peek where you are in a file when you're
         not sure of the layout. */
      TLevel2::TState GetState() const {
        assert(this);
        /* Make sure we're not clinging to the end of a field of bytes. */
        if (Level2->State == TLevel2::Bytes) {
          RefreshBytes(false);
        }
        return Level2->State;
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

      /* True when we have at least one more byte to peek. */
      bool CanPeek() const {
        assert(this);
        return RefreshBytes(false);
      }

      /* Matches current bytes against the given keyword or throws. */
      void MatchKeyword(const char *keyword);

      /* Matches the given state or throws. */
      void MatchState(TLevel2::TState state);

      /* Our current byte.  Throws if we're out of bytes. */
      uint8_t Peek() const {
        assert(this);
        RefreshBytes(true);
        return *Cursor;
      }

      /* Our current byte and advances to the next byte.  Throws if we're out
         of bytes. */
      uint8_t Pop() {
        assert(this);
        RefreshBytes(true);
        return *Cursor++;
      }

      /* Try to get more bytes for the current field.  Return success/failure.
         If required is true, we won't return false; we'll throw instead. */
      bool RefreshBytes(bool required) const;

      /* Our level-2 parser. */
      TLevel2 &Level2;

      /* The keywords we recognize, all lower-case. */
      const char *const TrueKwd, *const FalseKwd;

      /* The bytes we have buffered for the current field. */
      mutable const uint8_t *Cursor, *Limit;

    };  // TLevel3

  }  // CsvToBin

}  // Orly
