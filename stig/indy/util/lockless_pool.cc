/* <stig/indy/util/lockless_pool.cc>

   Implements <stig/indy/util/lockless_pool.h>.

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

#include <stig/indy/util/lockless_pool.h>

#include <cstdint>
#include <cstdlib>

using namespace std;
using namespace Stig::Indy::Util;

TLocklessPool::TLocklessPool(size_t block_size, const char *name, size_t block_count)
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

void TLocklessPool::Init(size_t block_count) {
  assert(this);
  assert(MaxBlocks == 0UL);
  MaxBlocks = block_count;
  if (block_count) {
    Blob = malloc(BlockSize * block_count);
    if (!Blob) {
      syslog(LOG_EMERG, "TLocklessPool::Init() [%s] bad_alloc while trying to init pool of %ld blocks [%ld bytes]", Name, block_count, (BlockSize * block_count));
      throw std::bad_alloc();
    }
    syslog(LOG_INFO, "TLocklessPool [%s] allocated [%ld] bytes for [%ld] blocks of size [%ld]", Name, (BlockSize * block_count), block_count, BlockSize);
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

TLocklessPool::~TLocklessPool() {
  assert(this);
  free(Blob);
}
