/* <stig/indy/disk/durable_manager.cc>

   Implements <stig/indy/disk/durable_manager.h>.

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

#include <stig/indy/disk/durable_manager.h>

#include <base/booster.h>
#include <stig/indy/disk/util/hash_util.h>

using namespace std;
using namespace Base;
using namespace Stig::Indy::Util;
using namespace Stig::Indy::Disk;

const Base::TUuid TDurableManager::DurableByIdFileId("20E91BAE-3465-4E9B-918F-C234DF84762A");

const Base::TUuid TDurableManager::TSortedByIdFile::NullId("00000000-0000-0000-0000-000000000000");

TDurableManager::TMapping::~TMapping() {
  assert(this);
  EntryCollection.DeleteEachMember();
}

void TDurableManager::TMemSlushLayer::FindMax(TSequenceNumber &cur_max_seq, const Base::TUuid &id, std::string &serialized_form_out) const {
  assert(this);
  for (TEntryCollection::TCursor csr(&EntryCollection); csr; ++csr) {
    int uuid_comp = uuid_compare(csr->GetId(), id.GetRaw());
    if (uuid_comp > 0) {
      break;
    } else if (uuid_comp == 0 && csr->GetSeqNum() > cur_max_seq) {
      cur_max_seq = csr->GetSeqNum();
      serialized_form_out = csr->GetSerializedForm();
    }
  }
}

TDurableManager::TDiskOrderedLayer::~TDiskOrderedLayer() {
  if (GetMarkedForDelete()) {
    try {
      TCompletionTrigger completion_trigger;
      Indy::Util::TBlockVec block_vec;
      /* read the blocks in the file */ {
        TSortedInFile in_file(Engine, Low, GenId);
        size_t num_blocks = in_file.GetNumBlocks();
        TInStream in_stream(HERE, Source::FileRemoval, Low, &in_file, Engine->GetPageCache(), TSortedByIdFile::NumMetaFields * sizeof(size_t));
        size_t block_id;
        for (size_t i = 0; i < num_blocks; ++i) {
          in_stream.Read(block_id);
          block_vec.PushBack(block_id);
        }
      }
      Engine->RemoveFile(DurableByIdFileId, GenId, completion_trigger);
      /* wait for everything to flush */ {
        completion_trigger.Wait();
      }
      for (const auto &iter : block_vec.GetSeqBlockMap()) {
        Engine->FreeSeqBlocks(iter.second.first, iter.second.second);
      }
    } catch (const Disk::TDiskServiceShutdown &/*ex*/) {
      /*ignore, we're shutting down by force! */
    }
  }
}

TDurableManager::TDurableManager(TScheduler *scheduler,
                                 Fiber::TRunner::TRunnerCons &runner_cons,
                                 Base::TThreadLocalPoolManager<Indy::Fiber::TFrame, size_t, Indy::Fiber::TRunner *> *frame_pool_manager,
                                 DurableManager::TManager *manager,
                                 Util::TEngine *engine,
                                 size_t max_cache_size,
                                 size_t write_delay,
                                 size_t merge_delay,
                                 size_t layer_cleaning_interval_milliseconds,
                                 size_t temp_file_consol_thresh,
                                 bool create,
                                 const TNotify *notify)
    : Durable::TManager(max_cache_size),
      Manager(manager),
      MergerScheduler(runner_cons),
      WriterScheduler(runner_cons),
      MergerFrame(nullptr),
      WriterFrame(nullptr),
      Engine(engine),
      CurMemoryLayer(new TMemSlushLayer(this)),
      SeqNum(1UL),
      NextSlushGenId(1UL),
      NextDurableByIdGenId(1UL),
      TempFileConsolThresh(temp_file_consol_thresh),
      DurableWriteDelay(write_delay),
      DurableMergeDelay(merge_delay),
      ShutDown(false),
      MappingCollection(this),
      RemovalCollection(this),
      LayerCleanerTimer(layer_cleaning_interval_milliseconds),
      Notify(notify) {
  /* acquire Mapping lock */ {
    std::lock_guard<std::mutex> mapping_lock(MappingLock);
    new TMapping(this);
  }  // release Mapping lock

  if (!create) {
    /* if we are starting from a cold image, add the mappings for the files that already exist */


    std::vector<Disk::TFileObj> file_vec;
    Engine->AppendFileGenSet(DurableByIdFileId, file_vec);
    size_t max_gen_id = 0UL;
    for (const auto &fi : file_vec) {
      max_gen_id = std::max(max_gen_id, fi.GenId);
      AddMapping(new TDiskOrderedLayer(this, Engine, fi.GenId, fi.NumKeys));
    }
    NextDurableByIdGenId = max_gen_id + 1UL;
  }
  scheduler->Schedule(std::bind(Fiber::LaunchSlowFiberSched, &WriterScheduler, frame_pool_manager));
  WriterFrame = Fiber::TFrame::LocalFramePool->Alloc();
  try {
    WriterFrame->Latch(&WriterScheduler, this, static_cast<Fiber::TRunnable::TFunc>(&TDurableManager::RunWriter));
  } catch (...) {
    Fiber::TFrame::LocalFramePool->Free(WriterFrame);
    throw;
  }
  scheduler->Schedule(std::bind(Fiber::LaunchSlowFiberSched, &MergerScheduler, frame_pool_manager));
  MergerFrame = Fiber::TFrame::LocalFramePool->Alloc();
  try {
    MergerFrame->Latch(&MergerScheduler, this, static_cast<Fiber::TRunnable::TFunc>(&TDurableManager::RunMerger));
  } catch (...) {
    Fiber::TFrame::LocalFramePool->Free(MergerFrame);
    throw;
  }
}

TDurableManager::~TDurableManager() {
  assert(this);
  /* we're going to lose the ones in memory because the shutdown routine we currently employ will interrupt any more disk writes
  assert(!CurMemoryLayer->GetNumEntries());
  */
  delete CurMemoryLayer;
  ShutDown = true;
  SlushSem.Push();
  MergeSem.Push();
  WriterFinishedSem.Pop();
  MergerFinishedSem.Pop();
  WriterScheduler.ShutDown();
  MergerScheduler.ShutDown();
  Fiber::TFrame::LocalFramePool->Free(MergerFrame);
  Fiber::TFrame::LocalFramePool->Free(WriterFrame);
}

void TDurableManager::CleanDisk(const Durable::TDeadline &/*now*/, Durable::TSem *sem) {
  sem->Push();
}

void TDurableManager::RunLayerCleaner() {
  assert(this);
  if (Engine->IsDiskBased()) {
    /* if this is a disk based engine, allocate event pools */
    assert(!Disk::Util::TDiskController::TEvent::LocalEventPool);
    Disk::Util::TDiskController::TEvent::LocalEventPool = new TThreadLocalPoolManager<Disk::Util::TDiskController::TEvent>::TThreadLocalRegisteredPool(&Disk::Util::TDiskController::TEvent::DiskEventPoolManager, 1000UL);
  }
  TDurableLayer *durable_layer = nullptr;
  for (;;) {
    LayerCleanerTimer.Pop();
    for (;;) {
      /* Acquire Removal lock */ {
        std::lock_guard<std::mutex> removal_lock(RemovalLock);
        durable_layer = RemovalCollection.TryGetFirstMember();
        if (durable_layer) {
          durable_layer->RemoveFromCollection();
        }
      }  // release Removal lock
      if (durable_layer) {
        delete durable_layer;
        durable_layer = nullptr;
      } else {
        break;
      }
    }
  }
}

bool TDurableManager::CanLoad(const Durable::TId &id) {
  TSequenceNumber cur_max_seq_num = 0UL;
  std::string serialized_form_out;
  TMapping::TView view(this);
  view.GetCurLayer()->FindMax(cur_max_seq_num, id, serialized_form_out);
  for (TMapping::TEntryCollection::TCursor csr(view.GetMapping()->GetEntryCollection()); csr; ++csr) {
    csr->GetLayer()->FindMax(cur_max_seq_num, id, serialized_form_out);
  }
  return cur_max_seq_num > 0UL;
}

void TDurableManager::Delete(const Durable::TId &/*id*/, Durable::TSem */*sem*/) {
  syslog(LOG_EMERG, "implement TDurableManager::Delete");
  throw;
}

void TDurableManager::Save(const Durable::TId &id, const Durable::TDeadline &deadline, const Durable::TTtl &ttl, const std::string &serialized_form, Durable::TSem *sem) {
  assert(this);
  assert(serialized_form.size() > 0);
  if (serialized_form.size() >= UINT32_MAX) {
    throw std::runtime_error("Serialized Durable size >= UINT32_MAX");
  }
  TDurableReplication *durable_replication = Manager->NewDurableReplication(id, ttl, serialized_form);
  try {
    std::string temp = serialized_form;
    /* acquire data lock */ {
      std::lock_guard<std::mutex> data_lock(DataLock);
      TSequenceNumber new_seq_num = ++SeqNum;
      assert(CurMemoryLayer);
      new TMemSlushLayer::TDurableEntry(CurMemoryLayer, id, deadline, std::move(temp), new_seq_num);
      Manager->EnqueueDurable(durable_replication);
    }  // release data lock
    SlushSem.Push();
    //SlushCounter.Push();
    /* TODO : we should only call the sem once we've actually written to disk! */
    sem->Push();
  } catch (...) {
    delete durable_replication;
    throw;
  }
}

bool TDurableManager::TryLoad(const Durable::TId &id, std::string &serialized_form_out) {
  TSequenceNumber cur_max_seq_num = 0UL;
  TMapping::TView view(this);
  view.GetCurLayer()->FindMax(cur_max_seq_num, id, serialized_form_out);
  for (TMapping::TEntryCollection::TCursor csr(view.GetMapping()->GetEntryCollection()); csr; ++csr) {
    csr->GetLayer()->FindMax(cur_max_seq_num, id, serialized_form_out);
  }
  return cur_max_seq_num > 0UL;
}

void TDurableManager::AddMapping(TDurableLayer *layer) {
  assert(this);
  /* acquire Mapping lock */ {
    std::lock_guard<std::mutex> mapping_lock(MappingLock);
    TMapping *last = MappingCollection.TryGetLastMember();
    last->Incr();
    assert(last);
    try {
      TMapping *new_mapping = new TMapping(this);
      for (TMapping::TEntryCollection::TCursor csr(last->GetEntryCollection()); csr; ++csr) {
        new TMapping::TEntry(new_mapping, csr->GetLayer());
      }
      new TMapping::TEntry(new_mapping, layer);
      last->Decr();
    } catch (const std::exception &ex) {
      syslog(LOG_ERR, "Error in TDurableManager::AddMapping [%s]", ex.what());
      throw;
    }
  }  // release Mapping lock
}

void TDurableManager::RunWriter() {
  assert(this);
  if (Engine->IsDiskBased()) {
    /* if this is a disk based engine, allocate event pools */
    assert(!Disk::Util::TDiskController::TEvent::LocalEventPool);
    Disk::Util::TDiskController::TEvent::LocalEventPool = new TThreadLocalPoolManager<Disk::Util::TDiskController::TEvent>::TThreadLocalRegisteredPool(&Disk::Util::TDiskController::TEvent::DiskEventPoolManager, 1000UL);
  }
  Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed = Disk::Util::TVolume::TDesc::TStorageSpeed::Fast;
  Base::TTime next_flush;
  next_flush.Now();
  next_flush += DurableWriteDelay;
  SlushSem.Pop();
  for (;!ShutDown; SlushSem.Pop()) {
    int64_t remaining = next_flush.Remaining();
    if (remaining) {
      timespec wait_spec {0, remaining * 1000000L};
      nanosleep(&wait_spec, nullptr);
    }

    next_flush.Now();
    next_flush += DurableWriteDelay;

    TMemSlushLayer *old_mem_layer = nullptr;
    /* acquire DataLayer lock */ {
      std::lock_guard<std::mutex> data_lock(DataLock);
      assert(CurMemoryLayer);
      if (CurMemoryLayer->GetNumEntries()) {
        old_mem_layer = CurMemoryLayer;
        AddMapping(CurMemoryLayer);
        CurMemoryLayer = new TMemSlushLayer(this);
      }
    }  // release DataLayer lock

    if (old_mem_layer) {
      auto now = Durable::TDeadline::clock::now();
      size_t gen_id = ++NextDurableByIdGenId;
      TSortedByIdFile sorted_by_id_file(old_mem_layer,
                                        Engine,
                                        storage_speed,
                                        gen_id,
                                        now.time_since_epoch().count(),
                                        TempFileConsolThresh,
                                        Medium);
      MergeSem.Push();

      /* acquire Mapping lock */ {
        std::lock_guard<std::mutex> mapping_lock(MappingLock);
        TMapping *cur_mapping = MappingCollection.TryGetLastMember();
        cur_mapping->Incr();
        try {
          TMapping *new_mapping = new TMapping(this);
          for (TMapping::TEntryCollection::TCursor cur_csr(cur_mapping->GetEntryCollection()); cur_csr; ++cur_csr) {
            /* if it's not old_mem_layer, keep it */
            if (cur_csr->GetLayer() != old_mem_layer) {
              new TMapping::TEntry(new_mapping, cur_csr->GetLayer());
            } else {
              cur_csr->GetLayer()->MarkForDelete();
            }
          }
          /* add our new sorted file */
          new TMapping::TEntry(new_mapping, new TDiskOrderedLayer(this, Engine, gen_id, sorted_by_id_file.GetNumDurable()));
          cur_mapping->Decr();
        } catch (...) {
          cur_mapping->Decr();
          throw;
        }
      }  // release Mapping lock
    }
  }
  WriterFinishedSem.Push();
}

void TDurableManager::RunMerger() {
  assert(this);
  if (Engine->IsDiskBased()) {
    /* if this is a disk based engine, allocate event pools */
    assert(!Disk::Util::TDiskController::TEvent::LocalEventPool);
    Disk::Util::TDiskController::TEvent::LocalEventPool = new TThreadLocalPoolManager<Disk::Util::TDiskController::TEvent>::TThreadLocalRegisteredPool(&Disk::Util::TDiskController::TEvent::DiskEventPoolManager, 1000UL);
  }
  Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed = Disk::Util::TVolume::TDesc::TStorageSpeed::Fast;
  Base::TTime next_flush;
  next_flush.Now();
  next_flush += DurableMergeDelay;
  MergeSem.Pop();
  for (;!ShutDown; MergeSem.Pop()) {

    int64_t remaining = next_flush.Remaining();
    if (remaining) {
      timespec wait_spec {0, remaining * 1000000L};
      nanosleep(&wait_spec, nullptr);
    }

    next_flush.Now();
    next_flush += DurableMergeDelay;


    std::vector<size_t> gen_vec;
    std::map<size_t, std::vector<TDiskOrderedLayer *>> gen_to_gen_id_map;
    std::vector<TDiskOrderedLayer *> gen_layer_vec;
    TMapping::TView view(this);
    for (TMapping::TEntryCollection::TCursor csr(view.GetMapping()->GetEntryCollection()); csr; ++csr) {
      if (csr->GetLayer()->GetKind() == TDurableLayer::DiskOrdered && !csr->GetLayer()->GetMarkedTaken()) {
        TDiskOrderedLayer *disk_layer = reinterpret_cast<TDiskOrderedLayer *>(csr->GetLayer());
        size_t suggested_gen = Util::SuggestGeneration(disk_layer->GetNumDurable());
        auto ret = gen_to_gen_id_map.insert(std::make_pair(suggested_gen, std::vector<TDiskOrderedLayer *>{disk_layer}));
        if (!ret.second) {
          ret.first->second.push_back(disk_layer);
        }
      }
    }
    bool done_picking = false;
    for (const auto &iter : gen_to_gen_id_map) {
      if (iter.second.size() >= 3) {
        if (!done_picking) {
          done_picking = true;
          for (auto layer : iter.second) {
            gen_vec.push_back(layer->GetGenId());
            gen_layer_vec.push_back(layer);
            assert(!layer->GetMarkedTaken());
            layer->MarkTaken();
          }
        }
        break;
      }
    }
    if (gen_vec.size()) {
      auto now = Durable::TDeadline::clock::now();
      size_t gen_id = ++NextDurableByIdGenId;
      TMergeSortedByIdFile merge_sort_file(gen_vec, Engine, storage_speed, gen_id, now.time_since_epoch().count(), TempFileConsolThresh, Low, Notify);
      /* acquire Mapping lock */ {
        std::lock_guard<std::mutex> mapping_lock(MappingLock);
        TMapping *cur_mapping = MappingCollection.TryGetLastMember();
        cur_mapping->Incr();
        try {
          TMapping *new_mapping = new TMapping(this);
          for (TMapping::TEntryCollection::TCursor cur_csr(cur_mapping->GetEntryCollection()); cur_csr; ++cur_csr) {
            /* keep it if it's not one we just merged */
            bool found = false;

            for (auto layer : gen_layer_vec) {
              if (layer == cur_csr->GetLayer()) {
                layer->MarkForDelete();
                found = true;
                break;
              }
            }
            if (!found) {
              new TMapping::TEntry(new_mapping, cur_csr->GetLayer());
            }
          }
          /* add our new sorted file */
          new TMapping::TEntry(new_mapping, new TDiskOrderedLayer(this, Engine, gen_id, merge_sort_file.GetNumDurable()));
          cur_mapping->Decr();
        } catch (...) {
          cur_mapping->Decr();
          throw;
        }
      }  // release Mapping lock
      MergeSem.Push();
    }
  }
  MergerFinishedSem.Push();
}

TDurableManager::TSortedByIdFile::TSortedByIdFile(TMemSlushLayer *mem_layer,
                                                  Util::TEngine *engine,
                                                  Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed,
                                                  size_t gen_id,
                                                  size_t latest_deadline_count,
                                                  size_t temp_file_consol_thresh,
                                                  DiskPriority priority)
    : Engine(engine), StorageSpeed(storage_speed), FileSize(0UL), NumDurable(0UL) {
  THashSorter hash_sorter(HERE, Source::DurableSortFileHashIndex, temp_file_consol_thresh, storage_speed, Engine, true);
  assert(mem_layer);
  assert(Engine);
  size_t total_durable_serialized_space = 0UL;
  size_t total_bytes = 0UL;
  size_t num_blocks = 0UL;
  size_t num_hash_fields = 0UL;

  size_t byte_offset_of_hash_index = 0UL;

  TCompletionTrigger completion_trigger;

  /* calculate some offset */ {
    Base::TOpt<Base::TUuid> last_id;
    for (TMemSlushLayer::TEntryCollection::TCursor csr(mem_layer->GetEntryCollection()); csr; ++csr) {
      TSerializedSize serialized_size = (*csr).GetSerializedSize();
      const uuid_t &cur_id = (*csr).GetId();
      const size_t cur_deadline_count = (*csr).GetDeadlineCount();
      if (!last_id || uuid_compare(cur_id, last_id->GetRaw()) != 0) {
        last_id = TUuid(cur_id);
        if (cur_deadline_count >= latest_deadline_count) {
          ++NumDurable;
          total_durable_serialized_space += serialized_size;  // size of serialized durable
        }
      }
    }

    num_hash_fields = Util::SuggestHashSize(NumDurable);

    total_bytes += NumMetaFields * sizeof(size_t);
    total_bytes += NumDurable * DurableEntrySize;
    total_bytes += total_durable_serialized_space;
    total_bytes += num_hash_fields * HashEntrySize;
  }

  /* calculate the number of blocks. */
  num_blocks = ceil(static_cast<double>(total_bytes) / Disk::Util::LogicalBlockSize);
  size_t num_bytes_for_block_data = num_blocks * sizeof(size_t);
  num_blocks = ceil(static_cast<double>(total_bytes + num_bytes_for_block_data) / Disk::Util::LogicalBlockSize);
  num_bytes_for_block_data = num_blocks * sizeof(size_t);
  num_blocks = ceil(static_cast<double>(total_bytes + num_bytes_for_block_data) / Disk::Util::LogicalBlockSize);
  num_bytes_for_block_data = num_blocks * sizeof(size_t);
  num_blocks = ceil(static_cast<double>(total_bytes + num_bytes_for_block_data) / Disk::Util::LogicalBlockSize);

  byte_offset_of_hash_index = (NumMetaFields + num_blocks) * sizeof(size_t);
  byte_offset_of_hash_index += NumDurable * DurableEntrySize;
  byte_offset_of_hash_index += total_durable_serialized_space;

  /* reserve the blocks. */
  Engine->AppendReserveBlocks(StorageSpeed, num_blocks, BlockVec);
  #ifndef NDEBUG
  std::unordered_set<size_t> written_block_set;
  #endif

  unordered_map<size_t, shared_ptr<const TBufBlock>> block_collision_map;

  /* set up block collision map */ {
    auto ret = block_collision_map.insert(make_pair(byte_offset_of_hash_index / Disk::Util::LogicalBlockSize, nullptr));
    if (ret.second) { // fresh insert
      ret.first->second = std::shared_ptr<const TBufBlock>(new TBufBlock());
    }
  }

  /* write out the file */ {
    TDataOutStream out_stream(HERE,
                              Source::DurableSortFileData,
                              Engine->GetVolMan(),
                              0UL,
                              BlockVec,
                              block_collision_map,
                              completion_trigger,
                              priority,
                              true,
                              #ifndef NDEBUG
                              written_block_set,
                              #endif
                              [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});

    out_stream << NumDurable;  // NumEntries
    out_stream << num_blocks;  // NumBlocks
    out_stream << byte_offset_of_hash_index;  // HashIndexOffset
    out_stream << num_hash_fields;  // HashFieldSize

    /* write out the block ids */
    for (auto iter : BlockVec) {
      out_stream << iter;  // block_id
    }

    /* write out num_entries * (DurableEntry + string)
       1. durable id
       2. seq_num
       3. deadline count
       4. size of serialized string.
       5. serialized string.
    */
    size_t key_offset = 0UL;
    Base::TOpt<Base::TUuid> last_id;
    for (TMemSlushLayer::TEntryCollection::TCursor csr(mem_layer->GetEntryCollection()); csr; ++csr) {



      TSerializedSize serialized_size = (*csr).GetSerializedSize();
      const uuid_t &cur_id = (*csr).GetId();
      const size_t cur_deadline_count = (*csr).GetDeadlineCount();
      if (!last_id || uuid_compare(cur_id, last_id->GetRaw()) != 0) {
        last_id = TUuid(cur_id);
        if (cur_deadline_count >= latest_deadline_count) {
          const size_t id_hash = *reinterpret_cast<size_t *>(const_cast<unsigned char *>(cur_id));
          out_stream.Write(&cur_id, sizeof(uuid_t));  // durable_id
          out_stream << (*csr).GetSeqNum();  // seq_num
          out_stream << (*csr).GetDeadlineCount();  // deadline_count
          out_stream << (*csr).GetSerializedSize();  // size of serialized string
          out_stream.Write((*csr).GetSerializedForm().data(), serialized_size); /* the serialized string. */
          hash_sorter.Emplace(cur_id, id_hash % num_hash_fields, key_offset);
          key_offset += DurableEntrySize + serialized_size;
        }
      }
    }
    FileSize = out_stream.GetOffset();
  }
  /* write out the hash index */ {
    THashSorter::TCursor hash_csr(&hash_sorter, 16UL);

    /* do the first pass over the hash map */ {
      TDataOutStream stream(HERE,
                            Source::DurableSortFileHash,
                            Engine->GetVolMan(),
                            byte_offset_of_hash_index,
                            BlockVec,
                            block_collision_map,
                            completion_trigger,
                            priority,
                            true,
                            #ifndef NDEBUG
                            written_block_set,
                            #endif
                            [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});

      for (size_t i = 0; i < num_hash_fields; ++i) {
        if (hash_csr && (*hash_csr).Hash <= i) {
          const THashObj &obj = *hash_csr;
          stream.Write(&obj.Id, sizeof(uuid_t));
          stream << obj.Offset;
          ++hash_csr;
        } else {
          stream.Write(&NullId.GetRaw(), sizeof(uuid_t));
          stream << 0UL;
        }
      }
      FileSize = stream.GetOffset();
    }  // do the first pass over the hash map
    if (hash_csr) {
      /* flush the collision blocks surrounding the hash map */ {
        Engine->GetVolMan()->WriteBlock(HERE,
                                        Disk::Util::PageCheckedBlock,
                                        Source::DurableSortFileHash,
                                        block_collision_map[byte_offset_of_hash_index / Disk::Util::LogicalBlockSize]->GetData(),
                                        BlockVec[byte_offset_of_hash_index / Disk::Util::LogicalBlockSize],
                                        priority,
                                        Disk::Util::CacheAll,
                                        completion_trigger);
      }
      /* wait for the first pass to finish flushing before doing the wrap around pass */ {
        completion_trigger.Wait();
      }

      TDataOutStream out_stream(HERE,
                                Source::DurableSortFileHash,
                                Engine->GetVolMan(),
                                byte_offset_of_hash_index,
                                BlockVec,
                                block_collision_map,
                                completion_trigger,
                                priority,
                                true,
                                #ifndef NDEBUG
                                written_block_set,
                                #endif
                                [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
      TMyFile my_file(this);
      TInStream in_stream(HERE, Source::DurableSortFileHash, priority, &my_file, Engine->GetPageCache(), byte_offset_of_hash_index);
      uuid_t cur_id;
      size_t key_ptr = 0;
      for (size_t i = 0; i < num_hash_fields; ++i) {
        in_stream.Read(&cur_id, sizeof(uuid_t));
        in_stream.Read(key_ptr);
        if (memcmp(&cur_id, &NullId.GetRaw(), sizeof(uuid_t)) == 0) {
          assert(hash_csr);

          const THashObj &obj = *hash_csr;
          out_stream.Write(&obj.Id, sizeof(uuid_t));
          out_stream << obj.Offset;
          ++hash_csr;
        } else {
          out_stream.Write(&cur_id, sizeof(uuid_t));
          out_stream << key_ptr;
        }
        if (!hash_csr) {
          break;
        }
      }
      size_t left = std::min(Disk::Util::LogicalBlockSize - in_stream.GetOffset() % Disk::Util::LogicalBlockSize, FileSize - in_stream.GetOffset());
      char buf[left];
      in_stream.Read(buf, left);
      out_stream.Write(buf, left);
    }
    assert(!hash_csr);
  }
  /* flush collision blocks */ {
    for (auto iter : block_collision_map) {
      Engine->GetVolMan()->WriteBlock(HERE,
                                      Disk::Util::PageCheckedBlock,
                                      Source::DurableSortFileOther,
                                      iter.second->GetData(),
                                      BlockVec[iter.first],
                                      priority,
                                      Disk::Util::CacheAll,
                                      completion_trigger);
    }
    completion_trigger.Wait();
  }

  /* wait for the pages to get flushed */ {
    completion_trigger.Wait();
  }
  /* wait for file entry to flush */ {
    Engine->InsertFile(DurableByIdFileId, TFileObj::TKind::DurableFile, gen_id, BlockVec.Front(), 0UL, BlockVec.Size() * Disk::Util::LogicalBlockSize, NumDurable, 0UL, 0UL, completion_trigger);
    completion_trigger.Wait();
  }
}

TDurableManager::TSortedInFile::TSortedInFile(Util::TPageCache *page_cache,
                                              DiskPriority priority,
                                              size_t /*gen_id*/,
                                              size_t starting_block_id,
                                              size_t starting_block_offset,
                                              size_t file_length)
    : PageCache(page_cache),
      FileLength(file_length),
      StartingBlockId(starting_block_id),
      StartingBlockOffset(starting_block_offset) {
  InStream = std::unique_ptr<TInStream>(new TInStream(HERE, Source::DurableFetch, priority, this, PageCache, 0));
  InStream->Read(NumEntries);
  InStream->Read(NumBlocks);
  InStream->Read(HashIndexOffset);
  InStream->Read(HashFieldSize);
}

TDurableManager::TSortedInFile::TSortedInFile(Util::TEngine *engine, DiskPriority priority, size_t gen_id)
    : PageCache(engine->GetPageCache()) {
  size_t num_keys;
  if (!engine->FindFile(DurableByIdFileId, gen_id, StartingBlockId, StartingBlockOffset, FileLength, num_keys)) {
    std::ostringstream ss;
    ss << DurableByIdFileId;
    syslog(LOG_ERR, "TSortedInFile() Can not find file %s[%ld]", ss.str().c_str(), gen_id);
    throw std::runtime_error("Could not find file.");
  }
  InStream = std::unique_ptr<TInStream>(new TInStream(HERE, Source::DurableFetch, priority, this, PageCache, 0));
  InStream->Read(NumEntries);
  InStream->Read(NumBlocks);
  InStream->Read(HashIndexOffset);
  InStream->Read(HashFieldSize);
}

TDurableManager::TSortedInFile::~TSortedInFile() {}

size_t TDurableManager::TSortedInFile::GetFileLength() const {
  assert(this);
  return FileLength;
}

size_t TDurableManager::TSortedInFile::GetStartingBlock() const {
  return StartingBlockId;
}

void TDurableManager::TSortedInFile::ReadMeta(size_t offset, size_t &out) const {
  assert(this);
  assert(InStream);
  size_t cur_offset = InStream->GetOffset();
  assert(cur_offset < FileLength);
  InStream->GoTo(offset);
  InStream->Read(out);
  InStream->GoTo(cur_offset);
}

size_t TDurableManager::TSortedInFile::FindPageIdOfByte(size_t offset) const {
  assert(this);
  assert(offset < FileLength);
  size_t num_blocks_into_file = offset / Disk::Util::LogicalBlockSize;
  if (num_blocks_into_file == 0) {
    return (StartingBlockId * Disk::Util::PagesPerBlock) + ((offset % Disk::Util::LogicalBlockSize) / Disk::Util::LogicalPageSize);
  } else {
    size_t byte_offset_of_layout_block = (TSortedByIdFile::NumMetaFields + num_blocks_into_file) * sizeof(size_t);
    size_t ret;
    ReadMeta(byte_offset_of_layout_block, ret);
    ret = (ret * Disk::Util::PagesPerBlock) + ((offset % Disk::Util::LogicalBlockSize) / Disk::Util::LogicalPageSize);
    return ret;
  }
}

void TDurableManager::TSortedInFile::FindInHash(TSequenceNumber &cur_max_seq_num, const Durable::TId &id, std::string &serialized_form_out) const {
  assert(this);
  assert(&id);
  assert(&serialized_form_out);
  assert(&cur_max_seq_num);
  assert(InStream);
  uuid_t cur_id;
  TInStream hash_stream(HERE, Source::DurableMergeFileHash, RealTime, this, PageCache, HashIndexOffset);
  const size_t id_hash = *reinterpret_cast<size_t *>(const_cast<unsigned char *>(id.GetRaw()));
  size_t hash = id_hash % HashFieldSize;
  hash_stream.GoTo(HashIndexOffset + hash * (sizeof(uuid_t) + sizeof(size_t)));
  bool done = false;
  for (size_t i = hash; i < HashFieldSize && !done; ++i) {
    hash_stream.Read(&cur_id, sizeof(uuid_t));
    if (memcmp(&cur_id, &TSortedByIdFile::NullId.GetRaw(), sizeof(uuid_t)) == 0) {
      done = true;
    } else {
      if (uuid_compare(id.GetRaw(), cur_id) == 0) {
        size_t byte_offset_of_durable;
        hash_stream.Read(byte_offset_of_durable);
        InStream->GoTo(GetStartOfDurableByIdIndex() + byte_offset_of_durable);
        uuid_t found_id;
        TSequenceNumber cur_seq;
        InStream->Read(&found_id, sizeof(uuid_t));
        InStream->Read(cur_seq);
        if (cur_seq > cur_max_seq_num) {
          size_t cur_deadline;
          TSerializedSize cur_serialized_size;
          cur_max_seq_num = cur_seq;
          InStream->Read(cur_deadline);
          InStream->Read(cur_serialized_size);
          serialized_form_out.resize(cur_serialized_size);
          InStream->Read(const_cast<char *>(serialized_form_out.data()), cur_serialized_size);
        }
        return;
      } else if (*reinterpret_cast<size_t *>(const_cast<unsigned char *>(cur_id)) % HashFieldSize > hash) {
        return;
      } else {
        hash_stream.Skip(sizeof(size_t));
      }
    }
  }
  if (!done) {
    hash_stream.GoTo(HashIndexOffset);
    for (size_t i = 0; i < hash && !done; ++i) {
      hash_stream.Read(&cur_id, sizeof(uuid_t));
      if (memcmp(&cur_id, &TSortedByIdFile::NullId.GetRaw(), sizeof(uuid_t)) == 0) {
        done = true;
      } else {
        if (uuid_compare(id.GetRaw(), cur_id) == 0) {
          size_t byte_offset_of_durable;
          hash_stream.Read(byte_offset_of_durable);
          InStream->GoTo(GetStartOfDurableByIdIndex() + byte_offset_of_durable);
          uuid_t found_id;
          TSequenceNumber cur_seq;
          InStream->Read(&found_id, sizeof(uuid_t));
          InStream->Read(cur_seq);
          if (cur_seq > cur_max_seq_num) {
            size_t cur_deadline;
            TSerializedSize cur_serialized_size;
            cur_max_seq_num = cur_seq;
            InStream->Read(cur_deadline);
            InStream->Read(cur_serialized_size);
            serialized_form_out.resize(cur_serialized_size);
            InStream->Read(const_cast<char *>(serialized_form_out.data()), cur_serialized_size);
          }
          return;
        } else if (*reinterpret_cast<size_t *>(const_cast<unsigned char *>(cur_id)) % HashFieldSize > hash) {
          return;
        } else {
          hash_stream.Skip(sizeof(size_t));
        }
      }
    }
  }
}

TDurableManager::TMergeSortedByIdFile::TMergeSortedByIdFile(const std::vector<size_t> &gen_vec,
                                                            Util::TEngine *engine,
                                                            Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed,
                                                            size_t gen_id,
                                                            size_t latest_deadline_count,
                                                            size_t temp_file_consol_thresh,
                                                            DiskPriority priority,
                                                            const TNotify *notify)
    : Engine(engine), StorageSpeed(storage_speed), NumDurable(0UL), FileSize(0UL) {
  THashSorter hash_sorter(HERE, Source::DurableMergeFileHashIndex, temp_file_consol_thresh, StorageSpeed, Engine, true);
  std::vector<std::unique_ptr<TSortedInFile>> in_file_vec;
  for (auto iter : gen_vec) {
    in_file_vec.push_back(std::unique_ptr<TSortedInFile>(new TSortedInFile(Engine, priority, iter)));
  }

  size_t total_durable_serialized_space = 0UL;
  size_t total_bytes = 0UL;
  size_t num_blocks = 0UL;
  size_t num_hash_fields = 0UL;

  size_t byte_offset_of_hash_index = 0UL;

  TCompletionTrigger completion_trigger;

  /* calculations */ {

    TDurableSorter<size_t> sorter;
    TDurableSorter<size_t>::TMergeElement *durable_sorter_alloc = 0;
    std::vector<std::unique_ptr<TInStream>> in_stream_vec;
    std::vector<size_t> entries_left_vec;
    size_t pos = 0U;
    try {
      durable_sorter_alloc = reinterpret_cast<TDurableSorter<size_t>::TMergeElement *>(malloc(sizeof(TDurableSorter<size_t>::TMergeElement) * in_file_vec.size()));
      uuid_t cur_id;
      TSequenceNumber cur_seq;
      for (const std::unique_ptr<TSortedInFile> &iter : in_file_vec) {
        size_t num_entries_in_file = iter->GetNumEntries();
        size_t byte_offset_of_durable_index = iter->GetStartOfDurableByIdIndex();

        entries_left_vec.push_back(num_entries_in_file);
        in_stream_vec.push_back(std::unique_ptr<TInStream>(new TInStream(HERE, Source::DurableMergeFileScan, priority, iter.get(), Engine->GetPageCache(), byte_offset_of_durable_index)));

        if (entries_left_vec.back() > 0) {
          TInStream &entry_stream = *in_stream_vec.back();
          entry_stream.Read(&cur_id, sizeof(uuid_t));
          entry_stream.Read(&cur_seq, sizeof(TSequenceNumber));
          new (durable_sorter_alloc + pos) TDurableSorter<size_t>::TMergeElement(&sorter, cur_id, cur_seq, pos);
          --entries_left_vec.back();
        }
        ++pos;
      }

      size_t cur_deadline_count;
      TSerializedSize cur_serialized_size;
      Base::TOpt<Base::TUuid> last_id;
      while (!sorter.IsEmpty()) {
        TSequenceNumber pop_num;
        size_t pos = sorter.Pop(pop_num, cur_id);
        TInStream &entry_stream = *in_stream_vec[pos];
        entry_stream.Read(cur_deadline_count);
        entry_stream.Read(cur_serialized_size);
        entry_stream.Skip(cur_serialized_size);
        if (!last_id || uuid_compare(cur_id, last_id->GetRaw()) != 0) {
          last_id = TUuid(cur_id);
          if (cur_deadline_count >= latest_deadline_count) {
            ++NumDurable;
            total_durable_serialized_space += cur_serialized_size;  // size of serialized durable
          }
        }
        if (entries_left_vec[pos] > 0) {
          entry_stream.Read(&cur_id, sizeof(uuid_t));
          entry_stream.Read(&cur_seq, sizeof(TSequenceNumber));
          new (durable_sorter_alloc + pos) TDurableSorter<size_t>::TMergeElement(&sorter, cur_id, cur_seq, pos);
          --entries_left_vec[pos];
        }
      }
      free(durable_sorter_alloc);
    } catch (...) {
      free(durable_sorter_alloc);
      throw;
    }

    num_hash_fields = Util::SuggestHashSize(NumDurable);

    total_bytes += TSortedByIdFile::NumMetaFields * sizeof(size_t);
    total_bytes += NumDurable * TSortedByIdFile::DurableEntrySize;
    total_bytes += total_durable_serialized_space;
    total_bytes += num_hash_fields * TSortedByIdFile::HashEntrySize;

    /* calculate the number of blocks. */
    num_blocks = ceil(static_cast<double>(total_bytes) / Disk::Util::LogicalBlockSize);
    size_t num_bytes_for_block_data = num_blocks * sizeof(size_t);
    num_blocks = ceil(static_cast<double>(total_bytes + num_bytes_for_block_data) / Disk::Util::LogicalBlockSize);
    num_bytes_for_block_data = num_blocks * sizeof(size_t);
    num_blocks = ceil(static_cast<double>(total_bytes + num_bytes_for_block_data) / Disk::Util::LogicalBlockSize);
    num_bytes_for_block_data = num_blocks * sizeof(size_t);
    num_blocks = ceil(static_cast<double>(total_bytes + num_bytes_for_block_data) / Disk::Util::LogicalBlockSize);

    byte_offset_of_hash_index = (TSortedByIdFile::NumMetaFields + num_blocks) * sizeof(size_t);
    byte_offset_of_hash_index += NumDurable * TSortedByIdFile::DurableEntrySize;
    byte_offset_of_hash_index += total_durable_serialized_space;
  }

  /* reserve the blocks. */
  Engine->AppendReserveBlocks(StorageSpeed, num_blocks, BlockVec);
  #ifndef NDEBUG
  std::unordered_set<size_t> written_block_set;
  #endif

  unordered_map<size_t, shared_ptr<const TBufBlock>> block_collision_map;
  /* fill in the block collision map */
  /* collision between durable index and string heap */
  auto ret = block_collision_map.insert(make_pair(byte_offset_of_hash_index / Disk::Util::LogicalBlockSize, nullptr));
  if (ret.second) { // fresh insert
    ret.first->second = std::shared_ptr<const TBufBlock>(new TBufBlock());
  }
  /* write out the file */ {
    TDataOutStream index_stream(HERE,
                                Source::DurableMergeFileData,
                                Engine->GetVolMan(),
                                0UL,
                                BlockVec,
                                block_collision_map,
                                completion_trigger,
                                priority,
                                true,
                                #ifndef NDEBUG
                                written_block_set,
                                #endif
                                [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});

    index_stream << NumDurable;  // NumEntries
    index_stream << num_blocks;  // NumBlocks
    index_stream << byte_offset_of_hash_index;  // HashIndexOffset
    index_stream << num_hash_fields;  // HashFieldSize

    /* write out the block ids */
    for (auto iter : BlockVec) {
      index_stream << iter;  // block_id
    }

    /* write out num_entries * (DurableEntry + serialized string)
       1. durable id
       2. seq_num
       3. deadline count
       4. size of serialized string.
       5. serialized string
    */

    TDurableSorter<size_t> sorter;
    TDurableSorter<size_t>::TMergeElement *durable_sorter_alloc = 0;
    std::vector<std::unique_ptr<TInStream>> index_in_stream_vec;
    std::vector<size_t> entries_left_vec;
    size_t pos = 0U;
    void *temp_space = nullptr;
    try {
      durable_sorter_alloc = reinterpret_cast<TDurableSorter<size_t>::TMergeElement *>(malloc(sizeof(TDurableSorter<size_t>::TMergeElement) * in_file_vec.size()));
      uuid_t cur_id;
      TSequenceNumber cur_seq;
      for (const std::unique_ptr<TSortedInFile> &iter : in_file_vec) {
        size_t num_entries_in_file = iter->GetNumEntries();
        size_t byte_offset_of_durable_index = iter->GetStartOfDurableByIdIndex();

        entries_left_vec.push_back(num_entries_in_file);
        index_in_stream_vec.push_back(std::unique_ptr<TInStream>(new TInStream(HERE, Source::DurableMergeFileScan, priority, iter.get(), Engine->GetPageCache(), byte_offset_of_durable_index)));

        if (entries_left_vec.back() > 0) {
          TInStream &index_in_stream = *index_in_stream_vec.back();
          index_in_stream.Read(&cur_id, sizeof(uuid_t));
          index_in_stream.Read(&cur_seq, sizeof(TSequenceNumber));
          new (durable_sorter_alloc + pos) TDurableSorter<size_t>::TMergeElement(&sorter, cur_id, cur_seq, pos);
          --entries_left_vec.back();
        }
        ++pos;
      }

      size_t cur_deadline_count;
      TSerializedSize cur_serialized_size;
      Base::TOpt<Base::TUuid> last_id;
      size_t key_offset = 0UL;
      while (!sorter.IsEmpty()) {
        TSequenceNumber pop_num;
        size_t pos = sorter.Pop(pop_num, cur_id);
        TInStream &index_in_stream = *index_in_stream_vec[pos];
        index_in_stream.Read(cur_deadline_count);
        index_in_stream.Read(cur_serialized_size);
        if (!last_id || uuid_compare(cur_id, last_id->GetRaw()) != 0) {
          last_id = TUuid(cur_id);
          if (cur_deadline_count >= latest_deadline_count) {
            const size_t id_hash = *reinterpret_cast<size_t *>(const_cast<unsigned char *>(cur_id));
            index_stream.Write(&cur_id, sizeof(uuid_t));  // durable_id
            index_stream << pop_num;  // seq_num
            index_stream << cur_deadline_count;  // deadline_count
            index_stream << cur_serialized_size;  // size of serialized string
            if ((temp_space = realloc(temp_space, cur_serialized_size)) == 0) {
              free(temp_space);
              temp_space = nullptr;
              syslog(LOG_EMERG, "bad alloc in DurableManager realloc [%d]", cur_serialized_size);
              throw std::bad_alloc();
            }
            index_in_stream.Read(temp_space, cur_serialized_size);
            index_stream.Write(temp_space, cur_serialized_size);
            hash_sorter.Emplace(cur_id, id_hash % num_hash_fields, key_offset);
            key_offset += TSortedByIdFile::DurableEntrySize + cur_serialized_size;
            if (notify) {
              (*notify)(pop_num, cur_id, Survived);
            }
          } else {
            index_in_stream.Skip(cur_serialized_size);
            if (notify) {
              (*notify)(pop_num, cur_id, Expired);
            }
          }
        } else {
          index_in_stream.Skip(cur_serialized_size);
          if (notify) {
            (*notify)(pop_num, cur_id, WasSuperceded);
          }
        }
        if (entries_left_vec[pos] > 0) {
          index_in_stream.Read(&cur_id, sizeof(uuid_t));
          index_in_stream.Read(&cur_seq, sizeof(TSequenceNumber));
          new (durable_sorter_alloc + pos) TDurableSorter<size_t>::TMergeElement(&sorter, cur_id, cur_seq, pos);
          --entries_left_vec[pos];
        }
      }
      free(temp_space);
      free(durable_sorter_alloc);
    } catch (...) {
      free(temp_space);
      free(durable_sorter_alloc);
      throw;
    }
  }
  /* write out the hash index */ {
    THashSorter::TCursor hash_csr(&hash_sorter, 16UL);

    /* do the first pass over the hash map */ {
      TDataOutStream stream(HERE,
                            Source::DurableMergeFileHash,
                            Engine->GetVolMan(),
                            byte_offset_of_hash_index,
                            BlockVec,
                            block_collision_map,
                            completion_trigger,
                            priority,
                            true,
                            #ifndef NDEBUG
                            written_block_set,
                            #endif
                            [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});

      for (size_t i = 0; i < num_hash_fields; ++i) {
        if (hash_csr && (*hash_csr).Hash <= i) {
          const THashObj &obj = *hash_csr;
          stream.Write(&obj.Id, sizeof(uuid_t));
          stream << obj.Offset;
          ++hash_csr;
        } else {
          stream.Write(&TSortedByIdFile::NullId.GetRaw(), sizeof(uuid_t));
          stream << 0UL;
        }
      }
      FileSize = stream.GetOffset();
    }  // do the first pass over the hash map
    if (hash_csr) {
      /* flush the collision blocks surrounding the hash map */ {
        Engine->GetVolMan()->WriteBlock(HERE,
                                        Disk::Util::PageCheckedBlock,
                                        Source::DurableMergeFileHash,
                                        block_collision_map[byte_offset_of_hash_index / Disk::Util::LogicalBlockSize]->GetData(),
                                        BlockVec[byte_offset_of_hash_index / Disk::Util::LogicalBlockSize],
                                        priority,
                                        Disk::Util::CacheAll,
                                        completion_trigger);
      }
      /* wait for the first pass to finish flushing before doing the wrap around pass */ {
        completion_trigger.Wait();
      }

      TDataOutStream out_stream(HERE,
                                Source::DurableMergeFileHash,
                                Engine->GetVolMan(),
                                byte_offset_of_hash_index,
                                BlockVec,
                                block_collision_map,
                                completion_trigger,
                                priority,
                                true,
                                #ifndef NDEBUG
                                written_block_set,
                                #endif
                                [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
      TMyFile my_file(this);
      TInStream in_stream(HERE, Source::DurableMergeFileHash, priority, &my_file, Engine->GetPageCache(), byte_offset_of_hash_index);
      uuid_t cur_id;
      size_t key_ptr = 0;
      for (size_t i = 0; i < num_hash_fields; ++i) {
        in_stream.Read(&cur_id, sizeof(uuid_t));
        in_stream.Read(key_ptr);
        if (memcmp(&cur_id, &TSortedByIdFile::NullId.GetRaw(), sizeof(uuid_t)) == 0) {
          assert(hash_csr);

          const THashObj &obj = *hash_csr;
          out_stream.Write(&obj.Id, sizeof(uuid_t));
          out_stream << obj.Offset;
          ++hash_csr;
        } else {
          out_stream.Write(&cur_id, sizeof(uuid_t));
          out_stream << key_ptr;
        }
        if (!hash_csr) {
          break;
        }
      }
      size_t left = std::min(Disk::Util::LogicalBlockSize - in_stream.GetOffset() % Disk::Util::LogicalBlockSize, FileSize - in_stream.GetOffset());
      char buf[left];
      in_stream.Read(buf, left);
      out_stream.Write(buf, left);
    }
    assert(!hash_csr);
  }
  /* flush collision blocks */ {
    for (auto iter : block_collision_map) {
      Engine->GetVolMan()->WriteBlock(HERE,
                                      Disk::Util::PageCheckedBlock,
                                      Source::DurableMergeFileOther,
                                      iter.second->GetData(),
                                      BlockVec[iter.first],
                                      priority,
                                      Disk::Util::CacheAll,
                                      completion_trigger);
    }
    completion_trigger.Wait();
  }
  /* wait for file entry to flush */ {
    Engine->InsertFile(DurableByIdFileId, TFileObj::TKind::DurableFile, gen_id, BlockVec.Front(), 0UL, BlockVec.Size() * Disk::Util::LogicalBlockSize, NumDurable, 0UL, 0UL, completion_trigger);
    completion_trigger.Wait();
  }
}