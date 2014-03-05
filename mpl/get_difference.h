/* <mpl/get_difference.h>

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

#include <c14/identity.h>
#include <c14/type_traits.h>

#include <mpl/conditional.h>
#include <mpl/contains.h>
#include <mpl/type_list.h>
#include <mpl/type_set.h>

namespace Mpl {

  /* GetDifference. */
  template <typename TList, typename TLhs, typename TRhs>
  struct GetDifferenceRecur;

  template <typename... TElems, typename TRhs>
  struct GetDifferenceRecur<TTypeList<TElems...>, TTypeList<>, TRhs> : public c14::identity<TTypeSet<TElems...>> {};

  template <typename TList, typename TElem, typename... TMoreElems, typename TRhs>
  struct GetDifferenceRecur<TList, TTypeList<TElem, TMoreElems...>, TRhs> : public Mpl::Conditional<
      Contains<TRhs, TElem>,
      GetDifferenceRecur<TList, TTypeList<TMoreElems...>, TRhs>,
      GetDifferenceRecur<TExtend<TList, TTypeList<TElem>>, TTypeList<TMoreElems...>, TRhs>> {};

  template <typename TLhs, typename TRhs>
  struct GetDifference : public GetDifferenceRecur<TTypeList<>, Mpl::TGetList<TLhs>, TRhs> {};

  template <typename TLhs, typename TRhs>
  using TGetDifference = typename GetDifference<TLhs, TRhs>::type;

}  // Mpl
