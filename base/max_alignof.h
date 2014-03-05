/* <base/max_alignof.h>

   Get the alignment the most aligned type in a list of types.

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

#include <cstddef>

#include <base/no_construction.h>

namespace Base {

  /* A recursive template in which to look up the alignment the most aligned of its types. */
  template <typename... TElems>
  class MaxAlignof;

  /* The base case. */
  template <>
  class MaxAlignof<> final {
    NO_CONSTRUCTION(MaxAlignof);
    public:

    /* Return the least restrictive alignment. */
    static constexpr size_t Get() {
      return __alignof__(char);
    }

  };  // MaxAlignof<>

  /* The recurring case. */
  template <typename TSomeElem, typename... TMoreElems>
  class MaxAlignof<TSomeElem, TMoreElems...> final {
    NO_CONSTRUCTION(MaxAlignof);
    public:

    /* Return the most restrictive alignment. */
    static constexpr size_t Get() {
      return (ThisValue >= RecurValue) ? ThisValue : RecurValue;
    }

    private:

    /* Conveniences. */
    static constexpr size_t ThisValue  = __alignof__(TSomeElem);
    static constexpr size_t RecurValue = MaxAlignof<TMoreElems...>::Get();

  };  // MaxAlignof<TSomeElem, TMoreElems...>

}  // Base
