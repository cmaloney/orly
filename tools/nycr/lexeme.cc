/* <tools/nycr/lexeme.cc>

   Implements <tools/nycr/lexeme.h>.

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

#include <tools/nycr/lexeme.h>

#include <cctype>
#include <cstdlib>
#include <sstream>

#include <base/convert.h>
#include <base/no_default_case.h>
#include <base/piece.h>
#include <base/syntax_error.h>
#include <base/thrower.h>

using namespace std;
using namespace Base;
using namespace Tools::Nycr;

string TLexeme::RemoveCurlyBraces(const string &str) {
  if (str.front() != '{') {
    throw TSyntaxError(HERE, "time_literals must start with '{'");
  }
  if (str.back() != '}') {
    throw TSyntaxError(HERE, "time_literals must end with '}'");
  }
  string result(str.begin() + 1, str.end() - 1);
  return result;
}

Chrono::TTimeDiff TLexeme::AsTimeDiff() const {
  assert(this);
  return Chrono::TTimeDiffInfo(RemoveCurlyBraces(Text)).AsTimeDiff();
}

Chrono::TTimePnt TLexeme::AsTimePnt() const {
  assert(this);
  return Chrono::TTimePntInfo(RemoveCurlyBraces(Text)).AsTimePnt();
}

double TLexeme::AsDouble() const {
  assert(this);
  const char *start = Text.c_str();
  char *end;
  errno = 0;
  double result = strtod(start, &end);
  if (end != start + Text.size()) {
    THROW << "real number contains illegal characters";
  }
  if (errno) {
    THROW << "real number is out of range";
  }
  return result;
}

int64_t TLexeme::AsInt() const {
  assert(this);
  return TConvertProxy(AsPiece(Text.c_str()));
}

long TLexeme::AsLong() const {
  assert(this);
  return TConvertProxy(AsPiece(Text.c_str()));
}

uint32_t TLexeme::AsUInt32() const {
  assert(this);
  return TConvertProxy(AsPiece(Text.c_str()));
}

string TLexeme::AsQuotedString(char quote, bool is_raw) const {
  assert(this);
  string result;
  if (Text.size() < (is_raw ? 3 : 2)) {
    THROW << "quoted string too short";
  }
  const char *cursor = &Text[0], *limit = cursor + Text.size() - 1;
  if (is_raw) {
    if (*cursor != 'r' && *cursor != 'R') {
      THROW << "raw quoted string incorrectly delimited";
    }
    ++cursor;
  }
  if (*cursor != quote || *limit != quote) {
    THROW << "quoted string incorrectly delimited";
  }
  ++cursor;
  enum state_t {
    normal, escaping, picking_hex_1, picking_hex_2
  } state = normal;
  char hex_buf[3];
  hex_buf[2] = '\0';
  for (; cursor < limit; cursor++) {
    char c = *cursor;
    bool keep_char = false;
    switch (state) {
      case normal: {
        if (iscntrl(c)) {
          THROW << "quoted string contains control character";
        }
        if (!is_raw && c == '\\') {
          state = escaping;
        } else {
          keep_char = true;
        }
        break;
      }
      case escaping: {
        switch (c) {
          case '\\': {
            c = '\\';
            break;
          }
          case 'n': {
            c = '\n';
            break;
          }
          case 'r': {
            c = '\r';
            break;
          }
          case 't': {
            c = '\t';
            break;
          }
          case 'x': {
            state = picking_hex_1;
            break;
          }
          default: {
            if (c != quote) {
              THROW << "quoted string contains bad escape sequence";
            }
          }
        }
        if (state == escaping) {
          keep_char = true;
          state = normal;
        }
        break;
      }
      case picking_hex_1: {
        hex_buf[0] = c;
        state = picking_hex_2;
        break;
      }
      case picking_hex_2: {
        hex_buf[1] = c;
        char *end;
        c = strtoul(cursor, &end, 16);
        if (end != hex_buf + 2) {
          THROW << "quoted string contains bad hex escape sequence";
        }
        keep_char = true;
        state = normal;
        break;
      }
      NO_DEFAULT_CASE;
    }
    if (keep_char) {
      result += c;
    }
  }
  if (state != normal) {
    THROW << "quoted string ends with incomplete escape sequence";
  }
  return result;
}
