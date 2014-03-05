/* <mpl/get_unique.h>

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
#include <mpl/extend.h>
#include <mpl/type_list.h>

namespace Mpl {

  /* GetUnique. */
  /* Our node with contains a type. */
  template <typename TElem>
  struct TNode {};

  /* Makes a root with N "children" for N >= 0. (really they're parents but it's a upside-down binary tree.) */
  template <typename... TNodes>
  struct TRoot : public TNodes... {};

  /* Forward declaration. */
  template <typename TList, typename TSet, typename TTypeList>
  struct GetUniqueRecur;

  /* Base case. */
  template <typename TList_, typename TSet_>
  struct GetUniqueRecur<TList_, TSet_, TTypeList<>> : public c14::identity<TList_> {};

  /* Recursive case. */
  template <typename TList_, typename TSet_, typename TElem, typename... TMoreElems>
  struct GetUniqueRecur<TList_, TSet_, TTypeList<TElem, TMoreElems...>> : public Mpl::Conditional<
      std::is_base_of<TNode<TElem>, TSet_>,
      GetUniqueRecur<TList_, TSet_, TTypeList<TMoreElems...>>,
      GetUniqueRecur<TExtend<TList_, TTypeList<TElem>>, TRoot<TSet_, TNode<TElem>>, TTypeList<TMoreElems...>>> {};

  template <typename TList>
  struct GetUnique : public GetUniqueRecur<TTypeList<>, TRoot<>, TList> {};

  template <typename TList>
  using TGetUnique = typename GetUnique<TList>::type;

}  // Mpl
