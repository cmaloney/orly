/* <orly/csv_to_bin/time_pnt_parser.cc>

   Implements <orly/csv_to_bin/time_pnt_parser.h>.

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

#include <orly/csv_to_bin/time_pnt_parser.h>

using namespace std;
using namespace Orly::CsvToBin;

void TTimePntParser::Parse(TTimePnt &that) {
  assert(this);
  assert(&that);
  /* Parse something like 2014-07-04. */
  int64_t year;
  size_t size;
  ReadDecimalInt(year, size);
  uint8_t match = MatchByte("-/");
  if (TryMatchByte("Ww")) {
    THROW_ERROR(TNotSupported) << "week number in time point";
  }
  int64_t month;
  ReadDecimalInt(month, size);
  if (!CanPeek()) {
    THROW_ERROR(TNotSupported) << "day-of-year number in time point";
  }
  MatchByte(match);
  int64_t day;
  ReadDecimalInt(day, size);
  /* Parse something like T15:38:20.14. */
  int64_t hour = 0, min = 0, sec = 0, nano = 0;
  if (CanPeek()) {
    MatchByte(" Tt/");
    ReadDecimalInt(hour, size);
    match = MatchByte(":/");
    if (CanPeek()) {
      ReadDecimalInt(min, size);
      MatchByte(match);
      if (CanPeek()) {
        ReadDecimalInt(sec, size);
        if (CanPeek() && Peek() == '.') {
          Pop();
          if (CanPeek() && isdigit(Peek())) {
            ReadDecimalInt(nano, size);
            if (size >= 10) {
              THROW_ERROR(TNumberOutOfRange)
                  << "too many decimals in time point";
            }
            size = 9 - size;
            for (; size > 0; --size) {
              nano *= 10;
            }
          }
        }
      }
    }
  }
  int64_t offset = 0;
  if (CanPeek() && TryMatchByte("Zz+-", match)) {
    int64_t sign;
    switch (match) {
      case '+': {
        sign = 1;
        break;
      }
      case '-': {
        sign = -1;
        break;
      }
      default: {
        sign = 0;
      }
    }
    if (sign) {
      int64_t hi = 0, lo = 0, temp;
      ReadDecimalInt(temp, size);
      switch (size) {
        case 2: {
          hi = temp;
          MatchByte(':');
          ReadDecimalInt(lo, size);
          if (size != 2) {
          THROW_ERROR(TSyntaxError)
              << "time zone minutes must be a 2-digit number";
          }
          break;
        }
        case 4: {
          hi = temp / 100,
          lo = temp % 100;
          break;
        }
        default: {
          THROW_ERROR(TSyntaxError)
              << "time zone must be 4-digit number or "
                 "a colon-separated pair of 2-digit numbers";
        }
      }
      offset = sign * (hi * 60 + lo);
    }
  }
  that = Base::Chrono::CreateTimePnt(
      year, month, day, hour, min, sec, nano, offset);
}

void TTimePntParser::MatchByte(uint8_t expected) {
  assert(this);
  if (!TryMatchByte(expected)) {
    THROW_ERROR(TSyntaxError)
        << "expected '" << expected
        << "', found '" << static_cast<char>(Peek()) << '\'';
  }
}

uint8_t TTimePntParser::MatchByte(const char *expected) {
  assert(this);
  uint8_t match;
  if (!TryMatchByte(expected, match)) {
    THROW_ERROR(TSyntaxError)
        << "expected any of '" << expected
        << "', found '" << static_cast<char>(Peek()) << '\'';
  }
  return match;
}

void TTimePntParser::MatchKeyword(const char *keyword) {
  assert(this);
  assert(keyword);
  for (const char *csr = keyword; *csr; ++csr) {
    if (tolower(Pop()) != *csr) {
      THROW_ERROR(TSyntaxError)
          << "expected keyword \"" << keyword << '"';
    }
  }
}

void TTimePntParser::ReadDecimalInt(int64_t &value, size_t &size) {
  assert(this);
  assert(&value);
  assert(&size);
  size = 0;
  __int128 result = 0;
  uint8_t c;
  while (CanPeek() && isdigit(c = Peek())) {
    result = (result * 10) + c - '0';
    if (result > numeric_limits<int64_t>::max()) {
      THROW_ERROR(TNumberOutOfRange) << "base-10 number is too big";
    }
    Pop();
    ++size;
  }
  if (!size) {
    if (CanPeek()) {
      THROW_ERROR(TSyntaxError)
          << "expected base-10 number, found '"
          << static_cast<char>(Peek()) << '\'';
    } else {
      THROW_ERROR(TSyntaxError)
          << "expected base-10 number, found end of field";
    }
  }
  value = result;
}

bool TTimePntParser::RefreshBytes(bool required) const {
  assert(this);
  bool success = (Cursor < Limit);
  if (!success) {
    auto mutable_this = const_cast<TTimePntParser *>(this);
    if (Cursor) {
      mutable_this->ReleaseData();
      Cursor = nullptr;
      Limit = nullptr;
    }
    mutable_this->AcquireData(Cursor, Limit);
    assert(Cursor <= Limit);
    success = (Cursor < Limit);
    if (!success && required) {
      THROW_ERROR(TPastEnd);
    }
  }
  return success;
}

bool TTimePntParser::TryMatchByte(uint8_t expected) {
  assert(this);
  bool success = (Peek() == expected);
  if (success) {
    Pop();
  }
  return success;
}

bool TTimePntParser::TryMatchByte(const char *expected) {
  assert(this);
  uint8_t dummy;
  return TryMatchByte(expected, dummy);
}

bool TTimePntParser::TryMatchByte(const char *expected, uint8_t &match) {
  assert(this);
  assert(&match);
  uint8_t c = Peek();
  bool success = (strchr(expected, c) != nullptr);
  if (success) {
    match = c;
    Pop();
  }
  return success;
}
