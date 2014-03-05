/* <stig/indy/util/pool.cc>

   Implements <stig/indy/util/pool.h>.

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

#include <stig/indy/util/pool.h>

#include <base/error_utils.h>

#include <cstdint>
#include <cstdlib>
#include <string.h>
#include <sys/mman.h>

using namespace std;
using namespace Stig::Indy::Util;

TPool::TPool(size_t block_size, const char *name, size_t block_count)
    : BlockSize(block_size),
      Blob(nullptr),
      FirstBlock(nullptr),
      Name(name),
      NumBlocksUsed(0UL),
      MaxBlocks(0UL) {
  assert(block_size >= sizeof(void*));
  if (block_count) {
    Init(block_count);
  }
}

void TPool::Init(size_t block_count) {
  assert(this);
  assert(MaxBlocks == 0UL);
  MaxBlocks = block_count;
  if (block_count) {
    Blob = malloc(BlockSize * block_count);
    if (!Blob) {
      syslog(LOG_EMERG, "TPool::Init() [%s] bad_alloc while trying to init pool of %ld blocks [%ld bytes]", Name, block_count, (BlockSize * block_count));
      throw std::bad_alloc();
    }
    Base::IfLt0(mlock(Blob, BlockSize * block_count));
    #ifndef NDEBUG
    memset(Blob, 0, BlockSize * block_count);
    #endif
    syslog(LOG_INFO, "TPool [%s] allocated [%ld] bytes for [%ld] blocks of size [%ld]", Name, (BlockSize * block_count), block_count, BlockSize);
    TBlock
        *prev_block = reinterpret_cast<TBlock *>(&FirstBlock),
        *block      = static_cast<TBlock *>(Blob);
    for (size_t i = 0; i < block_count; ++i) {
      prev_block->NextBlock = block;
      block     ->NextBlock = 0;
      prev_block = block;
      block      = reinterpret_cast<TBlock *>(reinterpret_cast<uint8_t *>(block) + BlockSize);
    }
  }
}

TPool::~TPool() {
  assert(this);
  if (NumBlocksUsed) {
    syslog(LOG_ERR, "[%ld] Blocks left in [%s] pool", NumBlocksUsed, Name);
  }
  free(Blob);
}

void TPool::Free(void *ptr) {
  assert(this);
  assert(ptr);
  TBlock *block = static_cast<TBlock *>(ptr);
  std::lock_guard<std::mutex> lock(Mutex);
  block->NextBlock = FirstBlock;
  --NumBlocksUsed;
  FirstBlock = block;
}

void *TPool::TryAlloc(size_t size) {
  assert(this);
  assert(size <= BlockSize);
  std::lock_guard<std::mutex> lock(Mutex);
  TBlock *block = FirstBlock;
  if (block) {
    FirstBlock = block->NextBlock;
    ++NumBlocksUsed;
  } else {
    FirstBlock = nullptr;
  }
  return block;
}
