/* <strm/bin/var_int.cc>

   Implements <strm/bin/var_int.h>.

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

#include <strm/bin/var_int.h>

using namespace std;
using namespace Strm::Bin;

const uint8_t *TVarIntDecoder::Decode(
    const uint8_t *start, const uint8_t *limit) noexcept {
  assert(this);
  /* If we're starting over at a new value, zero the accumulator. */
  if (!Shift) {
    Accum = 0;
  }
  /* Consume bytes. */
  while (start < limit) {
    /* Or the next 7 bits into the accumulator, shifted to their proper
       position. */
    uint8_t byte = *start++;
    Accum |= static_cast<int64_t>(byte & 0x7F) << Shift;
    /* If this isn't the end-byte, keep shifting; otherwise, we're done. */
    if (byte & 0x80) {
      Shift += 7;
    } else {
      Shift = 0;
      break;
    }
  }
  return start;
}

constexpr size_t TVarIntEncoder::MaxSize;

void TVarIntEncoder::Encode(uint64_t n) noexcept {
  assert(this);
  /* Loop until we have encoded the entire number. */
  Limit = Buffer;
  do {
    /* Copy the 7 least sigificant bits to a byte, then shift the more
       significant bits down. */
    uint8_t byte = n & 0x7F;
    n >>= 7;
    /* Write the 7 low-order bits, setting the high-order bit iff. the
       var-int continues. */
    if (n) {
      byte |= 0x80;
    }
    *Limit++ = byte;
  } while (n);
}
