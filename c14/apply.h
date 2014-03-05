/* <c14/apply.h>

   Defines a function which takes a callable thing (such as functions, function objects, lambdas, etc.) and invokes it with the
   elements of a tuple.

   For example, if we have a function:

     void F(int x, double y, const std::string &z) { ... }

   We can invoke it via:

     apply(F, make_tuple(101, 1.1, "hello"));

   Above is a trivial example that's no better than directly calling the function of course.

   apply() is useful in situations where we want to cache some arguments that we currently have but don't want to call the
   function yet. Sounds like std::bind()? it does, the distinction here however is that we don't have to be bound to a specific
   function.

   For example, if I have a set of functions that I want to call with the arguments that I have cached, I can do something like:

     auto args = make_tuple(101, 1.1, "hello");
     std::vector<void (*)(int, double, const std::string &)> functions;
     for (const auto &function : functions) {
       apply(function, args);
     }  // for

   There is also a variant of apply() which takes a callable thing, a tuple and an index_sequence. The regular apply()
   calls this version with the index_sequence defaulted to 0...N-1 where N is the size of the tuple, and these indices represent
   the indices of the elements to be passed to the function. So we can use this to do things like, pass the same argument multiple
   times, reorder the arguments so that it fits the function signature, pass a slice of the tuple, etc.

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

#include <c14/lang.h>
#include <c14/type_traits.h>
#include <c14/utility.h>
#include <tuple>
#include <utility>

namespace c14 {

  template <typename F, typename TTuple, std::size_t... Idx>
  auto apply(F &&f, TTuple &&tuple, c14::index_sequence<Idx...> &&)
      DECLTYPE_AUTO(std::forward<F>(f)(std::get<Idx>(std::forward<TTuple>(tuple))...));

  template <typename F, typename TTuple>
  auto apply(F &&f, TTuple &&tuple)
      DECLTYPE_AUTO(apply(std::forward<F>(f),
                          std::forward<TTuple>(tuple),
                          c14::make_index_sequence<std::tuple_size<c14::decay_t<TTuple>>::value>()));

}  // c14
