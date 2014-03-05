/* <stig/indy/disk/data_file.h>

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

#include <base/no_copy_semantics.h>
#include <stig/atom/kit2.h>
#include <stig/indy/disk/in_file.h>
#include <stig/indy/disk/out_stream.h>
#include <stig/indy/disk/util/cache.h>
#include <stig/indy/disk/util/engine.h>
#include <stig/indy/disk/util/index_manager.h>
#include <stig/indy/disk/util/volume_manager.h>
#include <stig/indy/util/block_vec.h>
#include <stig/indy/memory_layer.h>
#include <stig/sabot/all.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      /* TODO */
        class TRemapObj {
        public:

        /* TODO */
        TRemapObj(Atom::TCore::TArena *arena, Atom::TCore::TOffset old_key, Atom::TCore::TOffset new_key)
            : Arena(arena),
              OldKey(old_key),
              NewKey(new_key) {}

        /* TODO */
        bool operator==(const TRemapObj &that) const {
          assert(this);
          assert(&that);
          return Arena == that.Arena && OldKey == that.OldKey;
        }

        /* TODO */
        bool operator!=(const TRemapObj &that) const {
          assert(this);
          assert(&that);
          return Arena != that.Arena || OldKey != that.OldKey;
        }

        /* TODO */
        size_t GetHash() const {
          assert(this);
          return reinterpret_cast<size_t>(Arena) ^ OldKey;
        }

        /* TODO */
        Atom::TCore::TArena *Arena;

        /* TODO */
        Atom::TCore::TOffset OldKey;

        /* TODO */
        Atom::TCore::TOffset NewKey;

      };  // TRemapObj

    }  // Disk

  }  // Indy

}  // Stig

namespace std {

  /* A standard hasher for Stig::Indy::Disk::TRemapObj. */
  template <>
  struct hash<Stig::Indy::Disk::TRemapObj> {
    size_t operator()(const Stig::Indy::Disk::TRemapObj &that) const {
      return that.GetHash();
    }

    typedef size_t result_type;
    typedef Stig::Indy::Disk::TRemapObj argument_type;
  };

}  // std


namespace Stig {

  namespace Indy {

    namespace Disk {

      /* TODO */
      class TDataFile {
        NO_COPY_SEMANTICS(TDataFile);
        public:

        template <size_t LocalCacheSize>
        using TDataInStream = TStream<Disk::Util::LogicalBlockSize, Disk::Util::LogicalBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::PageCheckedBlock, LocalCacheSize>;
        typedef TOutStream<Disk::Util::LogicalPageSize, Disk::Util::LogicalBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::PageCheckedBlock> TDataOutStream;

        /* TODO */
        class TUpdateObj {
          public:

          /* TODO */
          TUpdateObj(TSequenceNumber seq_num, const Atom::TCore &metadata, const Atom::TCore &id, size_t offset, bool is_current, const Base::TUuid &index_id)
              : SequenceNumber(seq_num),
                Metadata(metadata),
                Id(id),
                Offset(offset),
                IsCurrent(is_current),
                IndexId(index_id) {}

          /* TODO */
          bool operator<(const TUpdateObj &that) const {
            assert(this);
            assert(&that);
            return SequenceNumber < that.SequenceNumber;
          }

          /* TODO */
          TSequenceNumber SequenceNumber;

          /* TODO */
          Atom::TCore Metadata;

          /* TODO */
          Atom::TCore Id;

          /* TODO */
          size_t Offset;

          /* TODO */
          bool IsCurrent;

          /* TODO */
          Base::TUuid IndexId;

        };  // TUpdateObj

        /* TODO */
        typedef Disk::Util::TIndexManager<TUpdateObj, Util::SortBufSize, Util::SortBufMinParallelSize> TUpdateCollector;
        typedef std::unordered_set<TRemapObj> TRemapIndex;
        typedef Indy::Util::TBlockVec TBlockVec;
        typedef std::vector<size_t> TTypeBoundaryOffsetVec;

        /* SequenceNumber, Key, Value, Num History Keys, Offset of History Keys */
        static const size_t KeyEntrySize = sizeof(TSequenceNumber) + sizeof(Atom::TCore) + sizeof(Atom::TCore) + sizeof(size_t) + sizeof(size_t);
        /* SequenceNumber, Key, Value */
        static const size_t KeyHistorySize = sizeof(TSequenceNumber) + sizeof(Atom::TCore) + sizeof(Atom::TCore);
        /* Key, offset */
        static const size_t HashEntrySize = sizeof(Atom::TCore) + sizeof(size_t);
        /* sequence number, metadata, id, bucket offset, num in bucket */
        static const size_t UpdateBucketEntrySize = sizeof(TSequenceNumber) + sizeof(Atom::TCore) + sizeof(Atom::TCore) + sizeof(size_t) + sizeof(size_t);
        /* ptr to key */
        static const size_t UpdateKeyPtrSize = sizeof(size_t);
        /*
        # of blocks
        # of meta-blocks (n)
        # of #block / block_id pairings (number of sequential blocks starting at) (m)
        # updates
        # index segments (p)
        # of arena notes
        # of arena bytes
        # of arena type boundaries
        offset of main arena
        offset of update index
        */
        //static const size_t NumMetaFields = 10UL;

        /* TODO */
        TDataFile(Util::TEngine *engine,
                  Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed,
                  TMemoryLayer *memory_layer,
                  const Base::TUuid &file_uid,
                  size_t gen_id,
                  size_t temp_file_consol_thresh,
                  TSequenceNumber release_up_to,
                  DiskPriority priority);

        /* TODO */
        inline size_t GetNumKeys() const {
          assert(this);
          return NumKeys;
        }

        /* TODO */
        inline TSequenceNumber GetLowestSequence() const {
          assert(this);
          return LowestSeq;
        }

        /* TODO */
        inline TSequenceNumber GetHighestSequence() const {
          assert(this);
          return HighestSeq;
        }

        private:

        /* TODO */
        Util::TEngine *Engine;

        /* TODO */
        Disk::Util::TVolume::TDesc::TStorageSpeed StorageSpeed;

        /* TODO */
        DiskPriority Priority;

        /* TODO */
        size_t NumUpdates;

        /* TODO */
        size_t MainArenaByteOffset;
        TRemapIndex MainArenaRemapIndex;
        TTypeBoundaryOffsetVec MainArenaTypeBoundaryOffsetVec;

        /* TODO */
        size_t NumKeys;
        TSequenceNumber LowestSeq;
        TSequenceNumber HighestSeq;

        /* TODO */
        size_t TempFileConsolThresh;

        /* TODO */
        size_t StartingBlockId;

        /* TODO */
        size_t StartingBlockOffset;

        /* TODO */
        size_t FileLength;

        /* TODO */
        TBlockVec BlockVec;

        /* TODO */
        TUpdateCollector UpdateCollector;

        /* TODO */
        #ifndef NDEBUG
        std::unordered_set<size_t> WrittenBlockSet;
        #endif

      };  // TDataFile

    }  // Disk

  }  // Indy

}  // Stig