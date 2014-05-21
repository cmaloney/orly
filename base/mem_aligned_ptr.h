/* <base/mem_aligned_ptr.h>

   Provides a pointer to a piece of memory that is explicitly aligned

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

#include <memory>
#include <type_traits>

#include <base/error_utils.h>

namespace Base {
  template <typename T>
  std::unique_ptr<T> MemAlignedAlloc(std::size_t align_to, std::size_t size) {
    static_assert(std::is_trivial<T>::value, "T must be a trivial type");
    T* mem;
    Base::IfNe0( posix_memalign(
          reinterpret_cast<void**>(&mem), align_to, size) );

    return std::unique_ptr<T>{mem};
  } // MemAlignedAlloc

} // Base
