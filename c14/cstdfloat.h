/* <c14/cstdfloat.h>

   Aliases for the built-in floating-point types.  Similar to <cstdint>.

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

using float32_t  = float;
static_assert(sizeof(float32_t) == 4, "32 bit float wasn't 4 bytes long");
using float64_t  = double;
static_assert(sizeof(float64_t) == 8, "64 bit float wasn't 8 bytes long");

#ifdef __clang__
using float128_t = long double;
#else
using float128_t = __float128;
#endif
static_assert(sizeof(float128_t) == 16, "128 bit float wasn't 16 bytes long");
