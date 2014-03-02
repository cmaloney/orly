/* <stig/rpc/buffer_pool.h> 

   A source of I/O buffers.

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

#include <mutex>

#include <base/no_copy_semantics.h>
#include <stig/rpc/buffer.h>

namespace Stig {

  namespace Rpc {

    /* A source of I/O buffers. */
    class TBufferPool final {
      NO_COPY_SEMANTICS(TBufferPool);
      public:

      /* Start out with the given number of buffers available. */
      explicit TBufferPool(size_t preallocate_count = 0);

      /* Releases all available buffers. */
      ~TBufferPool();

      /* Allocate the given number of buffers as a linked list.  (If you request 0 buffers, you'll get a null pointer.)
         If there's are unused buffers available, they will be reused now.  If not, we will try to allocate new buffers
         from the heap.  If that fails, we throw std::bad_alloc(), so we never return null if buffer_count is non-zero.
         When you are done with the buffers, make sure you return it by calling FreeBuffers().  This function is
         synchronized against itself and FreeBuffers(), so you may call it simultaneously from multiple threads. */
      TBuffer *AllocBuffers(size_t buffer_count = 1);

      /* Return the given linked list of buffers to the available list.  Once freed, a buffer may immediately be
         reused by another thread, so don't make any reference to a buffer after you've freed it.  It's ok to pass
         to this function; it's just ignored.  This function is synchronized against itself and AllocBuffer(), so
         you may call it simultaneously from multiple threads. */
      void FreeBuffers(TBuffer *buffer) noexcept;

      private:

      /* Returns a linked list of buffers (of which the given buffer is the head) to the heap.
         Passing a null pointer here is ok; the function will simply do nothing. */
      static void DeleteBuffers(TBuffer *&buffer) noexcept;

      /* Allocates a linked list of new buffers from the heap, linking them to the head of the list of which the given
         buffer is the head.  (If you pass a null here, the returned list will not be linked to an existing one and will
         instead end with a null link.)  The function returns the new list by updating the given pointer to point to its
         head. */
      static void NewBuffers(TBuffer *&buffer, size_t buffer_count);

      /* Covers FirstAvailableBuffer, below. */
      std::mutex Mutex;

      /* The first in a linked list of buffers available to be reused.
         This is null iff. we currently have no available buffers.
         AllocBuffer() will pull a buffer from this list if it can.
         FreeBuffers() returns buffers to this list. */
      TBuffer *FirstAvailableBuffer;

    };  // TBufferPool

  }  // Rpc

}  // Stig

