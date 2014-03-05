/* <stig/spa/flux_capacitor/sync.h>

   A recursive target for read-write synchronization.

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

#include <cassert>
#include <exception>
#include <pthread.h>

#include <base/no_copy_semantics.h>

namespace Stig {

  namespace Spa {

    namespace FluxCapacitor {

      /* A recursive target for read-write synchronization.  Here's an example:

           TSync ListSync;
           std::list<int> List;

           void Append(int val) {
            TSync::TExclusiveLock lock(ListSync);
            List.push_back(val);
           }

           int GetTotal() {
            TSync::TSharedLock lock(ListSync);
            int total = 0;
            for (auto iter = List.begin(); iter != List.end(); ++iter) {
              total += *iter;
            }
            return total;
           }

         Multiple threads can call Append() and GetTotal() concurrently and they will not interfere with each other.  In particular, multiple threads can
         simultaneously enter GetTotal() and in doing so will block threads trying to enter Append().  If a thread enters Append(), it will block all other
         threads from entering Append() or GetTotal().

         It's ok for a thread to construct multiple locks on the same target at the same time; however, in such situations, it's the first lock that counts.
         The first lock a thread acquires on a target establishes the relationship between the thread and the target.  If the first lock is exclusive, then all
         subsequent locks will be treated as exclusive (regardless of what kind of lock the thread actually requests).  If the first lock is shared, however,
         then all subsequent locks must also be shared.  Requesting an exclusive lock after already gaining one or more exclusive locks is an error. */
      class TSync {
        NO_COPY_SEMANTICS(TSync);
        private:

        /* This forward-declaration of a private type is necessary because the lock classes refer to it.  Ugly but oh well. */
        class TLocal;

        public:

        /* Construct an instance of this class to gain an exclusive lock on a target.
           The thread must not already have a shared lock on the target. */
        class TExclusiveLock {
          NO_COPY_SEMANTICS(TExclusiveLock);
          public:

          /* Does not return until the lock is granted. */
          TExclusiveLock(const TSync &sync) {
            Local = TLocal::GetLocal(sync);
            Local->IncrLockCount(true);
          }

          /* When the last instance of this class goes out of scope, the exclusive lock is released. */
          ~TExclusiveLock() {
            assert(this);
            Local->DecrLockCount();
          }

          private:

          /* The local object of the calling thread. */
          TLocal *Local;

        };  // TExclusiveLock

        /* Construct an instance of this class to gain a shared lock on a target.
           If the thread already has an exclusive lock on the target, this class increases the exclusive lock count instead. */
        class TSharedLock {
          NO_COPY_SEMANTICS(TSharedLock);
          public:

          /* Does not return until the lock is granted. */
          TSharedLock(const TSync &sync) {
            assert(this);
            Local = TLocal::GetLocal(sync);
            Local->IncrLockCount(false);
          }

          /* When the last instance of this class goes out of scope, the shared lock is released. */
          ~TSharedLock() {
            Local->DecrLockCount();
          }

          /* TODO */
          const TSync &GetSync() const {
            assert(this);
            return Local->GetSync();
          }

          private:

          /* The local object of the calling thread. */
          TLocal *Local;

        };  // TSharedLock

        /* Constructs a new, unlocked target. */
        TSync();

        /* Destroys the target.  The target must not currently be locked by any thread.  Destroying a locked target has undefined results. */
        ~TSync();

        private:

        /* Each thread has a local instance of this class.  It tracks the kind and number of locks the thread currently holds on this target. */
        class TLocal {
          NO_COPY_SEMANTICS(TLocal);
          public:

          /* When the count reaches zero, the lock is released. */
          void DecrLockCount();

          /* If lock count is currently zero, then this function will block until the lock can be obtained; otherwise, the lock count simply increments.
             It is not legal to request an exclusive lock after already requesting shared lock, but the reverse is ok. */
          void IncrLockCount(bool is_exclusive);

          /* Deletes the local object when a thread shuts down.  Called automatically by the thread library. */
          static void DeleteLocal(void *arg);

          /* Returns the thread-local object for the given target, constructing it if necessary.  This function never returns null. */
          static TLocal *GetLocal(const TSync &sync);

          /* TODO */
          const TSync &GetSync() const {
            assert(this);
            return Sync;
          }

          private:

          /* Construct a new thread-local object with a lock count of zero. */
          TLocal(const TSync &sync);

          /* Do-little destructor. */
          ~TLocal();

          /* The target to which we refer. */
          const TSync &Sync;

          /* The number of times this thread has locked the target.  Zero means no lock. */
          int LockCount;

          /* If LockCount is zero, then this field is undefined; otherwise, this field indicates whether our lock is exclusive. */
          bool IsExclusive;

        };  // TLocal

        /* Frees resources in preparation for destruction. */
        void Cleanup();

        /* True iff. Key has been initialized. */
        bool KeyInitialized;

        /* The key in which we store a pointer to the thread-local objects for this target. */
        pthread_key_t Key;

        /* True iff. RwLock has been initialized. */
        bool RwLockInitialized;

        /* The rw-lock for which threads will contend.  It's mutable so we can lock and unlock a constant target. */
        mutable pthread_rwlock_t RwLock;

      };  // TSync

      #ifndef NDEBUG
      /* TODO */
      class TSyncException
          : public std::exception {
        public:

        /* TODO */
        virtual const char *what() const throw() {
          return "IncrLockCount exception: expression: (LockCount == 0 || !is_exclusive || IsExclusive) is false; should be true";
        }

      };  // TSyncException
      #endif

    }  // FluxCapacitor

  }  // Spa

}  // Stig
