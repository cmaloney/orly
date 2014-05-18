/* <base/mem_aligned_ptr.h>
 
  Defines a scoped piece of memory that is posix_memaligned.
  This object assumes the ctor and dtor will be called properly and that the pointer contained was allocated via posix_memalign.
  To use you need to call TMemAlignedPtr::Allocate with the memory properties you want, this can be done from the two parameter ctor.
  The memory is cleaned upon scope exit.

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

#include <cassert>

#include <type_traits>

#include <base/class_traits.h>
#include <base/error_utils.h>

namespace Base {

  template <typename T>
  class TMemAlignedPtr {
    static_assert(std::is_trivial<T>::value, "T must be a trivial type");
    MOVE_ONLY(TMemAlignedPtr)

    public:

    constexpr TMemAlignedPtr() = default;

    TMemAlignedPtr(std::size_t align_to, std::size_t size) {
      Allocate(align_to, size);
    }

    ~TMemAlignedPtr() {
      Set(nullptr);
    }

    void Allocate(std::size_t align_to, std::size_t size) {
      T* tmp;
      Base::IfNe0( posix_memalign(
            reinterpret_cast<void**>(&tmp), align_to, size) );
      Set(tmp);
    }

    T* Get() const noexcept{ assert(Mem);return Mem; }

    T& operator[](std::size_t n) noexcept{ assert(Mem);return Mem[n]; }

    const T& operator[](std::size_t n) const noexcept{ assert(Mem);return Mem[n]; }

    T& operator*() const { assert(Mem);return *Mem; }

    T* operator->() const noexcept{ return Get(); }

    operator T*() const noexcept{ return Get(); }

    private:

    T* Mem = nullptr;

    void Set(T* new_mem) {
      if (Mem) {
        free(Mem);
      }
      Mem = new_mem;
    }
  }; // TMemAlignedPtr

} // Base
