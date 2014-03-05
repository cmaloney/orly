/* <stig/indy/disk/file_service.cc>

   Implements <stig/indy/disk/file_service.h>.

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

#include <stig/indy/disk/file_service.h>

#include <stig/indy/disk/indy_util_reporter.h>

using namespace std;
using namespace Stig::Indy::Disk;

TFileService::TFileService(Base::TScheduler *scheduler,
                           Fiber::TRunner::TRunnerCons &runner_cons,
                           Base::TThreadLocalPoolManager<Indy::Fiber::TFrame, size_t, Indy::Fiber::TRunner *> *frame_pool_manager,
                           Util::TVolumeManager *vol_man,
                           size_t image_1_block_id,
                           size_t image_2_block_id,
                           const std::vector<size_t> &append_log_block_vec,
                           const TFileInitCb &file_init_cb,
                           bool create)
    : Frame(nullptr),
      BGScheduler(runner_cons),
      VolMan(vol_man),
      NumFiles(0UL),
      NumRunnerCopyFiles(0UL),
      AppendLogBlockVec(append_log_block_vec),
      NumAppendLogSectors(AppendLogBlockVec.size() * NumSectorsPerBlock),
      VersionNumber(0UL),
      CurBaseImageCounter(0UL),
      CurRingSector(0UL),
      OpQueue(this),
      ShuttingDown(false) {
  /* allocate event pools */
  Image1BlockIdVec.push_back(image_1_block_id);
  Image2BlockIdVec.push_back(image_2_block_id);
  if (!create) {
    std::unique_ptr<TBufBlock> image_1_buf_block(new TBufBlock());
    std::unique_ptr<TBufBlock> image_2_buf_block(new TBufBlock());
    /* read both base images and use the one with the higher version number */

    size_t version_1 = 0UL;
    size_t version_2 = 0UL;
    bool both_images_loaded_success = true;
    try {
      TCompletionTrigger trigger;
      VolMan->ReadBlock(HERE, Util::CheckedBlock, Source::FileService, image_1_buf_block->GetData(), image_1_block_id, RealTime, trigger,
                        false /* don't abort on error since we're trying to build a valid image */);
      trigger.Wait();
      const size_t *buf_1 = reinterpret_cast<const size_t *>(image_1_buf_block->GetData());
      version_1 = buf_1[0];
    } catch (const TDiskError &/*err*/) {
      both_images_loaded_success = false;
    }
    try {
      TCompletionTrigger trigger;
      VolMan->ReadBlock(HERE, Util::CheckedBlock, Source::FileService, image_2_buf_block->GetData(), image_2_block_id, RealTime, trigger,
                        false /* don't abort on error since we're trying to build a valid image */);
      trigger.Wait();
      const size_t *buf_2 = reinterpret_cast<const size_t *>(image_2_buf_block->GetData());
      version_2 = buf_2[0];
    } catch (const TDiskError &/*err*/) {
      both_images_loaded_success = false;
    }
    TCompletionTrigger trigger;
    const size_t *buf_1 = reinterpret_cast<const size_t *>(image_1_buf_block->GetData());
    const size_t *buf_2 = reinterpret_cast<const size_t *>(image_2_buf_block->GetData());
    bool process_next_block = true;
    const size_t *cur_buf = nullptr;
    TBufBlock *cur_buf_block = nullptr;
    size_t cur_image_block = -1;
    const size_t *alternate_buf = nullptr;
    TBufBlock *alternate_buf_block = nullptr;
    size_t alternate_image_block = -1;
    //size_t alternate_version_number = 0UL;
    if (version_1 > version_2) {
      VersionNumber = version_1;
      CurBaseImageCounter = 0UL;
      cur_buf = buf_1;
      cur_buf_block = image_1_buf_block.get();
      cur_image_block = image_1_block_id;
      alternate_buf = buf_2;
      alternate_buf_block = image_2_buf_block.get();
      alternate_image_block = image_2_block_id;
      //alternate_version_number = version_2;
    } else if (version_2 > version_1) {
      VersionNumber = version_2;
      CurBaseImageCounter = 1UL;
      cur_buf = buf_2;
      cur_buf_block = image_2_buf_block.get();
      cur_image_block = image_2_block_id;
      alternate_buf = buf_1;
      alternate_buf_block = image_1_buf_block.get();
      alternate_image_block = image_1_block_id;
      //alternate_version_number = version_1;
    } else if (version_1 == version_2 && version_1 == 0UL) {
      /* we're early on in this file system's life, to the point where only the append log has any data */
      VersionNumber = 0UL;
      CurBaseImageCounter = 0UL;
      process_next_block = false;
      //ZeroImageBlocks(image_1_block_id, image_2_block_id);
    }
    if (process_next_block) {
      if (!TryLoadFromBaseImage(cur_image_block, cur_buf, cur_buf_block)) {
        /* the load was not successful, at this point we need to:
           - see if the other alternate block loaded successfully
           - see what the first version number (fvn) is in the append log.
           - if the fvn is the next expected version after the alternate image block, then we can
             recover from there... only losing what data was in the most recent image block
           - otherwise this corrupt base image was later followed by append logs and we can not
             recover without greater data loss, so we throw */
        if (!both_images_loaded_success) {
          throw std::runtime_error("File System is corrupt. Base Image was irrecoverable, alternate image was also corrupt");
        }
        std::unique_ptr<TBufBlock> append_log_first_buf_block(new TBufBlock());
        /* read the first append log entry to get its version. */
        assert(AppendLogBlockVec.size());
        size_t block_of_first_append_log = AppendLogBlockVec[0];


        VolMan->Read(HERE,
                     Util::CheckedSector,
                     Source::FileService,
                     append_log_first_buf_block->GetData(),
                     (block_of_first_append_log * Util::PhysicalBlockSize),
                     Util::PhysicalSectorSize,
                     RealTime,
                     trigger,
                     true /* We think of sectors as atomic. If our sector read fails we're favoring abort over recovering an old state. */);
        trigger.Wait();
        const size_t *first_append_log_buf = reinterpret_cast<const size_t *>(append_log_first_buf_block->GetData());
        const size_t first_append_log_version = first_append_log_buf[0];
        const size_t alternate_image_block_version = alternate_buf[0];
        if (alternate_image_block_version == 0UL && first_append_log_version == 1UL) {
          /* the alternate block was empty and the append log starts at v1, this means all our changes
             (except the last one in the most recent image block) are recoverable by playing back the
             append log. */
        } else if (first_append_log_version == alternate_image_block_version + 1) {
          /* we can recover using the alternate base image block and replaying the append log from there. */
          process_next_block = true;
          VersionNumber = alternate_image_block_version;
          ++CurBaseImageCounter;
          if (!TryLoadFromBaseImage(alternate_image_block, alternate_buf, alternate_buf_block)) {
            throw std::runtime_error("File System is corrupt. Both base images are irrecoverable");
          }
        } else {
          throw std::runtime_error("File System is corrupt. Current base image is irrecoverable.");
        }
      }
    }
    /* now spin over the append only log and grab all the valid sectors with their deltas */
    std::vector<std::unique_ptr<TBufBlock>> append_log_buf_vec;
    std::vector<size_t> ring_offset_vec;
    for (auto block_id : AppendLogBlockVec) {
      append_log_buf_vec.emplace_back(new TBufBlock());
      for (size_t i = 0; i < NumSectorsPerBlock; ++i) {

        VolMan->Read(HERE,
                     Util::CheckedSector,
                     Source::FileService,
                     append_log_buf_vec.back()->GetData() + (i * Util::PhysicalSectorSize),
                     (block_id * Util::PhysicalBlockSize) + (i * Util::PhysicalSectorSize),
                     Util::PhysicalSectorSize,
                     RealTime,
                     trigger,
                     true /* We think of sectors as atomic. If our sector read fails we're favoring abort over recovering an old state. */);
      }
    }

    for (size_t i = 0; i < NumAppendLogSectors; ++i) {
      ring_offset_vec.push_back((AppendLogBlockVec[i / NumSectorsPerBlock] * Util::PhysicalBlockSize) + ((i % NumSectorsPerBlock) * Util::PhysicalSectorSize));
    }
    try {
      trigger.Wait();
    } catch (const TDiskError &ex) {
      syslog(LOG_ERR, "File System is corrupt. Caught Disk Corruption in append log.");
      throw std::runtime_error("File System is corrupt. Caught Disk Corruption in append log.");
    }
    /* if one of the base images was corrupt, and the first version number (fvn) in the append log is greater than our base image version number (bivn) by more than 1, then we're in a corrupt state */ {
      const size_t *buf = reinterpret_cast<size_t *>(append_log_buf_vec[0]->GetData());
      const size_t fvn = buf[0];
      if (!both_images_loaded_success && fvn > VersionNumber && fvn - VersionNumber > 1) {
        throw std::runtime_error("File System is corrupt. Append log is past available base image. The required (alternate) base is corrupt");
      }
    }

    std::lock_guard<std::mutex> lock(Mutex);
    bool done = false;
    size_t expected_version = VersionNumber + 1UL;
    for (const auto &buf_block : append_log_buf_vec) {
      for (size_t i = 0; i < NumSectorsPerBlock; ++i) {
        const size_t offset_in_buf_of_sector = i * Util::PhysicalSectorSize;
        const size_t *buf = reinterpret_cast<size_t *>(buf_block->GetData() + offset_in_buf_of_sector);
        const size_t version = *buf;
        if (version == expected_version) {
          VersionNumber = version;
          ++expected_version;
          buf += 1UL;
          ApplyDeltasToMap(Map, buf, NumFiles);
          ApplyDeltasToMap(RunnerCopyMap, buf, NumRunnerCopyFiles);
          ++CurRingSector;
        } else {
          done = true;
          break;
        }
      }
      if (done) {
        break;
      }
    }
  } else {
    /* make sure we zero out both image blocks, and all the append log blocks */
    ZeroImageBlocks(image_1_block_id, image_2_block_id);
    ZeroAppendLog();
  }
  /* if we reconstructed from a cold image, this is the opportunity for us to give each file a chance to mark its blocks as used. */
  if (!create) {
    std::lock_guard<std::mutex> lock(Mutex);
    bool keep_going = true;
    for (const auto &uid_map : Map) {
      for (const auto &file_pair : uid_map.second) {
        const auto &file = file_pair.second;
        keep_going = file_init_cb(file.Kind, uid_map.first, file_pair.first, file.StartingBlockId, file.StartingBlockOffset, file.FileSize);
        if (!keep_going) {
          break;
        }
      }
      if (!keep_going) {
        break;
      }
    }
  }
  scheduler->Schedule(std::bind(Fiber::LaunchSlowFiberSched, &BGScheduler, frame_pool_manager));
  Frame = Fiber::TFrame::LocalFramePool->Alloc();
  try {
    Frame->Latch(&BGScheduler, this, static_cast<Fiber::TRunnable::TFunc>(&TFileService::Runner));
  } catch (...) {
    Fiber::TFrame::LocalFramePool->Free(Frame);
    throw;
  }
  //Scheduler->Schedule(std::bind(&TFileService::Runner, this));
}

TFileService::~TFileService() {
  ShuttingDown = true;
  RunSem.Push();
  BGScheduler.ShutDown();
}

void TFileService::InsertFile(const Base::TUuid &file_uid,
                              TFileObj::TKind file_kind,
                              size_t file_gen,
                              size_t starting_block_id,
                              size_t starting_block_offset,
                              size_t file_size,
                              size_t num_keys,
                              TSequenceNumber lowest_seq,
                              TSequenceNumber highest_seq,
                              TCompletionTrigger &trigger) {
  /* Acquire Mutex */ {
    std::lock_guard<std::mutex> lock(Mutex);
    AddToMap(Map,
             file_uid,
             file_kind,
             file_gen,
             starting_block_id,
             starting_block_offset,
             file_size,
             num_keys,
             lowest_seq,
             highest_seq);
    ++NumFiles;
  }  // release Mutex
  TOp *op = new TOp(TOp::InsertFile,
                    file_uid,
                    std::move(TFileObj(file_kind,
                                       file_gen,
                                       starting_block_id,
                                       starting_block_offset,
                                       file_size,
                                       num_keys,
                                       lowest_seq,
                                       highest_seq)),
                    trigger);
  try {
    /*Acquire Queue lock */ {
      std::lock_guard<std::mutex> lock(QueueLock);
      /* No-throws only */
      OpQueue.Insert(op->GetQueueMembership());
    }  // release Queue lock
  } catch (...) {
    delete op;
    throw;
  }
  RunSem.Push();
}

void TFileService::RemoveFile(const Base::TUuid &file_uid,
                              size_t file_gen,
                              TCompletionTrigger &trigger) {
  /* acquire Mutex */ {
    std::lock_guard<std::mutex> lock(Mutex);
    RemoveFromMap(Map,
                  file_uid,
                  file_gen);
    --NumFiles;
  }  // release Mutex
  TOp *op = new TOp(TOp::RemoveFile,
                    file_uid,
                    std::move(TFileObj(TFileObj::TKind::DataFile /* not used */,
                                       file_gen,
                                       0UL,
                                       0UL,
                                       0UL,
                                       0UL,
                                       0UL,
                                       0UL)),
                    trigger);
  try {
    /*Acquire Queue lock */ {
      std::lock_guard<std::mutex> lock(QueueLock);
      /* No-throws only */
      OpQueue.Insert(op->GetQueueMembership());
    }  // release Queue lock
  } catch (...) {
    delete op;
    throw;
  }
  RunSem.Push();
}

bool TFileService::FindFile(const Base::TUuid &file_uid,
                            size_t file_gen,
                            size_t &out_block_id,
                            size_t &out_block_offset,
                            size_t &out_file_size,
                            size_t &out_num_keys) const {
  std::lock_guard<std::mutex> lock(Mutex);
  auto uid_ret = Map.find(file_uid);
  if (uid_ret != Map.end()) {
    const auto &gen_map = uid_ret->second;
    auto gen_ret = gen_map.find(file_gen);
    if (gen_ret != gen_map.end()) {
      const auto &file = gen_ret->second;
      out_block_id = file.StartingBlockId;
      out_block_offset = file.StartingBlockOffset;
      out_file_size = file.FileSize;
      out_num_keys = file.NumKeys;
      return true;
    }
    return false;
  }
  return false;
}

void TFileService::AppendFileGenSet(const Base::TUuid &file_uid,
                                    std::vector<TFileObj> &out_vec) {
  std::lock_guard<std::mutex> lock(Mutex);
  auto uid_ret = Map.find(file_uid);
  if (uid_ret != Map.end()) {
    const auto &gen_map = uid_ret->second;
    for (const auto &fi : gen_map) {
      const auto &file = fi.second;
      out_vec.emplace_back(file);
    }
  }
}

bool TFileService::ForEachFile(const std::function<bool (const Base::TUuid &file_uid, const TFileObj &)> &cb) {
  std::lock_guard<std::mutex> lock(Mutex);
  for (auto uid_map : Map) {
    for (auto file : uid_map.second) {
      if (!cb(uid_map.first, file.second)) {
        return false;
      }
    }
  }
  return true;
}

void TFileService::Runner() {
  assert(this);
  assert(Util::PhysicalBlockSize >= Util::PhysicalSectorSize);
  /* allocate event pools */
  if (!Disk::Util::TDiskController::TEvent::LocalEventPool) {
    Disk::Util::TDiskController::TEvent::LocalEventPool = new Base::TThreadLocalPoolManager<Disk::Util::TDiskController::TEvent>::TThreadLocalRegisteredPool(&Disk::Util::TDiskController::TEvent::DiskEventPoolManager, 1000UL);
  }
  std::vector<size_t> ring_offset_vec;
  for (size_t i = 0; i < NumAppendLogSectors; ++i) {
    ring_offset_vec.push_back((AppendLogBlockVec[i / NumSectorsPerBlock] * Util::PhysicalBlockSize) + ((i % NumSectorsPerBlock) * Util::PhysicalSectorSize));
  }
  size_t *ring_buf = nullptr;
  Base::IfLt0(posix_memalign(reinterpret_cast<void **>(&ring_buf), getpagesize(), getpagesize()));
  std::vector<std::unique_ptr<TBufBlock>> image_buf_block_vec;
  image_buf_block_vec.emplace_back(new TBufBlock());
  TCompletionTrigger append_log_flush_trigger;
  try {
    for (;;) {
      RunSem.Pop();
      if (!ShuttingDown) {
        TOpQueue::TImpl cur_queue(this);
        size_t to_apply = 0UL;
        const size_t cur_version_num = ++VersionNumber;
        /* Acquire Queue lock */ {
          std::lock_guard<std::mutex> lock(QueueLock);
          TOp *op = nullptr;
          memset(ring_buf, 0, Util::PhysicalSectorSize);
          size_t *buf = ring_buf;
          *buf = cur_version_num;
          buf += 1;
          for (to_apply = 0; to_apply < NumFilesPerRingBuf; ++to_apply, buf += (EntrySize + OpSize)) {
            op = OpQueue.TryGetFirstMember();
            if (op) {
              op->Apply(RunnerCopyMap, NumRunnerCopyFiles);
              op->Apply(buf);
              op->Remove();
              cur_queue.Insert(op->GetQueueMembership());
            } else {
              break;
            }
            if (to_apply > 0) {
              RunSem.Pop();
            }
          }
        }  // release Queue lock
        try {
          if (CurRingSector == ring_offset_vec.size()) { /* we've filled the current ring buffer */
            TCompletionTrigger trigger;
            /* write out a new base image */
            const size_t blocks_required_for_base_image = std::max(1UL, static_cast<size_t>(ceil(static_cast<double>(NumRunnerCopyFiles) / NumElemPerBaseImageBlock)));
            auto &cur_image_block_vec = (CurBaseImageCounter % 2 == 0) ? Image1BlockIdVec : Image2BlockIdVec;
            if (cur_image_block_vec.size() > blocks_required_for_base_image) {
              size_t removed = 0UL;
              const size_t to_remove = cur_image_block_vec.size() - blocks_required_for_base_image;
              for (size_t i = cur_image_block_vec.size() - 1; i > 0 && removed < to_remove; --i, ++removed) {
                VolMan->FreeSequentialBlocks(Util::TBlockRange(cur_image_block_vec[i], 1UL));
              }
              cur_image_block_vec.resize(blocks_required_for_base_image);
            } else if (cur_image_block_vec.size() < blocks_required_for_base_image) {
              const size_t to_add = blocks_required_for_base_image - cur_image_block_vec.size();
              for (size_t i = 0; i < to_add; ++i) {
                VolMan->TryAllocateSequentialBlocks(Util::TVolume::TDesc::TStorageSpeed::Fast, 1UL, [&](const Util::TBlockRange &block_range) {
                  assert(block_range.second == 1UL);
                  cur_image_block_vec.push_back(block_range.first);
                });
              }
            }
            if (image_buf_block_vec.size() > blocks_required_for_base_image) {
              image_buf_block_vec.resize(blocks_required_for_base_image);
            } else if (image_buf_block_vec.size() < blocks_required_for_base_image) {
              const size_t to_add = blocks_required_for_base_image - image_buf_block_vec.size();
              for (size_t i = 0; i < to_add; ++i) {
                image_buf_block_vec.emplace_back(new TBufBlock());
              }
            }
            const std::unordered_map<size_t, TFileObj> empty_gen_map {};
            auto uid_iter = RunnerCopyMap.cbegin();
            const auto uid_end = RunnerCopyMap.cend();
            auto gen_iter = (uid_iter != uid_end) ? uid_iter->second.cbegin() : empty_gen_map.cbegin();
            auto gen_end = (uid_iter != uid_end) ? uid_iter->second.cend() : empty_gen_map.cend();
            size_t num_done = 0UL;
            std::vector<Util::TBlockRange> flush_block_range_vec;
            for (size_t pos = 0; pos < blocks_required_for_base_image; ++pos) {
              size_t *buf = reinterpret_cast<size_t *>(image_buf_block_vec[pos]->GetData());
              memset(buf, 0, Util::PhysicalBlockSize);
              *buf = cur_version_num;
              buf += 1;
              *buf = (pos != blocks_required_for_base_image - 1) ? cur_image_block_vec[pos + 1] : -1;
              buf += 1;
              for (size_t idx = 0; idx < NumElemPerBaseImageBlock && num_done < NumRunnerCopyFiles; ++idx, ++num_done) {
                assert(uid_iter != uid_end);
                const TFileObj &file = gen_iter->second;
                assert(reinterpret_cast<char *>(buf) < image_buf_block_vec[pos]->GetData() + Util::PhysicalBlockSize);
                uuid_copy(*reinterpret_cast<uuid_t *>(buf), uid_iter->first.GetRaw());
                *(buf + 2) = file.GenId; // file_gen
                *(buf + 3) = file.StartingBlockId; // starting_block_id
                *(buf + 4) = file.StartingBlockOffset; // starting_block_offset
                *(buf + 5) = file.FileSize; // file_size
                *(buf + 6) = file.NumKeys; // num_keys
                *(buf + 7) = file.LowestSeq; // lowest_seq
                *(buf + 8) = file.HighestSeq; // highest_seq
                *(buf + 9) = static_cast<uint64_t>(file.Kind); // file_kind
                buf += EntrySize;
                ++gen_iter;
                if (gen_iter == gen_end) {
                  ++uid_iter;
                  if (uid_iter != uid_end) {
                    gen_iter = uid_iter->second.cbegin();
                    gen_end = uid_iter->second.cend();
                    assert(gen_iter != gen_end);
                  }
                }
              }
              flush_block_range_vec.emplace_back(Util::TBlockRange{cur_image_block_vec[pos], 1UL});
              VolMan->WriteBlock(HERE,
                                 Util::CheckedBlock,
                                 Source::FileService,
                                 image_buf_block_vec[pos]->GetData(),
                                 cur_image_block_vec[pos],
                                 RealTime,
                                 Util::TCacheInstr::NoCache,
                                 trigger);
            }
            assert(num_done == NumRunnerCopyFiles);
            assert(uid_iter == uid_end);
            trigger.Wait();
            VolMan->SyncToDisk(flush_block_range_vec);
            CurRingSector = 0UL;
            ++CurBaseImageCounter;
          } else {
            assert(to_apply > 0);
            assert(ring_offset_vec.size() > CurRingSector);
            assert(ring_offset_vec[CurRingSector] % Util::PhysicalSectorSize == 0);
            VolMan->WriteAndFlush(HERE,
                                  Util::CheckedSector,
                                  Source::FileService,
                                  reinterpret_cast<char *>(ring_buf),
                                  ring_offset_vec[CurRingSector],
                                  Util::PhysicalSectorSize,
                                  RealTime,
                                  Util::TCacheInstr::NoCache,
                                  append_log_flush_trigger);
            ++CurRingSector;
          }
          for(;;) {
            TOp *op = cur_queue.TryGetFirstMember();
            if (op) {
              op->Complete(Success, nullptr);
              delete op;
            } else {
              break;
            }
          }
        } catch (const TDiskFailure &err) {
          for(;;) {
            TOp *op = cur_queue.TryGetFirstMember();
            if (op) {
              op->Complete(DiskFailure, nullptr);
              delete op;
            } else {
              break;
            }
          }
          throw;
        } catch (const TDiskError &err) {
          for(;;) {
            TOp *op = cur_queue.TryGetFirstMember();
            if (op) {
              op->Complete(Error, err.what());
              delete op;
            } else {
              break;
            }
          }
          throw;
        } catch (const TDiskServiceShutdown &err) {
          for(;;) {
            TOp *op = cur_queue.TryGetFirstMember();
            if (op) {
              op->Complete(ServerShutdown, nullptr);
              delete op;
            } else {
              break;
            }
          }
          throw;
        }
      } else {
        break;
      }
    }
    free(ring_buf);
  } catch (const std::exception &ex) {
    free(ring_buf);
    if (Disk::Util::TDiskController::TEvent::LocalEventPool) {
      delete Disk::Util::TDiskController::TEvent::LocalEventPool;
      Disk::Util::TDiskController::TEvent::LocalEventPool = nullptr;
    }
    throw;
  }
  if (Disk::Util::TDiskController::TEvent::LocalEventPool) {
    delete Disk::Util::TDiskController::TEvent::LocalEventPool;
    Disk::Util::TDiskController::TEvent::LocalEventPool = nullptr;
  }
  Fiber::FreeMyFrame(Fiber::TFrame::LocalFramePool);
}

void TFileService::AddToMap(TFileMap &file_map,
                            const Base::TUuid &file_uid,
                            TFileObj::TKind file_kind,
                            size_t file_gen,
                            size_t starting_block_id,
                            size_t starting_block_offset,
                            size_t file_size,
                            size_t num_keys,
                            TSequenceNumber lowest_seq,
                            TSequenceNumber highest_seq) {
  auto res = file_map.find(file_uid);
  if (res != file_map.end()) {
    auto r = res->second.emplace(file_gen, TFileObj(file_kind,
                                                    file_gen,
                                                    starting_block_id,
                                                    starting_block_offset,
                                                    file_size,
                                                    num_keys,
                                                    lowest_seq,
                                                    highest_seq));
    if (unlikely(!r.second)) {
      stringstream ss;
      ss << file_uid;
      syslog(LOG_ERR, "Trying to insert file that already exists [%s], [%ld]", ss.str().c_str(), file_gen);
      throw std::logic_error("Trying to insert file that already exists");
    }
  } else {
    file_map.emplace(file_uid, std::unordered_map<size_t, TFileObj>{{file_gen, TFileObj(file_kind,
                                                                                        file_gen,
                                                                                        starting_block_id,
                                                                                        starting_block_offset,
                                                                                        file_size,
                                                                                        num_keys,
                                                                                        lowest_seq,
                                                                                        highest_seq)}});
  }
}

void TFileService::RemoveFromMap(TFileMap &file_map,
                                 const Base::TUuid &file_uid,
                                 size_t file_gen) {
  auto res = file_map.find(file_uid);
  if (likely(res != file_map.end())) {
    const size_t num_removed = res->second.erase(file_gen);
    if (unlikely(num_removed != 1)) {
      stringstream ss;
      ss << file_uid;
      syslog(LOG_ERR, "Trying to remove file that does not exist [%s], [%ld]", ss.str().c_str(), file_gen);
      throw std::logic_error("Trying to remove file gen that does not exist");
    }
  } else {
    stringstream ss;
      ss << file_uid;
      syslog(LOG_ERR, "Trying to remove file that does not exist [%s], [%ld]", ss.str().c_str(), file_gen);
    throw std::logic_error("Trying to remove file that does not exist");
  }
}

void TFileService::ApplyDeltasToMap(TFileMap &file_map,
                                    const size_t *buf,
                                    size_t &num_files_in_map) {
  for (size_t i = 0; i < NumFilesPerRingBuf; ++i, buf += (EntrySize + OpSize)) {
    if (buf[0] == 0UL && buf[1] == 0UL) { /* if the first 16 bytes are all 0, then this is the end of the ring buffer log. */
      break;
    } else {
      size_t file_gen, starting_block_id, starting_block_offset, file_size, num_keys;
      TSequenceNumber lowest_seq, highest_seq;
      TFileObj::TKind file_kind = TFileObj::TKind::DataFile;
      uuid_t temp_uid;
      uuid_copy(temp_uid, *reinterpret_cast<const uuid_t *>(buf));
      file_gen = buf[2]; // file_gen
      starting_block_id = buf[3]; // starting_block_id
      starting_block_offset = buf[4]; // starting_block_offset
      file_size = buf[5]; // file_size
      num_keys = buf[6]; // num_keys
      lowest_seq = buf[7]; // lowest_seq
      highest_seq = buf[8]; // highest_seq
      switch (buf[9]) {
        case TFileObj::TKind::DataFile: {
          file_kind = TFileObj::TKind::DataFile;
          break;
        }
        case TFileObj::TKind::DurableFile: {
          file_kind = TFileObj::TKind::DurableFile;
          break;
        }
      }
      switch (buf[10]) {
        case TOp::TKind::InsertFile: {
          AddToMap(file_map, Base::TUuid(temp_uid), file_kind, file_gen, starting_block_id, starting_block_offset, file_size, num_keys, lowest_seq, highest_seq);
          ++num_files_in_map;
          break;
        }
        case TOp::TKind::RemoveFile: {
          RemoveFromMap(file_map, Base::TUuid(temp_uid), file_gen);
          --num_files_in_map;
          break;
        }
      }
    }
  }
}

void TFileService::ApplyImageBlock(TFileMap &file_map,
                                   const size_t *buf,
                                   size_t &num_files_in_map) {
  for (size_t i = 0; i < NumElemPerBaseImageBlock; ++i, buf += EntrySize) {
    if (*buf == 0UL && *(buf + 1) == 0UL) { /* if the first 16 bytes are all 0, then this is the end of the ring buffer log. */
      break;
    } else {
      size_t file_gen, starting_block_id, starting_block_offset, file_size, num_keys;
      TSequenceNumber lowest_seq, highest_seq;
      TFileObj::TKind file_kind = TFileObj::TKind::DataFile;
      uuid_t temp_uid;
      uuid_copy(temp_uid, *reinterpret_cast<const uuid_t *>(buf));
      file_gen = *(buf + 2); // file_gen
      starting_block_id = *(buf + 3); // starting_block_id
      starting_block_offset = *(buf + 4); // starting_block_offset
      file_size = *(buf + 5); // file_size
      num_keys = *(buf + 6); // num_keys
      lowest_seq = *(buf + 7); // lowest_seq
      highest_seq = *(buf + 8); // highest_seq
      switch (buf[9]) {
        case TFileObj::TKind::DataFile: {
          file_kind = TFileObj::TKind::DataFile;
          break;
        }
        case TFileObj::TKind::DurableFile: {
          file_kind = TFileObj::TKind::DurableFile;
          break;
        }
      }
      AddToMap(file_map, Base::TUuid(temp_uid), file_kind, file_gen, starting_block_id, starting_block_offset, file_size, num_keys, lowest_seq, highest_seq);
      ++num_files_in_map;
    }
  }
}

bool TFileService::TryLoadFromBaseImage(size_t base_image_block, const size_t *cur_buf, TBufBlock *cur_buf_block) {
  assert(this);
  TCompletionTrigger trigger;
  Map.clear();
  NumFiles = 0UL;
  RunnerCopyMap.clear();
  NumRunnerCopyFiles = 0UL;
  bool process_next_block = true;
  try {
    while (process_next_block) {
      const size_t cur_version_number = cur_buf[0];
      if (VersionNumber != cur_version_number) {
        throw std::runtime_error("Reading partial block image");
      }
      ApplyImageBlock(Map, &cur_buf[2], NumFiles);  // start after the version and next_block data
      ApplyImageBlock(RunnerCopyMap, &cur_buf[2], NumRunnerCopyFiles);  // start after the version and next_block data
      const size_t next_block_id = cur_buf[1];
      process_next_block = next_block_id != static_cast<size_t>(-1);
      if (process_next_block) {
        VolMan->ReadBlock(HERE,
                          Util::CheckedBlock,
                          Source::FileService,
                          cur_buf_block->GetData(),
                          next_block_id,
                          RealTime,
                          trigger,
                          false /* don't abort on error since we're trying to build a valid image */);
        trigger.Wait();
      }
    }
  } catch (const std::exception &ex) {
    syslog(LOG_ERR, "TFileService::TryLoadFromBaseImage caught exception while loading from block [%ld], ex: [%s]", base_image_block, ex.what());
    return false;
  }
  return true;
}

void TFileService::ZeroImageBlocks(size_t image_1_block_id, size_t image_2_block_id) {
  assert(this);
  std::unique_ptr<TBufBlock> buf_block(new TBufBlock());
  memset(buf_block->GetData(), 0, Util::PhysicalBlockSize);
  TCompletionTrigger trigger;
  /* zero-out image_block 1 */ {
    VolMan->WriteBlock(HERE,
                       Util::CheckedBlock,
                       Source::FileService,
                       buf_block->GetData(),
                       image_1_block_id,
                       RealTime,
                       Util::TCacheInstr::NoCache,
                       trigger);
    trigger.Wait();
  }
  /* zero-out image_block 2 */ {
    VolMan->WriteBlock(HERE,
                       Util::CheckedBlock,
                       Source::FileService,
                       buf_block->GetData(),
                       image_2_block_id,
                       RealTime,
                       Util::TCacheInstr::NoCache,
                       trigger);
    trigger.Wait();
  }
  trigger.Wait();
}

void TFileService::ZeroAppendLog() {
  assert(this);
  TCompletionTrigger trigger;
  std::unique_ptr<TBufBlock> buf_block(new TBufBlock());
  memset(buf_block->GetData(), 0, Util::PhysicalBlockSize);
  for (auto block_id : AppendLogBlockVec) { /* zero-out each append block */
    for (size_t i = 0; i < NumSectorsPerBlock; ++i) {
      VolMan->Write(HERE,
                    Util::CheckedSector,
                    Source::FileService,
                    buf_block->GetData() + (i * Util::PhysicalSectorSize),
                    (block_id * Util::PhysicalBlockSize) + (i * Util::PhysicalSectorSize),
                    Util::PhysicalSectorSize,
                    RealTime,
                    Util::TCacheInstr::NoCache,
                    trigger);
      trigger.Wait();
    }
  }
}
