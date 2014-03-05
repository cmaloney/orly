/* <stig/spa/flux_capacitor/sync.cc>

   Implements <stig/spa/flux_capacitor/sync.h>.

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

#include <stig/spa/flux_capacitor/sync.h>

#include <base/os_error.h>

using namespace Base;
using namespace Stig::Spa::FluxCapacitor;

#ifndef NDEBUG
TSyncException TSyncExcept;
#endif

TSync::TSync() : KeyInitialized(false), RwLockInitialized(false) {
  try {
    TOsError::IfNe0(HERE, pthread_key_create(&Key, TLocal::DeleteLocal));
    KeyInitialized = true;
    TOsError::IfNe0(HERE, pthread_rwlock_init(&RwLock, 0));
    RwLockInitialized = true;
  } catch (...) {
    Cleanup();
    throw;
  }
}

TSync::~TSync() {
  assert(this);
  Cleanup();
}

void TSync::TLocal::DecrLockCount() {
  assert(this);
  /* Make sure we're not decrementing a zero. */
  assert(LockCount > 0);
  --LockCount;
  if (!LockCount) {
    pthread_rwlock_unlock(&Sync.RwLock);
  }
}

void TSync::TLocal::IncrLockCount(bool is_exclusive) {
  assert(this);
  assert(LockCount >= 0);
  /* Make sure this thread either
        (1) has not yet locked this target,
        (2) already has a shared or exclusive lock and is requesting an additional shared lock (which is always ok), or
        (3) already has an exclusive lock on the target (which makes requesting any addtional lock ok).
     Requesting an exclusive lock when the thread already has a shared lock is explicitly not allowed.  This assertion is
     meant to catch that case, which is always a programming error. */
  #ifndef NDEBUG
  if (!(LockCount == 0 || !is_exclusive || IsExclusive)) {
    throw TSyncExcept;
  }
  #endif
  if (!LockCount) {
    TOsError::IfNe0(HERE, (is_exclusive ? pthread_rwlock_wrlock : pthread_rwlock_rdlock)(&Sync.RwLock));
    IsExclusive = is_exclusive;
  }
  ++LockCount;
  /* Make sure the increment didn't the counter over. */
  assert(LockCount >= 0);
}

void TSync::TLocal::DeleteLocal(void *arg) {
  delete static_cast<TLocal *>(arg);
}

TSync::TLocal *TSync::TLocal::GetLocal(const TSync &sync) {
  assert(&sync);
  TLocal *local = static_cast<TLocal *>(pthread_getspecific(sync.Key));
  if (!local) {
    local = new TLocal(sync);
    try {
      TOsError::IfNe0(HERE, pthread_setspecific(sync.Key, local));
    } catch (...) {
      delete local;
      throw;
    }
  }
  return local;
}

TSync::TLocal::TLocal(const TSync &sync) : Sync(sync), LockCount(0) {}

TSync::TLocal::~TLocal() {}

void TSync::Cleanup() {
  assert(this);
  if (RwLockInitialized) {
    pthread_rwlock_destroy(&RwLock);
  }
  if (KeyInitialized) {
    pthread_key_delete(Key);
    /* TODO: Deleting the TLS key doesn't invoke the destructor associated with the key.  This means that we'll leak TLocal objects.
       This isn't a huge deal because the locking threads should have already shut down by the time we're destroying the rw-lock target
       and the threads will release their TLocal objects when they go.  Nevertheless, we should keep a list of all the TLocal instances
       and destroy them that remain after they key is destroyed.  Destroying the key first prevents new TLocal instances from cropping
       up while we're mowing them down. */
  }
}
