/* <mpl/get_intersection.h>

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

#include <mpl/compare.h>
#include <mpl/conditional.h>
#include <mpl/get_size.h>
#include <mpl/type_list.h>
#include <mpl/type_set.h>

namespace Mpl {

  /* GetIntersection. */
  template <typename TList, typename TShort, typename TLong>
  struct GetIntersectionRecur;

  template <typename... TElems, typename TLong>
  struct GetIntersectionRecur<TTypeList<TElems...>, TTypeList<>, TLong> : public c14::identity<TTypeSet<TElems...>> {};

  template <typename TList, typename TElem, typename... TMoreElems, typename TLong>
  struct GetIntersectionRecur<TList, TTypeList<TElem, TMoreElems...>, TLong> : public Mpl::Conditional<
      Contains<TLong, TElem>,
      GetIntersectionRecur<TExtend<TList, TTypeList<TElem>>, TTypeList<TMoreElems...>, TLong>,
      GetIntersectionRecur<TList, TTypeList<TMoreElems...>, TLong>> {};

  template <typename TLhs, typename TRhs>
  struct GetIntersection : public Mpl::Conditional<Lt<GetSize<TLhs>, GetSize<TRhs>>,
                                                   GetIntersectionRecur<TTypeList<>, TGetList<TLhs>, TRhs>,
                                                   GetIntersectionRecur<TTypeList<>, TGetList<TRhs>, TLhs>> {};

  template <typename TLhs, typename TRhs>
  using TGetIntersection = typename GetIntersection<TLhs, TRhs>::type;

}  // Mpl
