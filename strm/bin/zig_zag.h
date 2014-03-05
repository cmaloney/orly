/* <strm/bin/zig_zag.h>

   Zig-zag encoding and decoding for signed integers.

   The point of this operation is to re-encode numbers such that numbers with
   small absolute values are closer to zero.  This is important when you wish
   to avoid having sign-extension make a signed value which happens to be
   negative appear to be a huge unsigned value.

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

#include <limits>
#include <type_traits>

namespace Strm {

  namespace Bin {

    /* Convert an unsigned, zig-zag encoded int to its signed normally encoded
       equivalent.  The output will be of the same number of bits as the input.
       For example, uint32_t in produces int32_t out.  This function is only
       defined for inputs of unsigned integral types. */
    template <
        typename TIn,
        typename TOut = typename std::enable_if<
            std::is_integral<TIn>::value && std::is_unsigned<TIn>::value,
            typename std::make_signed<TIn>::type
        >::type
    >
    inline constexpr TOut FromZigZag(TIn n) noexcept {
      return (n >> 1) ^ (-(n & 1));
    }

    /* Convert a signed, normally encoded int to its unsigned zig-zag encoded
       equivalent.  The output will be of the same number of bits as the input.
       For example, int32_t in produces uint32_t out.  This function is only
       defined for inputs of signed integral types. */
    template <
        typename TIn,
        typename TOut = typename std::enable_if<
            std::is_integral<TIn>::value && std::is_signed<TIn>::value,
            typename std::make_unsigned<TIn>::type
        >::type
    >
    inline constexpr TOut ToZigZag(TIn n) noexcept {
      return (n << 1) ^ (n >> std::numeric_limits<TIn>::digits);
    }

  }  // Bin

}  // Strm
