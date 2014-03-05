/* <stig/indy/disk/write_group.h>

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

#include <base/no_copy_semantics.h>
#include <inv_con/unordered_list.h>
#include <stig/indy/disk/buf_block.h>
#include <stig/indy/disk/result.h>
#include <stig/indy/disk/util/volume_manager.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      /* Forward Declarations. */
      class TController;

      template <size_t PageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind>
      class TOutStream;

      namespace Util {
        template <typename TOwner, typename TVal, size_t MemSize, class TComparator/* = std::less<TVal>*/>
        class TIndexSortFile;
      }  // Util

      /* TODO */
      class TWriteGroup {
        NO_COPY_SEMANTICS(TWriteGroup);
        protected:

        /* Forward Declarations. */
        class TBufferedWrite;

        /* TODO */
        typedef InvCon::UnorderedList::TCollection<TWriteGroup, TBufferedWrite> TWriteCollection;

        /* TODO */
        class TBufferedWrite {
          NO_COPY_SEMANTICS(TBufferedWrite);
          public:

          /* TODO */
          typedef InvCon::UnorderedList::TMembership<TBufferedWrite, TWriteGroup> TMembership;

          /* TODO */
          TBufferedWrite(TWriteGroup *write_group, size_t block_id, const TBufBlock *buf)
              : Membership(this, &write_group->WriteCollection),
                BlockId(block_id),
                Buf(buf) {}

          /* TODO */
          ~TBufferedWrite() {}

          /* TODO */
          const TBufBlock *GetBuf() const {
            assert(this);
            return Buf;
          }

          /* TODO */
          size_t GetBlockId() const {
            assert(this);
            return BlockId;
          }

          private:

          /* TODO */
          TMembership::TImpl Membership;

          /* TODO */
          size_t BlockId;

          /* TODO */
          const TBufBlock *Buf;

          /* TODO */
          friend class TWriteGroup;

        };  // TBufferedWrite

        /* TODO */
        TWriteGroup(size_t max_group_size)
            : WriteCollection(this),
              QueueSize(0UL),
              MaxGroupSize(max_group_size) {}

        /* TODO */
        virtual ~TWriteGroup() {}

        /* TODO */
        virtual void Flush() {
          assert(this);
          WriteCollection.DeleteEachMember();
          QueueSize = 0UL;
        }

        /* TODO */
        virtual size_t GetLogicalBlockSize() const = 0;

        /* TODO */
        virtual size_t GetPhysicalBlockSize() const = 0;

        /* TODO */
        size_t GetSize() const {
          assert(this);
          return QueueSize;
        }

        /* TODO */
        void Append(size_t cur_block_id, const TBufBlock *buf) {
          assert(this);
          auto handle = WriteCollection.TryGetLastMember();
          if (!handle) {
            /* it's empty, let's add ourselves */
            ++QueueSize;
            new TBufferedWrite(this, cur_block_id, buf);
          } else {
            /* it's not empty. we should make sure we sequentially follow the last block;
               otherwise flush and start fresh */
            if (handle->BlockId + 1 == cur_block_id) {
              /* append */
              ++QueueSize;
              new TBufferedWrite(this, cur_block_id, buf);
            } else {
              /* flush and start fresh */
              Flush();
              QueueSize = 1UL;
              new TBufferedWrite(this, cur_block_id, buf);
            }
          }
          if (QueueSize == MaxGroupSize) {
            Flush();
          }
        }

        private:

        /* TODO */
        mutable TWriteCollection::TImpl WriteCollection;

        /* TODO */
        size_t QueueSize;

        /* TODO */
        size_t MaxGroupSize;

        /* TODO */
        friend class TController;
        friend class TService;
        template <size_t PageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind>
        friend class TOutStream;

        template <typename TOwner, typename TVal, size_t MemSize, class TComparator>
        friend class Util::TIndexSortFile;

      };  // TWriteGroup

    }  // Disk

  }  // Indy

}  // Stig