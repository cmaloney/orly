/* <orly/csv_to_bin/level3.h>

   Level-3 parser for CSV files.

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
#include <utility>

#include <base/chrono.h>
#include <base/opt.h>
#include <base/thrower.h>
#include <base/uuid.h>
#include <orly/csv_to_bin/level2.h>

namespace Orly {

  namespace CsvToBin {

    /* For the benefit of the helper functions below. */
    class TLevel3;

    /* Helper functions you can include in an extraction expression to
       match given states or skip over remaining bytes in a field. */
    void EndOfField(TLevel3 &that);
    void EndOfFile(TLevel3 &that);
    void EndOfRecord(TLevel3 &that);
    void SkipBytes(TLevel3 &that);
    void StartOfField(TLevel3 &that);
    void StartOfFile(TLevel3 &that);
    void StartOfRecord(TLevel3 &that);

    /* Level-3 parser for CSV files. */
    class TLevel3 final {
      NO_COPY(TLevel3);
      public:

      /* Thrown when extracting data from a field and we encounter a
         convention we recognize but don't currently support. */
      DEFINE_ERROR(
          TNotSupported, std::runtime_error,
          "format of field in CSV file is not supported");

      /* Thrown when numeric limits are exceeded, such as a number with too
         many digits. */
      DEFINE_ERROR(
          TNumberOutOfRange, std::runtime_error,
          "number out of range in CSV file");

      /* Thrown when extracting data from a field and we run out of field
         before we run out of the desire to extract things from it. */
      DEFINE_ERROR(
          TPastEnd, std::runtime_error,
          "attempt to read past end of field in CSV file");

      /* Thrown when extracting data from a field and we encounter bad
         level-3 syntax, such as punctuation where a digit should be. */
      DEFINE_ERROR(
          TSyntaxError, std::runtime_error,
          "syntax error in field of CSV file");

      /* Thrown when the level-2 parsing state is not as expected. */
      DEFINE_ERROR(
          TUnexpectedState, std::runtime_error,
          "unexpected state in CSV file");

      /* Options for parsing. */
      struct TOptions final {

        /* The keyword we accept as bool true.  Must be all lower-case. */
        std::string TrueKwd;

        /* The keyword we accept as bool false.  Must be all lower-case. */
        std::string FalseKwd;

      };  // TLevel3::TOptions

      /* The default options use 'true' and 'false'. */
      static const TOptions DefaultOptions;

      /* Use the given level-2 parser. */
      explicit TLevel3(
          TLevel2 &level2, TOptions options = DefaultOptions)
          : Level2(level2), Options(std::move(options)),
            Cursor(nullptr), Limit(nullptr) {}

      /* Extract a value from the current field. */
      TLevel3 &operator>>(bool &that);

      /* Extract a value from the current field. */
      TLevel3 &operator>>(Base::TUuid &that);

      /* Extract a value from the current field. */
      TLevel3 &operator>>(int64_t &that);

      /* Extract a value from the current field. */
      TLevel3 &operator>>(double &that);

      /* Extract a value from the current field. */
      TLevel3 &operator>>(std::string &that);

      /* Extract a value from the current field. */
      TLevel3 &operator>>(Base::Chrono::TTimePnt &that);

      /* Extract an optional value from the current field. */
      template <typename TVal>
      TLevel3 &operator>>(Base::TOpt<TVal> &that) {
        assert(this);
        if (!RefreshBytes(false) && Cursor == Null) {
          that.Reset();
          RefreshBytes(false);
        } else {
          TVal temp;
          *this >> temp;
          that = std::move(temp);
        }
        return *this;
      }

      /* Use the given helper function in an extraction expression. */
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

      /* Like TryMatchByte(), throws on failure. */
      void MatchByte(uint8_t expected);

      /* Like TryMatchByte(), but throws on failure. */
      uint8_t MatchByte(const char *expected);

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

      /* Read base-10 digits from our bytes, stopping at the first non-
         digit. Returns the numeric value of the digits and the number of
         digits.  This doesn't check for a sign at the start, so the value
         will always be positive.  If there is not at least one digit, we
         throw, so size will always be > 0. */
      void ReadDecimalInt(int64_t &value, size_t &size);

      /* Try to get more bytes for the current field.  Return success/failure.
         If required is true, we won't return false; we'll throw instead. */
      bool RefreshBytes(bool required) const;

      /* Match the current byte against an expected byte.  If we succeed,
         return return true; otherwise, return false. */
      bool TryMatchByte(uint8_t expected);

      /* Like try match byte, below, only we don't care what the match is. */
      bool TryMatchByte(const char *expected);

      /* Match the current byte against a string of possible expected bytes.
         If we succeed, return the match via out-param and return true;
         otherwise, leave the out-param alone and return false. */
      bool TryMatchByte(const char *expected, uint8_t &match);

      /* Our level-2 parser. */
      TLevel2 &Level2;

      /* Parsing options, cached at construction time. */
      const TOptions Options;

      /* The bytes we have buffered for the current field. */
      mutable const uint8_t *Cursor, *Limit;

      /* Cursor and Limit point at this taboo when level-2 tells us we're
         at an explicit null. */
      static const uint8_t Null[1];

    };  // TLevel3

  }  // CsvToBin

}  // Orly
