/* <mpl/type_list.h>

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

#include <cstddef>

namespace Mpl {

  /* Forward declaration. */
  template <std::size_t Idx, typename... TElems>
  struct TTypeListRecur;

  /* Base case. */
  template <std::size_t Idx>
  struct TTypeListRecur<Idx> {
    protected:

    static constexpr std::size_t GetIdx() { return Idx; }

  };  // TTypeListRecur<Idx>

  /* Recursive case. */
  template <std::size_t Idx, typename TElem, typename... TMoreElems>
  struct TTypeListRecur<Idx, TElem, TMoreElems...> : public TTypeListRecur<Idx + 1, TMoreElems...> {
    protected:

    using TSuper = TTypeListRecur<Idx + 1, TMoreElems...>;

    using TSuper::GetIdx;

    static constexpr std::size_t GetIdx(const TElem *) { return Idx; }

  };  // TTypeListRecur<Idx, TElem, TMoreElems...>

  /* Final class. */
  template <typename... TElems>
  struct TTypeList : public TTypeListRecur<0, TElems...> {

    using TSuper = TTypeListRecur<0, TElems...>;

    template <typename TElem>
    static constexpr std::size_t GetIdx() {
      return TSuper::GetIdx(static_cast<const TElem *>(nullptr));
    }

    static constexpr std::size_t GetSize() { return TSuper::GetIdx(); }

  };  // TTypeList

}  // Mpl
