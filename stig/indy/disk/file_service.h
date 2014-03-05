/* <stig/indy/disk/file_service.h>

   TODO

   Copyright 2010-2014 Tagged

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

#include <unistd.h>

#include <base/event_semaphore.h>
#include <base/no_copy_semantics.h>
#include <base/scheduler.h>
#include <base/uuid.h>
#include <inv_con/unordered_list.h>
#include <stig/indy/disk/buf_block.h>
#include <stig/indy/disk/file_service_base.h>
#include <stig/indy/disk/util/engine.h>
#include <stig/indy/sequence_number.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      /* TODO */
      class TFileService
          : public TFileServiceBase, public Fiber::TRunnable {
        NO_COPY_SEMANTICS(TFileService);
        public:

        /* TODO */
        typedef std::function<bool (TFileObj::TKind /*file kind*/,
                                    const Base::TUuid &/*file_uid*/,
                                    size_t /*gen_id*/,
                                    size_t /*starting block id*/,
                                    size_t /*starting block offset*/,
                                    size_t /*file_length*/)> TFileInitCb;

        /* TODO */
        TFileService(Base::TScheduler *scheduler,
                     Fiber::TRunner::TRunnerCons &runner_cons,
                     Base::TThreadLocalPoolManager<Indy::Fiber::TFrame, size_t, Indy::Fiber::TRunner *> *frame_pool_manager,
                     Util::TVolumeManager *vol_man,
                     size_t image_1_block_id,
                     size_t image_2_block_id,
                     const std::vector<size_t> &append_log_block_vec,
                     const TFileInitCb &file_init_cb,
                     bool create = false,
                     bool abort_on_append_log_scan = true);

        /* TODO */
        ~TFileService();

        /* TODO */
        virtual void InsertFile(const Base::TUuid &file_uid,
                                TFileObj::TKind file_kind,
                                size_t file_gen,
                                size_t starting_block_id,
                                size_t starting_block_offset,
                                size_t file_size,
                                size_t num_keys,
                                TSequenceNumber lowest_seq,
                                TSequenceNumber highest_seq,
                                TCompletionTrigger &completion_trigger) override;

        /* TODO */
        virtual void RemoveFile(const Base::TUuid &file_uid,
                                size_t file_gen,
                                TCompletionTrigger &completion_trigger) override;

        /* TODO */
        virtual bool FindFile(const Base::TUuid &file_uid,
                              size_t file_gen,
                              size_t &out_block_id,
                              size_t &out_block_offset,
                              size_t &out_file_size,
                              size_t &out_num_keys) const override;

        /* TODO */
        virtual void AppendFileGenSet(const Base::TUuid &file_uid,
                                      std::vector<TFileObj> &out_vec) override;

        /* TODO */
        virtual bool ForEachFile(const std::function<bool (const Base::TUuid &file_uid, const TFileObj &)> &cb) override;

        /* TODO */
        inline size_t GetNumFiles() const;

        private:

        /* TODO */
        typedef std::unordered_map<Base::TUuid, std::unordered_map<size_t, TFileObj>> TFileMap;

        /* Forward Declarations. */
        class TOp;

        /* TODO */
        typedef InvCon::UnorderedList::TCollection<TFileService, TOp> TOpQueue;

        /* TODO */
        class TOp {
          NO_COPY_SEMANTICS(TOp);
          public:

          /* TODO */
          enum TKind {
            InsertFile,
            RemoveFile
          };

          /* TODO */
          typedef InvCon::UnorderedList::TMembership<TOp, TFileService> TQueueMembership;

          /* TODO */
          inline TOp(TKind kind, const Base::TUuid &file_uid, TFileObj &&fil_obj, TCompletionTrigger &trigger);

          /* TODO */
          inline ~TOp();

          /* TODO */
          inline void Apply(size_t *buf) const;

          /* TODO */
          inline void Apply(TFileMap &file_map, size_t &num_elem_in_file_map) const;

          /* TODO */
          inline TQueueMembership *GetQueueMembership();

          /* TODO */
          inline void Remove();

          /* TODO */
          inline void Complete(TDiskResult result, const char *err_str);

          private:

          /* TODO */
          TQueueMembership::TImpl QueueMembership;

          /* TODO */
          TKind Kind;

          /* TODO */
          Base::TUuid FileUUID;

          /* TODO */
          TFileObj FileObj;

          /* TODO */
          TCompletionTrigger &Trigger;

        };  // TOp

        /* TODO */
        void Runner();

        /* TODO */
        bool TryLoadFromBaseImage(size_t base_image_block, const size_t *cur_buf, TBufBlock *cur_buf_block);

        /* TODO */
        void ZeroImageBlocks(size_t image_1_block_id, size_t image_2_block_id);

        /* TODO */
        void ZeroAppendLog();

        /* TODO */
        static void AddToMap(TFileMap &file_map,
                             const Base::TUuid &file_uid,
                             TFileObj::TKind file_kind,
                             size_t file_gen,
                             size_t starting_block_id,
                             size_t starting_block_offset,
                             size_t file_size,
                             size_t num_keys,
                             TSequenceNumber lowest_seq,
                             TSequenceNumber highest_seq);

        /* TODO */
        static void RemoveFromMap(TFileMap &file_map,
                                  const Base::TUuid &file_uid,
                                  size_t file_gen);

        /* TODO */
        static void ApplyDeltasToMap(TFileMap &file_map,
                                     const size_t *buf,
                                     size_t &num_files_in_map);

        /* TODO */
        static void ApplyImageBlock(TFileMap &file_map,
                                    const size_t *buf,
                                    size_t &num_files_in_map);

        /* TODO */
        Fiber::TFrame *Frame;
        Fiber::TRunner BGScheduler;

        /* TODO */
        Util::TVolumeManager *VolMan;

        /* TODO */
        mutable std::mutex Mutex;

        /* TODO */
        TFileMap Map;
        size_t NumFiles;

        /* TODO */
        TFileMap RunnerCopyMap;
        size_t NumRunnerCopyFiles;

        /* TODO */
        std::vector<size_t> Image1BlockIdVec;
        std::vector<size_t> Image2BlockIdVec;
        std::vector<size_t> AppendLogBlockVec;
        size_t NumAppendLogSectors;
        size_t VersionNumber;
        size_t CurBaseImageCounter;
        size_t CurRingSector;

        /* TODO */
        std::mutex QueueLock;

        /* TODO */
        mutable TOpQueue::TImpl OpQueue;

        /* TODO */
        Base::TEventSemaphore RunSem;
        bool ShuttingDown;

        /* A flag used to test abort on append log corruption */
        bool AbortOnAppendLogScan;

        /* TODO */
        static constexpr size_t VersionSize = sizeof(uint64_t);

        /* TODO */
        static constexpr size_t NextBlockSize = sizeof(uint64_t);

        /* TODO */
        static constexpr size_t EntrySize = 10UL;

        /* TODO */
        static constexpr size_t OpSize = 1UL;

        /* TODO */
        static constexpr size_t EntryByteSize = EntrySize * sizeof(uint64_t);
        static_assert(sizeof(TFileObj) + sizeof(Base::TUuid) == EntryByteSize, "TFileObj size mismatch");

        /* TODO */
        static constexpr size_t OpByteSize = sizeof(uint64_t);

        /* Sector size, less version number */
        static constexpr size_t NumFilesPerRingBuf = (Util::LogicalSectorSize - VersionSize) / (EntryByteSize + OpByteSize);

        /* TODO */
        static constexpr size_t NumSectorsPerBlock = Util::PhysicalBlockSize / Util::PhysicalSectorSize;

        /* TODO */
        static constexpr size_t NumElemPerBaseImageBlock = (Util::LogicalCheckedBlockSize - VersionSize - NextBlockSize) / EntryByteSize;

      };  // TFileService

      /***************
        *** Inline ***
        *************/

      inline size_t TFileService::GetNumFiles() const {
        assert(this);
        return NumFiles;
      }

      inline void TFileService::TOp::Apply(size_t *buf) const {
        assert(this);
        uuid_copy(*reinterpret_cast<uuid_t *>(buf), FileUUID.GetRaw());
        buf[2] = FileObj.GenId; // file_gen
        buf[3] = FileObj.StartingBlockId; // starting_block_id
        buf[4] = FileObj.StartingBlockOffset; // starting_block_offset
        buf[5] = FileObj.FileSize; // file_size
        buf[6] = FileObj.NumKeys; // num_keys
        buf[7] = FileObj.LowestSeq; // lowest_seq
        buf[8] = FileObj.HighestSeq; // highest_seq
        buf[9] = static_cast<uint64_t>(FileObj.Kind); // file kind
        buf[10] = static_cast<uint64_t>(Kind);  // op
      }

      inline void TFileService::TOp::Apply(TFileMap &file_map, size_t &num_elem_in_file_map) const {
        switch (Kind) {
          case InsertFile: {
            TFileService::AddToMap(file_map,
                                   FileUUID,
                                   FileObj.Kind,
                                   FileObj.GenId,
                                   FileObj.StartingBlockId,
                                   FileObj.StartingBlockOffset,
                                   FileObj.FileSize,
                                   FileObj.NumKeys,
                                   FileObj.LowestSeq,
                                   FileObj.HighestSeq);
            ++num_elem_in_file_map;
            break;
          }
          case RemoveFile: {
            TFileService::RemoveFromMap(file_map,
                                        FileUUID,
                                        FileObj.GenId);
            --num_elem_in_file_map;
            break;
          }
        }
      }

      inline TFileService::TOp::TOp(TKind kind, const Base::TUuid &file_uid, TFileObj &&file_obj, TCompletionTrigger &trigger)
          : QueueMembership(this), Kind(kind), FileUUID(file_uid), FileObj(file_obj), Trigger(trigger) {
        trigger.WaitForOneMore();
      }

      inline TFileService::TOp::~TOp() {}

      inline TFileService::TOp::TQueueMembership *TFileService::TOp::GetQueueMembership() {
        assert(this);
        return &QueueMembership;
      }

      inline void TFileService::TOp::Remove() {
        assert(this);
        QueueMembership.Remove();
      }

      inline void TFileService::TOp::Complete(TDiskResult result, const char *err_str) {
        assert(this);
        Trigger.Callback(result, err_str);
      }

    }  // Disk

  }  // Indy

}  // Stig
