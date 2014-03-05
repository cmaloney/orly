/* <base/thread_local_registered_pool.h>

   Provides a way to manage thread local pools of an object. The pools are able to borrow objects from eachother when their pool runs low. This is
   done through a pool manager that holds together a collection of thread local pools.
   Characteristics:
      - Very fast allocation. There is no synchronization as only the thread to which the pool belongs to is allowed to allocate from it.
      - (Almost) very fast de-allocation. There is a CAS (Compare and Swap) which pushes the free'd element onto the thread local stack.
      - When the no-sync queue runs out, there is a single CAS to refill this queue from the local free queue.
      - When the no-sync queue as well as the local free pool run out, we iterate over the other pools that were registered and try to borrow from
        their free pool.
      - Not optimal use of pre-allocated space. When a thread's pool is empty, it tries to borrow from a neighbor. Only elements in the free queue are
        elligible for loaning, so it is possible to get a std::bad_alloc error when there were still free elements in other thread(s)' no-sync
        (available) queue(s).
      - The space backing the individual pools is not free'd until the pool manager goes out of scope.
   Rules:
      - Allocation of an object must always come from the same thread as the thread local.

   NOTE: TODO

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

#include <base/no_copy_semantics.h>
#include <base/likely.h>
#include <inv_con/atomic_unordered_list.h>

namespace Base {

  /* TODO */
  template <typename TObj, typename... TArgs>
  class TThreadLocalPoolManager {
    NO_COPY_SEMANTICS(TThreadLocalPoolManager);
    public:

    /* Forward Declaration. */
    class TThreadLocalRegisteredPool;

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

      friend class TThreadLocalRegisteredPool;

    };  // TObjBase

    /* TODO */
    class TThreadLocalRegisteredPool {
      NO_COPY_SEMANTICS(TThreadLocalRegisteredPool);
      public:

      /* TODO */
      static_assert(std::is_base_of<TObjBase, TObj>::value, "TThreadLocalRegisteredPool requires TObj to be derived from TObjBase");

      /* TODO */
      typedef InvCon::AtomicUnorderedList::TMembership<TThreadLocalRegisteredPool, TThreadLocalPoolManager> TManagerMembership;

      /* TODO */
      TThreadLocalRegisteredPool(TThreadLocalPoolManager *manager, size_t num_elems, const TArgs &... args)
          : FreeQueue(nullptr), AvailableQueue(nullptr), Manager(manager), MyAlloc(nullptr), ManagerMembership(this, &manager->PoolCollection) {
        assert(num_elems > 0);
        assert(manager);
        TObjBase *prev_base = nullptr;
        MyAlloc = Manager->AllocateSpace(num_elems);
        for (size_t i = 0; i < num_elems; ++i) {
          TObj *const obj = new (MyAlloc + i) TObj(args...);
          TObjBase *const obj_base = static_cast<TObjBase *>(obj);
          obj_base->NextObj = prev_base;
          prev_base = obj_base;
        }
        AvailableQueue = prev_base;
      }

      /* TODO */
      virtual ~TThreadLocalRegisteredPool() {}

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
            syslog(LOG_ERR, "Bad Alloc in ThreadLocalRegisteredPool");
            throw std::bad_alloc();
          }
        }
        return static_cast<TObj *>(alloc_obj);
      }

      /* TODO */
      inline TThreadLocalPoolManager *GetPoolManager() const {
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
          /* the free queue was empty as well. Time to try to borrow from other pools...
             - We want to borrow starting from our right neighbor.
             - Until AtomicUnorderedList supports a "ForEachFromThis" we will use it (possibly) twice to scan right from our current member
          */

          bool skip_to = true;
          bool reached = false;
          auto for_each_cb = [this, &cur_tail, &alloc_obj, &skip_to, &reached](const TThreadLocalRegisteredPool &pool) {
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
      TObjBase * AvailableQueue;

      /* TODO */
      TThreadLocalPoolManager *Manager;

      /* TODO */
      TObj *MyAlloc;

      /* TODO */
      typename TManagerMembership::TImpl ManagerMembership;

    };  // TThreadLocalRegisteredPool

    /* TODO */
    TThreadLocalPoolManager()
        : PoolCollection(this) {}

    /* TODO */
    ~TThreadLocalPoolManager() {
      assert(this);
      size_t counter = 0UL;
      PoolCollection.ForEach([&counter](const TThreadLocalRegisteredPool &){++counter; return true;});
      if (counter != 0UL) {
        throw std::logic_error("TThreadLocalPoolManager should not be destroyed while owning any pools");
      }
      for (auto alloc_pair : AllocVec) {
        for (size_t i = 0; i < alloc_pair.second; ++i) {
          TObj *const obj = alloc_pair.first + i;
          obj->~TObj();
        }
        free(alloc_pair.first);
      }
    }

    private:

    /* TODO */
    typedef InvCon::AtomicUnorderedList::TCollection<TThreadLocalPoolManager, TThreadLocalRegisteredPool> TPoolCollection;

    /* TODO */
    TObj *AllocateSpace(size_t num_elems) {
      TObj *alloc = reinterpret_cast<TObj *>(malloc(sizeof(TObj) * num_elems));
      if (unlikely(!alloc)) {
        throw std::bad_alloc();
      }
      std::lock_guard<std::mutex> lock(AllocateMutex);
      AllocVec.emplace_back(alloc, num_elems);
      return alloc;
    }

    /* TODO */
    std::mutex AllocateMutex;

    /* TODO */
    std::vector<std::pair<TObj *, size_t>> AllocVec;

    /* TODO */
    mutable typename TPoolCollection::TImpl PoolCollection;

  };  // ThreadLocalPoolManager

}  // Base