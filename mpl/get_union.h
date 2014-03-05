/* <mpl/get_union.h>

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

#include <mpl/type_list.h>
#include <mpl/type_set.h>

namespace Mpl {

  /* GetUnion. */
  template <typename TLhs, typename TRhs>
  struct GetUnionImpl;

  template <typename... TLhsElems, typename... TRhsElems>
  struct GetUnionImpl<TTypeList<TLhsElems...>, TTypeList<TRhsElems...>>
      : public c14::identity<TTypeSet<TLhsElems..., TRhsElems...>> {};

  template <typename TLhs, typename TRhs>
  using TGetUnionImpl = typename GetUnionImpl<TGetList<TLhs>, TGetList<TRhs>>::type;

  template <typename TSet, typename... TMoreSets>
  struct GetUnion;

  template <typename... TElems>
  struct GetUnion<TTypeSet<TElems...>> : public c14::identity<TTypeSet<TElems...>> {};

  template <typename TFirstSet, typename TSecondSet, typename... TMoreSets>
  struct GetUnion<TFirstSet, TSecondSet, TMoreSets...> : public GetUnion<TGetUnionImpl<TFirstSet, TSecondSet>, TMoreSets...> {};

  template <typename TSet, typename... TMoreSets>
  using TGetUnion = typename GetUnion<TSet, TMoreSets...>::type;

}  // Mpl
