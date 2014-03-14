/* <base/thread_local_global_pool.h>

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
#include <condition_variable>
#include <mutex>
#include <stdexcept>
#include <vector>

#include <syslog.h>

#include <base/assert_true.h>
#include <base/likely.h>
#include <base/no_copy_semantics.h>
#include <base/spin_lock.h>
#include <inv_con/atomic_unordered_list.h>

namespace Base {

  /* TODO */
  template <typename TObj, typename... TArgs>
  class TThreadLocalGlobalPoolManager {
    NO_COPY_SEMANTICS(TThreadLocalGlobalPoolManager);
    public:

    /* Forward Declaration. */
    class TThreadLocalPool;

    /* TODO */
    class TObjBase {
      NO_COPY_SEMANTICS(TObjBase);
      public:

      protected:

      /* TODO */
      TObjBase()
          : NextObj(nullptr) {}

      private:

      /* TODO */
      TObjBase *NextObj;

      friend class TThreadLocalPool;
      friend class TThreadLocalGlobalPoolManager;

    };  // TObjBase

    /* TODO */
    class TThreadLocalPool {
      NO_COPY_SEMANTICS(TThreadLocalPool);
      public:

      /* TODO */
      static_assert(std::is_base_of<TObjBase, TObj>::value, "TThreadLocalPool requires TObj to be derived from TObjBase");

      /* TODO */
      typedef InvCon::AtomicUnorderedList::TMembership<TThreadLocalPool, TThreadLocalGlobalPoolManager> TManagerMembership;

      /* TODO */
      TThreadLocalPool(TThreadLocalGlobalPoolManager *manager)
          : FreeQueue(nullptr), AvailableQueue(nullptr), Manager(manager), ManagerMembership(this, &Base::AssertTrue(manager)->PoolCollection) {
        assert(manager);
      }

      /* TODO */
      virtual ~TThreadLocalPool() {}

      /* TODO */
      inline void Free(TObjBase *obj) {
        assert(this);
        #ifndef NDEBUG
        static_cast<TObj *>(obj)->AssertCanFree();
        #endif
        do {
          obj->NextObj = FreeQueue;
        } while (!__sync_bool_compare_and_swap(&FreeQueue, obj->NextObj, obj));
      }

      /* TODO */
      inline TObj *Alloc() {
        assert(this);
        TObjBase *alloc_obj = AvailableQueue;
        if (alloc_obj) {
          /* Our available queue had something to offer. */
          AvailableQueue = alloc_obj->NextObj;
        } else {
          alloc_obj = TryAllocUncommon();
          if (!alloc_obj) {
            syslog(LOG_ERR, "Bad Alloc in TThreadLocalPool");
            throw std::bad_alloc();
          }
        }
        return static_cast<TObj *>(alloc_obj);
      }

      /* TODO */
      inline TThreadLocalGlobalPoolManager *GetPoolManager() const {
        assert(this);
        return Manager;
      }

      private:

      /* TODO */
      TObjBase *TryAllocUncommon() {
        TObjBase *alloc_obj = nullptr;
        assert(this);
        /* let's swap in our free queue and try to allocate from that. */
        TObjBase *cur_tail = __sync_lock_test_and_set(&FreeQueue, nullptr);
        if (cur_tail) {
          /* There were element(s) on the free queue. */
          assert(AvailableQueue == nullptr);
          MakeStackAvailable(cur_tail);
          assert(AvailableQueue != nullptr);
          alloc_obj = AvailableQueue;
          AvailableQueue = alloc_obj->NextObj;

        } else {
          /* the free queue was empty as well. Time to borrow from the global pool */
          alloc_obj = Manager->TryAlloc();
          if (!alloc_obj) {
            /* the global pool was empty as well. Time to try to borrow from other pools...
               - We want to borrow starting from our right neighbor.
               - Until AtomicUnorderedList supports a "ForEachFromThis" we will use it (possibly) twice to scan right from our current member
            */

            bool skip_to = true;
            bool reached = false;
            auto for_each_cb = [this, &cur_tail, &alloc_obj, &skip_to, &reached](const TThreadLocalPool &pool) {
              if (skip_to) {
                /* we skip until we reach this */
                if (!reached && &pool != this) {
                  return true;
                } else {
                  reached = true;
                }
              } else {
                /* we pass through until we reach this */
                if (&pool == this) {
                  return false;
                }
              }
              if (&pool != this) {
                cur_tail = __sync_lock_test_and_set(&pool.FreeQueue, nullptr);
                if (cur_tail) {
                  /* There were element(s) on the other pool's free queue. */
                  assert(AvailableQueue == nullptr);
                  MakeStackAvailable(cur_tail);
                  assert(AvailableQueue != nullptr);
                  alloc_obj = AvailableQueue;
                  AvailableQueue = alloc_obj->NextObj;
                  return false;
                }
              }
              return true;
            };
            Manager->PoolCollection.ForEach(for_each_cb);
            if (!alloc_obj) {
              /* we looked at all the neigbors to our right. Now we start from the left and keep going till we see oursleves. */
              skip_to = false;
              Manager->PoolCollection.ForEach(for_each_cb);
            }
          }
        }
        return alloc_obj;
      }

      /* TODO */
      inline void MakeStackAvailable(TObjBase *cur_tail) {
        assert(this);
        /* fast pop */
        AvailableQueue = cur_tail;
        /* reverse pop (circular queue) */
        #if 0
        TObjBase *new_head = nullptr;
        while (cur_tail) {
          TObjBase *next = cur_tail->NextObj;
          cur_tail->NextObj = new_head;
          new_head = cur_tail;
          cur_tail = next;
        }
        AvailableQueue = new_head;
        #endif
      }

      /* TODO */
      mutable TObjBase *FreeQueue;

      /* TODO */
      TObjBase *AvailableQueue;

      /* TODO */
      TThreadLocalGlobalPoolManager *Manager;

      /* TODO */
      TObj *MyAlloc;

      /* TODO */
      typename TManagerMembership::TImpl ManagerMembership;

    };  // TThreadLocalPool

    /* TODO */
    TThreadLocalGlobalPoolManager(size_t num_elems, const TArgs &... args)
        : NumElems(num_elems), Alloc(nullptr), PoolCollection(this) {
      Alloc = reinterpret_cast<TObj *>(malloc(sizeof(TObj) * num_elems));
      if (unlikely(!Alloc)) {
        throw std::bad_alloc();
      }
      TObjBase *prev_base = nullptr;
      for (size_t i = 0; i < num_elems; ++i) {
        TObj *const obj = new (Alloc + i) TObj(args...);
        TObjBase *const obj_base = static_cast<TObjBase *>(obj);
        obj_base->NextObj = prev_base;
        prev_base = obj_base;
      }
      FreeQueue = prev_base;
    }

    /* TODO */
    ~TThreadLocalGlobalPoolManager() {
      assert(this);
      size_t counter = 0UL;
      PoolCollection.ForEach([&counter](const TThreadLocalPool &){++counter; return true;});
      if (counter != 0UL) {
        throw std::logic_error("TThreadLocalGlobalPoolManager should not be destroyed while owning any pools");
      }
      for (size_t i = 0; i < NumElems; ++i) {
        TObj *const obj = Alloc + i;
        obj->~TObj();
      }
      free(Alloc);
    }

    private:

    /* TODO */
    inline TObjBase *TryAlloc() {
      assert(this);
      TObjBase *obj = nullptr;
      /* for now we use a spin lock. Anyone allocating from here will contend for this. The idea is that we finely distribute objects to the thread
         local pools. Once they are there they can be allocated without a lock. We will hit this function often at the early life of a local pool
         and then less often later. Consider it a warm-up semantic. */
      Base::TSpinLock::TLock lock(FreeQueueLock);
      obj = FreeQueue;
      if (obj) {
        FreeQueue = obj->NextObj;
      }
      return obj;
    }

    /* TODO */
    typedef InvCon::AtomicUnorderedList::TCollection<TThreadLocalGlobalPoolManager, TThreadLocalPool> TPoolCollection;

    /* TODO */
    const size_t NumElems;

    /* TODO */
    TObj *Alloc;

    /* TODO */
    Base::TSpinLock FreeQueueLock;
    TObjBase *FreeQueue;

    /* TODO */
    mutable typename TPoolCollection::TImpl PoolCollection;

  };  // TThreadLocalGlobalPoolManager

}  // Base
