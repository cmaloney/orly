/* <orly/csv_to_bin/level3.cc>

   Implements <orly/csv_to_bin/level3.h>.

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

#include <orly/csv_to_bin/level3.h>

#include <limits>

using namespace std;
using namespace Orly::CsvToBin;

void Orly::CsvToBin::EndOfField(TLevel3 &that) {
  assert(&that);
  that.MatchState(TLevel2::EndOfField);
}

void Orly::CsvToBin::EndOfFile(TLevel3 &that) {
  assert(&that);
  that.MatchState(TLevel2::EndOfFile);
}
void Orly::CsvToBin::EndOfRecord(TLevel3 &that) {
  assert(&that);
  that.MatchState(TLevel2::EndOfRecord);
}

void Orly::CsvToBin::SkipBytes(TLevel3 &that) {
  assert(&that);
  while (that.RefreshBytes(false)) {
    that.Cursor = that.Limit;
  }
}

void Orly::CsvToBin::StartOfField(TLevel3 &that) {
  assert(&that);
  that.MatchState(TLevel2::StartOfField);
}

void Orly::CsvToBin::StartOfFile(TLevel3 &that) {
  assert(&that);
  that.MatchState(TLevel2::StartOfFile);
}

void Orly::CsvToBin::StartOfRecord(TLevel3 &that) {
  assert(&that);
  that.MatchState(TLevel2::StartOfRecord);
}

TLevel3 &TLevel3::operator>>(bool &that) {
  assert(this);
  assert(&that);
  uint8_t c = tolower(Peek());
  const char *kwd;
  bool result;
  if (c == *TrueKwd) {
    kwd = TrueKwd;
    result = true;
  } else if (c == *FalseKwd) {
    kwd = FalseKwd;
    result = false;
  } else {
    THROW_ERROR(TSyntaxError)
        << "expected keyword \"" << TrueKwd
        << "\" or \"" << FalseKwd << '"';
  }
  MatchKeyword(kwd);
  that = result;
  return *this;
}

TLevel3 &TLevel3::operator>>(Base::TUuid &that) {
  assert(this);
  assert(&that);
  static constexpr size_t size = Base::TUuid::StrSize;
  char buf[size + 1];
  for (size_t i = 0; i < size; ++i) {
    buf[i] = Pop();
  }
  buf[size] = '\0';
  that = Base::TUuid(buf);
  return *this;
}

TLevel3 &TLevel3::operator>>(int64_t &that) {
  assert(this);
  assert(&that);
  uint8_t c = Peek();
  int64_t sign;
  if (c == '+') {
    sign = 1;
    Pop();
  } else if (c == '-') {
    sign = -1;
    Pop();
  } else {
    sign = 1;
  }
  int64_t temp;
  size_t dummy;
  ReadDecimalInt(temp, dummy);
  that = temp * sign;
  return *this;
}

TLevel3 &TLevel3::operator>>(double &that) {
  assert(this);
  assert(&that);
  /* TODO: Yeah, this is cheating.  We really shouldn't be consuming all the
     bytes in the field and then translating just the prefix into a number.
     We should scan property to conserve the rest of the field. */
  string temp;
  *this >> temp;
  that = stod(temp);
  return *this;
}

TLevel3 &TLevel3::operator>>(std::string &that) {
  assert(this);
  assert(&that);
  string temp;
  while (RefreshBytes(false)) {
    temp.append(Cursor, Limit);
    Cursor = Limit;
  }
  that = move(temp);
  return *this;
}

TLevel3 &TLevel3::operator>>(Base::Chrono::TTimePnt &that) {
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
  return *this;
}

bool TLevel3::AtField() const {
  assert(this);
  bool result;
  auto state = GetState();
  switch (state) {
    case TLevel2::StartOfField: {
      result = true;
      break;
    }
    case TLevel2::EndOfRecord: {
      result = false;
      break;
    }
    default: {
      THROW_ERROR(TUnexpectedState)
          << "expected StartOfField or EndOfRecord, found "
          << TLevel2::GetName(state);
    }
  }
  return result;
}

bool TLevel3::AtRecord() const {
  assert(this);
  bool result;
  auto state = GetState();
  switch (state) {
    case TLevel2::StartOfRecord: {
      result = true;
      break;
    }
    case TLevel2::EndOfFile: {
      result = false;
      break;
    }
    default: {
      THROW_ERROR(TUnexpectedState)
          << "expected StartOfRecord or EndOfFile, found "
          << TLevel2::GetName(state);
    }
  }
  return result;
}

void TLevel3::MatchByte(uint8_t expected) {
  assert(this);
  if (!TryMatchByte(expected)) {
    THROW_ERROR(TSyntaxError)
        << "expected '" << expected
        << "', found '" << static_cast<char>(Peek()) << '\'';
  }
}

uint8_t TLevel3::MatchByte(const char *expected) {
  assert(this);
  uint8_t match;
  if (!TryMatchByte(expected, match)) {
    THROW_ERROR(TSyntaxError)
        << "expected any of '" << expected
        << "', found '" << static_cast<char>(Peek()) << '\'';
  }
  return match;
}

void TLevel3::MatchKeyword(const char *keyword) {
  assert(this);
  for (const char *csr = keyword; *csr; ++csr) {
    if (tolower(Pop()) != *csr) {
      THROW_ERROR(TSyntaxError)
          << "expected keyword \"" << keyword << '"';
    }
  }
}

void TLevel3::MatchState(TLevel2::TState state) {
  assert(this);
  if (GetState() != state) {
    THROW_ERROR(TUnexpectedState)
        << "expected " << TLevel2::GetName(state)
        << ", found " << TLevel2::GetName(Level2->State);
  }
  ++Level2;
}

void TLevel3::ReadDecimalInt(int64_t &value, size_t &size) {
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

bool TLevel3::RefreshBytes(bool required) const {
  assert(this);
  bool success = (Cursor < Limit);
  if (!success) {
    if (Cursor) {
      ++Level2;
      Cursor = nullptr;
      Limit = nullptr;
    }
    switch (Level2->State) {
      case TLevel2::StartOfFile:
      case TLevel2::StartOfRecord:
      case TLevel2::StartOfField:
      case TLevel2::EndOfRecord:
      case TLevel2::EndOfFile: {
        THROW_ERROR(TUnexpectedState)
            << "expected Bytes or EndOfField, found "
            << TLevel2::GetName(Level2->State);
      }
      case TLevel2::Bytes: {
        Cursor = Level2->Start;
        Limit = Level2->Limit;
        assert(Cursor);
        success = (Cursor < Limit);
        break;
      }
      case TLevel2::EndOfField: {
        break;
      }
    }
    if (!success && required) {
      THROW_ERROR(TPastEnd);
    }
  }
  return success;
}

bool TLevel3::TryMatchByte(uint8_t expected) {
  assert(this);
  bool success = (Peek() == expected);
  if (success) {
    Pop();
  }
  return success;
}

bool TLevel3::TryMatchByte(const char *expected) {
  assert(this);
  uint8_t dummy;
  return TryMatchByte(expected, dummy);
}

bool TLevel3::TryMatchByte(const char *expected, uint8_t &match) {
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
