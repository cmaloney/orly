/* <base/mem_aligned_ptr.h>

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

#include <base/class_traits.h>
#include <base/error_utils.h>

namespace Base {

  template <typename T>
  class TMemAlignedPtr {
    NO_COPY(TMemAlignedPtr)
    NO_MOVE(TMemAlignedPtr)
    public:
      constexpr TMemAlignedPtr() = default;

      TMemAlignedPtr(std::size_t align_to, std::size_t size) {
        Allocate(align_to, size);
      }

      ~TMemAlignedPtr() {
        Reset(nullptr);
      }

      void Allocate(std::size_t align_to, std::size_t size) {
        T* tmp;
        Base::IfNe0( posix_memalign(
              reinterpret_cast<void**>(&tmp), align_to, size) );
        Reset(tmp);
      }

      T* Get() const noexcept{return Mem;}

      T& operator[](std::size_t n) noexcept{ assert(Mem);return Mem[n]; }

      const T& operator[](std::size_t n) const noexcept{ assert(Mem);return Mem[n]; }

      operator T*() const noexcept{return Get();}
    private:
      T* Mem = nullptr;
      void Reset(T* new_mem) {
        if (Mem) {
          free(Mem);
        }
        Mem = new_mem;
      }
  }; // TMemAlignedPtr

} // Base
