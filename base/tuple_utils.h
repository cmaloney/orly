/* <base/tuple_utils.h>

   TODO

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

#include <array>
#include <tuple>

namespace Base {

  /* NOTE: The current version only supports 0 or 1 out parameters.
           This is ok for now since we don't have a particular use case for them.

           We can use variadic templates on the out parameter to support 0 to N out parameters,
           In which case we can simply have 2 versions (const and non-const) of each (recursive, base case, wrapper)
           However G++ 4.7 currently doesn't seem to support this fully. */

  // Compile-time counter

  template <size_t index>
  struct TIndex {};

  // Recursive

  template <template <typename> class TCb, typename TTuple, size_t index>
  bool ForEachElem(TTuple &that, TIndex<index>) {
    typedef typename std::tuple_element<index, TTuple>::type TType; // Type of the current element
    if (ForEachElem<TCb>(that, TIndex<index - 1>())) {
      return TCb<TType>()(std::get<index>(that), index);
    }
    return false;
  }

  template <template <typename> class TCb, typename TTuple, size_t index, typename TOut>
  bool ForEachElem(TTuple &that, TIndex<index>, TOut &out) {
    typedef typename std::tuple_element<index, TTuple>::type TType; // Type of the current element
    if (ForEachElem<TCb>(that, TIndex<index - 1>(), out)) {
      return TCb<TType>()(std::get<index>(that), index, out);
    }
    return false;
  }

  template <template <typename> class TCb, typename TTuple, size_t index>
  bool ForEachElem(const TTuple &that, TIndex<index>) {
    typedef typename std::tuple_element<index, TTuple>::type TType; // Type of the current element
    if (ForEachElem<TCb>(that, TIndex<index - 1>())) {
      return TCb<TType>()(std::get<index>(that), index);
    }
    return false;
  }

  template <template <typename> class TCb, typename TTuple, size_t index, typename TOut>
  bool ForEachElem(const TTuple &that, TIndex<index>, TOut &out) {
    typedef typename std::tuple_element<index, TTuple>::type TType; // Type of the current element
    if (ForEachElem<TCb>(that, TIndex<index - 1>(), out)) {
      return TCb<TType>()(std::get<index>(that), index, out);
    }
    return false;
  }

  // Base case

  template <template <typename> class TCb, typename TTuple>
  bool ForEachElem(TTuple &that, TIndex<0>) {
    typedef typename std::tuple_element<0, TTuple>::type TType; // Type of the first element
    return TCb<TType>()(std::get<0>(that), 0); // Call callback function
  }

  template <template <typename> class TCb, typename TTuple, typename TOut>
  bool ForEachElem(TTuple &that, TIndex<0>, TOut &out) {
    typedef typename std::tuple_element<0, TTuple>::type TType; // Type of the first element
    return TCb<TType>()(std::get<0>(that), 0, out); // Call callback function
  }

  template <template <typename> class TCb, typename TTuple>
  bool ForEachElem(const TTuple &that, TIndex<0>) {
    typedef typename std::tuple_element<0, TTuple>::type TType; // Type of the first element
    return TCb<TType>()(std::get<0>(that), 0); // Call callback function
  }

  template <template <typename> class TCb, typename TTuple, typename TOut>
  bool ForEachElem(const TTuple &that, TIndex<0>, TOut &out) {
    typedef typename std::tuple_element<0, TTuple>::type TType; // Type of the first element
    return TCb<TType>()(std::get<0>(that), 0, out); // Call callback function
  }

  // These are the one that gets used by the caller

  template <template <typename> class TCb, typename TTuple>
  void ForEachElem(TTuple &that) {
    ForEachElem<TCb>(that, TIndex<std::tuple_size<TTuple>::value - 1>());
  }

  template <template <typename> class TCb, typename TTuple, typename TOut>
  void ForEachElem(TTuple &that, TOut &out) {
    ForEachElem<TCb>(that, TIndex<std::tuple_size<TTuple>::value - 1>(), out);
  }

  template <template <typename> class TCb, typename TTuple>
  void ForEachElem(const TTuple &that) {
    ForEachElem<TCb>(that, TIndex<std::tuple_size<TTuple>::value - 1>());
  }

  template <template <typename> class TCb, typename TTuple, typename TOut>
  void ForEachElem(const TTuple &that, TOut &out) {
    ForEachElem<TCb>(that, TIndex<std::tuple_size<TTuple>::value - 1>(), out);
  }

  template <typename THead, typename... TRest>
  const THead &GetHead(const std::tuple<THead, TRest...> &that) {
    return std::_Tuple_impl<0, THead, TRest...>::_M_head(that);
  }

  template <size_t N, typename THead, typename... TRest>
  const THead &GetHead(const std::_Tuple_impl<N, THead, TRest...> &that) {
    return std::_Tuple_impl<N, THead, TRest...>::_M_head(that);
  }

  template <typename THead, typename... TRest>
  const std::_Tuple_impl<1, TRest...> &GetTail(const std::tuple<THead, TRest...> &that) {
    return std::_Tuple_impl<0, THead, TRest...>::_M_tail(that);
  }

  template <size_t N, typename THead, typename... TRest>
  const std::_Tuple_impl<N + 1, TRest...> &GetTail(const std::_Tuple_impl<N, THead, TRest...> &that) {
    return std::_Tuple_impl<N, THead, TRest...>::_M_tail(that);
  }

}  // Base
