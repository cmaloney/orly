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
  string temp;
  *this >> temp;
  that = stoll(temp);
  return *this;
}

TLevel3 &TLevel3::operator>>(double &that) {
  assert(this);
  assert(&that);
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
    }
    default: {
      THROW_ERROR(TUnexpectedState)
          << "expected StartOfRecord or EndOfFile, found "
          << TLevel2::GetName(state);
    }
  }
  return result;
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
        assert(Cursor < Limit);
        success = true;
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
