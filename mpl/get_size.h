/* <mpl/get_size.h>

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

  template <typename TContainer>
  struct GetSize;

  template <typename... TElems>
  struct GetSize<TTypeList<TElems...>> : public std::integral_constant<std::size_t, TTypeList<TElems...>::GetSize()> {};

  template <typename... TElems>
  struct GetSize<TTypeSet<TElems...>> : public GetSize<Mpl::TGetList<TTypeSet<TElems...>>> {};

}  // Mpl
