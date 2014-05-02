/* <orly/rt/tuple.h>

   The 'tuple' runtime value.

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

#include <base/class_traits.h>
#include <orly/desc.h>
#include <orly/rt/operator.h>

namespace Orly {

  namespace Rt {

    /* Match two tuples. */
    template <typename... TArgs>
    bool Match(const std::tuple<TArgs...> &/*lhs*/, const std::tuple<TArgs...> &/*rhs*/) {
      throw std::runtime_error("TODO: Match for tuple");
    }

    /* MatchLess two tuples. */
    template <typename... TArgs>
    bool MatchLess(const std::tuple<TArgs...> &/*lhs*/, const std::tuple<TArgs...> &/*rhs*/) {
      throw std::runtime_error("TODO: MatchLess for tuple");
    }

    template <typename TVal>
    inline size_t GetTupleSize(const TVal &) {
      return std::tuple_size<TVal>::value;
    }

  }  // Rt

}  // Orly