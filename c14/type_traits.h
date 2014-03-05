/* <c14/type_traits.h>

   These will be in <type_traits> header for C++14.

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

#include <type_traits>

namespace c14 {

  template <typename T>
  using add_const_t = typename std::add_const<T>::type;

  template <typename T>
  using add_cv_t = typename std::add_cv<T>::type;

  template <typename T>
  using add_lvalue_reference_t = typename std::add_lvalue_reference<T>::type;

  template <typename T>
  using add_pointer_t = typename std::add_pointer<T>::type;

  template <typename T>
  using add_rvalue_reference_t = typename std::add_rvalue_reference<T>::type;

  template <typename T>
  using add_volatile_t = typename std::add_volatile<T>::type;

  template <typename... T>
  using common_type_t = typename std::common_type<T...>::type;

  template <bool B, typename T, typename F>
  using conditional_t = typename std::conditional<B, T, F>::type;

  template <typename T>
  using decay_t = typename std::decay<T>::type;

  template <bool B, class T = void>
  using enable_if_t = typename std::enable_if<B, T>::type;

  template <typename T>
  using make_signed_t = typename std::make_signed<T>::type;

  template <typename T>
  using make_unsigned_t = typename std::make_unsigned<T>::type;

  template <typename T>
  using remove_all_extents_t = typename std::remove_all_extents<T>::type;

  template <typename T>
  using remove_const_t = typename std::remove_const<T>::type;

  template <typename T>
  using remove_cv_t = typename std::remove_cv<T>::type;

  template <typename T>
  using remove_extent_t = typename std::remove_extent<T>::type;

  template <typename T>
  using remove_pointer_t = typename std::remove_pointer<T>::type;

  template <typename T>
  using remove_reference_t = typename std::remove_reference<T>::type;

  template <typename T>
  using remove_volatile_t = typename std::remove_volatile<T>::type;

  template <typename T>
  using result_of_t = typename std::result_of<T>::type;

  template <typename T>
  using underlying_type_t = typename std::underlying_type<T>::type;

}  // c14