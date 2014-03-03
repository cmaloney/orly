/* <stig/indy/manager_base.cc> 

   Implements <stig/indy/manager_base.h>.

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

#include <stig/indy/manager_base.h>

#include <sys/syscall.h>

#include <base/assert_true.h>
#include <base/shutting_down.h>

using namespace std;
using namespace Base;
using namespace Stig::Indy::L0;

TManager::TRepo::~TRepo() {
  assert(this);
  assert(MappingCollection.IsEmpty()); /* otherwise didn't call PreDtor */
}

TManager::TRepo::TMapping::TEntry::TEntry(TMapping *mapping, TDataLayer *layer)
    : MappingMembership(this, Base::AssertTrue(layer)->GetLowestSeq(), &Base::AssertTrue(mapping)->EntryCollection),
      Layer(layer) {
  assert(mapping);
  assert(layer);
  /* these assertions are dependent on a neighbourly merging algorithm */
  assert(!MappingMembership.TryGetPrevMember() || (Layer->GetLowestSeq() > MappingMembership.TryGetPrevMember()->GetLayer()->GetHighestSeq()));
  assert(!MappingMembership.TryGetNextMember() || (Layer->GetHighestSeq() < MappingMembership.TryGetNextMember()->GetLayer()->GetLowestSeq()));
  Layer->Incr();
}

TManager::TRepo::TMapping::TEntry::~TEntry() {
  assert(this);
  assert(Layer);
  Layer->Decr();
}

TManager::TRepo::TDataLayer *TManager::TRepo::TMapping::TEntry::GetLayer() const {
  assert(this);
  assert(Layer);
  return Layer;
}

TManager::TRepo::TMapping::TEntry *TManager::TRepo::TMapping::TEntry::TryGetNextMember() const {
  assert(this);
  return MappingMembership.TryGetNextMember();
}

TManager::TRepo::TMapping::TEntry *TManager::TRepo::TMapping::TEntry::TryGetPrevMember() const {
  assert(this);
  return MappingMembership.TryGetPrevMember();
}

TManager::TRepo::TMapping::TMapping(TRepo *repo)
    : RepoMembership(this, &repo->MappingCollection),
      EntryCollection(this),
      RefCount(0U),
      MarkedForDelete(false) {}

TManager::TRepo::TMapping::~TMapping() {
  assert(this);
  EntryCollection.DeleteEachMember();
}

TManager::TRepo::TDataLayer::TDataLayer(TManager *manager)
    : Manager(manager),
      RemovalMembership(this),
      RefCount(0U),
      MarkedForDelete(false),
      MarkedTaken(false) {}

TManager::TRepo::TDataLayer::~TDataLayer() {
  assert(RefCount == 0UL);
}

void TManager::TRepo::TDataLayer::Incr() {
  assert(this);
  assert(!RemovalMembership.TryGetCollection());
  assert(!(MarkedTaken && RefCount == 0));
  __sync_add_and_fetch(&RefCount, 1U);
}

void TManager::TRepo::TDataLayer::Decr() {
  assert(this);
  size_t count = __sync_sub_and_fetch(&RefCount, 1U);
  if (MarkedTaken && count == 0) {
    std::lock_guard<std::mutex> removal_lock(Manager->RemovalLock);
    assert(!RemovalMembership.TryGetCollection());
    RemovalMembership.Insert(&Manager->RemovalCollection);
  }
}

TManager::TRepo::TRepo(TManager *manager, const TUuid &repo_id, const TTtl &ttl, TStatus status)
    : TObj(manager, repo_id, ttl),
      MappingCollection(this),
      Manager(manager),
      Status(status),
      Id(repo_id),
      MergeMemMembership(this),
      MergeDiskMembership(this) {
  TTime now;
  now.Now();
  SetTimeOfNextMergeMem(now);
  SetTimeOfNextMergeDisk(now);
}

void TManager::TRepo::PreDtor() {
  assert(this);
  /* We should be able to re-construct repo information by what files is made up of */
  #if 0
  if (GetTtl() > std::chrono::seconds(0)) {
    Manager->SaveRepo(this);
  }
  #endif
  /* Acquire Mapping lock */ {
    for (TMappingCollection::TCursor mapping_csr(&MappingCollection); mapping_csr; ++mapping_csr) {
      for (TMapping::TEntryCollection::TCursor entry_csr(mapping_csr->GetEntryCollection()); entry_csr; ++entry_csr) {
        entry_csr->GetLayer()->MarkTaken();
      }
    }
  }
  MappingCollection.DeleteEachMember();
}

size_t TManager::TRepo::MergeFiles(const std::vector<size_t> &/*gen_id_vec*/,
                                   Disk::Util::TVolume::TDesc::TStorageSpeed /*storage_speed*/,
                                   size_t /*max_block_cache_read_slots_allowed*/,
                                   size_t /*temp_file_consol_thresh*/,
                                   TSequenceNumber &/*out_saved_low_seq*/,
                                   TSequenceNumber &/*out_saved_high_seq*/,
                                   size_t &/*out_num_keys*/,
                                   TSequenceNumber /*release_up_to*/,
                                   bool /*can_tail*/,
                                   bool /*can_tail_tombstones*/) {
  assert(false);  /* repo's with files should implement this virtual function; otherwise it should never get called. */
  throw;
}

void TManager::TRepo::RemoveFile(size_t /*gen_id*/) {
  assert(false);  /* repo's with files should implement this virtual function; otherwise it should never get called. */
  throw;
}

size_t TManager::TRepo::WriteFile(TMemoryLayer */*memory_layer*/,
                                  Disk::Util::TVolume::TDesc::TStorageSpeed /*storage_speed*/,
                                  TSequenceNumber &/*out_saved_low_seq*/,
                                  TSequenceNumber &/*out_saved_high_seq*/,
                                  size_t &/*out_num_keys*/,
                                  TSequenceNumber /*release_up_to*/) {
  assert(false);  /* repo's with files should implement this virtual function; otherwise it should never get called. */
  throw;
}

std::unique_ptr<Stig::Indy::TPresentWalker> TManager::TRepo::NewPresentWalkerFile(size_t /*gen_id*/,
                                                                                  const TIndexKey &/*from*/,
                                                                                  const TIndexKey &/*to*/) const {
  assert(false);  /* repo's with files should implement this virtual function; otherwise it should never get called. */
  throw;
}

std::unique_ptr<Stig::Indy::TPresentWalker> TManager::TRepo::NewPresentWalkerFile(size_t /*gen_id*/,
                                                                                  const TIndexKey &/*key*/) const {
  assert(false);  /* repo's with files should implement this virtual function; otherwise it should never get called. */
  throw;
}

std::unique_ptr<Stig::Indy::TUpdateWalker> TManager::TRepo::NewUpdateWalkerFile(size_t /*gen_id*/, TSequenceNumber /*from*/) const {
  assert(false);  /* repo's with files should implement this virtual function; otherwise it should never get called. */
  throw;
}

void TManager::TRepo::MakeDirty() {
  if (!DirtyPtr) {
    DirtyPtr = Manager->Open<TRepo>(GetId());
  }
}

void TManager::TRepo::RemoveFromDirty() {
  DirtyPtr.Reset();
}

TManager::TManager(Disk::Util::TEngine *engine,
                   size_t merge_mem_delay,
                   size_t merge_disk_delay,
                   bool allow_tailing,
                   bool /*no_realtime*/,
                   size_t layer_cleaning_interval_milliseconds,
                   Base::TScheduler *scheduler,
                   size_t block_slots_available_per_merger,
                   size_t max_repo_cache_size,
                   size_t walker_local_cache_size,
                   size_t temp_file_consol_thresh,
                   const std::vector<size_t> &merge_mem_cores,
                   const std::vector<size_t> &merge_disk_cores,
                   bool /*create_new*/)
    :
      Scheduler(scheduler),
      ShuttingDown(false),
      AllowTailing(allow_tailing),
      RemovalCollection(this),
      LayerCleanerTimer(layer_cleaning_interval_milliseconds),
      MergeMemQueue(this),
      MergeDiskQueue(this),
      MergeMemDelay(merge_mem_delay),
      MergeDiskDelay(merge_disk_delay),
      BlockSlotsAvailablePerMerger(block_slots_available_per_merger),
      MaxCacheSize(max_repo_cache_size),
      Engine(engine),
      WalkerLocalCacheSize(walker_local_cache_size),
      TempFileConsolThresh(temp_file_consol_thresh),
      MergeMemCores(merge_mem_cores),
      MergeDiskCores(merge_disk_cores),
      TetrisManager(nullptr),
      OnCloseCb(std::bind(&TManager::OnClose, this, std::placeholders::_1)) {}

TManager::~TManager() {
  RemoveLayersFromQueue(); /* get rid of any layers pushed by the removal of the system repo (predtor) */
  ShuttingDown = true;
}

void TManager::CloseAllUnreferencedObjects() {
  assert(this);
  MergeDiskQueue.RemoveEachMember();
  MergeMemQueue.RemoveEachMember();
  RemoveLayersFromQueue(); /* get rid of any layers that were previously registered in this queue */
  std::vector<TId> to_erase;
  for (const auto &item: OpenableObjs) {
    if (item.second->PtrCount == 0) {
      to_erase.emplace_back(item.first);
      delete item.second;
    }
  }
  for (const auto &id : to_erase) {
    OpenableObjs.erase(id);
  }
  RemoveLayersFromQueue(); /* get rid of all layers pushed by the closing of the open map */
}

bool TManager::PreDtor() {
  assert(this);
  MergeDiskQueue.RemoveEachMember();
  MergeMemQueue.RemoveEachMember();
  for (const auto &item: OpenableObjs) {
    /* If this assertion fails, it means there is at least one ptr still alive someplace. */
    assert(item.second->PtrCount == 0);
    delete item.second;
  }
  return false;
}

Stig::Server::TTetrisManager *TManager::GetTetrisManager() const {
  return TetrisManager;
}

void TManager::SetTetrisManager(Stig::Server::TTetrisManager *tetris_manager) {
  assert(this);
  assert(TetrisManager == nullptr);
  TetrisManager = tetris_manager;
}

void TManager::CompactOpemMap() {
  assert(this);
  //throw std::logic_error("TODO: Implement CompactOpenMap()");
}

void TManager::RunLayerCleaner() {
  assert(this);
  if (Engine->IsDiskBased()) {
    /* if this is a disk based engine, allocate event pools */
    assert(!Disk::Util::TDiskController::TEvent::LocalEventPool);
    Disk::Util::TDiskController::TEvent::LocalEventPool = new TThreadLocalPoolManager<Disk::Util::TDiskController::TEvent>::TThreadLocalRegisteredPool(&Disk::Util::TDiskController::TEvent::DiskEventPoolManager, 1000UL);
  }
  for (;;) {
    LayerCleanerTimer.Pop();
    RemoveLayersFromQueue();
  }
}

void TManager::RunMergeMem() {
  assert(this);
  cpu_set_t mask;
  CPU_ZERO(&mask);
  assert(MergeMemCores.size());
  for (size_t core : MergeMemCores) {
    CPU_SET(core, &mask);
  }
  Base::IfLt0(sched_setaffinity(syscall(SYS_gettid), sizeof(cpu_set_t), &mask));
  if (Engine->IsDiskBased()) {
    /* if this is a disk based engine, allocate event pools */
    assert(!Disk::Util::TDiskController::TEvent::LocalEventPool);
    Disk::Util::TDiskController::TEvent::LocalEventPool = new TThreadLocalPoolManager<Disk::Util::TDiskController::TEvent>::TThreadLocalRegisteredPool(&Disk::Util::TDiskController::TEvent::DiskEventPoolManager, 10000UL);
  }
  int timeout = -1;
  Base::TTime deadline;
  TRepo *repo = nullptr;
  /* Register ourselves for CPU time collection */ {
    lock_guard<mutex> lock(MergeThreadCPUMutex);
    timespec start_val;
    clockid_t cid;
    Base::IfNe0(pthread_getcpuclockid(pthread_self(), &cid));
    Base::IfLt0(clock_gettime(cid, &start_val));
    MergeMemThreadCPUMap.insert(make_pair(pthread_self(), start_val));
  }
  for (; !ShuttingDown;) {
    /* we can only have 1 thread waiting on MergeMemSem at a time */ {
      lock_guard<mutex> epoll_lock(MergeMemEpollLock);
      if (timeout > 0) {
        timeout = deadline.Remaining();
        if (timeout > 0) {
          timespec wait_time {timeout / 1000, (timeout % 1000L) * 1000000L};
          nanosleep(&wait_time, NULL);
        }
      }
      MergeMemSem.Pop();
    }
    /* acquire MergeMem lock */ {
      std::lock_guard<std::mutex> lock(MergeMemLock);
      repo = MergeMemQueue.TryGetFirstMember();
      if (repo && !ShuttingDown) {
        deadline = repo->GetTimeOfNextMergeMem();
        size_t remaining = deadline.Remaining();
        bool cont = false;
        if (remaining) {
          timeout = remaining;
          cont = true;
        } else {
          TTime next;
          next.Now();
          next += MergeMemDelay;
          repo->SetTimeOfNextMergeMem(next);
          repo->MergeMemMembership.Remove();
        }
        if (!MergeMemQueue.IsEmpty()) {
          MergeMemSem.Push();
        }
        if (cont) {
          continue;
        }
      } else {
        timeout = -1;
        continue;
      }
    }  // release MergeMem lock
    assert(repo);
    try {
      repo->StepMergeMem();
    } catch (...) {
      EnqueueMergeMem(repo);
      throw;
    }
  }
  /* De-Register ourselves for CPU time collection */ {
    lock_guard<mutex> lock(MergeThreadCPUMutex);
    MergeMemThreadCPUMap.erase(pthread_self());
  }
}

void TManager::RunMergeDisk() {
  assert(this);
  cpu_set_t mask;
  CPU_ZERO(&mask);
  assert(MergeDiskCores.size());
  for (size_t core : MergeDiskCores) {
    CPU_SET(core, &mask);
  }
  Base::IfLt0(sched_setaffinity(syscall(SYS_gettid), sizeof(cpu_set_t), &mask));
  if (Engine->IsDiskBased()) {
    /* if this is a disk based engine, allocate event pools */
    assert(!Disk::Util::TDiskController::TEvent::LocalEventPool);
    Disk::Util::TDiskController::TEvent::LocalEventPool = new TThreadLocalPoolManager<Disk::Util::TDiskController::TEvent>::TThreadLocalRegisteredPool(&Disk::Util::TDiskController::TEvent::DiskEventPoolManager, 10000UL);
  }
  int timeout = -1;
  Base::TTime deadline;
  TRepo *repo = nullptr;
  /* Register ourselves for CPU time collection */ {
    lock_guard<mutex> lock(MergeThreadCPUMutex);
    timespec start_val;
    clockid_t cid;
    Base::IfNe0(pthread_getcpuclockid(pthread_self(), &cid));
    Base::IfLt0(clock_gettime(cid, &start_val));
    MergeDiskThreadCPUMap.insert(make_pair(pthread_self(), start_val));
  }
  for (; !ShuttingDown;) {
    /* we can only have 1 thread waiting on MergeDiskSem at a time */ {
      lock_guard<mutex> epoll_lock(MergeDiskEpollLock);
      if (timeout > 0) {
        timeout = deadline.Remaining();
        if (timeout > 0) {
          timespec wait_time {timeout / 1000, (timeout % 1000L) * 1000000L};
          nanosleep(&wait_time, NULL);
        }
      }
      MergeDiskSem.Pop();
    }
    /* acquire MergeDisk lock */ {
      std::lock_guard<std::mutex> lock(MergeDiskLock);
      repo = MergeDiskQueue.TryGetFirstMember();
      if (repo && !ShuttingDown) {
        deadline = repo->GetTimeOfNextMergeDisk();
        size_t remaining = deadline.Remaining();
        bool cont = false;
        if (remaining) {
          timeout = remaining;
          cont = true;
        } else {
          TTime next;
          next.Now();
          next += MergeDiskDelay;
          repo->SetTimeOfNextMergeDisk(next);
          repo->MergeDiskMembership.Remove();
        }
        if (!MergeDiskQueue.IsEmpty()) {
          MergeDiskSem.Push();
        }
        if (cont) {
          continue;
        }
      } else {
        timeout = -1;
        continue;
      }
    }  // release MergeDisk lock
    assert(repo);
    try {
      repo->StepMergeDisk(BlockSlotsAvailablePerMerger);
    } catch (...) {
      EnqueueMergeDisk(repo);
      throw;
    }
  }
  /* De-Register ourselves for CPU time collection */ {
    lock_guard<mutex> lock(MergeThreadCPUMutex);
    MergeDiskThreadCPUMap.erase(pthread_self());
  }
}

void TManager::ReportMergeCPUTime(double &out_merge_mem, double &out_merge_disk) {
  assert(this);
  out_merge_mem = 0.0;
  out_merge_disk = 0.0;
  lock_guard<mutex> lock(MergeThreadCPUMutex);
  for (auto &iter : MergeMemThreadCPUMap) {
    timespec cur_val;
    clockid_t cid;
    Base::IfNe0(pthread_getcpuclockid(iter.first, &cid));
    Base::IfLt0(clock_gettime(cid, &cur_val));
    out_merge_mem += ((cur_val.tv_sec - iter.second.tv_sec) * 1000000000L) + (cur_val.tv_nsec - iter.second.tv_nsec);
    std::swap(iter.second, cur_val);
  }
  for (auto &iter : MergeDiskThreadCPUMap) {
    timespec cur_val;
    clockid_t cid;
    Base::IfNe0(pthread_getcpuclockid(iter.first, &cid));
    Base::IfLt0(clock_gettime(cid, &cur_val));
    out_merge_disk += ((cur_val.tv_sec - iter.second.tv_sec) * 1000000000L) + (cur_val.tv_nsec - iter.second.tv_nsec);
    std::swap(iter.second, cur_val);
  }
  out_merge_mem /= 1000000000.0;
  out_merge_disk /= 1000000000.0;
}

void TManager::GetFileGenSet(const Base::TUuid &repo_id, std::vector<Disk::TFileObj> &file_vec) {
  assert(this);
  Engine->AppendFileGenSet(repo_id, file_vec);
}

void TManager::OnClose(TRepo *repo) {
  assert(this);
  assert(repo);
  assert(repo->Manager == this);
  throw std::logic_error("TODO: implement TManager::OnClose()");
}

void TManager::EnqueueMergeMem(TRepo *repo) {
  /* acquire MergeMem lock */ {
    std::lock_guard<std::mutex> lock(MergeMemLock);
    if (repo->MergeMemMembership.TryGetCollector() == nullptr) {
      MergeMemQueue.Insert(&repo->MergeMemMembership);
      MergeMemSem.Push();
    }
  }  // release MergeMem lock
}

void TManager::EnqueueMergeDisk(TRepo *repo) {
  /* acquire MergeDisk lock */ {
    std::lock_guard<std::mutex> lock(MergeDiskLock);
    if (repo->MergeDiskMembership.TryGetCollector() == nullptr) {
      MergeDiskQueue.Insert(&repo->MergeDiskMembership);
      MergeDiskSem.Push();
    }
  }  // release MergeDisk lock
}

void TManager::RemoveLayersFromQueue() {
  assert(this);
  TRepo::TDataLayer *layer = nullptr;
  for (;;) {
    /* acquire Removal lock */ {
      std::lock_guard<std::mutex> lock(RemovalLock);
      layer = RemovalCollection.TryGetFirstMember();
      if (layer) {
        layer->RemoveFromCollection();
      }
    }  // release Removal lock
    if (layer) {
      delete layer; /* POSSIBLE SEGFAULT */
      layer = nullptr;
    } else {
      break;
    }
  }
}

void TManager::DestroyObj(TObj *obj) noexcept {
  assert(this);
  assert(obj);
  size_t erased_from_openable = OpenableObjs.erase(obj->GetId());
  assert(erased_from_openable == 1);
  delete obj;
}

bool TManager::TryCacheObj(TObj *obj) noexcept {
  assert(this);
  assert(obj);
  bool success = false;
  if (MaxCacheSize) {
    try {
      /* The object is entering the cache.  It should have no sem right now, but it will need one later. */
      assert(!(obj->Sem));
      obj->Sem = new TSem;
      /* Discard objects from the cache until there's room for the new object.
         Start with the object with the soonest deadline and work forward. */
      while (ClosedObjs.size() >= MaxCacheSize) {
        auto iter = ClosedObjs.begin();
        TObj *cached_obj = iter->second;
        ClosedObjs.erase(iter);
        DestroyObj(cached_obj);
      }
      /* Insert this object into the cache in order of its deadline. */
      ClosedObjs.insert(make_pair(make_pair(*(obj->GetDeadline()), obj->GetId()), obj));
      success = true;
    } catch (const exception &ex) {
      obj->Log(LOG_INFO, "caching", ex);
    }
  }
  return success;
}

const char *TManager::TObj::GetKind() const noexcept {
  return "<partially open>";
}

void TManager::TObj::Log(int level, const char *action, const exception &ex) const noexcept {
  assert(this);
  Log(level, action, ex.what());
}

void TManager::TObj::Log(int level, const char *action, const char *msg) const noexcept {
  assert(this);
  assert(action);
  assert(msg);
  char buf[TId::MinBufSize];
  Id.Format(buf);
  syslog(level, "while %s durable %s, id = {%s}: %s", action, GetKind(), buf, msg);
}

void TManager::TObj::SetTtl(const TTtl &ttl) {
  assert(this);
  assert(ttl.count() >= 0);
  assert(!Deadline);
  Ttl = ttl;
}

TManager::TObj::TObj(TManager *manager, const TId &id, const TTtl &ttl)
    : Manager(manager), Id(id), PtrCount(0), OnDisk(false), Ttl(ttl) {
  assert(manager);
  Sem = new TSem;
}

TManager::TObj::~TObj() {
  assert(this);
  delete Sem;
}

bool TManager::TObj::ForEachDependentPtr(const function<bool (TAnyPtr &)> &) noexcept {
  return true;
}

void TManager::TObj::OnPtrAcquire() noexcept {
  assert(this);
  lock_guard<mutex> lock(Manager->DurableMutex);
  PtrCount += 1;
  assert(PtrCount > 0);
}

void TManager::TObj::OnPtrRelease() noexcept {
  assert(this);
  assert(PtrCount);
  bool async = false;
  TSem *sem = nullptr;
  unordered_set<TObj *> dependent_objs;
  /* extra */ {
    lock_guard<mutex> lock(Manager->DurableMutex);
    --PtrCount;
    if (!PtrCount) {
      /* We're transitioning from open to closed.  We should not yet have a deadline but we should have a sem available. */
      assert(!Deadline);
      assert(Sem);
      /* Steal the sem.  We may be destroyed by the ensuing logic and we'll need to keep the sem around to wait for. */
      sem = Sem;
      Sem = nullptr;
      if (Ttl.count() > 0) {
        /* We have a non-zero time-to-live, so establish a deadline and save the object. */
        Deadline = TDeadline::clock::now() + Ttl;
        try {
          /* For now, let's not write this repo to this disk, as we don't want a system repo transaction per read that opens a POV. */
          sem->Push();
          //OnDisk = true;
          async = true;
        } catch (const exception &ex) {
          Log(LOG_ERR, "saving", ex);
        }
        /* Before we cache or delete the object, force it to release all its dependent objects.
           We'll take responsibility for them here and release them properly outside of the lock. */
        ForEachDependentPtr(
            [this, &dependent_objs](TAnyPtr &ptr) {
              try {
                TObj *obj = ptr.ForceRelease();
                if (obj) {
                  dependent_objs.insert(obj);
                }
              } catch (const exception &ex) {
                Log(LOG_CRIT, "releasing dependent", ex);
              }
              return true;
            }
        );
        /* If we saved successfully, try to cache the object. */
        bool cached = async ? Manager->TryCacheObj(this) : false;
        /* If the object is not now in the cache, evict it from the openable set and destroy it. */
        if (!cached) {
          Manager->DestroyObj(this);
        }
      } else {
        /* We have a zero time-to-live, so delete the object from disk (if necessary) and evict it. */
        if (OnDisk) {
          try {
            Manager->Delete(Id, sem);
            async = true;
          } catch (const exception &ex) {
            Log(LOG_CRIT, "deleting", ex);
          }
        }
        /* Before we cache or delete the object, force it to release all its dependent objects.
           We'll take responsibility for them here and release them properly outside of the lock. */
        ForEachDependentPtr(
            [this, &dependent_objs](TAnyPtr &ptr) {
              try {
                TObj *obj = ptr.ForceRelease();
                if (obj) {
                  dependent_objs.insert(obj);
                }
              } catch (const exception &ex) {
                Log(LOG_CRIT, "releasing dependent", ex);
              }
              return true;
            }
        );
        Manager->DestroyObj(this);
      }
    }
  }  // end of mutex lock; after this scope closes, 'this' may be a bad pointer
  /* Release any dependents we found. */
  for (auto obj: dependent_objs) {
    stringstream ss;
    ss << obj->GetId();
    obj->OnPtrRelease();
  }
  /* If there's an asynch operation pending, we'll wait for it using the semaphore we stole. */
  if (async) {
    assert(sem);
    try {
      sem->Pop();
    } catch (const exception &ex) {
      syslog(LOG_CRIT, "while synching durable object: %s", ex.what());
    }
  }
  /* Dispose of the stolen semaphore. */
  delete sem;
}

void TManager::TObj::OnPtrAdoptNew() noexcept {
  assert(this);
  assert(!PtrCount);
  PtrCount = 1;
}

void TManager::TObj::OnPtrAdoptOld() noexcept {
  assert(this);
  PtrCount += 1;
  assert(PtrCount > 0);
}

template <>
TManager::TPtr<TManager::TRepo> TManager::Open(const TId &id) {
  assert(this);
  std::pair<std::unordered_map<TId, TObj *>::iterator, bool> ret;
  for (;;) {
    /* Lock the manager and find/create the requested slot among the openable objects. */
    std::unique_lock<std::mutex> lock(DurableMutex);
    ret = OpenableObjs.insert(std::pair<TId, TObj *>(id, nullptr));
    TObj *&openable_obj = ret.first->second;
    if (openable_obj) {
      /* We found an object with the given id. */
      TPtr<TManager::TRepo> ptr(openable_obj, Stig::Indy::L0::Old);
      const auto &deadline = openable_obj->GetDeadline();
      if (deadline) {
        /* The object is being re-opened from a closed state, so remove it from the set of closed objects. */
        size_t erased_from_closed = ClosedObjs.erase(std::make_pair(*deadline, id));
        assert(erased_from_closed == 1);
        openable_obj->Deadline.Reset();
      }
      return ptr;
    } else if (ret.second) { /* freshly inserted */
      break;
    } else {
      DurableCond.wait(lock);
    }
  }
  try {
    /* Load the object from disk and keep it in the openable set. */
    TRepo *repo = ReconstructRepo(id);
    std::lock_guard<std::mutex> lock(DurableMutex);
    TObj *&openable_obj = ret.first->second;
    assert(!openable_obj);
    openable_obj = repo;
    DurableCond.notify_all();
    return TPtr<TManager::TRepo>(repo, Stig::Indy::L0::New);
    throw std::logic_error("TODO: implement L0::TManager Load from disk");
  } catch (...) {
    /* We could not find the object on disk or the object's constructor failed.
       Either way, we need to dispose of the slot we made before continuing to handle the error. */
    std::lock_guard<std::mutex> lock(DurableMutex);
    OpenableObjs.erase(id);
    throw;
  }
}
