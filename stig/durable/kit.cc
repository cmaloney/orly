/* <stig/durable/kit.cc>

   Implements <stig/durable/kit.h>.

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

#include <stig/durable/kit.h>

#include <syslog.h>

using namespace std;
using namespace chrono;
using namespace Io;
using namespace Stig::Durable;

void TManager::Clean() {
  assert(this);
  TSem sem;
  /* extra */ {
    lock_guard<mutex> lock(Mutex);
    auto now = TDeadline::clock::now();
    while (!ClosedObjs.empty()) {
      auto iter = ClosedObjs.begin();
      if (iter->first.first > now) {
        break;
      }
      TObj *cached_obj = iter->second;
      ClosedObjs.erase(iter);
      DestroyObj(cached_obj);
    }
    CleanDisk(now, &sem);
  }
  sem.Pop();
}

TManager::TManager(size_t max_cache_size)
    : MaxCacheSize(max_cache_size) {}

TManager::~TManager() {
  assert(this);
  for (const auto &item: OpenableObjs) {
    /* If this assertion fails, it means there is at least one ptr still alive someplace. */
    assert(item.second->PtrCount == 0);
    delete item.second;
  }
}

void TManager::Clear() {
  for (const auto &item: OpenableObjs) {
    /* If this assertion fails, it means there is at least one ptr still alive someplace. */
    assert(item.second->PtrCount == 0);
    delete item.second;
  }
  OpenableObjs.clear();
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

const char *TObj::GetKind() const noexcept {
  return "<partially open>";
}

void TObj::Log(int level, const char *action, const exception &ex) const noexcept {
  assert(this);
  Log(level, action, ex.what());
}

void TObj::Log(int level, const char *action, const char *msg) const noexcept {
  assert(this);
  assert(action);
  assert(msg);
  char buf[TId::MinBufSize];
  Id.Format(buf);
  syslog(level, "while %s durable %s, id = {%s}: %s", action, GetKind(), buf, msg);
}

void TObj::SetTtl(const TTtl &ttl) {
  assert(this);
  assert(ttl.count() >= 0);
  assert(!Deadline);
  Ttl = ttl;
}

void TObj::Write(TBinaryOutputStream &strm) const {
  assert(this);
  assert(&strm);
  strm << Ttl;
}

TObj::TObj(TManager *manager, const TId &id, const TTtl &ttl)
    : Manager(manager), Id(id), PtrCount(0), OnDisk(false), Ttl(ttl) {
  assert(manager);
  Sem = new TSem;
}

TObj::TObj(TManager *manager, const TId &id, Io::TBinaryInputStream &strm)
    : Manager(manager), Id(id), PtrCount(0), OnDisk(true) {
  assert(manager);
  assert(&strm);
  Sem = new TSem;
  try {
    strm >> Ttl;
  } catch (...) {
    this->~TObj();
    throw;
  }
}

TObj::~TObj() {
  assert(this);
  delete Sem;
}

bool TObj::ForEachDependentPtr(const function<bool (TAnyPtr &)> &) noexcept {
  return true;
}

void TObj::OnPtrAcquire() noexcept {
  assert(this);
  lock_guard<mutex> lock(Manager->Mutex);
  PtrCount += 1;
  assert(PtrCount > 0);
}

void TObj::OnPtrRelease() noexcept {
  assert(this);
  assert(PtrCount);
  bool async = false;
  TSem *sem = nullptr;
  unordered_set<TObj *> dependent_objs;
  /* extra */ {
    lock_guard<mutex> lock(Manager->Mutex);
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
          string blob;
          /* extra */ {
            auto recorder = make_shared<TRecorder>();
            TBinaryOutputOnlyStream strm(recorder);
            Write(strm);
            strm.Flush();
            recorder->CopyOut(blob);
          }
          Manager->Save(Id, *Deadline, Ttl, blob, sem);
          OnDisk = true;
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
        Manager->DestroyObj(this);
      }
    }
  }  // end of mutex lock; after this scope closes, 'this' may be a bad pointer
  /* Release any dependents we found. */
  for (auto obj: dependent_objs) {
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

void TObj::OnPtrAdoptNew() noexcept {
  assert(this);
  assert(!PtrCount);
  PtrCount = 1;
}

void TObj::OnPtrAdoptOld() noexcept {
  assert(this);
  PtrCount += 1;
  assert(PtrCount > 0);
}
