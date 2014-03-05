/* <stig/indy/disk/file_service_base.h>

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
#include <base/uuid.h>
#include <stig/indy/disk/result.h>
#include <stig/indy/sequence_number.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      /* TODO */
      class TFileObj {
        public:

        /* TODO */
        enum TKind {
          DataFile,
          DurableFile
        };

        /* TODO */
        inline TFileObj(TKind kind,
                        size_t gen_id,
                        size_t starting_block,
                        size_t starting_block_offset,
                        size_t file_size,
                        size_t num_keys,
                        TSequenceNumber lowest_seq,
                        TSequenceNumber highest_seq);

        TKind Kind;

        /* TODO */
        size_t GenId;

        /* TODO */
        size_t StartingBlockId;

        /* TODO */
        size_t StartingBlockOffset;

        /* TODO */
        size_t FileSize;

        /* TODO */
        size_t NumKeys;

        /* TODO */
        TSequenceNumber LowestSeq;

        /* TODO */
        TSequenceNumber HighestSeq;

      };  // TFileObj

      /* TODO */
      class TFileServiceBase {
        NO_COPY_SEMANTICS(TFileServiceBase);
        public:

        /* TODO */
        virtual bool FindFile(const Base::TUuid &file_uid,
                              size_t gen_id,
                              size_t &starting_block,
                              size_t &starting_block_offset,
                              size_t &file_length,
                              size_t &num_keys) const = 0;

        /* TODO */
        virtual void InsertFile(const Base::TUuid &file_uid,
                                TFileObj::TKind kind,
                                size_t gen_id,
                                size_t starting_block_id,
                                size_t starting_block_offset,
                                size_t file_size,
                                size_t num_keys,
                                TSequenceNumber lowest_seq,
                                TSequenceNumber highest_seq,
                                TCompletionTrigger &completion_trigger) = 0;

        /* TODO */
        virtual void RemoveFile(const Base::TUuid &file_uid,
                                size_t gen_id,
                                TCompletionTrigger &completion_trigger) = 0;

        /* TODO */
        virtual void AppendFileGenSet(const Base::TUuid &file_uid,
                                      std::vector<TFileObj> &out_vec) = 0;

        /* TODO */
        virtual bool ForEachFile(const std::function<bool (const Base::TUuid &file_uid, const TFileObj &)> &cb) = 0;

        /* TODO */
        virtual ~TFileServiceBase() {}

        protected:

        /* TODO */
        TFileServiceBase() {}

      };  // TFileServiceBase

      /***************
        *** Inline ***
        *************/

      inline TFileObj::TFileObj(TKind kind,
                                size_t gen_id,
                                size_t starting_block_id,
                                size_t starting_block_offset,
                                size_t file_size,
                                size_t num_keys,
                                TSequenceNumber lowest_seq,
                                TSequenceNumber highest_seq)
          : Kind(kind),
            GenId(gen_id),
            StartingBlockId(starting_block_id),
            StartingBlockOffset(starting_block_offset),
            FileSize(file_size),
            NumKeys(num_keys),
            LowestSeq(lowest_seq),
            HighestSeq(highest_seq) {}

    }  // Disk

  }  // Indy

}  // Stig