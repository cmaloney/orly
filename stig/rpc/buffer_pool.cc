/* <stig/rpc/buffer_pool.cc> 

   Implements <stig/rpc/buffer_pool.h>.

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

#include <stig/rpc/buffer_pool.h>

#include <cassert>
#include <cstdlib>
#include <new>

using namespace std;
using namespace Stig::Rpc;

TBufferPool::TBufferPool(size_t preallocate_count)
    : FirstAvailableBuffer(nullptr) {
  NewBuffers(FirstAvailableBuffer, preallocate_count);
}

TBufferPool::~TBufferPool() {
  assert(this);
  DeleteBuffers(FirstAvailableBuffer);
}

TBuffer *TBufferPool::AllocBuffers(size_t buffer_count) {
  assert(this);
  /* Checking for the degenerate case here isn't semantically necessary, but
     it save us the overhead of unnecessarily nabbing the mutex. */
  TBuffer *buffer = nullptr;
  if (buffer_count) {
    /* extra */ {
      /* Satisfy as much of the request as possible by unlinking buffers from
         the available list. */
      lock_guard<mutex> lock(Mutex);
      while (FirstAvailableBuffer && buffer_count) {
        /* Unlink the first buffer from the available list. */
        TBuffer *reused_buffer = FirstAvailableBuffer;
        FirstAvailableBuffer = reused_buffer->NextBuffer;
        /* Link the buffer to the list we're going to return. */
        reused_buffer->NextBuffer = buffer;
        buffer = reused_buffer;
        /* We now need one less buffer. */
        --buffer_count;
      }
    }
    /* If we need more buffers, we'll have to allocate them from the heap. */
    NewBuffers(buffer, buffer_count);
  }
  return buffer;
}

void TBufferPool::FreeBuffers(TBuffer *buffer) noexcept {
  assert(this);
  if (buffer) {
    /* Synchronize. */
    lock_guard<mutex> lock(Mutex);
    do {
      /* Cache a pointer to the next buffer to free. */
      TBuffer *next_buffer = buffer->NextBuffer;
      /* Link this buffer to the head of the available list. */
      buffer->NextBuffer = FirstAvailableBuffer;
      FirstAvailableBuffer = buffer;
      /* Move to the next buffer to free. */
      buffer = next_buffer;
    } while (buffer);
  }
}

void TBufferPool::DeleteBuffers(TBuffer *&buffer) noexcept {
  /* Loop over the list, freeing each buffer. */
  while (buffer) {
    TBuffer *next_buffer = buffer->NextBuffer;
    free(buffer);
    buffer = next_buffer;
  }
}

void TBufferPool::NewBuffers(TBuffer *&buffer, size_t buffer_count) {
  assert(&buffer);
  try {
    /* Loop until we allocate enough buffers. */
    for (; buffer_count; --buffer_count) {
      /* Allocate the memory, or die trying. */
      auto *new_buffer = static_cast<TBuffer *>(malloc(BufferSize));
      if (!new_buffer) {
        throw bad_alloc();
      }
      /* Link the buffer to the list, leaving junk in the buffer's data page. */
      new_buffer->NextBuffer = buffer;
      buffer = new_buffer;
    }
  } catch (...) {
    /* In the event of error, return all buffers in the list to the heap.  This
       will include any which were part of the list we were passed to start with. */
    DeleteBuffers(buffer);
    throw;
  }
}

