/* <utf8/piece.cc>

   Implements <utf8/piece.h>.

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

#include <utf8/piece.h>

using namespace std;
using namespace Utf8;

TPiece::TPiece(const char *that) {
  if (that) {
    Start = that;
    for (; *that; ++that);
    Limit = that;
  } else {
    Start = nullptr;
    Limit = nullptr;
  }
}

int TPiece::Compare(const TPiece &that) const {
  assert(this);
  assert(&that);
  TPiece
      lhs = *this,
      rhs = that;
  while (lhs && rhs) {
    uint32_t
        lhs_c = lhs.Pop(),
        rhs_c = rhs.Pop();
    if (lhs_c < rhs_c) {
      return -1;
    }
    if (lhs_c > rhs_c) {
      return 1;
    }
  }
  if (!lhs && rhs) {
    return -1;
  }
  if (lhs && !rhs) {
    return 1;
  }
  return 0;
}

size_t TPiece::GetHash() const {
  assert(this);
  return _Hash_bytes(Start, Limit - Start, 0);
}

size_t TPiece::GetLength() const {
  assert(this);
  size_t result = 0;
  for (TPiece temp = *this; temp; temp.Pop()) {
    ++result;
  }
  return result;
}

uint32_t TPiece::Pop() {
  assert(this);
  if (Limit == Start) {
    THROW_ERROR(TEmpty);
  }
  assert(Start);
  uint8_t c = *Start;
  uint32_t result;
  size_t byte_count;
  bool is_bad;
  if (c <= 0x7F) {
    result = c;
    byte_count = 1;
    is_bad = false;
  } else if ((c & 0xE0) == 0xC0) {
    result = c & 31;
    byte_count = 2;
    is_bad = false;
  } else if ((c & 0xF0) == 0xE0) {
    result = c & 15;
    byte_count = 3;
    is_bad = false;
  } else if ((c & 0xF8) == 0xF0) {
    result = c & 7;
    byte_count = 4;
    is_bad = false;
  } else if ((c & 0xFC) == 0xF8) {
    byte_count = 5;
    is_bad = true;
  } else if ((c & 0xFE) == 0xFC) {
    byte_count = 6;
    is_bad = true;
  } else {
    byte_count = 1;
    is_bad = true;
  }
  const char *limit = Start + byte_count;
  if (limit > Limit) {
    Start = Limit;
    THROW_ERROR(TBadEncoding) << "multi-byte code point goes past end of string";
  }
  if (is_bad) {
    Start = limit;
    THROW_ERROR(TBadEncoding) << byte_count << "-byte encoding";
  }
  const char *start = Start;
  for (++Start; Start < limit; ++Start) {
    c = *Start;
    if ((c & 0xC0) != 0x80) {
      THROW_ERROR(TBadEncoding) << byte_count << "-byte encoding ends after only " << (Start - start) << " bytes";
    }
    result = (result << 6) | (c & 0x3F);
  }
  return result;
}
