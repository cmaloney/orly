/* <stig/indy/util/growing_pool.cc>

   Implements <stig/indy/util/growing_pool.h>.

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

#include <stig/indy/util/growing_pool.h>

#include <base/error_utils.h>

#include <cstdint>
#include <cstdlib>
#include <string.h>
#include <sys/mman.h>

using namespace std;
using namespace Stig::Indy::Util;

TGrowingPool::TGrowingPool(size_t block_size, const char *name)
    : BlockSize(block_size),
      Blob(nullptr),
      FirstBlock(nullptr),
      Name(name),
      NumBlocksUsed(0UL),
      MaxBlocks(0UL) {
  assert(block_size >= sizeof(void*));
}

TGrowingPool::~TGrowingPool() {
  assert(this);
  if (NumBlocksUsed) {
    syslog(LOG_ERR, "[%ld] Blocks left in [%s] pool", NumBlocksUsed, Name);
  }
  for (TBlock *block = FirstBlock; block;) {
    TBlock *to_free = block;
    block = block->NextBlock;
    free(to_free);
  }
}

void TGrowingPool::Free(void *ptr) {
  assert(this);
  assert(ptr);
  TBlock *block = static_cast<TBlock *>(ptr);
  std::lock_guard<std::mutex> lock(Mutex);
  block->NextBlock = FirstBlock;
  --NumBlocksUsed;
  FirstBlock = block;
}

void *TGrowingPool::TryAlloc(size_t size) {
  assert(this);
  assert(size <= BlockSize);
  std::lock_guard<std::mutex> lock(Mutex);
  TBlock *block = FirstBlock;
  if (block) {
    FirstBlock = block->NextBlock;
    ++NumBlocksUsed;
  } else {
    syslog(LOG_INFO, "Allocating new GrowingPool block [%s] of size [%ld]", Name, BlockSize);
    block = reinterpret_cast<TBlock *>(malloc(BlockSize));
    if (!block) {
      syslog(LOG_EMERG, "TGrowingPool::TryAlloc() [%s] bad_alloc while trying to allocate block [%ld bytes]", Name, BlockSize);
      throw std::bad_alloc();
    }
    Base::IfLt0(mlock(block, BlockSize));
    #ifndef NDEBUG
    memset(block, 0, BlockSize);
    #endif
    ++NumBlocksUsed;
    ++MaxBlocks;
    FirstBlock = nullptr;
  }
  return block;
}
