/* <mpl/is_subset_of.h>

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

#include <mpl/type_set.h>

namespace Mpl {

  /* IsSubsetOf. */
  /* Forward declaration. */
  template <typename TLhs, typename TRhs>
  struct IsSubsetOfRecur;

  /* Base case. */
  template <typename TRhs>
  struct IsSubsetOfRecur<TTypeList<>, TRhs> : public std::true_type {};

  /* Recursive case. */
  template <typename TElem, typename... TMoreElems, typename TRhs>
  struct IsSubsetOfRecur<TTypeList<TElem, TMoreElems...>, TRhs>
      : public std::integral_constant<bool,
                                      Contains<TRhs, TElem>::value &&
                                      IsSubsetOfRecur<TTypeList<TMoreElems...>, TRhs>::value> {};

  /* Final function. */
  template <typename TLhs, typename TRhs>
  struct IsSubsetOf
      : public std::integral_constant<bool,
                                      (GetSize<TLhs>::value <= GetSize<TRhs>::value) &&
                                      IsSubsetOfRecur<TGetList<TLhs>, TRhs>::value> {};

}  // Mpl
