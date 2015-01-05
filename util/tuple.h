/* <util/tuple.h>

   Utilities for working with tuples.

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

#pragma once

#include <initializer_list>
#include <tuple>
#include <type_traits>
#include <utility>

namespace Util {

template <typename TTuple, typename TUnaryFn, std::size_t... Is>
auto ForEachImpl(TTuple &&tuple, TUnaryFn &&unary_fn, std::index_sequence<Is...>)
    -> decltype(std::initializer_list<int>(
                    {(std::forward<TUnaryFn>(unary_fn)(std::get<Is>(std::forward<TTuple>(tuple))),
                      0)...}),
                std::forward<TUnaryFn>(unary_fn)) {
  std::initializer_list<int>(
      {(std::forward<TUnaryFn>(unary_fn)(std::get<Is>(std::forward<TTuple>(tuple))), 0)...});
  return std::forward<TUnaryFn>(unary_fn);
}

template <typename TTuple, typename TUnaryFn, std::size_t... Is>
auto ForEachImpl(TTuple &&tuple, TUnaryFn &&unary_fn, std::index_sequence<Is...>)
    -> decltype(
          std::initializer_list<int>({(std::forward<TUnaryFn>(unary_fn).template operator()<Is>(
                                           std::get<Is>(std::forward<TTuple>(tuple))),
                                       0)...}),
          std::forward<TUnaryFn>(unary_fn)) {
  std::initializer_list<int>({(std::forward<TUnaryFn>(unary_fn).template operator()<Is>(
                                   std::get<Is>(std::forward<TTuple>(tuple))),
                               0)...});
  return std::forward<TUnaryFn>(unary_fn);
}

/**
 *   Examples:
 *   If you don't need the index: Use a lambda
 *
 *     ForEach(std::make_tuple(42, 1.1, "hello"),
 *             [](const auto &elem) {
 *               std::cout << elem << std::endl;
 *             });
 *
 *   If you need the index, use a functor with an `operator()` like:
 *
 *     struct TPrint {
 *
 *       explicit TPrint(std::ostream &strm) : Strm(strm) {}
 *
 *       template <std::size_t Idx, typename TElem>
 *       void operator()(const TElem &elem) const {
 *         if (Idx) {
 *           Strm << ", ";
 *         }  // if
 *         Strm << elem;
 *       }
 *
 *       private:
 *
 *       std::ostream &Strm;
 *
 *     };  // TPrint
 *
 *     ForEach(std::make_tuple(42, 1.1, "hello"), TPrint(std::cout));
 *
 **/

template <typename TTuple, typename TUnaryFn>
std::enable_if_t<std::tuple_size<std::decay_t<TTuple>>::value == 0, TUnaryFn> ForEach(
    TTuple &&, TUnaryFn &&unary_fn) {
  return std::forward<TUnaryFn>(unary_fn);
}

template <typename TTuple, typename TUnaryFn>
std::enable_if_t<std::tuple_size<std::decay_t<TTuple>>::value != 0, TUnaryFn> ForEach(
    TTuple &&tuple, TUnaryFn &&unary_fn) {
  return ForEachImpl(std::forward<TTuple>(tuple),
                     std::forward<TUnaryFn>(unary_fn),
                     std::make_index_sequence<std::tuple_size<std::decay_t<TTuple>>::value>());
}

}  // Util
