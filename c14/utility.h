/* <c14/utility.h>

   These will be in <utility> header for C++14.

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

#include <c14/identity.h>
#include <cstddef>
#include <type_traits>

namespace c14 {

  /* integer_sequence<>. */
  template <typename T, T... Ints>
  struct integer_sequence : public std::integral_constant<std::size_t, sizeof...(Ints)> {};

  template <typename T, T Cur, T End, T... Ints>
  struct make_integer_range_impl;

  template <typename T, T End, T... Ints>
  struct make_integer_range_impl<T, End, End, Ints...> : public c14::identity<integer_sequence<T, Ints...>> {};

  template <typename T, T Cur, T End, T... Ints>
  struct make_integer_range_impl : public make_integer_range_impl<T, Cur + 1, End, Ints..., Cur> {};

  template <typename T, T Begin, T End>
  using make_integer_range = typename make_integer_range_impl<T, Begin, End>::type;

  template <typename T, T N>
  using make_integer_sequence = make_integer_range<T, 0, N>;

  /* index_sequence<>. */
  template <std::size_t... Idxs>
  using index_sequence = integer_sequence<std::size_t, Idxs...>;

  template <std::size_t Idx>
  using make_index_sequence = make_integer_sequence<std::size_t, Idx>;

  template <std::size_t Begin, std::size_t End>
  using make_index_range = make_integer_range<std::size_t, Begin, End>;

}  // c14