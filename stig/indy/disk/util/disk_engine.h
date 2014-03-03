/* <stig/indy/disk/util/disk_engine.h> 

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

#include <stig/indy/disk/util/disk_util.h>
#include <stig/indy/disk/util/engine.h>
#include <stig/indy/disk/file_service.h>
#include <stig/indy/disk/read_file.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      namespace Util {

        /* TODO */
        class TDiskEngine {
          NO_COPY_SEMANTICS(TDiskEngine);
          public:

          /* TODO */
          TDiskEngine(Base::TScheduler *scheduler,
                      Fiber::TRunner::TRunnerCons &runner_cons,
                      Base::TThreadLocalPoolManager<Indy::Fiber::TFrame, size_t, Indy::Fiber::TRunner *> *frame_pool_manager,
                      const std::vector<size_t> &disk_controller_core_vec,
                      const std::string &instance_name,
                      bool discard_on_create,
                      bool do_fsync,
                      size_t page_cache_size,
                      size_t num_page_lru,
                      size_t block_cache_size,
                      size_t num_block_lru,
                      size_t append_log_mb,
                      bool create = false,
                      bool no_realtime = false)
            : Scheduler(scheduler),
              SystemBlockId(0UL),
              FileAppendLogBlocks((append_log_mb * 1024 * 1024) / Util::PhysicalBlockSize) {
            assert(disk_controller_core_vec.size());
            CacheCb = [this](Util::TCacheInstr cache_instr, const Util::TOffset logical_start_offset, void *buf, size_t count) {
              switch (cache_instr) {
                case Util::TCacheInstr::CacheAll: {
                  assert(PageCache.get());
                  assert(BlockCache.get());
                  assert(logical_start_offset % Util::TPageCache::DataSize == 0);
                  assert(count % Util::TPageCache::DataSize == 0);
                  assert(logical_start_offset % Util::TBlockCache::DataSize == 0);
                  assert(count % Util::TBlockCache::DataSize == 0);
                  for (size_t i = 0; i < (count / Util::TPageCache::DataSize); ++i) {
                    PageCache->Replace((logical_start_offset / Util::TPageCache::DataSize) + i, reinterpret_cast<uint8_t *>(buf) + (i * Util::TPageCache::DataSize));
                  }
                  for (size_t i = 0; i < (count / Util::TBlockCache::DataSize); ++i) {
                    BlockCache->Replace((logical_start_offset / Util::TBlockCache::DataSize) + i, reinterpret_cast<uint8_t *>(buf) + (i * Util::TBlockCache::DataSize));
                  }
                }
                case Util::TCacheInstr::CachePageOnly: {
                  assert(PageCache.get());
                  assert(logical_start_offset % Util::TPageCache::DataSize == 0);
                  assert(count % Util::TPageCache::DataSize == 0);
                  for (size_t i = 0; i < (count / Util::TPageCache::DataSize); ++i) {
                    PageCache->Replace((logical_start_offset / Util::TPageCache::DataSize) + i, reinterpret_cast<uint8_t *>(buf) + (i * Util::TPageCache::DataSize));
                  }
                  break;
                }
                case Util::TCacheInstr::CacheBlockOnly: {
                  assert(BlockCache.get());
                  assert(logical_start_offset % Util::TBlockCache::DataSize == 0);
                  assert(count % Util::TBlockCache::DataSize == 0);
                  for (size_t i = 0; i < (count / Util::TBlockCache::DataSize); ++i) {
                    BlockCache->Replace((logical_start_offset / Util::TBlockCache::DataSize) + i, reinterpret_cast<uint8_t *>(buf) + (i * Util::TBlockCache::DataSize));
                  }
                  break;
                }
                case Util::TCacheInstr::ClearAll: {
                  assert(PageCache.get());
                  assert(BlockCache.get());
                  assert(logical_start_offset % Util::TPageCache::DataSize == 0);
                  assert(count % Util::TPageCache::DataSize == 0);
                  assert(logical_start_offset %Util::TBlockCache::DataSize == 0);
                  assert(count % Util::TBlockCache::DataSize == 0);
                  for (size_t i = 0; i < (count / Util::TPageCache::DataSize); ++i) {
                    PageCache->Clear((logical_start_offset / Util::TPageCache::DataSize) + i);
                  }
                  for (size_t i = 0; i < (count / Util::TBlockCache::DataSize); ++i) {
                    BlockCache->Clear((logical_start_offset / Util::TBlockCache::DataSize) + i);
                  }
                }
                case Util::TCacheInstr::ClearPageOnly: {
                  assert(PageCache.get());
                  assert(logical_start_offset % Util::TPageCache::DataSize == 0);
                  assert(count % Util::TPageCache::DataSize == 0);
                  for (size_t i = 0; i < (count / Util::TPageCache::DataSize); ++i) {
                    PageCache->Clear((logical_start_offset / Util::TPageCache::DataSize) + i);
                  }
                  break;
                }
                case Util::TCacheInstr::ClearBlockOnly: {
                  assert(BlockCache.get());
                  assert(logical_start_offset %Util::TBlockCache::DataSize == 0);
                  assert(count % Util::TBlockCache::DataSize == 0);
                  for (size_t i = 0; i < (count / Util::TBlockCache::DataSize); ++i) {
                    BlockCache->Clear((logical_start_offset / Util::TBlockCache::DataSize) + i);
                  }
                  break;
                }
                case Util::TCacheInstr::NoCache: {
                  break;
                }
              }
            };
            DiskController = std::unique_ptr<TDiskController>(new TDiskController());
            DiskUtil = std::unique_ptr<TDiskUtil>(new TDiskUtil(scheduler, DiskController.get(), instance_name, do_fsync, CacheCb, true));
            VolMan = DiskUtil->GetVolumeManager(instance_name);
            std::vector<std::vector<TPersistentDevice *>> device_vec;
            size_t pos = 0UL;
            for (const auto &dev : DiskUtil->GetPersistentDeviceSet()) {
              device_vec.push_back(std::vector<TPersistentDevice *>{dynamic_cast<TPersistentDevice *>(dev.get())});
              const size_t max_aio_num = 4096 * 16;
              io_context_t ctxp(0);
              try {
                Base::IfWeird(io_setup(max_aio_num, &ctxp));
              } catch (const std::exception &ex) {
                syslog(LOG_ERR, "Error in io_setup: [%s]", ex.what());
                throw;
              }
              Scheduler->Schedule(std::bind(&TDiskController::QueueRunner, DiskController.get(), device_vec.back(), no_realtime, disk_controller_core_vec[pos % disk_controller_core_vec.size()]));
              syslog(LOG_INFO, "Assigning Device [%s] to core [%ld] because pos [%ld]", device_vec.back().front()->GetDevicePath(), disk_controller_core_vec[pos % disk_controller_core_vec.size()], pos);
              ++pos;
            }
            //#endif

            /* Allocate the system block at position 0. */
            VolMan->MarkBlockRangeUsed(TBlockRange(0UL, 1UL));
            SystemBlockId = 0UL;

            PageCache = std::unique_ptr<Util::TPageCache>(new Util::TPageCache(VolMan, page_cache_size, num_page_lru));
            BlockCache = std::unique_ptr<Util::TBlockCache>(new Util::TBlockCache(VolMan, block_cache_size, num_block_lru));

            std::unique_ptr<const TBufBlock> buf_block(new TBufBlock());
            memset(buf_block->GetData(), 0, PhysicalBlockSize);
            if (create) {
              if (discard_on_create) {
                VolMan->DiscardAllDevices();
              }
              TBlockRange tmp_range;
              VolMan->TryAllocateSequentialBlocks(Util::TVolume::TDesc::TStorageSpeed::Fast, 1UL, [&](const TBlockRange &block_range) {
                tmp_range = block_range;
              });
              assert(tmp_range.second == 1UL);
              Image1BlockId = tmp_range.first;
              VolMan->TryAllocateSequentialBlocks(Util::TVolume::TDesc::TStorageSpeed::Fast, 1UL, [&](const TBlockRange &block_range) {
                tmp_range = block_range;
              });
              assert(tmp_range.second == 1UL);
              Image2BlockId = tmp_range.first;
              Util::TBlockRange block_range;
              VolMan->TryAllocateSequentialBlocks(Util::TVolume::TDesc::TStorageSpeed::Fast, FileAppendLogBlocks, [&](const TBlockRange &range) {
                block_range = range;
              });
              if (unlikely(block_range.second != FileAppendLogBlocks)) {
                throw std::runtime_error("not enough blocks allocated for the file service append log");
              }
              AppendLogBlockVec.reserve(block_range.second);
              for (size_t i = 0; i < block_range.second; ++i) {
                AppendLogBlockVec.push_back(block_range.first + i);
              }
              /* write the system block with our core information. */
              size_t *buf = reinterpret_cast<size_t *>(buf_block->GetData());
              buf[0] = Image1BlockId;
              buf[1] = Image2BlockId;
              buf[2] = FileAppendLogBlocks;
              if (unlikely((FileAppendLogBlocks * sizeof(size_t)) > (LogicalCheckedBlockSize - (3 * sizeof(size_t))))) {
                throw std::runtime_error("FileAppendLog meta data is too large to fit in system block");
              }
              assert(AppendLogBlockVec.size() == FileAppendLogBlocks);
              for (size_t i = 0; i < FileAppendLogBlocks; ++i) {
                buf[3 + i] = AppendLogBlockVec[i];
              }
              TCompletionTrigger trigger;
              VolMan->WriteAndFlush(HERE,
                                    CheckedBlock,
                                    Source::System,
                                    buf_block->GetData(),
                                    SystemBlockId * PhysicalBlockSize,
                                    PhysicalBlockSize,
                                    RealTime,
                                    TCacheInstr::NoCache,
                                    trigger);
              trigger.Wait();
            } else {
              TCompletionTrigger trigger;
              VolMan->ReadBlock(HERE, CheckedBlock, Source::System, buf_block->GetData(), SystemBlockId, RealTime, trigger);
              trigger.Wait();
              size_t *buf = reinterpret_cast<size_t *>(buf_block->GetData());
              Image1BlockId = buf[0];
              Image2BlockId = buf[1];
              FileAppendLogBlocks = buf[2];
              if (unlikely((FileAppendLogBlocks * sizeof(size_t)) > (LogicalCheckedBlockSize - (3 * sizeof(size_t))))) {
                throw std::runtime_error("FileAppendLog meta data is too large to fit in system block");
              }
              VolMan->MarkBlockRangeUsed(TBlockRange(Image1BlockId, 1UL)); /* mark block 1 */
              VolMan->MarkBlockRangeUsed(TBlockRange(Image2BlockId, 1UL)); /* mark block 2 */
              AppendLogBlockVec.reserve(FileAppendLogBlocks);
              assert(AppendLogBlockVec.empty());
              for (size_t i = 0; i < FileAppendLogBlocks; ++i) {
                AppendLogBlockVec.push_back(buf[3 + i]);
                VolMan->MarkBlockRangeUsed(TBlockRange(buf[3 + i], 1UL)); /* mark append log blocks */
              }
            }
            TFileService::TFileInitCb file_init_cb = [this](TFileObj::TKind file_kind,
                                                            const Base::TUuid &file_uid,
                                                            size_t gen_id,
                                                            size_t starting_block_id,
                                                            size_t starting_block_offset,
                                                            size_t file_length) -> bool {
              switch (file_kind) {
                case TFileObj::TKind::DataFile: {
                  TDataFileReader reader(PageCache.get(),
                                         file_uid,
                                         RealTime,
                                         gen_id,
                                         starting_block_id,
                                         starting_block_offset,
                                         file_length);


                  TDataFileReader::TInStream in_stream(HERE, Source::System, RealTime, &reader, PageCache.get(), (reader.GetStartingBlockOffset() * Disk::Util::LogicalBlockSize) + (TData::NumMetaFields * sizeof(size_t)));
                  size_t block_id;
                  for (size_t i = 0; i < reader.GetNumMetaBlocks(); ++i) {
                    in_stream.Read(block_id);
                    VolMan->MarkBlockRangeUsed(TBlockRange(block_id, 1UL));
                  }
                  size_t num_contig_blocks;
                  for (size_t i = 0; i < reader.GetNumSequentialBlockPairings(); ++i) {
                    in_stream.Read(block_id);
                    in_stream.Read(num_contig_blocks);
                    VolMan->MarkBlockRangeUsed(TBlockRange(block_id, num_contig_blocks));
                  }
                  break;
                }
                case TFileObj::TKind::DurableFile: {
                  TDurableManager::TSortedInFile sorted_in_file(PageCache.get(),
                                                                RealTime,
                                                                gen_id,
                                                                starting_block_id,
                                                                starting_block_offset,
                                                                file_length);
                  const size_t num_blocks = sorted_in_file.GetNumBlocks();
                  typedef TStream<Util::LogicalPageSize, Util::LogicalBlockSize, Util::PhysicalBlockSize, Util::CheckedPage, 0UL> TInStream;
                  TInStream in_stream(HERE, Source::System, RealTime, &sorted_in_file, PageCache.get(), TDurableManager::TSortedByIdFile::NumMetaFields * sizeof(size_t));
                  size_t block_id;
                  for (size_t i = 0; i < num_blocks; ++i) {
                    in_stream.Read(block_id);
                    /* TODO: we could mark these in ranges. */
                    VolMan->MarkBlockRangeUsed(TBlockRange(block_id, 1UL));
                  }
                  break;
                }
              }
              return true;
            };
            FileService = std::unique_ptr<TFileService>(new TFileService(scheduler,
                                                                         runner_cons,
                                                                         frame_pool_manager,
                                                                         VolMan,
                                                                         Image1BlockId,
                                                                         Image2BlockId,
                                                                         AppendLogBlockVec,
                                                                         file_init_cb,
                                                                         create));

            Engine = std::unique_ptr<Util::TEngine>(new Util::TEngine(VolMan, PageCache.get(), BlockCache.get(), FileService.get(), true));
          }

          /* TODO */
          Util::TEngine *GetEngine() const {
            return Engine.get();
          }

          /* TODO */
          Util::TPageCache *GetPageCache() const {
            return PageCache.get();
          }

          /* TODO */
          Util::TBlockCache *GetBlockCache() const {
            return BlockCache.get();
          }

          /* TODO */
          Util::TVolumeManager *GetVolMan() const {
            return VolMan;
          }

          /* TODO */
          void Report(std::stringstream &ss, double elapsed_time) const {
            DiskController->Report(ss, elapsed_time);
          }

          private:

          /* TODO */
          class TDataFileReader
              : public TReadFile<LogicalPageSize, LogicalBlockSize, PhysicalBlockSize, CheckedPage> {
            NO_COPY_SEMANTICS(TDataFileReader);
            public:

            /* TODO */
            typedef TStream<Disk::Util::LogicalPageSize, Disk::Util::LogicalBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::CheckedPage, 0UL> TInStream;
            typedef Stig::Indy::Disk::TReadFile<Disk::Util::LogicalPageSize, Disk::Util::LogicalBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::CheckedPage> TMyReadFile;

            /* TODO */
            TDataFileReader(Disk::Util::TPageCache *page_cache,
                            const Base::TUuid &file_id,
                            DiskPriority priority,
                            size_t gen_id,
                            size_t starting_block_id,
                            size_t starting_block_offset,
                            size_t file_length)
                : TMyReadFile(HERE,
                              Source::FileRemoval,
                              page_cache,
                              file_id,
                              priority,
                              gen_id,
                              starting_block_id,
                              starting_block_offset,
                              file_length) {}

            /* TODO */
            virtual ~TDataFileReader() {}

            /* TODO */
            using TReadFile::GetStartingBlockOffset;
            using TReadFile::GetNumBlocks;
            using TReadFile::GetNumMetaBlocks;
            using TReadFile::GetNumSequentialBlockPairings;
            using TReadFile::GetNumUpdates;

            using TReadFile::FindInHash;

          };  // TDataFileReader

          /* TODO */
          Base::TScheduler *Scheduler;

          /* TODO */
          std::unique_ptr<TDiskController> DiskController;

          /* TODO */
          std::unique_ptr<TDiskUtil> DiskUtil;

          /* TODO */
          size_t SystemBlockId;

          /* TODO */
          std::unique_ptr<TFileService> FileService;
          size_t FileAppendLogBlocks;
          size_t Image1BlockId;
          size_t Image2BlockId;
          std::vector<size_t> AppendLogBlockVec;

          /* TODO */
          Util::TVolumeManager *VolMan;

          /* TODO */
          std::unique_ptr<Util::TPageCache> PageCache;
          std::unique_ptr<Util::TBlockCache> BlockCache;
          std::unique_ptr<Util::TEngine> Engine;

          /* TODO */
          Util::TCacheCb CacheCb;

        };  // TDiskEngine

      }  // Util

    }  // Disk

  }  // Indy

}  // Stig
