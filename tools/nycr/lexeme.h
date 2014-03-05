/* <tools/nycr/lexeme.h>

   A lexeme in a source file.

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
#include <chrono>
#include <cstdint>
#include <ostream>
#include <string>

#include <base/chrono.h>
#include <tools/nycr/escape.h>
#include <tools/nycr/pos_range.h>

namespace Tools {

  namespace Nycr {

    /* A lexeme in a source file. */
    class TLexeme {
      public:

      /* Do-little. */
      TLexeme(int start_line, int start_col, int limit_line, int limit_col, const char *text, int len)
          : PosRange(TPos(start_line, start_col), TPos(limit_line, limit_col)), Text(text, len) {}

      static std::string RemoveCurlyBraces(const std::string &str);

      /* Translate the lexeme into a chrono::time_point */
      Base::Chrono::TTimeDiff AsTimeDiff() const;

      /* Translate the lexeme into a chrono::time_point */
      Base::Chrono::TTimePnt AsTimePnt() const;

      /* Translate the lexeme into a double. Exponent notation is supported. */
      double AsDouble() const;

      /* Translate the lexeme to a native string, treating the lexeme as a double-quoted raw string.  That is, the lexeme begins and ends with
         double-quote (") and contains no double-quotes, back-slashes, or control characters. */
      std::string AsDoubleQuotedRawString() const {
        assert(this);
        return AsQuotedString('"', true);
      }

      /* Translate the lexeme to a native string, treating the lexeme as a double-quoted raw string.  That is, the lexeme begins and ends with
         double-quote (") and contains no control characters, but may contain escape sequences such as \n, \t, \", or \\. */
      std::string AsDoubleQuotedString() const {
        assert(this);
        return AsQuotedString('"', false);
      }

      /* Translate the lexeme into an signed int.  Assumes base-10. */
      int64_t AsInt() const;

      /* Translate the lexeme into a long. */
      long AsLong() const;

      /* Translate the lexeme to a native string, treating the lexeme as a double-quoted raw string.  That is, the lexeme begins and ends with
         single-quote (') and contains no single-quotes, back-slashes, or control characters. */
      std::string AsSingleQuotedRawString() const {
        assert(this);
        return AsQuotedString('\'', true);
      }

      /* Translate the lexeme to a native string, treating the lexeme as a double-quoted raw string.  That is, the lexeme begins and ends with
         single-quote (') and contains no control characters, but may contain escape sequences such as \n, \t, \', or \\. */
      std::string AsSingleQuotedString() const {
        assert(this);
        return AsQuotedString('\'', false);
      }

      /* Translate the lexeme into a uint32, following the base-selection rules common to C.  That is, if it starts with 0x, treat it as hex, if it
         starts with 0, treat it as octal, and otherwise treat it as decimal. */
      uint32_t AsUInt32() const;

      /* The position range covered by the lexeme. */
      const TPosRange &GetPosRange() const {
        assert(this);
        return PosRange;
      }

      /* The text of the lexeme. */
      const std::string &GetText() const {
        assert(this);
        return Text;
      }

      /* Used to dump the tree. */
      void Write(std::ostream &strm) const {
        assert(this);
        assert(&strm);
        strm << PosRange << ' ' << TEscape(Text);
      }

      private:

      /* Helper for translating quoted strings. */
      std::string AsQuotedString(char quote, bool is_raw) const;

      /* See accessor. */
      const TPosRange PosRange;

      /* See accessor. */
      std::string Text;

    };  // TLexeme

    /* Standard inserter for Tools::Nycr::TLexeme. */
    inline std::ostream &operator<<(std::ostream &strm, const Tools::Nycr::TLexeme &that) {
      assert(&that);
      that.Write(strm);
      return strm;
    }

  }  // Nycr

}  // Tools