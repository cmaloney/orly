/* <base/max_sizeof.h>

   Get the sizeof the largest type in a list of types.

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

  /* A recursive template in which to look up the sizeof the largest of its types. */
  template <typename... TElems>
  class MaxSizeof;

  /* The base case. */
  template <>
  class MaxSizeof<> final {
    NO_CONSTRUCTION(MaxSizeof);
    public:

    /* Return zero. */
    static constexpr size_t Get() {
      return 0;
    }

  };  // MaxSizeof<>

  /* The recurring case. */
  template <typename TSomeElem, typename... TMoreElems>
  class MaxSizeof<TSomeElem, TMoreElems...> final {
    NO_CONSTRUCTION(MaxSizeof);
    public:

    /* Return the largest size. */
    static constexpr size_t Get() {
      return (ThisValue >= RecurValue) ? ThisValue : RecurValue;
    }

    private:

    /* Conveniences. */
    static constexpr size_t ThisValue  = sizeof(TSomeElem);
    static constexpr size_t RecurValue = MaxSizeof<TMoreElems...>::Get();

  };  // MaxSizeof<TSomeElem, TMoreElems...>

}  // Base
