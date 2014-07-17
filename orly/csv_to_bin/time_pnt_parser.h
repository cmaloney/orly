/* <orly/csv_to_bin/time_pnt_parser.h>

   A parser dedicated to the TTimePnt type.  This should really be integrated
   into our general stream-handling, but this is a stop-gap.

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
#include <cstdint>

#include <base/chrono.h>
#include <base/class_traits.h>
#include <base/thrower.h>

namespace Orly {

  namespace CsvToBin {

    /* A parser dedicated to the TTimePnt type. */
    class TTimePntParser {
      NO_COPY(TTimePntParser);
      public:

      /* Borrow the type from Base. */
      using TTimePnt = Base::Chrono::TTimePnt;

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

      /* Do-little. */
      virtual ~TTimePntParser() {}

      /* Parse a time point. */
      void Parse(TTimePnt &that);

      protected:

      /* Do-little. */
      TTimePntParser()
          : Cursor(nullptr), Limit(nullptr) {}

      /* Called to acquire the next chunk of data from whatever our
         underlying source is.  When we're done with it, we will call
         ReleaseData(), below. */
      virtual void AcquireData(
          const uint8_t *&cursor, const uint8_t *&limit) = 0;

      /* Called when we're done with a chunk of data previously returned by
         AcquireData(), above. */
      virtual void ReleaseData() = 0;

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

      /* The bytes we have buffered for the current field. */
      mutable const uint8_t *Cursor, *Limit;

    };  // TTimePntParser

  }  // CsvToBin

}  // Orly
