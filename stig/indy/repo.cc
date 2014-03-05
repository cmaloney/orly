/* <stig/indy/repo.cc>

   Implements <stig/indy/repo.h>.

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

#include <stig/indy/repo.h>

#include <fstream>
#include <limits>

#include <base/debug_log.h>
#include <stig/indy/disk/util/hash_util.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::Atom;
using namespace Stig::Indy;
using namespace Stig::Indy::Disk;
using namespace Stig::Indy::Util;

class TReader
    : public TReadFile<Disk::Util::LogicalPageSize, Disk::Util::LogicalBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::CheckedPage> {
  NO_COPY_SEMANTICS(TReader);
  public:

  /* TODO */
  typedef TStream<Disk::Util::LogicalPageSize, Disk::Util::LogicalBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::CheckedPage, 0UL> TInStream;
  typedef Stig::Indy::Disk::TReadFile<Disk::Util::LogicalPageSize, Disk::Util::LogicalBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::CheckedPage> TMyReadFile;

  TReader(Disk::Util::TEngine *engine, const Base::TUuid &file_id, DiskPriority priority, size_t gen_id)
      : TMyReadFile(HERE, Source::FileRemoval, engine, file_id, priority, gen_id) {}

  virtual ~TReader() {}

  using TReadFile::GetStartingBlockOffset;
  using TReadFile::GetNumBlocks;
  using TReadFile::GetNumMetaBlocks;
  using TReadFile::GetNumSequentialBlockPairings;
  using TReadFile::GetNumUpdates;

  using TReadFile::FindInHash;
};

void TRepo::AddImportLayer(TMemoryLayer *mem_layer, Base::TEventSemaphore &sem, Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed) {
  assert(this);
  //assert(!ParentRepo);
  TDataLayer *new_layer = nullptr;
  if (IsSafeRepo()) {
    TSequenceNumber lower_seq_bound;
    /* acquire Data lock */ {
      std::lock_guard<std::mutex> lock(DataLock);
      lower_seq_bound = ReleasedUpTo;
    }  // release Data lock
    size_t num_keys = 0UL;
    TSequenceNumber saved_low_seq = 0UL, saved_high_seq = 0UL;
    size_t gen_id = WriteFile(mem_layer, storage_speed, saved_low_seq, saved_high_seq, num_keys, lower_seq_bound);
    /* acquire DataLayer lock */ {
      std::lock_guard<std::mutex> lock(DataLock);
      new_layer = new TDiskLayer(Manager, this, gen_id, num_keys, saved_low_seq, saved_high_seq);
      delete mem_layer;
    }  // release DataLayer lock
  } else {
    new_layer = mem_layer;
  }
  size_t total_layers = 0UL;
  sem.Pop();
  /* acquire Mapping lock */ {
    std::lock_guard<std::mutex> lock(MappingLock);
    TMapping *cur_mapping = MappingCollection.TryGetLastMember();
    cur_mapping->Incr();
    try {
      TMapping *new_mapping = new TMapping(this);
      assert(cur_mapping);
      for (TMapping::TEntryCollection::TCursor cur_csr(cur_mapping->GetEntryCollection()); cur_csr; ++cur_csr) {
        ++total_layers;
        new TMapping::TEntry(new_mapping, cur_csr->GetLayer());
      }
      new TMapping::TEntry(new_mapping, new_layer);
      ++total_layers;
      cur_mapping->Decr();
    } catch (...) {
      cur_mapping->Decr();
      throw;
    }
  }
  if (ParentRepo && !InTetris) {
    Manager->GetTetrisManager()->Join((*ParentRepo)->GetId(), GetId());
    InTetris = true;
  }
  if (total_layers >= 3) {
    EnqueueMergeDisk();
  }
}

void TRepo::AddFileToRepo(size_t gen_id, TSequenceNumber saved_low_seq, TSequenceNumber saved_high_seq, size_t num_keys) {
  assert(this);
  TDiskLayer *new_disk = nullptr;
  /* acquire DataLayer lock */ {
    std::lock_guard<std::mutex> lock(DataLock);
    new_disk = new TDiskLayer(Manager, this, gen_id, num_keys, saved_low_seq, saved_high_seq);
  }  // release DataLayer lock
  size_t total_layers = 0UL;
  /* acquire Mapping lock */ {
    std::lock_guard<std::mutex> lock(MappingLock);
    TMapping *cur_mapping = MappingCollection.TryGetLastMember();
    cur_mapping->Incr();
    try {
      TMapping *new_mapping = new TMapping(this);
      assert(cur_mapping);
      for (TMapping::TEntryCollection::TCursor cur_csr(cur_mapping->GetEntryCollection()); cur_csr; ++cur_csr) {
        ++total_layers;
        new TMapping::TEntry(new_mapping, cur_csr->GetLayer());
      }
      new TMapping::TEntry(new_mapping, new_disk);
      ++total_layers;
      cur_mapping->Decr();
    } catch (...) {
      cur_mapping->Decr();
      throw;
    }
  }
  if (total_layers >= 3) {
    EnqueueMergeDisk();
  }
}

void TRepo::ReleaseUpdate(TSequenceNumber seq_num, bool ensure_or_discard) {
  assert(this);
  assert(seq_num < NextUpdate);
  assert(seq_num == ReleasedUpTo + 1L || ensure_or_discard);
  if (!ensure_or_discard || seq_num == ReleasedUpTo + 1L) {
    ReleasedUpTo = seq_num;
  }
  /* Acquire Data lock */ {
    std::lock_guard<std::mutex> lock(DataLock);
    if ((HighestSeqNum && seq_num == *HighestSeqNum) || (!HighestSeqNum && seq_num == NextUpdate - 1)) {
      if (!IsSafeRepo()) {
        RemoveFromDirty();
      }
    }
  }
}

unique_ptr<Indy::TPresentWalker> TRepo::NewPresentWalker(const std::unique_ptr<TView> &view,
                                                         const TIndexKey &from,
                                                         const TIndexKey &to,
                                                         bool ignore_tombstone) {
  assert(this);
  assert(view);
  return unique_ptr<TPresentWalker>(new TPresentWalker(view, from, to, ignore_tombstone));
}

unique_ptr<Indy::TPresentWalker> TRepo::NewPresentWalker(const std::unique_ptr<TView> &view,
                                                         const TIndexKey &key,
                                                         bool ignore_tombstone) {
  assert(this);
  assert(view);
  return unique_ptr<TPresentWalker>(new TPresentWalker(view, key, ignore_tombstone));
}

unique_ptr<Indy::TUpdateWalker> TRepo::NewUpdateWalker(const std::unique_ptr<TView> &view,
                                                       TSequenceNumber from,
                                                       const Base::TOpt<TSequenceNumber> &to) {
  assert(this);
  assert(view);
  return unique_ptr<TUpdateWalker>(new TUpdateWalker(view, from, to));
}

unique_ptr<Indy::TUpdateWalker> TRepo::NewUpdateWalker(const std::unique_ptr<TView> &view,
                                                       TSequenceNumber from) {
  assert(this);
  return NewUpdateWalker(view, from, Base::TOpt<TSequenceNumber>());
}

TRepo::TView::TView(const L0::TManager::TPtr<TRepo> &repo)
    : TView::TView(repo.Get()) {}

TRepo::TView::TView(TRepo *repo)
    : Repo(repo) {
  assert(Repo);
  /* acquire Data lock */ {
    std::lock_guard<std::mutex> lock(Repo->DataLock);
    Mapping = Repo->AcquireCurrentMapping();
    CurrentMemoryLayer = Repo->CurMemoryLayer;
    assert(CurrentMemoryLayer);
    CurrentMemoryLayer->Incr();
    LowerBound = Repo->LowestSeqNum;
    UpperBound = Repo->HighestSeqNum;
    NextId = Repo->NextUpdate;
  }  // release DataLayer lock
}

TRepo::TView::~TView() {
  assert(this);
  assert(CurrentMemoryLayer);
  assert(Repo);
  std::lock_guard<std::mutex> lock(Repo->MappingLock);
  CurrentMemoryLayer->Decr();
  Mapping->Decr();
}

const TMemoryLayer *TRepo::TView::GetCurMem() const {
  assert(this);
  return CurrentMemoryLayer;
}

const TRepo::TMapping *TRepo::TView::GetMapping() const {
  assert(this);
  return Mapping;
}

const Base::TOpt<TSequenceNumber> &TRepo::TView::GetLower() const {
  assert(this);
  return LowerBound;
}

const Base::TOpt<TSequenceNumber> &TRepo::TView::GetUpper() const {
  assert(this);
  return UpperBound;
}

TRepo::TRepo(L0::TManager *manager,
             const TUuid &repo_id,
             const TTtl &ttl,
             const Base::TOpt<L0::TManager::TPtr<L0::TManager::TRepo>> &parent_repo)
    : L0::TManager::TRepo(manager, repo_id, ttl, Normal),
      CurMemoryLayer(new TMemoryLayer(manager)),
      ParentRepo(parent_repo),
      NextUpdate(1U),
      ReleasedUpTo(0U),
      InTetris(false) {
  try {
    /* acquire Mapping lock */ {
      std::lock_guard<std::mutex> lock(MappingLock);
      new TMapping(this);
    }  // release Mapping lock
  } catch (...) {
    delete CurMemoryLayer;
    throw;
  }
}

TRepo::TRepo(L0::TManager *manager,
             const Base::TUuid &repo_id,
             const TTtl &ttl,
             const Base::TOpt<L0::TManager::TPtr<L0::TManager::TRepo>> &parent_repo,
             const Base::TOpt<TSequenceNumber> &lowest,
             const Base::TOpt<TSequenceNumber> &highest,
             TSequenceNumber next_update,
             TStatus status)
    : L0::TManager::TRepo(manager, repo_id, ttl, status),
      CurMemoryLayer(new TMemoryLayer(manager)),
      ParentRepo(parent_repo),
      LowestSeqNum(lowest),
      HighestSeqNum(highest),
      NextUpdate(next_update),
      ReleasedUpTo(lowest ? *lowest : 0UL),
      InTetris(false) {
  try {
    /* acquire Mapping lock */ {
      std::lock_guard<std::mutex> lock(MappingLock);
      new TMapping(this);
    }  // release Mapping lock
  } catch (...) {
    delete CurMemoryLayer;
    throw;
  }
}

TRepo::~TRepo() {
  assert(this);
  assert(CurMemoryLayer->IsEmpty());
  assert(!InTetris);
  if (ParentRepo && InTetris) {
    Manager->GetTetrisManager()->Part((*ParentRepo)->GetId(), GetId());
    InTetris = false;
  }
  delete CurMemoryLayer;
}

Base::TOpt<TSequenceNumber> TRepo::AppendUpdate(TUpdate *update, TSequenceNumber &next_update) NO_THROW {
  assert(this);
  Base::TOpt<TSequenceNumber> new_seq;
  bool add_to_tetris = false;
  /* acquire Data lock */ {
    std::lock_guard<std::mutex> lock(DataLock);
    HighestSeqNum = NextUpdate;
    ++NextUpdate;
    next_update = NextUpdate;
    if (!LowestSeqNum) {
      if (ParentRepo) {
        add_to_tetris = true;
      }
      LowestSeqNum = *HighestSeqNum;
    }
    new_seq = HighestSeqNum;

    update->SetSequenceNumber(*new_seq);
    assert(CurMemoryLayer);
    bool was_empty = CurMemoryLayer->IsEmpty();
    CurMemoryLayer->Insert(update);
    if (was_empty) {
      EnqueueMergeMem();
    }
    MakeDirty();
    if (add_to_tetris && Status == Normal) {
      assert(!InTetris);
      Manager->GetTetrisManager()->Join((*ParentRepo)->GetId(), GetId());
      InTetris = true;
    }
  }  // release Data lock
  return new_seq;
}

Base::TOpt<TSequenceNumber> TRepo::PopLowest(TSequenceNumber &next_update) NO_THROW {
  assert(this);
  assert(Status == Normal);
  assert(LowestSeqNum);
  assert(HighestSeqNum);
  Base::TOpt<TSequenceNumber> popped_seq;
  /* acquire Data lock */ {
    std::lock_guard<std::mutex> lock(DataLock);
    popped_seq = LowestSeqNum;
    next_update = NextUpdate;
    if (*LowestSeqNum < *HighestSeqNum) {
      ++(*LowestSeqNum);
    } else {
      LowestSeqNum.Reset();
      HighestSeqNum.Reset();
      if (ParentRepo) {
        assert(InTetris);
        Manager->GetTetrisManager()->Part((*ParentRepo)->GetId(), GetId());
        InTetris = false;
      }
    }
  }  // release Data lock
  return popped_seq;
}

std::shared_ptr<TUpdate> TRepo::GetLowestUpdate() {
  assert(this);
  assert(LowestSeqNum);
  assert(HighestSeqNum);
  auto view = unique_ptr<TRepo::TView>(new TRepo::TView(this));
  auto walker_ptr = NewUpdateWalker(view, *LowestSeqNum, *LowestSeqNum);
  auto &walker = *walker_ptr;
  assert(walker);
  if (walker) {
    const TUpdateWalker::TItem &item = *walker;
    TUpdate::TOpByKey op_by_key;
    for (const auto &iter : item.EntryVec) {
      op_by_key.insert(make_pair(iter.first, TKey(iter.second, item.MainArena)));
    }
    return TUpdate::NewUpdate(op_by_key, TKey(item.Metadata, item.MainArena), TKey(item.Id, item.MainArena));
  }
  return nullptr;
}

Base::TOpt<TSequenceNumber> TRepo::ChangeStatus(TStatus status, TSequenceNumber &next_update) NO_THROW {
  assert(this);
  switch (Status) {
    case Normal : {
      assert(status != Normal);  // you probably meant to unpause something. if it's not already paused you may have forgotten your pause!
      Status = status;
      break;
    }
    case Paused : {
      assert(status == Normal);  // you probably didn't meant to pause an already paused repo
      Status = status;
      break;
    }
    case Failed : {
      assert(status == Failed);
      break;
    }
  }
  /* Tetris */
  switch (Status) {
    case Normal : {
      std::lock_guard<std::mutex> lock(DataLock);
      if (!LowestSeqNum) {
        if (ParentRepo && !InTetris) {
          Manager->GetTetrisManager()->Join((*ParentRepo)->GetId(), GetId());
          InTetris = true;
        }
      }
      break;
    }
    case Paused : {
      if (ParentRepo && InTetris) {
        Manager->GetTetrisManager()->Part((*ParentRepo)->GetId(), GetId());
        InTetris = false;
      }
      break;
    }
    case Failed : {
      if (ParentRepo && InTetris) {
        Manager->GetTetrisManager()->Part((*ParentRepo)->GetId(), GetId());
        InTetris = false;
      }
      break;
    }
  }
  next_update = NextUpdate;
  return LowestSeqNum;
}

class TUpdateSortComparator {
  public:

  /* TODO */
  TUpdateSortComparator() {}

  /* TODO */
  bool operator()(const TUpdate *lhs, const TUpdate *rhs) const {
    assert(lhs);
    assert(rhs);
    return lhs->GetSequenceNumber() < rhs->GetSequenceNumber();
  }

};  // TUpdateSortComparator

void TRepo::StepMergeMem() {
  assert(this);
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed = Disk::Util::TVolume::TDesc::TStorageSpeed::Fast;
  try {
    /*** If the current memory layer is not empty, add it to the mapping layer and create a new current memory layer ***/
    /* acquire DataLayer lock */ {
      std::lock_guard<std::mutex> lock(DataLock);
      assert(CurMemoryLayer);
      if (!CurMemoryLayer->IsEmpty()) {
        AddMapping(CurMemoryLayer);
        try {
          CurMemoryLayer = new TMemoryLayer(Manager);
        } catch (const std::exception &ex) {
          CurMemoryLayer = nullptr;
          syslog(LOG_EMERG, "Error allocating new CurMemoryLayer for Repo [%s]", ex.what());
          throw;
        }
        //EnqueueMergeMem();
      }
    }  // release DataLayer lock

    /*** find the memory layers that we can merge and then flush to disk if we're a safe repo ***/

    /* acquire MemMerge lock */ {
      std::lock_guard<std::mutex> lock(MemMergeLock);

      /* grab the current mapping */ {
        TMapping *mapping = AcquireCurrentMapping();
        TMemoryLayer *new_mem = nullptr;
        TDiskLayer *new_disk = nullptr;
        TSequenceNumber lower_seq_bound;
        /* acquire DataLayer lock */ {
          std::lock_guard<std::mutex> lock(DataLock);
          lower_seq_bound = ReleasedUpTo;
          assert(lower_seq_bound < NextUpdate);
          new_mem = new TMemoryLayer(Manager);
        }  // release DataLayer lock
        assert(new_mem);
        try {
          std::vector<TMemoryLayer *> mem_to_merge_vec;
          for (TMapping::TEntryCollection::TCursor csr(mapping->GetEntryCollection(), InvCon::TOrient::Rev); csr; ++csr) {
            TDataLayer *layer = csr->GetLayer();
            if (layer->GetKind() != TDataLayer::Mem || layer->GetMarkedTaken()) {
              break;
            }
            assert(!layer->GetMarkedTaken());
            layer->MarkTaken();
            mem_to_merge_vec.push_back(reinterpret_cast<TMemoryLayer *>(layer));
          }
          //syslog(LOG_INFO, "Layout Disk=[%ld]\tMem=[%ld]\tToMerge=[%ld]\t\t\tTaken=[%ld]", num_disk, total_count - num_disk, mem_to_merge_vec.size(), taken);
          if (mem_to_merge_vec.size() == 1) {
              //syslog(LOG_INFO, "mem_to_merge_vec.size() == 1");
              if (IsSafeRepo() && !reinterpret_cast<TMemoryLayer *>(mem_to_merge_vec[0])->IsEmpty()) {
                size_t num_keys = 0U;
                TSequenceNumber saved_low_seq = 0UL, saved_high_seq = 0UL;
                size_t gen_id = WriteFile(reinterpret_cast<TMemoryLayer *>(mem_to_merge_vec[0]), storage_speed, saved_low_seq, saved_high_seq, num_keys, lower_seq_bound);
                {
                  std::lock_guard<std::mutex> lock(Manager->MergeMemCPULock);
                  Manager->MergeMemAverageKeysCalc.Push(num_keys);
                }
                new_disk = new TDiskLayer(Manager, this, gen_id, num_keys, saved_low_seq, saved_high_seq);
                delete new_mem;
                new_mem = nullptr;
              } else {
                if (mem_to_merge_vec[0]->GetSize() && mem_to_merge_vec[0]->GetUpdateCollection()->TryGetLastMember()->GetSequenceNumber() <= lower_seq_bound) {
                  DEBUG_LOG("Skipping because of lower_seq_bound [%ld]", lower_seq_bound);
                  delete new_mem;
                  new_mem = nullptr;
                } else {
                  mem_to_merge_vec[0]->UnmarkTaken();
                  delete new_mem;
                  ReleaseMapping(mapping);
                  return;
                }
              }
            } else if(mem_to_merge_vec.size() >= 2) {
              //syslog(LOG_INFO, "mem_to_merge_vec.size() >= 2");
              TUpdateSortComparator comparator;
              TCopyMergeSorter<TUpdate *, size_t, TUpdateSortComparator> update_sorter(comparator);
              TCopyMergeSorter<TUpdate *, size_t, TUpdateSortComparator>::TMergeElement *update_sorter_alloc = 0;
              TKeySorter<size_t> entry_sorter;
              TKeySorter<size_t>::TMergeElement *entry_sorter_alloc = 0;
              /* sorter alloca scope */ {
                std::unordered_map<const TUpdate *, TUpdate *> update_remap;
                update_sorter_alloc = reinterpret_cast<TCopyMergeSorter<TUpdate *, size_t, TUpdateSortComparator>::TMergeElement *>(alloca(sizeof(TCopyMergeSorter<TUpdate *, size_t, TUpdateSortComparator>::TMergeElement) * mem_to_merge_vec.size()));
                entry_sorter_alloc = reinterpret_cast<TKeySorter<size_t>::TMergeElement *>(alloca(sizeof(TKeySorter<size_t>::TMergeElement) * mem_to_merge_vec.size()));
                std::vector<TMemoryLayer::TUpdateCollection::TCursor> update_csr_vec;
                std::vector<TMemoryLayer::TEntryCollection::TCursor> entry_csr_vec;
                size_t update_pos = 0UL;
                size_t entry_pos = 0UL;
                for (auto layer : mem_to_merge_vec) {
                  assert(layer->GetMarkedTaken());
                  TMemoryLayer::TUpdateCollection::TCursor update_csr(reinterpret_cast<TMemoryLayer *>(layer)->GetUpdateCollection());
                  TMemoryLayer::TEntryCollection::TCursor entry_csr(reinterpret_cast<TMemoryLayer *>(layer)->GetEntryCollection());
                  if (update_csr) {
                    update_csr_vec.push_back(update_csr);
                    new (update_sorter_alloc + update_pos) TCopyMergeSorter<TUpdate *, size_t, TUpdateSortComparator>::TMergeElement(&update_sorter, &*update_csr, update_pos);
                    ++update_pos;
                  }
                  if (entry_csr) {
                    entry_csr_vec.push_back(entry_csr);
                    new (entry_sorter_alloc + entry_pos) TKeySorter<size_t>::TMergeElement(&entry_sorter, entry_csr->GetKey(), entry_csr->GetSequenceNumber(), entry_pos);
                    ++entry_pos;
                  }
                }
                while (!update_sorter.IsEmpty()) {
                  size_t pos;
                  TUpdate *cur_update = update_sorter.Pop(pos);
                  auto &csr = update_csr_vec[pos];
                  assert(cur_update == &*csr);
                  if (csr->GetSequenceNumber() > lower_seq_bound) {
                    TUpdate *new_update = TUpdate::ShallowCopy(cur_update, state_alloc);
                    auto ret = update_remap.insert(make_pair(cur_update, new_update));
                    assert(ret.second);
                    new_mem->ImporterAppendUpdate(new_update);
                  }
                  ++csr;
                  if (csr) {
                    new (update_sorter_alloc + pos) TCopyMergeSorter<TUpdate *, size_t, TUpdateSortComparator>::TMergeElement(&update_sorter, &*csr, pos);
                  }
                }
                while(!entry_sorter.IsEmpty()) {
                  size_t pos = entry_sorter.Pop();
                  auto &csr = entry_csr_vec[pos];
                  const TUpdate::TEntry &cur_entry = *csr;
                  if (csr->GetSequenceNumber() > lower_seq_bound) {
                    const TUpdate *cur_update = cur_entry.GetUpdate();
                    const auto iter = update_remap.find(cur_update);
                    assert(iter != update_remap.end());
                    TUpdate *new_update = iter->second;
                    TUpdate::TEntry *new_entry = new_update->AddEntry(cur_entry.GetIndexKey(), TKey(cur_entry.GetOp(), &cur_entry.GetSuprena()));
                    new_mem->ImporterAppendEntry(new_entry);
                  }
                  ++csr;
                  if (csr) {
                    new (entry_sorter_alloc + pos) TKeySorter<size_t>::TMergeElement(&entry_sorter, csr->GetKey(), csr->GetSequenceNumber(), pos);
                  }
                }
              }  // end sorter alloca scope
              if (IsSafeRepo() && !new_mem->IsEmpty()) {
                size_t num_keys = 0U;
                TSequenceNumber saved_low_seq = 0UL, saved_high_seq = 0UL;
                size_t gen_id = WriteFile(new_mem, storage_speed, saved_low_seq, saved_high_seq, num_keys, lower_seq_bound);
                {
                  std::lock_guard<std::mutex> lock(Manager->MergeMemCPULock);
                  Manager->MergeMemAverageKeysCalc.Push(num_keys);
                }
                new_disk = new TDiskLayer(Manager, this, gen_id, num_keys, saved_low_seq, saved_high_seq);
                delete new_mem;
                new_mem = nullptr;
              }
            } else {
              //syslog(LOG_INFO, "mem_to_merge_vec.size() == 0");
              delete new_mem;
              ReleaseMapping(mapping);
              return;
            }
          /* acquire Mapping lock */ {
            std::lock_guard<std::mutex> lock(MappingLock);
            TMapping *cur_mapping = MappingCollection.TryGetLastMember();
            cur_mapping->Incr();
            size_t total_disk_layers = 0U;
            try {
              TMapping *new_mapping = new TMapping(this);
              assert(cur_mapping);
              for (TMapping::TEntryCollection::TCursor cur_csr(cur_mapping->GetEntryCollection()); cur_csr; ++cur_csr) {
                assert(cur_csr->GetLayer() != new_mem);
                assert(cur_csr->GetLayer() != new_disk);
                bool found = false;
                for (auto layer : mem_to_merge_vec) {
                  if (layer == cur_csr->GetLayer()) {
                    found = true;
                    layer->MarkForDelete();
                    break;
                  }
                }
                if (!found) {
                  new TMapping::TEntry(new_mapping, cur_csr->GetLayer());
                  if (cur_csr->GetLayer()->GetKind() == TDataLayer::Disk && !cur_csr->GetLayer()->GetMarkedTaken()) {
                    ++total_disk_layers;
                  }
                }
              }
              if (new_mem) {
                assert(!IsSafeRepo());
                assert(new_mem->IsEmpty() == (new_mem->GetSize() == 0UL));
                if (new_mem->IsEmpty()) {
                  DEBUG_LOG("New mem is empty, resetting to nullptr");
                  delete new_mem;
                  new_mem = nullptr;
                }
              }
              if (new_disk) {
                new TMapping::TEntry(new_mapping, new_disk);
              } else if (new_mem) {
                new TMapping::TEntry(new_mapping, new_mem);
              } else {
                DEBUG_LOG("We cleaned away everything, not adding anything to the mapping");
                /* we've cleaned away everything */
              }
              total_disk_layers += new_disk ? 1UL : 0UL;
              cur_mapping->Decr();
            } catch (...) {
              cur_mapping->Decr();
              throw;
            }
            /* TODO: only enqueue if we are likely to be able to merge files of the same generation. */
            if (total_disk_layers >= 3) {
              EnqueueMergeDisk();
            }
          }
        } catch (const exception &ex) {
          syslog(LOG_ERR, "Caught exception in StepMergeMem [%s]", ex.what());
          ReleaseMapping(mapping);
          throw;
        }
        ReleaseMapping(mapping);
      }  // release the current mapping
      CheckRemoveDirty();
    }  // release MemMerge lock
  } catch (const std::exception &ex) {
    syslog(LOG_EMERG, "StepMergeMem caught error [%s]", ex.what());
    abort();
  }
}

size_t TRepo::AddMapping(TDataLayer *layer) {
  assert(this);
  size_t total = 0;
  /* acquire Mapping lock */ {
    std::lock_guard<std::mutex> lock(MappingLock);
    TMapping *last = MappingCollection.TryGetLastMember();
    last->Incr();
    assert(last);
    try {
      TMapping *new_mapping = new TMapping(this);
      for (TMapping::TEntryCollection::TCursor csr(last->GetEntryCollection()); csr; ++csr) {
        new TMapping::TEntry(new_mapping, csr->GetLayer());
        ++total;
      }
      TMapping::TEntry *new_entry = new TMapping::TEntry(new_mapping, layer);
      assert(new_mapping->GetEntryCollection()->TryGetLastMember() == new_entry);
      ++total;
      last->Decr();
      assert(MappingCollection.TryGetLastMember() == new_mapping);
    } catch (const std::exception &ex) {
      syslog(LOG_ERR, "Error in TRepo::AddMapping [%s]", ex.what());
      throw;
    }
  }  // release Mapping lock
  return total;
}

TRepo::TPresentWalker::TPresentWalker(const unique_ptr<TView> &view,
                                      const TIndexKey &from,
                                      const TIndexKey &to,
                                      bool ignore_tombstone)
    : Stig::Indy::TPresentWalker(Range),
      From(from),
      To(to),
      View(view),
      Lower(View->GetLower() ? *View->GetLower() : 0UL),
      Upper(View->GetUpper() ? *View->GetUpper() : 0UL),
      MinHeap(View->GetNumEntries() + 1UL),
      Valid(false),
      IgnoreTombstone(ignore_tombstone) {
  if (View->GetLower() && View->GetUpper()) {
    size_t pos = 0UL;
    for (TMapping::TEntryCollection::TCursor mapping_csr(View->GetMapping()->GetEntryCollection()); mapping_csr; ++mapping_csr, ++pos) {
      WalkerVec.emplace_back(mapping_csr->GetLayer()->NewPresentWalker(From, To));
      Indy::TPresentWalker &walker = *WalkerVec.back();
      if (walker) {
        MinHeap.Insert(*walker, pos);
      }
    }
    assert(View->GetCurMem());
    WalkerVec.emplace_back(View->GetCurMem()->NewPresentWalker(From, To));
    Indy::TPresentWalker &mem_walker = *WalkerVec.back();
    if (mem_walker) {
      MinHeap.Insert(*mem_walker, pos);
    }
    Valid = static_cast<bool>(MinHeap);
    Init();
  }
}

TRepo::TPresentWalker::TPresentWalker(const unique_ptr<TView> &view,
                                      const TIndexKey &key,
                                      bool ignore_tombstone)
    : Stig::Indy::TPresentWalker(Match),
      From(key),
      View(view),
      Lower(View->GetLower() ? *View->GetLower() : 0UL),
      Upper(View->GetUpper() ? *View->GetUpper() : 0UL),
      MinHeap(View->GetNumEntries() + 1UL),
      Valid(false),
      IgnoreTombstone(ignore_tombstone) {
  if (View->GetLower() && View->GetUpper()) {
    size_t pos = 0UL;
    Fiber::TSync sync(View->GetNumEntries());
    PrepVec.reserve(View->GetNumEntries());
    for (TMapping::TEntryCollection::TCursor mapping_csr(View->GetMapping()->GetEntryCollection()); mapping_csr; ++mapping_csr) {
      PrepVec.emplace_back(mapping_csr->GetLayer(), this, &sync);
      //WalkerVec.emplace_back(mapping_csr->GetLayer()->NewPresentWalker(From));
    }
    sync.Sync(true);
    for (auto &walker_ptr : WalkerVec) {
      Indy::TPresentWalker &walker = *walker_ptr;
      if (walker) {
        MinHeap.Insert(*walker, pos);
      }
      ++pos;
    }
    assert(View->GetCurMem());
    WalkerVec.emplace_back(View->GetCurMem()->NewPresentWalker(From));
    Indy::TPresentWalker &mem_walker = *WalkerVec.back();
    if (mem_walker) {
      MinHeap.Insert(*mem_walker, pos);
    }
    Valid = static_cast<bool>(MinHeap);
    Init();
  }
}

TRepo::TUpdateWalker::TUpdateWalker(const unique_ptr<TView> &view,
                                    TSequenceNumber from,
                                    const Base::TOpt<TSequenceNumber> &to)
    : From(from),
      To(to),
      View(view),
      SorterAlloc(nullptr),
      Valid(false) {
  assert(View);
  if (View->GetLower() && View->GetUpper()) {
    size_t num_walkers = 0U;
    assert(View->GetMapping());
    assert(View->GetMapping()->GetEntryCollection());
    for (TMapping::TEntryCollection::TCursor mapping_csr(View->GetMapping()->GetEntryCollection()); mapping_csr; ++mapping_csr) {
      assert(mapping_csr->GetLayer());
      WalkerMap[mapping_csr->GetLayer()] = make_pair(mapping_csr->GetLayer()->NewUpdateWalker(From), num_walkers);
      ++num_walkers;
    }
    assert(View->GetCurMem());
    WalkerMap[View->GetCurMem()] = make_pair(View->GetCurMem()->NewUpdateWalker(From), num_walkers);
    SorterAlloc = reinterpret_cast<TMergeSorter<TSequenceNumber, const TDataLayer *>::TMergeElement *>(malloc(sizeof(TMergeSorter<TSequenceNumber, const TDataLayer *>::TMergeElement) * WalkerMap.size()));
    if (!SorterAlloc) {
      syslog(LOG_EMERG, "bad alloc in TRepo::TUpdateWalker");
      throw std::bad_alloc();
    }
    try {
      for (auto &iter : WalkerMap) {
        Indy::TUpdateWalker &walker = *iter.second.first;
        if (walker) {
          assert(iter.second.second < WalkerMap.size());
          new (SorterAlloc + iter.second.second) TMergeSorter<TSequenceNumber, const TDataLayer *>::TMergeElement(&MergeSorter, (*walker).SequenceNumber, iter.first);
        }
      }
      Valid = !MergeSorter.IsEmpty();
      Refresh();
    } catch (...) {
      MergeSorter.Clear();
      assert(MergeSorter.IsEmpty());
      free(SorterAlloc);
      SorterAlloc = nullptr;
      throw;
    }
  }
}

TRepo::TUpdateWalker::~TUpdateWalker() {
  MergeSorter.Clear();
  assert(MergeSorter.IsEmpty());
  free(SorterAlloc);
}

TRepo::TUpdateWalker::operator bool() const {
  assert(this);
  return Valid;
}

const TUpdateWalker::TItem &TRepo::TUpdateWalker::operator*() const {
  assert(this);
  assert(Valid);
  return Item;
}

TRepo::TUpdateWalker &TRepo::TUpdateWalker::operator++() {
  assert(this);
  assert(Valid);
  Valid = !MergeSorter.IsEmpty();
  Refresh();
  return *this;
}

void TRepo::TUpdateWalker::Refresh() {
  assert(this);
  bool done = false;
  while (Valid && !done) {
    const TDataLayer *layer;
    /*TSequenceNumber seq_num = */MergeSorter.Pop(layer);
    pair<unique_ptr<Indy::TUpdateWalker>, size_t> &found = WalkerMap.find(layer)->second;
    Indy::TUpdateWalker &walker = *(found.first);
    if (To && (*walker).SequenceNumber > *To) {
      Valid = false;
      break;
    }
    if ((*walker).SequenceNumber >= *View->GetLower() && (*walker).SequenceNumber <= *View->GetUpper()) {
      Item = *walker;
      done = true;
    }
    ++walker;
    if (walker) {
      assert(found.second < WalkerMap.size());
      new (SorterAlloc + found.second) TMergeSorter<TSequenceNumber, const TDataLayer *>::TMergeElement(&MergeSorter, (*walker).SequenceNumber, layer);
    }
    if (!done) {
      Valid = !MergeSorter.IsEmpty();
    }
  }
}

TRepo::TMapping *TRepo::AcquireCurrentMapping() {
  assert(this);
  TMapping *mapping = 0;
  /* acquire Mapping lock */ {
    std::lock_guard<std::mutex> lock(MappingLock);
    mapping = MappingCollection.TryGetLastMember();
    mapping->Incr();
    assert(mapping);
    return mapping;
  }  // release Mapping lock
}

void TRepo::ReleaseMapping(TMapping *mapping) {
  assert(this);
  /* acquire Mapping lock */ {
    std::lock_guard<std::mutex> lock(MappingLock);
    mapping->Decr();
  }  // release Mapping lock
}

void TRepo::CheckRemoveDirty() {
  assert(this);
  TMapping *mapping = AcquireCurrentMapping();
  bool found_non_empty_mem = false;
  try {
    for (TMapping::TEntryCollection::TCursor csr(mapping->GetEntryCollection()); csr; ++csr) {
      if (csr->GetLayer()->GetKind() == TDataLayer::TKind::Mem && !dynamic_cast<TMemoryLayer *>(csr->GetLayer())->IsEmpty()) {
        found_non_empty_mem = true;
      }
    }
    if (!found_non_empty_mem) {
      RemoveFromDirty();
    } else {
      EnqueueMergeMem();
    }
  } catch (...) {
    ReleaseMapping(mapping);
    throw;
  }
  ReleaseMapping(mapping);
}

TFastRepo::TFastRepo(L0::TManager *manager,
                     const TUuid &repo_id,
                     const TTtl &ttl,
                     const Base::TOpt<L0::TManager::TPtr<L0::TManager::TRepo>> &parent_repo)
    : TRepo(manager,
            repo_id,
            ttl,
            parent_repo) {}

TFastRepo::TFastRepo(L0::TManager *manager,
                     const Base::TUuid &repo_id,
                     const TTtl &ttl,
                     const Base::TOpt<L0::TManager::TPtr<L0::TManager::TRepo>> &parent_repo,
                     const Base::TOpt<TSequenceNumber> &lowest,
                     const Base::TOpt<TSequenceNumber> &highest,
                     TSequenceNumber next_update,
                     TStatus status)
    : TRepo(manager,
            repo_id,
            ttl,
            parent_repo,
            lowest,
            highest,
            next_update,
            status) {}

TFastRepo::~TFastRepo() {
  PreDtor();
}

bool TFastRepo::IsSafeRepo() const {
  assert(this);
  return false;
}

void TFastRepo::StepMergeDisk(size_t /*block_slots_available*/) {
  assert(false); /* Fast repo should never have disk files to merge. */
}

void TFastRepo::StepTail(size_t /*block_slots_available*/) {
  assert(false);
}

size_t TFastRepo::AddSyncedFileToRepo(size_t /*starting_block_id*/,
                                      size_t /*starting_block_offset*/,
                                      size_t /*file_length*/,
                                      TSequenceNumber /*low_saved*/,
                                      TSequenceNumber /*high_saved*/,
                                      size_t /*num_keys*/) {
  throw std::logic_error("TFastRepo does not support disk files");
}

TSafeRepo::TSafeRepo(L0::TManager *manager,
                     const TUuid &repo_id,
                     const TTtl &ttl,
                     const Base::TOpt<L0::TManager::TPtr<L0::TManager::TRepo>> &parent_repo)
    : TRepo(manager,
            repo_id,
            ttl,
            parent_repo),
      NextGenId(1U) {
  #ifndef NDEBUG
  std::vector<Disk::TFileObj> file_vec;
  Manager->GetFileGenSet(repo_id, file_vec);
  assert(!file_vec.size());
  #endif
}

TSafeRepo::TSafeRepo(L0::TManager *manager,
                     const Base::TUuid &repo_id,
                     const TTtl &ttl,
                     const Base::TOpt<L0::TManager::TPtr<L0::TManager::TRepo>> &parent_repo,
                     const Base::TOpt<TSequenceNumber> &lowest,
                     const Base::TOpt<TSequenceNumber> &highest,
                     TSequenceNumber next_update,
                     TStatus status)
    : TRepo(manager,
            repo_id,
            ttl,
            parent_repo,
            lowest,
            highest,
            next_update,
            status),
      NextGenId(0UL) {
  std::vector<Disk::TFileObj> file_vec;
  Manager->GetFileGenSet(repo_id, file_vec);
  size_t max_gen_id = 0UL;
  /* we need to sort these first to make sure we add them in sequence. */
  std::sort(file_vec.begin(), file_vec.end(), [](const Disk::TFileObj &lhs, const Disk::TFileObj &rhs) -> bool {
    return lhs.LowestSeq < rhs.LowestSeq;
  });
  try {
    /* acquire Mapping lock */ {
      std::lock_guard<std::mutex> lock(MappingLock);
      TMapping *mapping = new TMapping(this);
      for (const auto &iter : file_vec) {
        max_gen_id = std::max(max_gen_id, iter.GenId);
        new TMapping::TEntry(mapping, new TDiskLayer(Manager, this, iter.GenId, iter.NumKeys, iter.LowestSeq, iter.HighestSeq));
      }
    }  // release Mapping lock
  } catch (...) {
    MappingCollection.DeleteEachMember();
    throw;
  }
  NextGenId = max_gen_id + 1UL;
}

TSafeRepo::~TSafeRepo() {
  PreDtor();
}

void TSafeRepo::StepTail(size_t block_slots_available) {
  assert(!GetParentRepo());
  Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed = Disk::Util::TVolume::TDesc::TStorageSpeed::Fast;
  try {
    /* Tail a merge disk file if available */ {
      /* grab the current mapping */ {
        TMapping *mapping = AcquireCurrentMapping();
        assert(mapping);
        TDiskLayer *new_merge_disk = 0;
        size_t gen_id_to_tail = 0UL;
        TDiskLayer *gen_layer_to_tail = nullptr;
        size_t num_keys = 0U;
        TSequenceNumber lowest_seq = numeric_limits<uint64_t>::max(), highest_seq = 0UL;
        try {
          /* acquire Merge lock */ {
            std::lock_guard<std::mutex> lock(MergeLock);
            TDataLayer *lowest_layer = mapping->GetEntryCollection()->TryGetFirstMember() ? mapping->GetEntryCollection()->TryGetFirstMember()->GetLayer() : nullptr;
            if (lowest_layer && lowest_layer->GetKind() == TDataLayer::TKind::Disk && !lowest_layer->GetMarkedTaken()) {
              num_keys += lowest_layer->GetSize();
              lowest_seq = std::min(lowest_seq, lowest_layer->GetLowestSeq());
              highest_seq = std::max(highest_seq, lowest_layer->GetHighestSeq());
              gen_layer_to_tail = reinterpret_cast<TDiskLayer *>(lowest_layer);
              gen_id_to_tail = reinterpret_cast<TDiskLayer *>(lowest_layer)->GetGenId();
              lowest_layer->MarkTaken();
            }
          }
          if (gen_layer_to_tail) {
            syslog(LOG_INFO, "Tailing file [%ld] with [%ld] num keys", gen_id_to_tail, num_keys);
            size_t gen_id = MergeFiles(std::vector<size_t>{gen_id_to_tail}, storage_speed, block_slots_available, Manager->GetTempFileConsolThresh(), lowest_seq, highest_seq, num_keys, GetReleasedUpTo(), true, true);
            {
              std::lock_guard<std::mutex> lock(Manager->MergeDiskCPULock);
              Manager->MergeDiskAverageKeysCalc.Push(num_keys);
            }
            new_merge_disk = new TDiskLayer(Manager, this, gen_id, num_keys, lowest_seq, highest_seq);
          }
        } catch (const std::exception &ex) {
          syslog(LOG_EMERG, "StepTail [1083] caught error [%s]", ex.what());
          ReleaseMapping(mapping);
          throw;
        } catch (...) {
          syslog(LOG_EMERG, "StepTail [1088] caught error");
          ReleaseMapping(mapping);
          throw;
        }
        try {
          if (gen_layer_to_tail) {
            assert(new_merge_disk);
            /* acquire Mapping lock */ {
              std::lock_guard<std::mutex> lock(MappingLock);
              size_t total_disk_layers = 0U;
              TMapping *cur_mapping = MappingCollection.TryGetLastMember();
              cur_mapping->Incr();
              try {
                TMapping *new_mapping = new TMapping(this);
                assert(cur_mapping);
                for (TMapping::TEntryCollection::TCursor cur_csr(cur_mapping->GetEntryCollection()); cur_csr; ++cur_csr) {
                  assert(cur_csr->GetLayer() != new_merge_disk);
                  bool found = false;
                  if (gen_layer_to_tail == cur_csr->GetLayer()) {
                    gen_layer_to_tail->MarkForDelete();
                    found = true;
                  }
                  if (!found) {
                    new TMapping::TEntry(new_mapping, cur_csr->GetLayer());
                    if (cur_csr->GetLayer()->GetKind() == TDataLayer::Disk) {
                      ++total_disk_layers;
                    }
                  }
                }
                new TMapping::TEntry(new_mapping, new_merge_disk);
                ++total_disk_layers;
                cur_mapping->Decr();
              } catch (const std::exception &ex) {
                syslog(LOG_EMERG, "StepTail [1161] caught error [%s]", ex.what());
                cur_mapping->Decr();
                throw;
              } catch (...) {
                syslog(LOG_EMERG, "StepTail [1165] caught error");
                cur_mapping->Decr();
                throw;
              }
              /* TODO: only enqueue if we are likely to be able to merge files of the same generation. */
              if (total_disk_layers >= 3) {
                EnqueueMergeDisk();
              }
            }
          }
        } catch (const std::exception &ex) {
          syslog(LOG_EMERG, "StepTail [1176] caught error [%s]", ex.what());
          ReleaseMapping(mapping);
          throw;
        } catch (...) {
          syslog(LOG_EMERG, "StepTail [1180] caught error");
          ReleaseMapping(mapping);
          throw;
        }
        ReleaseMapping(mapping);
      }
    }  // done flushing a tailed file
  } catch (const std::exception &ex) {
    syslog(LOG_EMERG, "StepTail [1188] caught error [%s]", ex.what());
    abort();
  }
}

void TSafeRepo::StepMergeDisk(size_t block_slots_available) {
  Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed = Disk::Util::TVolume::TDesc::TStorageSpeed::Fast;
  try {
    /* Flush a merge disk file if available */ {
      /* grab the current mapping */ {
        TMapping *mapping = AcquireCurrentMapping();
        assert(mapping);
        TDiskLayer *new_merge_disk = 0;
        std::vector<size_t> gen_id_vec;
        std::vector<TDiskLayer *> gen_layer_vec;
        size_t num_keys = 0U;
        TSequenceNumber lowest_seq = numeric_limits<uint64_t>::max(), highest_seq = 0UL;
        try {
          //std::map<size_t, std::vector<TDiskLayer *>> gen_to_gen_id_map;
          /* acquire Merge lock */ {
            std::lock_guard<std::mutex> lock(MergeLock);
            for (TMapping::TEntryCollection::TCursor csr(mapping->GetEntryCollection()); csr; ++csr) {
              TDataLayer *lhs_layer = csr->GetLayer();
              TDataLayer *rhs_layer = csr->TryGetNextMember() ? csr->TryGetNextMember()->GetLayer() : nullptr;
              /* if i'm a disk layer, so is my neighbor on the right, neither of us are marked as taken, and i'm in the same or lower gen set than my neigbor */
              if ((lhs_layer && rhs_layer)  // i have a neighbor
                  && (lhs_layer->GetKind() == TDataLayer::TKind::Disk)  // I'm a disk layer
                  && (rhs_layer->GetKind() == TDataLayer::TKind::Disk)  // my neighbor is a disk layer
                  && (!lhs_layer->GetMarkedTaken())  // I'm not marked taken
                  && (!rhs_layer->GetMarkedTaken())  // my neighbor is not marked taken
                  && (Disk::Util::SuggestGeneration(lhs_layer->GetSize()) <= Disk::Util::SuggestGeneration(rhs_layer->GetSize()))  // in in the same or lower gen set than my neighbor
                  ) {
                lowest_seq = std::min(lowest_seq, lhs_layer->GetLowestSeq());
                highest_seq = std::max(highest_seq, lhs_layer->GetHighestSeq());
                num_keys += lhs_layer->GetSize();
                gen_layer_vec.push_back(reinterpret_cast<TDiskLayer *>(lhs_layer));
                gen_id_vec.push_back(reinterpret_cast<TDiskLayer *>(lhs_layer)->GetGenId());
                lhs_layer->MarkTaken();
                lowest_seq = std::min(lowest_seq, rhs_layer->GetLowestSeq());
                highest_seq = std::max(highest_seq, rhs_layer->GetHighestSeq());
                num_keys += rhs_layer->GetSize();
                gen_layer_vec.push_back(reinterpret_cast<TDiskLayer *>(rhs_layer));
                gen_id_vec.push_back(reinterpret_cast<TDiskLayer *>(rhs_layer)->GetGenId());
                rhs_layer->MarkTaken();
                EnqueueMergeDisk();
                break;
              }
            }
          }  // release Merge lock
          if (gen_id_vec.size() > 0) {
            size_t gen_id = MergeFiles(gen_id_vec, storage_speed, block_slots_available, Manager->GetTempFileConsolThresh(), lowest_seq, highest_seq, num_keys, GetReleasedUpTo(), false, false);
            {
              std::lock_guard<std::mutex> lock(Manager->MergeDiskCPULock);
              Manager->MergeDiskAverageKeysCalc.Push(num_keys);
            }
            new_merge_disk = new TDiskLayer(Manager, this, gen_id, num_keys, lowest_seq, highest_seq);
          }
        } catch (const std::exception &ex) {
          syslog(LOG_EMERG, "StepMergeDisk [1113] caught error [%s]", ex.what());
          ReleaseMapping(mapping);
          throw;
        } catch (...) {
          syslog(LOG_EMERG, "StepMergeDisk [1117] caught error");
          ReleaseMapping(mapping);
          throw;
        }
        try {
          if (gen_id_vec.size() > 0) {
            assert(new_merge_disk);
            assert(gen_layer_vec.size() == gen_id_vec.size());
            /* acquire Mapping lock */ {
              std::lock_guard<std::mutex> lock(MappingLock);
              size_t total_disk_layers = 0U;
              TMapping *cur_mapping = MappingCollection.TryGetLastMember();
              cur_mapping->Incr();
              try {
                TMapping *new_mapping = new TMapping(this);
                assert(cur_mapping);
                for (TMapping::TEntryCollection::TCursor cur_csr(cur_mapping->GetEntryCollection()); cur_csr; ++cur_csr) {
                  assert(cur_csr->GetLayer() != new_merge_disk);
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
                    if (cur_csr->GetLayer()->GetKind() == TDataLayer::Disk) {
                      ++total_disk_layers;
                    }
                  }
                }
                new TMapping::TEntry(new_mapping, new_merge_disk);
                ++total_disk_layers;
                cur_mapping->Decr();
              } catch (const std::exception &ex) {
                syslog(LOG_EMERG, "StepMergeDisk [1161] caught error [%s]", ex.what());
                cur_mapping->Decr();
                throw;
              } catch (...) {
                syslog(LOG_EMERG, "StepMergeDisk [1165] caught error");
                cur_mapping->Decr();
                throw;
              }
              /* TODO: only enqueue if we are likely to be able to merge files of the same generation. */
              if (total_disk_layers >= 3) {
                EnqueueMergeDisk();
              }
            }
          }
        } catch (const std::exception &ex) {
          syslog(LOG_EMERG, "StepMergeDisk [1176] caught error [%s]", ex.what());
          ReleaseMapping(mapping);
          throw;
        } catch (...) {
          syslog(LOG_EMERG, "StepMergeDisk [1180] caught error");
          ReleaseMapping(mapping);
          throw;
        }
        ReleaseMapping(mapping);
      }
    }  // done flushing a merge file
  } catch (const std::exception &ex) {
    syslog(LOG_EMERG, "StepMergeDisk [1188] caught error [%s]", ex.what());
    abort();
  }
}

size_t TSafeRepo::AddSyncedFileToRepo(size_t starting_block_id,
                                      size_t starting_block_offset,
                                      size_t file_length,
                                      TSequenceNumber low_saved,
                                      TSequenceNumber high_saved,
                                      size_t num_keys) {
  assert(this);
  size_t gen_id = GetNextGenId();
  /* wait for file entry to flush */ {
    TCompletionTrigger trigger;
    syslog(LOG_INFO, "Adding file gen_id=[%ld], starting_block_id=[%ld], starting_block_offset=[%ld], file_length=[%ld], num_keys=[%ld], low_saved=[%ld], high_saved=[%ld]",
           gen_id, starting_block_id, starting_block_offset, file_length, num_keys, low_saved, high_saved);
    Manager->GetEngine()->InsertFile(GetId(), TFileObj::TKind::DataFile, gen_id, starting_block_id, starting_block_offset, file_length, num_keys, low_saved, high_saved, trigger);
    trigger.Wait();
  }
  AddFileToRepo(gen_id, low_saved, high_saved, num_keys);
  return gen_id;
}

TSafeRepo *TSafeRepo::ReConstructFromDisk(L0::TManager *manager,
                                          const Base::TUuid &repo_id,
                                          const TDeadline &deadline) {
  std::vector<Disk::TFileObj> file_vec;
  manager->GetFileGenSet(repo_id, file_vec);
  Base::TOpt<TSequenceNumber> lowest;
  Base::TOpt<TSequenceNumber> highest;
  TSequenceNumber next_update = 1L;
  size_t max_gen_id = 0UL;
  TParentRepo parent_repo;
  std::vector<size_t> gen_id_vec_to_remove;
  /* remove any files that are obsolete due to merging */ {
    for (;;) {
      bool found_dup = false;
      for (auto cur = file_vec.begin(); cur != file_vec.end(); ++cur) {
        const auto &file = *cur;
        for (const auto &that_file : file_vec) {
          if (file.LowestSeq >= that_file.LowestSeq && file.HighestSeq <= that_file.HighestSeq && file.GenId != that_file.GenId) {
            gen_id_vec_to_remove.push_back(file.GenId);
            file_vec.erase(cur);
            found_dup = true;
            break;
          }
        }
        if (found_dup) {
          break;
        }
      }
      if (!found_dup) {
        break;
      }
    }
  }
  for (const auto &file : file_vec) {
    max_gen_id = std::max(max_gen_id, file.GenId);
    syslog(LOG_INFO, "File [%ld] has [%ld] keys with seq range [%ld -> %ld]", file.GenId, file.NumKeys, file.LowestSeq, file.HighestSeq);
    lowest = !lowest ? file.LowestSeq : std::min(*lowest, file.LowestSeq);
    highest = !highest ? file.HighestSeq : std::max(*highest, file.HighestSeq);
    next_update = std::max(next_update, *highest + 1UL);
  }
  TCompletionTrigger trigger;
  for (size_t gen_id : gen_id_vec_to_remove) {
    manager->GetEngine()->RemoveFile(repo_id, gen_id, trigger);
  }
  trigger.Wait();
  TSafeRepo *safe_repo = new TSafeRepo(manager, repo_id, TTtl(deadline.time_since_epoch().count()), parent_repo, lowest, highest, next_update, TStatus::Normal);
  return safe_repo;
}

bool TSafeRepo::IsSafeRepo() const {
  assert(this);
  return true;
}

size_t TSafeRepo::MergeFiles(const std::vector<size_t> &gen_id_vec,
                             Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed,
                             size_t max_block_cache_read_slots_allowed,
                             size_t temp_file_consol_thresh,
                             TSequenceNumber &out_saved_low_seq,
                             TSequenceNumber &out_saved_high_seq,
                             size_t &out_num_keys,
                             TSequenceNumber release_up_to,
                             bool can_tail,
                             bool can_tail_tombstone) {
  assert(this);
  size_t gen_id = GetNextGenId();
  bool my_can_tail = can_tail && !static_cast<bool>(GetParentRepo()) && IsTailingAllowed();
  bool my_can_tail_tombstone = my_can_tail && can_tail_tombstone && (gen_id_vec.size() == 1);
  TMergeDataFile merge_data_file(Manager->GetEngine(), storage_speed, GetId(), gen_id_vec, GetId(), gen_id, release_up_to, Low, max_block_cache_read_slots_allowed, temp_file_consol_thresh, my_can_tail, my_can_tail_tombstone);
  out_num_keys = merge_data_file.GetNumKeys();
  out_saved_low_seq = merge_data_file.GetLowestSequence();
  out_saved_high_seq = merge_data_file.GetHighestSequence();
  return gen_id;
}

void TSafeRepo::RemoveFile(size_t gen_id) {
  assert(this);
  Util::TBlockVec block_vec;
  /* reader life span */ {
    TReader reader(Manager->GetEngine(), GetId(), Low, gen_id);
    try {
      TReader::TInStream in_stream(HERE, Source::FileRemoval, Low, &reader, Manager->GetEngine()->GetPageCache(), (reader.GetStartingBlockOffset() * Disk::Util::LogicalBlockSize) + (TData::NumMetaFields * sizeof(size_t)));
      size_t block_id;
      for (size_t i = 0; i < reader.GetNumMetaBlocks(); ++i) {
        in_stream.Read(block_id);
        block_vec.PushBack(block_id);
      }
      size_t num_contig_blocks;
      for (size_t i = 0; i < reader.GetNumSequentialBlockPairings(); ++i) {
        in_stream.Read(block_id);
        in_stream.Read(num_contig_blocks);
        block_vec.PushBack(std::make_pair(block_id, num_contig_blocks));
      }
      assert(block_vec.Size() == reader.GetNumBlocks());
    } catch (const std::exception &ex) {
      stringstream ss;
      ss << GetId();
      syslog(LOG_ERR, "RemoveFile [%s][%ld] caught error [%s] with NumBlocks=[%ld], NumMetaBlocks=[%ld], NumSequentialBlocks=[%ld], BlockVec.Size=[%ld], StartingBlockOffset=[%ld]",
             ss.str().c_str(), gen_id, ex.what(), reader.GetNumBlocks(), reader.GetNumMetaBlocks(), reader.GetNumSequentialBlockPairings(), block_vec.Size(), reader.GetStartingBlockOffset());
      throw;
    }
  }
  /* Now we can go to each scheduler and remove anything they have cached about this file... */ {
    Manager->ForEachScheduler([this, gen_id](Fiber::TRunner *runner) {
      Fiber::TRunner *cur_runner = Fiber::TRunner::LocalRunner;
      Fiber::SwitchTo(runner);
      const Base::TUuid &repo_id = GetId();
      Disk::TLocalReadFileCache<Disk::Util::LogicalPageSize,
        Disk::Util::LogicalBlockSize,
        Disk::Util::PhysicalBlockSize,
        Disk::Util::CheckedPage>::TLocalReadFile *my_read_file = Disk::TLocalReadFileCache<Disk::Util::LogicalPageSize,
        Disk::Util::LogicalBlockSize,
        Disk::Util::PhysicalBlockSize,
        Disk::Util::CheckedPage>::Cache->Get(Manager->GetEngine(), repo_id, gen_id);
      for (const auto &index_pair : my_read_file->GetIndexByIdMap()) {
        Disk::TLocalWalkerCache::Cache->Clear(repo_id, gen_id, index_pair.first);
      }
      Disk::TLocalReadFileCache<Disk::Util::LogicalPageSize,
        Disk::Util::LogicalBlockSize,
        Disk::Util::PhysicalBlockSize,
        Disk::Util::CheckedPage, true>::Cache->Clear(repo_id, gen_id);
      Fiber::SwitchTo(cur_runner);
      return true;
    });
  }
  Disk::TCompletionTrigger completion_trigger;
  try {
    Manager->GetEngine()->RemoveFile(GetId(), gen_id, completion_trigger);
  } catch (const std::exception &ex) {
    syslog(LOG_ERR, "TSafeRepo::RemoveFile error [%s]", ex.what());
    throw;
  }
  /* wait for the file to be removed from the file map */ {
    completion_trigger.Wait();
  }
  for (const auto &iter : block_vec.GetSeqBlockMap()) {
    Manager->GetEngine()->FreeSeqBlocks(iter.second.first, iter.second.second);
  }
}

size_t TSafeRepo::WriteFile(TMemoryLayer *memory_layer,
                            Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed,
                            TSequenceNumber &out_saved_low_seq,
                            TSequenceNumber &out_saved_high_seq,
                            size_t &out_num_keys,
                            TSequenceNumber release_up_to) {
  size_t gen_id = GetNextGenId();
  TDataFile data_file(Manager->GetEngine(), storage_speed, memory_layer, GetId(), gen_id, Manager->GetTempFileConsolThresh(), release_up_to, Medium/*, !static_cast<bool>(GetParentRepo())*/);
  out_num_keys = data_file.GetNumKeys();
  out_saved_low_seq = data_file.GetLowestSequence();
  out_saved_high_seq = data_file.GetHighestSequence();
  return gen_id;
}

std::unique_ptr<Stig::Indy::TPresentWalker> TSafeRepo::NewPresentWalkerFile(size_t gen_id,
                                                                            const TIndexKey &index_from,
                                                                            const TIndexKey &index_to) const {
  assert(this);
  return unique_ptr<Disk::TPresentWalkFileWrapper>(new Disk::TPresentWalkFileWrapper(Manager->GetEngine(),
                                                                                     GetId(),
                                                                                     gen_id,
                                                                                     index_from.GetIndexId(),
                                                                                     index_from.GetKey(),
                                                                                     index_to.GetKey()));
}

std::unique_ptr<Stig::Indy::TPresentWalker> TSafeRepo::NewPresentWalkerFile(size_t gen_id,
                                                                            const TIndexKey &index_key) const {
  assert(this);
  return unique_ptr<Disk::TPresentWalkFileWrapper>(new Disk::TPresentWalkFileWrapper(Manager->GetEngine(),
                                                                                     GetId(),
                                                                                     gen_id,
                                                                                     index_key.GetIndexId(),
                                                                                     index_key.GetKey()));
}

std::unique_ptr<Stig::Indy::TUpdateWalker> TSafeRepo::NewUpdateWalkerFile(size_t gen_id, TSequenceNumber from) const {
  assert(this);
  return unique_ptr<Disk::TUpdateWalkFile>(new Disk::TUpdateWalkFile(Manager->GetEngine(), GetId(), gen_id, from));
}