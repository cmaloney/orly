/* <stig/rpc/blob.h> 

   A blob of data sent or received by the I/O system.

   The blob stores its data in discontiguous buffers allocated from a buffer pool.
   The blob returns these buffers to the pool when it's done with them, so memory
   use is efficient.

   You can't copy a blob, but you can move it.  It actually wouldn't be difficult
   to implement copy semantics for blobs, but the point of a blob is to avoid
   making unnecessary copies of data.  A copy-constructor and/or assignment operator
   would undermine this purpose.

   Use TBlob::TWriter to build a blob incrementally.
   You can stream into it, like this:

      TBufferPool buffer_pool;
      int a = 101, b = 202;
      TBlob blob = (TBlob::TWriter(&buffer_pool) << a << " + " << b << " = " << (a + b)).DraftBlob();

   Use TBlob::TReader to get sequential access to the data in a blob.
   You can stream out of it like this:

      int a, b, sum;
      std::string str1, str2;
      TBlob::TReader(&blob) >> a >> str >> b >> str2 >> sum;

   The reader and writer have streaming operator overloads for all the built-in types
   (except wchar_t), Base::TUuid, c-string (writer only), std::string, std::pair, std::tuple,
   std::chrono::duration, std::chrono::time point, std::list, std::vector, std::set,
   std::multiset, std::map, std::multimap,  std::unordered_set, std::unordered_multiset,
   std::unordered_map, and std::unordered_multimap.

   The streamers for the multi-byte integer types produce and require their data in network
   byte order.

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

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <utility>

#include <sys/uio.h>

#include <base/no_copy_semantics.h>
#include <stig/rpc/buffer.h>
#include <stig/rpc/buffer_pool.h>

namespace Stig {

  namespace Rpc {

    /* A blob of data sent or received by the I/O system. */
    class TBlob final {
      public:

      /* This is the type used when streaming element counts into or out of blobs.  The element count is
         preprepended to the elements themselves.  For example, when streaming a string, you'll find the
         number of bytes in the string, followed by the string itself.  This is also true for lists, vectors,
         sets, maps, etc. */
      using TElemCount = uint32_t;

      /* Provides sequential access to the data in a blob.  This object provides a single forward
         pass over the blob.  A reader doesn't allow rewinding; however, readers can be cheaply copied.
         Feel free to do so if you need to read ahead speculatively while keeping your finger on a
         particular position within a blob. */
      class TReader final {
        public:

        /* Construct a reader to read the data from the given blob.  The reader does NOT make its
           own copy of the blob's data, so you must make sure that the blob continues to exist as
           long as it has readers. */
        explicit TReader(const TBlob *blob) noexcept {
          assert(blob);
          Blob = blob;
          Buffer = blob->FirstBuffer;
          Cursor = Buffer ? Buffer->Block : nullptr;
        }

        /* Shallow copy is ok. */
        TReader(const TReader &that) = default;

        /* Shallow copy is ok. */
        TReader &operator=(const TReader &) = default;

        /* True iff. we have not yet reached the end of the blob. */
        operator bool() const noexcept {
          assert(this);
          return Cursor;
        }

        /* Copy data from the blob and advance our position.  If you attempt to read past the end
           of the blob, this function throws std::out_of_range.  Note that a read of zero bytes is
           always safe. */
        TReader &Read(void *data, size_t size);

        private:

        /* The blob we are reading. */
        const TBlob *Blob;

        /* The buffer from which we are currently reading.  Null iff. we have reached the end. */
        TBuffer *Buffer;

        /* Our position within our current buffer.  Null iff. we have reached the end. */
        char *Cursor;

      };  // TBlob::TReader

      /* Builds blobs incrementally.  Construct a writer by giving a pool from which to draw buffers.
         Then you call Write() zero or more times to copy data into the writer.  The writer will allocate
         buffers from the pool as required.  When you have finished writing, call DraftBlob() to return
         your data in blob form.  The writer is then ready to be used again.  If you wish to reset
         the writer without constructing a blob, call CancelBlob(). */
      class TWriter final {
        NO_COPY_SEMANTICS(TWriter);
        public:

        /* A newly created writer has no data. */
        TWriter(TBufferPool *buffer_pool) noexcept;

        /* Any pending blob will be canceled automatically. */
        ~TWriter();

        /* Drop any data we've written so far and return to the default-constructed state. */
        void CancelBlob() noexcept;

        /* Return the blob we've built and reset to the default-constructed state. */
        TBlob DraftBlob() noexcept;

        /* Copy the given data to the end of the blob we're building. */
        TWriter &Write(const void *data, size_t size);

        private:

        /* Returns the writer to the empty state.  This simply nulls out FirstBuffer, LastBuffer,
           and Cursor without freeing anything, so make sure ownership of the data has been
           transferred to some other structure before you call this function. */
        void BeEmpty() noexcept;

        /* The pool which we allocate buffers. */
        TBufferPool *BufferPool;

        /* The first and last buffers in our linked list.  These are both null iff. we're empty.
           If we're non-empty but, both pointers will be non-null.  If all our data fits in one
           chunk, both pointers will point to the same buffer.  If we use two or more chunks,
           then these pointers will point to different buffers. */
        TBuffer *FirstBuffer, *LastBuffer;

        /* The position with in our LastBuffer's chuck where Write() will append more data.
           This is null iff. LastBuffer is null. */
        char *Cursor;

      };  // TBlob::TWriter

      /* Construct an empty blob. */
      TBlob() noexcept
          : BufferPool(nullptr), FirstBuffer(nullptr), LastBlockSize(0) {}

      /* Construct a blob attached to the given buffer pool and consisting of the given number
         of bytes of junk data.  You can use this to create a buffer into which to scatter-read. */
      TBlob(TBufferPool *buffer_pool, size_t byte_count);

      /* Move the data from that blob into a new one, leaving that blob empty. */
      TBlob(TBlob &&that) noexcept
          : TBlob() {
        Swap(that);
      }

      /* No copying allowed. */
      TBlob(const TBlob &) = delete;

      /* Free our buffers, returning them to the pool. */
      ~TBlob() {
        assert(this);
        if (BufferPool) {
          BufferPool->FreeBuffers(FirstBuffer);
        }
      }

      /* True iff. this blob is non-empty. */
      operator bool() const noexcept {
        assert(this);
        return FirstBuffer;
      }

      /* Move the data from that blob into this one, leaving that blob empty. */
      TBlob &operator=(TBlob &&that) noexcept {
        assert(this);
        TBlob temp(std::move(*this));
        return Swap(that);
      }

      /* No copying allowed. */
      TBlob &operator=(const TBlob &) = delete;

      /* Compare the contents of this blob with the contents of that one. */
      bool operator==(const TBlob &that) const noexcept;

      /* Compare the contents of this blob with the contents of that one. */
      bool operator!=(const TBlob &that) const noexcept {
        assert(this);
        return !(*this == that);
      }

      /* Dump the contents of the blob to the given std stream. */
      void Dump(std::ostream &strm) const noexcept;

      /* Initialze an array of iovec structures to point to the blocks containing this blob's data.
         If there are enough elements in the array, this function returns a pointer to the element immediately
         after the last one it initialized.  If there are not enough elements in the array, this function
         initializes all available elements and returns null.  Use GetCounts() to find out size of the iovec
         array that you require.  You'll need one element per buffer in the blob. */
      iovec *InitIoVecs(iovec *cursor, iovec *limit) const noexcept;

      /* The total number of buffers and bytes in this blob.  These will be zero iff. the blob is empty.
         Note that this function walks the blob's linked list of buffers, so, if possible, cache the results
         to avoid calling it repeatedly. */
      void GetCounts(size_t &buffer_count, size_t &byte_count) const noexcept;

      /* Return this blob to the default-constructed state; that is, empty, but still connected to the buffer pool. */
      TBlob &Reset() noexcept {
        assert(this);
        TBlob temp(std::move(*this));
        return *this;
      }

      /* Swap this blob with that one and return this one (which is now that one).
         The two blobs can be connected to different buffer pools; those connections are swapped, too. */
      TBlob &Swap(TBlob &that) noexcept {
        assert(this);
        std::swap(BufferPool, that.BufferPool);
        std::swap(FirstBuffer, that.FirstBuffer);
        std::swap(LastBlockSize, that.LastBlockSize);
        return *this;
      }

      private:

      /* Used by TWrite::DraftBlob().  We just cache all the given parameters, which means we end up
         attached to the same buffer pool as the writer and we take over ownership of the writer's current buffers. */
      TBlob(TBufferPool *buffer_pool, TBuffer *first_buffer, size_t last_block_size) noexcept
          : BufferPool(buffer_pool), FirstBuffer(first_buffer), LastBlockSize(last_block_size) {
        assert(buffer_pool);
        assert(first_buffer || !last_block_size);
      }

      /* The pool to which we will return our buffers. */
      TBufferPool *BufferPool;

      /* The first buffer in our linked list. */
      TBuffer *FirstBuffer;

      /* The number of bytes used in the last buffer's block.  All other buffers are completely full. */
      size_t LastBlockSize;

    };  // TBlob

    /* Std stream inserter for blobs.  This is really just to make unit tests read better. */
    inline std::ostream &operator<<(std::ostream &strm, const Stig::Rpc::TBlob &that) {
      assert(&that);
      that.Dump(strm);
      return strm;
    }

  }  // Rpc

}  // Stig

