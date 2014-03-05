/* <stig/indy/disk/util/engine.h>

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
#include <stig/indy/disk/file_service_base.h>
#include <stig/indy/disk/util/cache.h>
#include <stig/indy/disk/util/volume_manager.h>
#include <stig/indy/util/block_vec.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      namespace Util {

        /* TODO */
        typedef TCache<PhysicalPageSize> TPageCache;
        typedef TCache<PhysicalBlockSize> TBlockCache;

        /* TODO */
        class TEngine {
          NO_COPY_SEMANTICS(TEngine);
          public:

          /* TODO */
          TEngine(TVolumeManager *vol_man,
                  TPageCache *page_cache,
                  TBlockCache *block_cache,
                  TFileServiceBase *file_service,
                  bool is_disk_engine)
              : VolMan(vol_man),
                PageCache(page_cache),
                BlockCache(block_cache),
                FileService(file_service),
                IsDiskBasedEngine(is_disk_engine) {}

          /* TODO */
          ~TEngine() {}

          /* TODO */
          bool FindFile(const Base::TUuid &file_uid, size_t gen_id, size_t &starting_block, size_t &starting_block_offset, size_t &file_length, size_t &num_keys) {
            return FileService->FindFile(file_uid, gen_id, starting_block, starting_block_offset, file_length, num_keys);
          }

          /* TODO */
          void InsertFile(const Base::TUuid &file_uid,
                          TFileObj::TKind file_kind,
                          size_t gen_id,
                          size_t starting_block_id,
                          size_t starting_block_offset,
                          size_t file_size,
                          size_t num_keys,
                          TSequenceNumber lowest_seq,
                          TSequenceNumber highest_seq,
                          TCompletionTrigger &completion_trigger) {
            FileService->InsertFile(file_uid, file_kind, gen_id, starting_block_id, starting_block_offset, file_size, num_keys, lowest_seq, highest_seq, completion_trigger);
          }

          /* TODO */
          void RemoveFile(const Base::TUuid &file_uid, size_t gen_id, TCompletionTrigger &completion_trigger) {
            FileService->RemoveFile(file_uid, gen_id, completion_trigger);
          }

          /* TODO */
          void AppendFileGenSet(const Base::TUuid &file_uid, std::vector<TFileObj> &out_vec) {
            return FileService->AppendFileGenSet(file_uid, out_vec);
          }

          /* TODO */
          bool ForEachFile(const std::function<bool (const Base::TUuid &file_uid, const TFileObj &)> &cb) {
            return FileService->ForEachFile(cb);
          }

          /* TODO */
          size_t ReserveBlock(TVolume::TDesc::TStorageSpeed storage_speed) {
            assert(this);
            TBlockRange range;
            VolMan->TryAllocateSequentialBlocks(storage_speed, 1UL, [&](const TBlockRange &block_range) {
              range = block_range;
            });
            assert(range.second == 1UL);
            return range.first;
          }

          /* TODO */
          /* TODO: can we get rid of this version? */
          void AppendReserveBlocks(TVolume::TDesc::TStorageSpeed storage_speed, size_t num_blocks, std::vector<size_t> &append_vec) {
            assert(this);
            assert(num_blocks > 0);
            size_t left = num_blocks;
            try {
              while (left > 0) {
                VolMan->TryAllocateSequentialBlocks(storage_speed, left, [&](const TBlockRange &range) {
                  for (size_t i = 0; i < range.second; ++i) {
                    assert(GetPageCache()->AssertNoRefCount((range.first + i) * 16));
                    assert(GetBlockCache()->AssertNoRefCount(range.first + i));
                    append_vec.push_back(range.first + i);
                  }
                  left -= range.second;
                });
              }
            } catch (const std::exception &/*ex*/) {
              append_vec.erase(append_vec.begin() + (append_vec.size() - (num_blocks - left)), append_vec.end());
              throw;
            }
          }

          /* TODO */
          void AppendReserveBlocks(TVolume::TDesc::TStorageSpeed storage_speed, size_t num_blocks, Indy::Util::TBlockVec &append_vec) {
            assert(this);
            assert(num_blocks > 0);
            size_t left = num_blocks;
            while (left > 0) {
              VolMan->TryAllocateSequentialBlocks(storage_speed, left, [&](const TBlockRange &range) {
                append_vec.PushBack(range);
                #ifndef NDEBUG
                for (size_t i = 0; i < range.second; ++i) {
                  assert(GetPageCache()->AssertNoRefCount((range.first + i) * 16));
                  assert(GetBlockCache()->AssertNoRefCount(range.first + i));
                }
                #endif
                left -= range.second;
              });
            }
          }

          /* TODO */
          void FreeBlock(size_t block_id) {
            assert(this);
            VolMan->FreeSequentialBlocks(TBlockRange(block_id, 1UL));
          }

          /* TODO */
          void FreeSeqBlocks(size_t block_id, size_t num_blocks) {
            assert(this);
            VolMan->FreeSequentialBlocks(TBlockRange(block_id, num_blocks));
          }

          /* TODO */
          inline TVolumeManager *GetVolMan() const {
            assert(this);
            return VolMan;
          }

          /* TODO */
          template <size_t PhysicalCacheSize>
          inline Util::TCache<PhysicalCacheSize> *GetCache() const {
            return TCacheGetter<PhysicalCacheSize>::Get(this);
          }

          /* TODO */
          inline TPageCache *GetPageCache() const {
            assert(this);
            return PageCache;
          }

          /* TODO */
          inline TBlockCache *GetBlockCache() const {
            assert(this);
            return BlockCache;
          }

          /* TODO */
          inline bool IsDiskBased() const {
            assert(this);
            return IsDiskBasedEngine;
          }

          private:

          /* TODO */
          template <size_t PhysicalCacheSize>
          class TCacheGetter {
            NO_CONSTRUCTION(TCacheGetter);
            public:

            /* TODO */
            static Util::TCache<PhysicalCacheSize> *Get(TEngine *) {
              //static_assert(false, "TCacheGetter not specialized on the cache size you are trying to get");
            }

          };  // TCacheGetter

          /* TODO */
          TVolumeManager *VolMan;

          /* TODO */
          TPageCache *PageCache;

          /* TODO */
          TBlockCache *BlockCache;

          /* TODO */
          TFileServiceBase *FileService;

          /* TODO */
          bool IsDiskBasedEngine;

        };  // TEngine

        template <>
        class TEngine::TCacheGetter<4096> {
          public:
          static Util::TCache<4096> *Get(const TEngine *engine) {
            return engine->PageCache;
          }
        };

        template <>
        class TEngine::TCacheGetter<65536> {
          public:
          static Util::TCache<65536> *Get(const TEngine *engine) {
            return engine->BlockCache;
          }
        };

      }  // Util

    }  // Disk

  }  // Indy

}  // Stig