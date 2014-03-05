/* <mpl/sort.h>

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
#include <c14/utility.h>

#include <mpl/get_size.h>
#include <mpl/get_slice.h>
#include <mpl/merge.h>
#include <mpl/type_list.h>

namespace Mpl {

  template <template <typename, typename> class TCompare, typename TList>
  struct Sort;

  template <template <typename, typename> class TCompare, typename TList>
  using TSort = typename Sort<TCompare, TList>::type;

  template <template <typename, typename> class TCompare>
  struct Sort<TCompare, TTypeList<>> : public c14::identity<TTypeList<>> {};

  template <template <typename, typename> class TCompare, typename TElem>
  struct Sort<TCompare, TTypeList<TElem>> : public c14::identity<TTypeList<TElem>> {};

  template <template <typename, typename> class TCompare, typename TList>
  struct Sort : public Merge<
      TCompare,
      TSort<TCompare, TGetSlice<TList, c14::make_index_range<0, GetSize<TList>::value / 2>>>,
      TSort<TCompare, TGetSlice<TList, c14::make_index_range<GetSize<TList>::value / 2, GetSize<TList>::value>>>> {};

}  // Mpl
