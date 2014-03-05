/* <mpl/is_same.h>

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

#include <c14/type_traits.h>

#include <mpl/get_size.h>
#include <mpl/is_subset_of.h>
#include <mpl/type_set.h>

namespace std {

  /* std::is_same<> specialization for Mpl::TTypeSet<Elems...>. */
  template <typename... TElems>
  struct is_same<Mpl::TTypeSet<TElems...>, Mpl::TTypeSet<TElems...>>
      : public std::integral_constant<
                   bool,
                   (Mpl::GetSize<Mpl::TTypeSet<TElems...>>() ==
                    Mpl::GetSize<Mpl::TTypeSet<TElems...>>()) &&
                       Mpl::IsSubsetOf<Mpl::TTypeSet<TElems...>,
                                       Mpl::TTypeSet<TElems...>>()> {};

  /* std::is_same<> specialization for Mpl::TTypeSet<Elems...>. */
  template <typename... TLhsElems, typename... TRhsElems>
  struct is_same<Mpl::TTypeSet<TLhsElems...>, Mpl::TTypeSet<TRhsElems...>>
      : public std::integral_constant<
                   bool,
                   (Mpl::GetSize<Mpl::TTypeSet<TLhsElems...>>() ==
                    Mpl::GetSize<Mpl::TTypeSet<TRhsElems...>>()) &&
                       Mpl::IsSubsetOf<Mpl::TTypeSet<TLhsElems...>,
                                       Mpl::TTypeSet<TRhsElems...>>()> {};

}  // std

namespace Mpl {

  /* Decay the types and pass-through std::is_same<>. */
  template <typename TLhs, typename TRhs>
  struct IsSame : public std::is_same<c14::decay_t<TLhs>, c14::decay_t<TRhs>> {};

}  // Mpl
