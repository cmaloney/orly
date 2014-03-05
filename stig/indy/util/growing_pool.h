/* <stig/indy/util/growing_pool.h>

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
#include <mutex>
#include <new>
#include <time.h>
#include <syslog.h>

#include <base/no_copy_semantics.h>

namespace Stig {

  namespace Indy {

    namespace Util {

      /* TODO */
      class TGrowingPool {
        NO_COPY_SEMANTICS(TGrowingPool);
        public:

        /* TODO */
        TGrowingPool(size_t block_size, const char *name);

        /* TODO */
        ~TGrowingPool();

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
            syslog(LOG_EMERG, "TGrowingPool::Alloc() [%s] bad_alloc", Name);
            throw std::bad_alloc();
          }
          return ptr;
        }

        /* TODO */
        void Free(void *ptr);

        /* TODO */
        void *TryAlloc(size_t size);

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
        std::mutex Mutex;

        /* TODO */
        const char *Name;

        /* TODO */
        size_t NumBlocksUsed;

        /* TODO */
        size_t MaxBlocks;

      };  // TGrowingPool

      inline const char *TGrowingPool::GetName() const {
        assert(this);
        return Name;
      }

      inline size_t TGrowingPool::GetNumBlocksUsed() const {
        assert(this);
        return NumBlocksUsed;
      }

      inline size_t TGrowingPool::GetMaxBlocks() const {
        assert(this);
        return MaxBlocks;
      }

    }  // Util

  }  // Indy

}  // Stig
