/* <orly/indy/disk/block_hit_counter.h>

   TODO

   Copyright 2010-2014 OrlyAtomics, Inc.

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
#include <limits>
#include <mutex>

#include <math.h>
#include <string.h>
#include <unistd.h>

#include <base/class_traits.h>
#include <base/error_utils.h>
#include <base/mem_aligned_ptr.h>

namespace Orly {

  namespace Indy {

    namespace Disk {

      /* TODO */
      class TBlockHitCounter {
        NO_COPY(TBlockHitCounter);
        public:

        /* TODO */
        TBlockHitCounter(size_t block_size, size_t num_blocks)
            : BlockSize(block_size),
              NumBlocks(num_blocks),
              WorksetBuf(nullptr),
              FlushBuf(nullptr) {
          WorksetBuf = Base::MemAlignedAllocZeroInitialized<uint8_t>(getpagesize(), GetNumBytes());
          FlushBuf = Base::MemAlignedAllocZeroInitialized<uint8_t>(getpagesize(), GetNumBytes());
        }

        /* TODO */
        inline void AddHits(size_t block_num, size_t num_hits) {
          assert(this);
          std::lock_guard<std::mutex> lock(CountLock);
          uint8_t *val = WorksetBuf.get() + block_num;
          double cur = log(exp(*val) + num_hits);
          *val = std::min(std::numeric_limits<uint8_t>::max(), static_cast<uint8_t>(floor(cur)));
        }

        /* TODO */
        inline uint8_t GetNumHits(size_t block_num) const {
          assert(this);
          std::lock_guard<std::mutex> lock(CountLock);
          return *(WorksetBuf.get() + block_num);
        }

        /* TODO */
        inline void Reset(size_t block_num) {
          assert(this);
          std::lock_guard<std::mutex> lock(CountLock);
          *(WorksetBuf.get() + block_num) = 0;
        }

        private:

        /* TODO */
        inline size_t GetNumBytes() const {
          assert(this);
          return ceil(static_cast<double>(NumBlocks) / BlockSize) * BlockSize;
        }

        /* TODO */
        const size_t BlockSize;

        /* TODO */
        const size_t NumBlocks;

        /* TODO */
        std::unique_ptr<uint8_t> WorksetBuf;

        /* TODO */
        std::unique_ptr<uint8_t> FlushBuf;

        /* TODO */
        mutable std::mutex CountLock;

      };  // TBlockHitCounter

    }  // Disk

  }  // Indy

}  // Orly
