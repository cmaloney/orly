/* <stig/indy/disk/merge_data_file.h>

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

#include <base/no_copy_semantics.h>
#include <stig/atom/kit2.h>
#include <stig/indy/disk/data_file.h>
#include <stig/indy/disk/out_stream.h>
#include <stig/indy/disk/read_file.h>
#include <stig/indy/disk/util/index_manager.h>
#include <stig/indy/memory_layer.h>
#include <stig/sabot/all.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      /* TODO */
      template <bool ScanAheadAllowed>
      using TDataDiskArena = TDiskArena<Util::LogicalBlockSize, Util::LogicalBlockSize, Util::PhysicalBlockSize, Util::PageCheckedBlock, DiskArenaMaxCacheSize, ScanAheadAllowed>;

      /* TODO */
      class TMergeDataFile {
        NO_COPY_SEMANTICS(TMergeDataFile);
        public:

        /* TODO */
        TMergeDataFile(Util::TEngine *engine,
                       Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed,
                       const Base::TUuid &file_uuid,
                       const std::vector<size_t> &gen_vec,
                       const Base::TUuid &file_uid,
                       size_t gen_id,
                       TSequenceNumber /*release_up_to*/,
                       DiskPriority priority,
                       size_t max_block_cache_read_slots_allowed,
                       size_t temp_file_consol_thresh,
                       bool can_tail,
                       bool can_tail_tombstone);

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

        size_t NumKeys;
        TSequenceNumber LowestSeq;
        TSequenceNumber HighestSeq;

      };  // TMergeDataFile

    }  // Disk

  }  // Indy

}  // Stig