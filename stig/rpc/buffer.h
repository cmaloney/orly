/* <stig/rpc/buffer.h> 

   A buffer used to store I/O data.

   Copyright 2010-2014 Tagged
   
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

#include <base/no_copy_semantics.h>

namespace Stig {

  namespace Rpc {

      /* A 'block' is a single, contiguous extent of memory which data is copied into or out of during an I/O operation.
         Multiple blocks may be strung together in a singly linked list to handle I/O operations of arbitrary size.
         A 'buffer' is a structure which holds a block along with its list linkage pointer.  We want to manage our buffers
         efficiently, so we'll pick a size for our data structure that's friendly to the heap, then adjust the size of the
         block to fit. */
      static constexpr size_t
          BufferSize = 4096,
          BlockSize  = BufferSize - sizeof(void *);

      /* A buffer used to store I/O data. */
      class TBuffer {
        NO_COPY_SEMANTICS(TBuffer);
        public:

        /* The number of buffers (including this one) which form the linked list of which this buffer is the head.
           Never returns zero.  Note that this function walks the list, so cache the result, if possible, to avoid
           calling it repeatedly. */
        size_t GetBufferCount() const noexcept;

        /* The next buffer in the list, or null if we're the last. */
        TBuffer *NextBuffer;

        /* Storage space for I/O data. */
        char Block[BlockSize];

      };  // TBuffer

      /* Let's just make sure our buffer structure is the size we think it is. */
      static_assert(sizeof(TBuffer) == BufferSize, "TBuffer is of incorrect size");

  }  // Rpc

}  // Stig

