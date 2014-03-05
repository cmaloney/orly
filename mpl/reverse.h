/* <mpl/reverse.h>

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
#include <mpl/extend.h>
#include <mpl/type_list.h>

namespace Mpl {

  template <typename TList>
  struct Reverse;

  template <typename TList>
  using TReverse = typename Reverse<TList>::type;

  template <>
  struct Reverse<TTypeList<>> : public c14::identity<TTypeList<>> {};

  template <typename TElem, typename... TMoreElems>
  struct Reverse<TTypeList<TElem, TMoreElems...>>
      : public Extend<TReverse<TTypeList<TMoreElems...>>, TTypeList<TElem>> {};

}  // Mpl
