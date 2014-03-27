/* <strm/bin/var_int.h>

   Encode and decode integers such that smaller values take fewer bytes.

   Encoding puts the least significant bits first (and so is little-endian),
   breaking the number into 7-bit chunks.  Each chunk is encoded in a single
   byte, with the high-order bit set on each byte except for the last one.

   When decoding, read bytes until you find one with the high-order bit off.
   It and the bytes which precede it make up the var-int.

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
#include <cstddef>
#include <cstdint>

namespace Strm {

  namespace Bin {

    /* Decodes var-ints from an external byte buffer. */
    class TVarIntDecoder final {
      public:

      /* A default-constructed decoder behaves as though it has just finished
         decoding a value of zero. */
      TVarIntDecoder() noexcept
          : Accum(0), Shift(0) {}

      /* Returns a pointer to the last value decoded, or null if we're in the
         middle of decoding. */
      const uint64_t *operator*() const noexcept {
        assert(this);
        return !Shift ? &Accum : nullptr;
      }

      /* Decode the given sequence of bytes, returning a pointer just after the
         last byte consumed in the decoding.  Never returns a value outside of
         the given start-limit range.  If the decoder returns a value < limit,
         it means it stopped at the end of a var-int and operator* will return
         a pointer to the decoded value.  Calling Decode() again will discard
         the previously decoded value.  If Decode() returns limit, we ran out
         of bytes in the middle of a var-int and and opeartor* will return a
         null pointer until we get more bytes.  Calling Decode() again with
         more data will case decoding to pick up where it left off. */
      const uint8_t *Decode(const uint8_t *start, const uint8_t *limit) noexcept;

      private:

      /* The value we have decoded or are in the process of decoding. */
      uint64_t Accum;

      /* If zero, then Accum, above, contains a full-decoded value; otherwise,
         this is the amount we will shift the next byte we consume before or-
         ing it into Accum. */
      unsigned Shift;

    };  // TVarIntDecoder

    /* Encodes var-ints in an internal byte buffer. */
    class TVarIntEncoder final {
      public:

      /* The largest number of bytes a 64-bit integer can require when encoded
         as a var-int. */
      static constexpr size_t MaxSize = 10;

      /* Start out empty. */
      TVarIntEncoder() noexcept
          : Limit(Buffer) {}

      /* Start out with the given value encoded into our buffer. */
      explicit TVarIntEncoder(uint64_t n) noexcept {
        Encode(n);
      }

      /* Encode the given value into our buffer, overwriting anything that was
         there previously. */
      void Encode(uint64_t n) noexcept;

      /* The limit of our encoded data.  Always >= GetStart(). */
      const uint8_t *GetLimit() const noexcept {
        assert(this);
        return Limit;
      }

      /* The number of bytes of encoded data we currently hold.
         This can be zero. */
      size_t GetSize() const noexcept {
        assert(this);
        return Limit - Buffer;
      }

      /* The start of our encoded data.  Always <= GetLimit(). */
      const uint8_t *GetStart() const noexcept {
        assert(this);
        return Buffer;
      }

      private:

      /* Our buffer space. */
      uint8_t Buffer[MaxSize];

      /* See accessor. */
      uint8_t *Limit;

    };  // TVarIntEncoder

  }  // Bin

}  // Strm
