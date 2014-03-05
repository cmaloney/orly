/* <mpl/contains.h>

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

#include <mpl/type_list.h>
#include <mpl/type_set.h>

namespace Mpl {

  /* Forward declaration. */
  template <typename TContainer, typename TElem>
  struct Contains;

  /* Test if the TElem is already in the TTypeSet<>. */
  template <typename... TElems, typename TElem>
  struct Contains<TTypeSet<TElems...>, TElem> : public std::is_base_of<TNode<TElem>, typename TTypeSet<TElems...>::TSet> {};

  /* Construct a set and defer to Contains<> on TTypeSet. */
  template <typename... TElems, typename TElem>
  struct Contains<TTypeList<TElems...>, TElem> : public Contains<TTypeSet<TElems...>, TElem> {};

}  // Mpl
