/* <util/struct.h>

   Utilities for working with structures.

   Copyright 2010-2014 OrlyAtomics, Inc.

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

#include <cstddef>

/* A typesafe replacement for the C-style offsetof(). */
#define OFFSET_OF(st, m) ::Util::OffsetOf(&st::m)

/* A typesafe replacement for the C-style containerof(). */
#define CONTAINER_OF(p, st, m) ::Util::ContainerOf(p, &st::m)

namespace Util {

  /* The offset (in bytes) of a member of a struct. */
  template <typename TStruct, typename TVal>
  constexpr size_t OffsetOf(TVal (TStruct::*member)) {
    return reinterpret_cast<size_t>(&(static_cast<TStruct *>(nullptr)->*member));
  }

  /* A pointer to the struct which contains the given member. */
  template <typename TStruct, typename TVal>
  TStruct *ContainerOf(TVal *val, TVal (TStruct::*member)) {
    return reinterpret_cast<TStruct *>(reinterpret_cast<char *>(val) - OffsetOf(member));
  }

}  // Base
