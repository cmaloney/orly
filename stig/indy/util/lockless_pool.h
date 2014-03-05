/* <stig/indy/util/lockless_pool.h>

   TODO

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

#pragma once

#include <cassert>
#include <cstddef>
#include <new>
#include <syslog.h>

#include <base/no_copy_semantics.h>

namespace Stig {

  namespace Indy {

    namespace Util {

      /* TODO */
      class TLocklessPool {
        NO_COPY_SEMANTICS(TLocklessPool);
        public:

        /* TODO */
        TLocklessPool(size_t block_size, const char *name, size_t block_count = 0UL);

        /* TODO */
        ~TLocklessPool();

        /* TODO */
        void Init(size_t block_count);

        /* TODO */
        inline const char *GetName() const;

        /* TODO */
        inline size_t GetNumBlocksUsed() const;

        /* TODO */
        inline size_t GetMaxBlocks() const;

        /* TODO */
        void *Alloc(size_t size) {
          assert(this);
          void *ptr = TryAlloc(size);
          if (!ptr) {
            syslog(LOG_EMERG, "TLocklessPool::Alloc() [%s] bad_alloc", Name);
            throw std::bad_alloc();
          }
          ++NumBlocksUsed;
          return ptr;
        }

        /* TODO */
        inline void Free(void *ptr);

        /* TODO */
        inline void *TryAlloc(size_t size);

        private:

        /* TODO */
        class TBlock {
          NO_COPY_SEMANTICS(TBlock);
          public:

          /* TODO */
          TBlock *NextBlock;

        };  // TBlock

        /* TODO */
        const size_t BlockSize;

        /* TODO */
        void *Blob;

        /* TODO */
        TBlock *FirstBlock;

        /* TODO */
        const char *Name;

        /* TODO */
        size_t NumBlocksUsed;

        /* TODO */
        size_t MaxBlocks;

      };  // TLocklessPool

      inline const char *TLocklessPool::GetName() const {
        assert(this);
        return Name;
      }

      inline size_t TLocklessPool::GetNumBlocksUsed() const {
        assert(this);
        return NumBlocksUsed;
      }

      inline size_t TLocklessPool::GetMaxBlocks() const {
        assert(this);
        return MaxBlocks;
      }

      inline void TLocklessPool::Free(void *ptr) {
        assert(this);
        assert(ptr);
        TBlock *block = static_cast<TBlock *>(ptr);
        block->NextBlock = FirstBlock;
        --NumBlocksUsed;
        FirstBlock = block;
      }

      inline void *TLocklessPool::TryAlloc(size_t size) {
        assert(this);
        assert(size <= BlockSize);
        TBlock *block = FirstBlock;
        FirstBlock = (block ? block->NextBlock : 0);
        return block;
      }

    }  // Util

  }  // Indy

}  // Stig
