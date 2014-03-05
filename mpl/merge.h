/* <mpl/merge.h>

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
#include <mpl/extend.h>
#include <mpl/type_list.h>

namespace Mpl {

  template <template <typename, typename> class TCompare, typename TLhs, typename TRhs>
  struct Merge;

  template <template <typename, typename> class TCompare, typename TLhs, typename TRhs>
  using TMerge = typename Merge<TCompare, TLhs, TRhs>::type;

  template <template <typename, typename> class TCompare>
  struct Merge<TCompare, TTypeList<>, TTypeList<>> : public c14::identity<TTypeList<>> {};

  template <template <typename, typename> class TCompare, typename... TLhsElems>
  struct Merge<TCompare, TTypeList<TLhsElems...>, TTypeList<>> : public c14::identity<TTypeList<TLhsElems...>> {};

  template <template <typename, typename> class TCompare, typename... TRhsElems>
  struct Merge<TCompare, TTypeList<>, TTypeList<TRhsElems...>> : public c14::identity<TTypeList<TRhsElems...>> {};

  template <template <typename, typename> class TCompare,
            typename TLhsElem,
            typename... TMoreLhsElems,
            typename TRhsElem,
            typename... TMoreRhsElems>
  struct Merge<TCompare, TTypeList<TLhsElem, TMoreLhsElems...>, TTypeList<TRhsElem, TMoreRhsElems...>> : public std::conditional<
      TCompare<TLhsElem, TRhsElem>::value,
      TExtend<TTypeList<TLhsElem>, TMerge<TCompare, TTypeList<TMoreLhsElems...>, TTypeList<TRhsElem, TMoreRhsElems...>>>,
      TExtend<TTypeList<TRhsElem>, TMerge<TCompare, TTypeList<TLhsElem, TMoreLhsElems...>, TTypeList<TMoreRhsElems...>>>> {};

}  // Mpl
