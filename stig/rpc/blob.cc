/* <stig/rpc/blob.cc> 

   Implements <stig/rpc/blob.h>.

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

#include <stig/rpc/blob.h>

#include <cstring>
#include <stdexcept>
#include <utility>

using namespace std;
using namespace Stig::Rpc;

TBlob::TReader &TBlob::TReader::Read(void *data, size_t size) {
  assert(this);
  assert(data || !size);
  while (size) {
    /* If we're out of data, throw. */
    if (!Cursor) {
      throw out_of_range("attempt to read past end of Stig RPC blob");
    }
    /* See how much data is available in the current buffer.  If it's more than enough to satisfy the read,
       we'll just finish up and exit the loop. */
    size_t avail = Buffer ? (Buffer->Block + (Buffer->NextBuffer ? BlockSize : Blob->LastBlockSize) - Cursor) : 0;
    if (size < avail) {
      memcpy(data, Cursor, size);
      Cursor += size;
      break;
    }
    /* Copy the available data from this buffer and reduce the remaining read.
       This might reduce the remaining read to zero. */
    memcpy(data, Cursor, avail);
    reinterpret_cast<char *&>(data) += avail;
    size -= avail;
    /* Try to move to the next buffer and, if successful, position the cursor at the start of the new buffer's block. */
    Buffer = Buffer->NextBuffer;
    Cursor = Buffer ? Buffer->Block : nullptr;
  }
  return *this;
}

TBlob::TWriter::TWriter(TBufferPool *buffer_pool) noexcept
    : BufferPool(buffer_pool) {
  assert(buffer_pool);
  BeEmpty();
}

TBlob::TWriter::~TWriter() {
  assert(this);
  CancelBlob();
}

void TBlob::TWriter::CancelBlob() noexcept {
  assert(this);
  BufferPool->FreeBuffers(FirstBuffer);
  BeEmpty();
}

TBlob TBlob::TWriter::DraftBlob() noexcept {
  assert(this);
  TBlob result(BufferPool, FirstBuffer, LastBuffer ? (Cursor - LastBuffer->Block) : 0);
  BeEmpty();
  return move(result);
}

TBlob::TWriter &TBlob::TWriter::Write(const void *data, size_t size) {
  assert(this);
  assert(data || !size);
  /* Determine if we need to allocate more buffers and, if so, where we'll link them on to our list. */
  TBuffer **link;
  if (LastBuffer) {
    size_t avail = LastBuffer->Block + BlockSize - Cursor;
    if (size <= avail) {
      /* The new data fits entirely in the existing space, so we won't need to link on any more buffers. */
      memcpy(Cursor, data, size);
      Cursor += size;
      link = nullptr;
    } else {
      /* The new data doesn't entirely fit, but we'll make a start.  We will need to allocate more buffers,
         and we'll link them to the end of our existing list. */
      memcpy(Cursor, data, avail);
      reinterpret_cast<const char *&>(data) += avail;
      size -= avail;
      link = &(LastBuffer->NextBuffer);
    }
  } else {
    /* We don't have any buffers yet, so we'll definitely need to allocate some.
       We'll link them on as the first buffers in our list. */
    link = &FirstBuffer;
  }
  if (link) {
    /* Allocate enough buffers to hold the remainder of the new data and link them on. */
    TBuffer *buffer = BufferPool->AllocBuffers((size + BlockSize - 1) / BlockSize);
    *link = buffer;
    /* Copy as many whole blocks as we can. */
    while (size > BlockSize) {
      assert(buffer);
      memcpy(buffer->Block, data, BlockSize);
      reinterpret_cast<const char *&>(data) += BlockSize;
      size -= BlockSize;
      buffer = buffer->NextBuffer;
    }
    /* Copy in the last of the new data (a whole or partial block) and get ready for the next write. */
    memcpy(buffer->Block, data, size);
    LastBuffer = buffer;
    Cursor = buffer->Block + size;
  }
  return *this;
}

void TBlob::TWriter::BeEmpty() noexcept {
  assert(this);
  FirstBuffer = nullptr;
  LastBuffer = nullptr;
  Cursor = nullptr;
}

TBlob::TBlob(TBufferPool *buffer_pool, size_t byte_count)
    : BufferPool(buffer_pool) {
  assert(buffer_pool);
  FirstBuffer = buffer_pool->AllocBuffers((byte_count + BlockSize - 1) / BlockSize);
  LastBlockSize = byte_count % BlockSize;
  if (!LastBlockSize) {
    LastBlockSize = BlockSize;
  }
}

bool TBlob::operator==(const TBlob &that) const noexcept {
  assert(this);
  /* Easy outs. */
  if (LastBlockSize != that.LastBlockSize) {
    return false;
  }
  TBuffer
      *this_csr = FirstBuffer,
      *that_csr = that.FirstBuffer;
  if (!this_csr && !that_csr) {
    return true;
  }
  /* Compare all the completely full buffers, exiting immediately if we find a difference. */
  while (this_csr && that_csr && this_csr->NextBuffer && that_csr->NextBuffer) {
    if (memcmp(this_csr->Block, that_csr->Block, BlockSize) != 0) {
      return false;
    }
    this_csr = this_csr->NextBuffer;
    that_csr = that_csr->NextBuffer;
  }
  /* If somebody ran short on buffers, it's a mismatch. */
  if (!this_csr || !that_csr) {
    return false;
  }
  /* Compare the final blocks. */
  return memcmp(this_csr->Block, that_csr->Block, LastBlockSize) == 0;
}

void TBlob::Dump(ostream &strm) const noexcept {
  assert(this);
  for (TBuffer *buffer = FirstBuffer; buffer; buffer = buffer->NextBuffer) {
    strm.write(static_cast<const char *>(buffer->Block), buffer->NextBuffer ? BlockSize : LastBlockSize);
  }
}

iovec *TBlob::InitIoVecs(iovec *cursor, iovec *limit) const noexcept {
  assert(this);
  assert(cursor <= limit);
  assert(cursor || !FirstBuffer);
  TBuffer *buffer;
  for (buffer = FirstBuffer; buffer && cursor < limit; buffer = buffer->NextBuffer, ++cursor) {
    cursor->iov_base = buffer->Block;
    cursor->iov_len  = buffer->NextBuffer ? BlockSize : LastBlockSize;
  }
  return !buffer ? cursor : nullptr;
}

void TBlob::GetCounts(size_t &buffer_count, size_t &byte_count) const noexcept {
  assert(this);
  assert(&buffer_count);
  assert(&byte_count);
  if (FirstBuffer) {
    buffer_count = FirstBuffer->GetBufferCount();
    byte_count = ((buffer_count - 1) * BlockSize) + LastBlockSize;
  } else {
    buffer_count = 0;
    byte_count = 0;
  }
}

