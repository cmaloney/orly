/* <utf8/piece.h>

   A reference to a UTF-8 string defined by a pair of start/limit pointers.

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
#include <cstdint>
#include <ostream>

#include <base/thrower.h>

namespace Utf8 {

  /* A reference to a UTF-8 string defined by a pair of start/limit pointers.
     Remember, we don't own our data, we just point at it.
     That makes this a light-weight value type, suitable for shallow-copying.
     The text is not validated when the piece is constructed, so a piece may point to invalid UTF-8. */
  class TPiece {
    public:

    /* The errors we throw. */
    DEFINE_ERROR(TBadEncoding, std::runtime_error, "UTF-8 piece contains bad encoding");
    DEFINE_ERROR(TEmpty, std::out_of_range,  "UTF-8 piece is empty");

    /* Construct an empty piece. */
    TPiece()
        : Start(nullptr), Limit(nullptr) {}

    /* Default copy-constructor. */
    TPiece(const TPiece &that) = default;

    /* Point at the given c-string. */
    TPiece(const char *that);

    /* Point at the given string. */
    TPiece(const char *start, const char *limit)
        : Start(start), Limit(limit) {
      assert(start <= limit);
    }

    /* Point at the given string. */
    TPiece(const char *start, size_t size) {
      assert(start || !size);
      Start = start;
      Limit = start + size;
    }

    /* Default assignment-operator. */
    TPiece &operator=(const TPiece &that) = default;

    /* True iff. the this piece is non-empty. */
    operator bool() const {
      assert(this);
      return Limit != Start;
    }

    /* Same as Compare(that) == 0. */
    bool operator==(const TPiece &that) const {
      assert(this);
      return Compare(that) == 0;
    }

    /* Same as Compare(that) != 0. */
    bool operator!=(const TPiece &that) const {
      assert(this);
      return Compare(that) != 0;
    }

    /* Same as Compare(that) < 0. */
    bool operator<(const TPiece &that) const {
      assert(this);
      return Compare(that) < 0;
    }

    /* Same as Compare(that) <= 0. */
    bool operator<=(const TPiece &that) const {
      assert(this);
      return Compare(that) <= 0;
    }

    /* Same as Compare(that) > 0. */
    bool operator>(const TPiece &that) const {
      assert(this);
      return Compare(that) > 0;
    }

    /* Same as Compare(that) >= 0. */
    bool operator>=(const TPiece &that) const {
      assert(this);
      return Compare(that) >= 0;
    }

    /* Compare the text of this piece with the text of that one and return as strcmp() does. */
    int Compare(const TPiece &that) const;

    /* True iff. this piece contains the given address. */
    bool Contains(const char *ptr) const {
      assert(this);
      return Start <= ptr && ptr < Limit;
    }

    /* The length of the of this piece, in bytes.
       Runs in constant time.
       The piece need not be valid. */
    size_t GetByteCount() const {
      assert(this);
      return Limit - Start;
    }

    /* The hash of the text of this piece.
       The piece need not be valid to be hashed, as we hash it as raw data.
       The hash of an empty piece is zero. */
    size_t GetHash() const;

    /* The length of the of this piece, in code points.
       Runs in linear time.
       Throws if the piece is invalid. */
    size_t GetLength() const;

    /* The limit of the string.
       Always >= Start.
       Can be null iff. Start is also null. */
    const char *GetLimit() const {
      assert(this);
      return Limit;
    }

    /* The start of the string.
       Always <= Limit.
       Can be null. */
    const char *GetStart() const {
      assert(this);
      return Start;
    }

    /* True iff. this string piece and that one are pointing at the same memory. */
    bool Is(const TPiece &that) const {
      assert(this);
      assert(&that);
      return Start == that.Start && Limit == that.Limit;
    }

    /* Return the first code point in this piece and shorten this piece by one code point.
       If this piece is empty or if the first code point is invalid, throw. */
    uint32_t Peek() const {
      assert(this);
      return TPiece(*this).Pop();
    }

    /* Return the first code point in this piece and shorten this piece by one code point.
       If this piece is empty, throw.
       If the first code point is invalid, shorten this piece anyway, but then throw. */
    uint32_t Pop();

    /* Reset to the default-constructed state. */
    TPiece &Reset() {
      assert(this);
      Start = nullptr;
      Limit = nullptr;
      return *this;
    }

    /* Split the first code point in this piece into a separate piece and shorten this piece by one code point.
       If this piece is empty, throw.
       If the first code point is invalid, shorten this piece anyway, but then throw. */
    TPiece Split() {
      assert(this);
      const char *temp = Start;
      Pop();
      return TPiece(temp, Start);
    }

    private:

    /* See accessors. */
    const char *Start, *Limit;

  };  // TPiece

  /* A std stream inserter for Utf8::TPiece. */
  inline std::ostream &operator<<(std::ostream &strm, const TPiece &that) {
    assert(&strm);
    assert(&that);
    return strm.write(that.GetStart(), that.GetByteCount());
  }

}  // Utf8

namespace std {

  /* A std hasher for Utf8::TPiece. */
  template <>
  struct hash<Utf8::TPiece> {
    inline size_t operator()(const Utf8::TPiece &that) const {
      assert(&that);
      return that.GetHash();
    }
  };

}
