/* <stig/indy/disk/util/cache.h>

   TODO

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

#include <sched.h>
#include <sys/mman.h>
#include <unistd.h>

#include <atomic>
#include <stdexcept>
#include <iostream> /* TODO: GET RID OF */

#include <base/error_utils.h>
#include <base/likely.h>
#include <base/no_copy_semantics.h>
#include <inv_con/atomic_unordered_list.h>
#include <stig/indy/disk/util/hash_util.h>
#include <stig/indy/disk/util/volume_manager.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      namespace Util {

        /* TODO */
        template <size_t PageSize>
        class TCache {
          NO_COPY_SEMANTICS(TCache);
          private:

          /* TODO */
          class TLRU;

          public:

          static constexpr size_t DataSize = PageSize;

          /* Used to remove the 3 high order bits of a BufAddr */
          static constexpr size_t Highest3Bits = (7UL << 61);
          static constexpr size_t All1But3HighestBits = ~Highest3Bits;
          static constexpr size_t HighestBit = 1UL << 63;
          static constexpr size_t SecondHighestBit = 1UL << 62;
          static constexpr size_t ThirdHighestBit = 1UL << 61;

          /* TODO */
          static constexpr size_t LockedSlot = -1;
          static constexpr size_t EmptySlot = -2;
          static constexpr size_t DummyStartSlot = -3;

          /* TODO */
          struct TSlot {

            typedef InvCon::AtomicUnorderedList::TMembership<TSlot, TLRU> TLRUMembership;

            /* TODO */
            TSlot()
                : PageId(EmptySlot), RefCount(0UL), BufAddr(0UL), LRUMembership(this), NextSlot(nullptr), MainSlot(nullptr) {}

            /* This function assumes the data in the cache is already loaded! */
            inline const char *KnownGetData(TCache *cache) const {
              assert(this);
              const size_t val = std::atomic_load(&BufAddr);
              assert((val & HighestBit) == HighestBit);
              assert((val & ThirdHighestBit) == 0UL);
              const size_t page_offset = val & All1But3HighestBits;
              return cache->PageData + (page_offset * PageSize);
            }

            /* TODO */
            inline void AsyncGetData(const Base::TCodeLocation &code_location,
                                     DiskPriority priority,
                                     TCache *cache,
                                     TSlot *main_slot,
                                     TBufKind buf_kind,
                                     uint8_t util_src,
                                     size_t page_id,
                                     bool can_release,
                                     TCompletionTrigger &async_trigger) {
              for (;;) {
                size_t val = std::atomic_load(&BufAddr);
                if ((val & HighestBit) == HighestBit) {
                  /* if the highest bit is set, then we know the data is already loaded. */
                  break;
                } else {
                  /* if the highest bit is not set, then we use the second highest bit to see if someone is already loading this data. */
                  if ((val & SecondHighestBit) == SecondHighestBit) {
                    /* second highest bit is set, someone is loading the data, we can return. */
                    break;
                  } else {
                    /* second highest bit is not set, let's try to set it and load the data if we're successful */
                    size_t new_val = val | SecondHighestBit;
                    if (!std::atomic_compare_exchange_strong(&BufAddr, &val, new_val)) {
                      /* the value has changed since we read it. this means we can continue in the loop and check it again. */
                      continue;
                    }
                    /* we successfully swapped in the second highest bit. this means we need to load the data and then set the highest bit once it's valid. */
                    const size_t page_offset = val & All1But3HighestBits;
                    const size_t logical_offset = page_id * PageSize;
                    char *buf = cache->PageData + (page_offset * PageSize);

                    TSlot *this_slot = this; /* TODO: have to do this right now because it segfaults (ICE) gcc 4.7.1 if you capture [this] */
                    TCompletionTrigger *trigger_ptr = &async_trigger;
                    cache->VolumeManager->Read(code_location,
                                               buf_kind,
                                               util_src,
                                               buf,
                                               logical_offset,
                                               PageSize,
                                               priority,
                                               async_trigger,
                                               [this_slot, main_slot, can_release, new_val, trigger_ptr, cache, page_id, code_location](TDiskResult result, const char *err_str) {
                      if (result == TDiskResult::Success) {
                        size_t val = new_val | HighestBit;
                        std::atomic_store(&(this_slot->BufAddr), val);
                        trigger_ptr->Callback(result, err_str);
                      } else {
                        size_t val = new_val | HighestBit;
                        val |= ThirdHighestBit; /* set the error bit */
                        std::atomic_store(&(this_slot->BufAddr), val);
                      }
                      if (can_release) {
                        cache->Release(main_slot, page_id);
                      }
                    });
                    return; /* exit so we don't release twice. */
                  }
                }
              }
              if (can_release) {
                cache->Release(main_slot, page_id);
              }
            }

            /* TODO */
            inline const char *SyncGetData(const Base::TCodeLocation &code_location,
                                           DiskPriority priority,
                                           TCache *cache,
                                           TBufKind buf_kind,
                                           uint8_t util_src,
                                           size_t page_id,
                                           TCompletionTrigger &trigger) const {
              assert(this);
              for (;;) {
                size_t val = std::atomic_load(&BufAddr);
                if ((val & HighestBit) == HighestBit) {
                  /* if the highest bit is set, then we can just return the buffer after checking for an error. */
                  if ((val & ThirdHighestBit) == 0) {
                    const size_t page_offset = val & All1But3HighestBits;
                    return cache->PageData + (page_offset * PageSize);
                  } else {
                    std::cerr << "Disk page loaded with error [" << val << "]" << std::endl;
                    throw std::logic_error("Disk page was loaded with error.");
                  }
                } else {
                  /* if the highest bit is not set, then we use the second highest bit to see if someone is already loading this data. */
                  if ((val & SecondHighestBit) == SecondHighestBit) {
                    /* second highest bit is set, someone is loading the data, we just have to wait. */
                    //nanosleep(&(cache->ReadWait), NULL);
                    /* TODO: we should try to join some form of queue of frames that will get re-activated when the frame performing the read on our
                       behalf is finished. */
                    Fiber::YieldSlow();
                    continue;
                  } else {
                    /* second highest bit is not set, let's try to set it and load the data if we're successful */
                    size_t new_val = val | SecondHighestBit;
                    if (!std::atomic_compare_exchange_strong(&BufAddr, &val, new_val)) {
                      /* the value has changed since we read it. this means we can continue in the loop and check it again. */
                      continue;
                    }
                    /* we successfully swapped in the second highest bit. this means we need to load the data and then set the highest bit once it's valid. */
                    const size_t page_offset = val & All1But3HighestBits;
                    const size_t logical_offset = page_id * PageSize;
                    char *buf = cache->PageData + (page_offset * PageSize);
                    cache->VolumeManager->Read(code_location, buf_kind, util_src, buf, logical_offset, PageSize, priority, trigger);
                    try {
                      trigger.Wait(true);
                      val = new_val | HighestBit;
                      std::atomic_store(&BufAddr, val);
                      return buf;
                    } catch (const std::exception &ex) {
                      val = new_val | HighestBit;
                      val |= ThirdHighestBit; /* set the error bit */
                      std::atomic_store(&BufAddr, val);
                      throw;
                    }
                  }
                }
              }
              throw std::logic_error("Cannot be reached");
            }

            private:

            /* TODO */
            mutable std::atomic<size_t> PageId;
            size_t RefCount;
            /* BufAddr represents the offset of the page from the start of the cache's in a multiple of the page size:
               eg. offset 4 would be the 4th page into the cache's data buffer, or start + (4 * pagesize)
               - the highest order bit represents whether data has finished loading
               - the second highest order bit represents whether someone is in the process of loading the data
               - the third highest order bit represents whether the page loaded with an error (eg. corrupt check) */
            mutable std::atomic<size_t> BufAddr;
            typename TLRUMembership::TImpl LRUMembership;
            TSlot *NextSlot;
            TSlot *MainSlot;

            /* TODO */
            friend class TCache;

          };  // TSlot

          /* TODO */
          TCache(TVolumeManager *volume_manager,
                 size_t max_cache_size,
                 size_t num_lru)
              : VolumeManager(volume_manager),
                MaxCacheSize(max_cache_size),
                NumSlots(SuggestHashSize(MaxCacheSize)),
                NumLRU(num_lru),
                SlotArray(nullptr),
                LRUArray(nullptr),
                PageData(nullptr) {
            ReadWait.tv_sec = 0;
            ReadWait.tv_nsec = 25000L;
            SlotArray = new TSlot[NumSlots];
            try {
              LRUArray = new TLRU[NumLRU];
              try {
                assert(PageSize >= static_cast<size_t>(getpagesize()));
                assert(PageSize % getpagesize() == 0);
                Base::IfLt0(posix_memalign(reinterpret_cast<void **>(&PageData), PageSize, PageSize * MaxCacheSize));
                Base::IfLt0(mlock(PageData, PageSize * MaxCacheSize));
                #ifndef NDEBUG
                memset(PageData, 0, PageSize * MaxCacheSize);
                #endif
                try {
                  /* we're going to init max_cache_size slots with DummyStartSlot with a valid
                     BuffAddr and put them in the LRU so they can be reclaimed using a unified strategy */
                  for (size_t i = 0; i < MaxCacheSize; ++i) {
                    TSlot &slot = SlotArray[i];
                    std::atomic_store(&slot.PageId, DummyStartSlot);
                    std::atomic_store(&slot.BufAddr, i);
                    TLRU &lru = LRUArray[i % NumLRU];
                    lru.SlotCollection.Insert(&slot.LRUMembership);
                    #ifdef PERF_STATS
                    ++lru.NumBufInLRU;
                    #endif
                  }
                  TryRemoveSlotFunc = std::bind(&TCache::TryRemoveSlot, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
                } catch (...) {
                  free(PageData);
                  throw;
                }
              } catch (...) {
                delete LRUArray;
                LRUArray = nullptr;
                throw;
              }
            } catch (...) {
              delete SlotArray;
              SlotArray = nullptr;
              throw;
            }
          }

          /* TODO */
          ~TCache() {
            assert(this);
            free(PageData);
            delete[] LRUArray;
            delete[] SlotArray;
          }

          /* STATS */
          #ifdef PERF_STATS
          inline size_t GetMaxCacheSize() const {
            assert(this);
            return MaxCacheSize;
          }
          #endif

          /* STATS */
          #ifdef PERF_STATS
          inline size_t CountNumBufInLRU() const {
            assert(this);
            size_t total = 0UL;
            for (size_t i = 0; i < NumLRU; ++i) {
              total += std::atomic_load(&(LRUArray[i].NumBufInLRU));
            }
            return total;
          }
          #endif

          /* TODO */
          inline void PreGet(size_t page_id) {
            const size_t slot_num = page_id % NumSlots;
            TSlot *const my_slot = SlotArray + slot_num;
            _mm_prefetch(my_slot, _MM_HINT_T0);
            _mm_prefetch(reinterpret_cast<uint8_t *>(my_slot) + sizeof(TSlot), _MM_HINT_T0);
          }

          /* Returns a pointer to the main slot object. Initializes the slot if it does not exist yet. Increments the reference count on the actual slot holding our page. */
          inline TSlot *Get(size_t page_id, TSlot *&data_slot) {
            assert(this);
            const size_t slot_num = page_id % NumSlots;
            TSlot &slot = SlotArray[slot_num];
            _mm_prefetch(&slot, _MM_HINT_T0);
            _mm_prefetch(reinterpret_cast<uint8_t *>(&slot) + sizeof(TSlot), _MM_HINT_T0);
            std::atomic<size_t> *const cur_slot = &(slot.PageId);
            for (;;) {
              size_t val = std::atomic_load(cur_slot);
              if (val == LockedSlot) {
                continue;
              } else if (val == EmptySlot) {
                /* TRY transition from EmptySlot -> LockedSlot */
                size_t expected = EmptySlot;
                if (!std::atomic_compare_exchange_strong(cur_slot, &expected, LockedSlot)) {
                  continue;
                }
                /* now we hold a lock on the slot that was previously empty:
                   - allocate a page buf for it
                   - set the page buf address
                   - increment the reference count
                   - transition from LockedSlot -> page_id
                */
                try {
                  slot.BufAddr = NewPageBuf();
                } catch (...) {
                  /* we need to unwind. this means:
                     - transition from LockedSlot -> EmptySlot
                     - rethrow
                  */
                  std::atomic_store(cur_slot, EmptySlot);
                  throw;
                }
                ++slot.RefCount;
                assert(slot.RefCount == 1);
                assert(slot.NextSlot == nullptr);
                std::atomic_store(cur_slot, page_id);
                data_slot = &slot;
                return &slot;
              } else {
                /* this slot is being used. let's grab a lock on it. */
                /* TRY transition from val -> LockedSlot */
                if (!std::atomic_compare_exchange_strong(cur_slot, &val, LockedSlot)) {
                  continue;
                }
                /* now we hold a lock on the slot that was previously val. */
                if (val == page_id) {
                  /* this slot is the page we're interested in, and we've got it locked. now:
                     - increment the reference count
                     - if the previous reference count was 0, remove from LRU
                     - transition from LockedSlot -> page_id
                  */
                  const size_t prev_ref_count = slot.RefCount++;
                  assert(prev_ref_count == slot.RefCount - 1);
                  if (prev_ref_count == 0) {
                    assert(slot.LRUMembership.TryGetCollector());
                    #ifdef PERF_STATS
                    --(slot.LRUMembership.TryGetCollector()->NumBufInLRU);
                    #endif
                    slot.LRUMembership.Remove();
                  }
                  std::atomic_store(cur_slot, val);
                  data_slot = &slot;
                  return &slot;
                } else {
                  /* this slot is a different page. we're going to have to look down the chain, if there is one. */
                  TSlot *prev_slot = &slot;
                  TSlot *next_slot = slot.NextSlot;
                  for (; next_slot; next_slot = next_slot->NextSlot) {
                    prev_slot = next_slot;
                    const size_t this_page_id = std::atomic_load(&(next_slot->PageId));
                    if (this_page_id == page_id) {
                      /* The page we're looking for exists. It's a member in the chain. now:
                         - increment the reference count
                         - if the previous reference count was 0, remove from LRU
                         - transition from LockedSlot -> val (the page_id of the first entry in the chain)
                         - return a pointer to the first slot in the chain.
                      */
                      const size_t prev_ref_count = (next_slot->RefCount)++;
                      assert(prev_ref_count == (next_slot->RefCount) - 1);
                      if (prev_ref_count == 0) {
                        assert(next_slot->LRUMembership.TryGetCollector());
                        #ifdef PERF_STATS
                        --(next_slot->LRUMembership.TryGetCollector()->NumBufInLRU);
                        #endif
                        next_slot->LRUMembership.Remove();
                      }
                      std::atomic_store(cur_slot, val);
                      data_slot = next_slot;
                      return &slot;
                    }
                  }
                  /* if this main slot is empty, let's just replace it... */
                  if (slot.RefCount == 0) {
                    /* remove the high order bits from the reclaimed page. */
                    const size_t new_buf_addr = std::atomic_load(&slot.BufAddr) & All1But3HighestBits;
                    std::atomic_store(&slot.BufAddr, new_buf_addr);
                    ++slot.RefCount;
                    assert(slot.LRUMembership.TryGetCollector());
                    #ifdef PERF_STATS
                    --(slot.LRUMembership.TryGetCollector()->NumBufInLRU);
                    #endif
                    slot.LRUMembership.Remove();
                    std::atomic_store(cur_slot, page_id);
                    data_slot = &slot;
                    return &slot;
                  }
                  /* if we get here, it means none of the slots in the chain are the page_id we're looking for. now:
                     - allocate a TSlot for it
                     - set the MainSlot ptr to the beginning of the chain
                     - allocate a page buf for it
                     - set the page buf address
                     - set the page id
                     - increment the reference count
                     - add the new TSlot to the chain
                     - transition from LockedSlot -> val (the page_id of the first entry in the chain)
                  */
                  TSlot *new_slot = nullptr;
                  try {
                    new_slot = NewSlot();
                    assert(new_slot->NextSlot == nullptr);
                    new_slot->MainSlot = &slot;
                    try {
                      new_slot->BufAddr = NewPageBuf();
                      std::atomic_store(&(new_slot->PageId), page_id);
                      /* the following should be no-throws... */
                      ++(new_slot->RefCount);
                      prev_slot->NextSlot = new_slot;
                    } catch (...) {
                      DeleteSlot(new_slot);
                      throw;
                    }
                  } catch (...) {
                    /* we need to unwind. this means:
                       - transition from LockedSlot -> val (the page_id of the first entry in the chain)
                       - rethrow
                    */
                    std::atomic_store(cur_slot, val);
                    throw;
                  }
                  std::atomic_store(cur_slot, val);
                  data_slot = new_slot;
                  return &slot;
                }
              }
            }
          }

          /* Decrements the reference count on the given page_id for this slot. */
          inline void Release(TSlot *slot_ptr, size_t page_id) {
            assert(this);
            assert(&SlotArray[page_id % NumSlots] == slot_ptr);
            _mm_prefetch(slot_ptr, _MM_HINT_T0);
            _mm_prefetch(reinterpret_cast<uint8_t *>(slot_ptr) + sizeof(TSlot), _MM_HINT_T0);
            TSlot &slot = *slot_ptr;
            std::atomic<size_t> *const cur_slot = &(slot.PageId);
            /* lock the slot */
            for (;;) {
              size_t val = std::atomic_load(cur_slot);
              assert(val != EmptySlot);
              if (val == LockedSlot) {
                continue;
              } else {
                /* TRY transition from val -> LockedSlot */
                if (!std::atomic_compare_exchange_strong(cur_slot, &val, LockedSlot)) {
                  continue;
                }
                /* we have a lock on this slot. now:
                   - find the slot in the chain that has our page id.
                   - decrement the ref_count                                                        .
                   - if we decrement to 0 then add to an LRU                                                                               .
                   - transition from LockedSlot -> val (the page_id of the first entry in the chain).
                */
                if (val == page_id) {
                  /* this is the most common case. The first entry in the chain is the one that matches our page_id. */
                  assert(slot.RefCount > 0);
                  const size_t new_ref_count = --slot.RefCount;
                  assert(new_ref_count == slot.RefCount);
                  if (new_ref_count == 0UL) {
                    TLRU &lru = LRUArray[sched_getcpu() % NumLRU];
                    assert(!slot.LRUMembership.TryGetCollector());
                    lru.SlotCollection.Insert(&slot.LRUMembership);
                    #ifdef PERF_STATS
                    ++lru.NumBufInLRU;
                    #endif
                  }
                  std::atomic_store(cur_slot, val);
                  return;
                } else {
                  /* we need to look down the chain for the slot that matches our page id. */
                  TSlot *next_slot = slot.NextSlot;
                  assert(next_slot);
                  for (; next_slot; next_slot = next_slot->NextSlot) {
                    const size_t this_page_id = std::atomic_load(&(next_slot->PageId));
                    if (this_page_id == page_id) {
                      assert(next_slot->RefCount > 0);

                      const size_t new_ref_count = --(next_slot->RefCount);
                      assert(new_ref_count == (next_slot->RefCount));
                      if (new_ref_count == 0UL) {
                        TLRU &lru = LRUArray[sched_getcpu() % NumLRU];
                        assert(!next_slot->LRUMembership.TryGetCollector());
                        lru.SlotCollection.Insert(&next_slot->LRUMembership);
                        #ifdef PERF_STATS
                        ++lru.NumBufInLRU;
                        #endif
                      }
                      std::atomic_store(cur_slot, val);
                      return;
                    }
                  }
                  std::cerr << "Cannot release slot - page_id combination that does not exist in cache. [" << page_id << "]" << std::endl;
                  throw std::logic_error("Cannot release slot - page_id combination that does not exist in cache.");
                }

              }

            }
          }

          /* TODO */
          inline void Clear(size_t page_id) {
            assert(this);
            const size_t slot_num = page_id % NumSlots;
            TSlot &slot = SlotArray[slot_num];
            _mm_prefetch(&slot, _MM_HINT_T0);
            _mm_prefetch(reinterpret_cast<uint8_t *>(&slot) + sizeof(TSlot), _MM_HINT_T0);
            std::atomic<size_t> *const cur_slot = &(slot.PageId);
            for (;;) {
              size_t val = std::atomic_load(cur_slot);
              if (val == LockedSlot) {
                continue;
              } else if (val == EmptySlot) {
                /* It's already empty, perfect, we're done! */
                return;
              } else {
                /* this slot is being used. let's grab a lock on it. */
                /* TRY transition from val -> LockedSlot */
                if (!std::atomic_compare_exchange_strong(cur_slot, &val, LockedSlot)) {
                  continue;
                }
                /* now we hold a lock on the slot that was previously val. */
                if (val == page_id) {
                  /* this slot is the page we're interested in, and we've got it locked. now:
                     - assure the reference count is 0
                     - if we just set the BufAddr to unread ... then we're forcing someone new who comes
                       in to go back to the disk, which is what we want.
                     - transition from LockedSlot -> page_id
                  */
                  assert(slot.RefCount == 0UL);
                  size_t cur_addr = std::atomic_load(&slot.BufAddr);
                  if ((cur_addr & Highest3Bits) != 0) {
                    size_t new_addr = cur_addr & All1But3HighestBits;
                    if (!std::atomic_compare_exchange_strong(&slot.BufAddr, &cur_addr, new_addr)) {
                      /* since the reference count is 0 and we've got this whole slot chain locked, no one should be changing this value on us... */
                      throw;
                    }
                  }
                  std::atomic_store(cur_slot, val);
                  return;
                } else {
                  /* this slot is a different page. we're going to have to look down the chain, if there is one. */
                  TSlot *next_slot = slot.NextSlot;
                  for (; next_slot; next_slot = next_slot->NextSlot) {
                    const size_t this_page_id = std::atomic_load(&(next_slot->PageId));
                    if (this_page_id == page_id) {
                      /* The page we're looking for exists. It's a member in the chain. now:
                         - assure the reference count is 0
                         - if we just set the BufAddr to unread ... then we're forcing someone new who comes
                           in to go back to the disk, which is what we want.
                         - transition from LockedSlot -> val (the page_id of the first entry in the chain)
                      */
                      assert(next_slot->RefCount == 0);
                      size_t cur_addr = std::atomic_load(&(next_slot->BufAddr));
                      if ((cur_addr & Highest3Bits) != 0) {
                        size_t new_addr = cur_addr & All1But3HighestBits;
                        if (!std::atomic_compare_exchange_strong(&(next_slot->BufAddr), &cur_addr, new_addr)) {
                          /* since the reference count is 0 and we've got this whole slot chain locked, no one should be changing this value on us... */
                          throw;
                        }
                      }
                      std::atomic_store(cur_slot, val);
                      return;
                    }
                  }
                  /* if we get here then that means the slot doesn't exist, so it's already cleared :-) */
                  std::atomic_store(cur_slot, val);
                  return;
                }
              }
            }
          }

          /* TODO */
          inline void Replace(size_t page_id, void *buf) {
            TSlot *data_slot;
            TSlot *main_slot = Get(page_id, data_slot);
            /* assert that we are the only one referencing this block (refcount == 1) */
            assert(data_slot->RefCount == 1);
            /* copy the data from buf into the address referenced by BufAddr and set it as "successfully read" */
            size_t cur_addr = std::atomic_load(&(data_slot->BufAddr));
            const size_t page_offset = cur_addr & All1But3HighestBits;
            char *data = PageData + (page_offset * PageSize);
            memcpy(data, buf, PageSize);
            if (!std::atomic_compare_exchange_strong(&(data_slot->BufAddr), &cur_addr, (cur_addr & All1But3HighestBits) | HighestBit)) {
              /* since the reference count is 1 and we're the one modifying this page, no one should be changing this value from under us... */
              throw;
            }
            Release(main_slot, page_id);
          }

          /* TODO */
          inline bool AssertNoRefCount(size_t page_id) {
            TSlot *data_slot;
            TSlot *main_slot = Get(page_id, data_slot);
            bool ret = data_slot->RefCount == 1;
            Release(main_slot, page_id);
            if (!ret) {
              syslog(LOG_EMERG,"Page [%ld] has ref count with page size = [%ld]", page_id, PageSize);
            }
            return ret;
          }

          /* TODO */
          inline void AsyncMultiGet(const Base::TCodeLocation &code_location,
                                    DiskPriority priority,
                                    TCache *cache,
                                    TBufKind buf_kind,
                                    uint8_t util_src,
                                    size_t page_id,
                                    size_t num_consec_pages,
                                    bool can_release,
                                    TCompletionTrigger &async_trigger) {
            assert(this);
            TSlot *main_slots[num_consec_pages];
            TSlot *data_slots[num_consec_pages];
            for (size_t i = 0; i < num_consec_pages; ++i) {
              main_slots[i] = Get(page_id + i, data_slots[i]);
            }

            auto load_range_func = [&data_slots, &main_slots, page_id, cache, &async_trigger, &code_location, buf_kind, util_src, can_release, priority](size_t from_page_id, size_t num_pages) {
              //std::cout << "Load Range [" << from_page_id << "] for " << num_pages << std::endl;
              //#if 0
              void *buf_array[num_pages];
              const size_t logical_offset = from_page_id * PageSize;
              std::vector<TSlot *> my_main_slots(num_pages);
              std::vector<TSlot *> my_data_slots(num_pages);
              for (size_t i = 0; i < num_pages; ++i) {
                const size_t this_page_id = from_page_id + i;
                my_main_slots[i] = main_slots[this_page_id - page_id];
                my_data_slots[i] = data_slots[this_page_id - page_id];
                TSlot *const data_slot = my_data_slots[i];
                size_t new_val = std::atomic_load(&(data_slot->BufAddr));
                const size_t page_offset = new_val & All1But3HighestBits;
                char *const buf = cache->PageData + (page_offset * PageSize);
                buf_array[i] = buf;
              }
              TCompletionTrigger *trigger_ptr = &async_trigger;
              cache->VolumeManager->ReadV(code_location,
                                          buf_kind,
                                          util_src,
                                          buf_array,
                                          num_pages,
                                          logical_offset,
                                          PageSize * num_pages,
                                          priority,
                                          async_trigger,
                                          [my_data_slots, my_main_slots, can_release, trigger_ptr, cache, page_id, from_page_id, num_pages, code_location](TDiskResult result, const char *err_str) {
                if (result == TDiskResult::Success) {
                  for (size_t i = 0; i < num_pages; ++i) {
                    TSlot *const this_data_slot = my_data_slots[i];
                    size_t val = std::atomic_load(&(this_data_slot->BufAddr));
                    val |= HighestBit;
                    std::atomic_store(&(this_data_slot->BufAddr), val);
                  }
                  trigger_ptr->Callback(result, err_str);
                } else {
                  for (size_t i = 0; i < num_pages; ++i) {
                    //const size_t this_page_id = from_page_id + i;
                    TSlot *const this_data_slot = my_data_slots[i];
                    size_t val = std::atomic_load(&(this_data_slot->BufAddr));
                    val |= ThirdHighestBit; /* set the error bit */
                    std::atomic_store(&(this_data_slot->BufAddr), val);
                  }
                }
                if (can_release) {
                  for (size_t i = 0; i < num_pages; ++i) {
                    const size_t this_page_id = from_page_id + i;
                    TSlot *const this_main_slot = my_main_slots[i];
                    cache->Release(this_main_slot, this_page_id);
                  }
                }
              });
              //#endif
            };

            /* these variables accumulate sequential sections that can be loaded */
            size_t consec_starting_page_id = -1;
            size_t consec_next_page_id = -1;
            size_t num_consec_to_load = 0UL;

            for (size_t i = 0; i < num_consec_pages; ++i) {
              TSlot *const data_slot = data_slots[i];
              for (;;) {
                size_t val = std::atomic_load(&(data_slot->BufAddr));
                if ((val & HighestBit) == HighestBit) {
                  /* if the highest bit is set, then we know the data is already loaded. we can skip. */
                  if (num_consec_to_load > 0) {
                    load_range_func(consec_starting_page_id, num_consec_to_load);
                  }
                  consec_starting_page_id = -1;
                  consec_next_page_id = -1;
                  num_consec_to_load = 0UL;
                  if (can_release) {
                    cache->Release(main_slots[i], page_id + i);
                  }
                  break;
                } else {
                  /* if the highest bit is not set, then we use the second highest bit to see if someone is already loading this data. */
                  if ((val & SecondHighestBit) == SecondHighestBit) {
                    /* second highest bit is set, someone is loading the data, we can skip. */
                    if (num_consec_to_load > 0) {
                      load_range_func(consec_starting_page_id, num_consec_to_load);
                    }
                    consec_starting_page_id = -1;
                    consec_next_page_id = -1;
                    num_consec_to_load = 0UL;
                    if (can_release) {
                      cache->Release(main_slots[i], page_id + i);
                    }
                    break;
                  } else {
                    /* second highest bit is not set, let's try to set it and load the data if we're successful */
                    size_t new_val = val | SecondHighestBit;
                    if (!std::atomic_compare_exchange_strong(&(data_slot->BufAddr), &val, new_val)) {
                      /* the value has changed since we read it. this means we can continue in the loop and check it again. */
                      continue;
                    }
                    /* we successfully swapped in the second highest bit. this means we need to load the data and then set the highest bit once it's valid. */

                    if (num_consec_to_load > 0) {
                      ++consec_next_page_id;
                      ++num_consec_to_load;
                    } else {
                      consec_starting_page_id = page_id + i;
                      consec_next_page_id = consec_starting_page_id + 1UL;
                      num_consec_to_load = 1UL;
                    }
                    break;
                  }
                }
              }
            }
            /* make sure we catch the tail end. */
            if (num_consec_to_load > 0) {
              load_range_func(consec_starting_page_id, num_consec_to_load);
            }
          }

          private:

          /* TODO */
          class __attribute__((aligned(64))) TLRU {
            NO_COPY_SEMANTICS(TLRU);
            public:

            typedef InvCon::AtomicUnorderedList::TCollection<TLRU, TSlot> TSlotCollection;

            /* TODO */
            TLRU() : SlotCollection(this)
            #ifdef PERF_STATS
            , NumBufInLRU(0UL)
            #endif
            {}

            /* TODO */
            mutable typename TSlotCollection::TImpl SlotCollection;

            /* Stats */
            #ifdef PERF_STATS
            std::atomic<size_t> NumBufInLRU;
            #endif

          };  // TLRU

          /* TODO */
          inline size_t NewPageBuf() {
            assert(this);
            int lru_start = sched_getcpu() % NumLRU;
            size_t stop_lru = NumLRU;
            for (size_t cur_lru = lru_start; cur_lru < stop_lru; ++cur_lru) {
              TLRU &lru = LRUArray[cur_lru];
              size_t reclaimed_page_buf;
              size_t main_slot_page_id;
              TSlot *slot_to_remove = nullptr;
              lru.SlotCollection.ForEach(TryRemoveSlotFunc, slot_to_remove, reclaimed_page_buf, main_slot_page_id);
              if (likely(slot_to_remove)) {
                assert(slot_to_remove->LRUMembership.TryGetCollector());
                #ifdef PERF_STATS
                --(slot_to_remove->LRUMembership.TryGetCollector()->NumBufInLRU);
                #endif
                slot_to_remove->LRUMembership.Remove();
                if (slot_to_remove->MainSlot == nullptr) {
                  /* common case : this is a main slot */
                  std::atomic_store(&(slot_to_remove->PageId), EmptySlot);
                } else {
                  /* uncommon case : this is a chain slot */
                  std::atomic_store(&(slot_to_remove->MainSlot->PageId), main_slot_page_id);
                  DeleteSlot(slot_to_remove);
                }
              } else {
                /* continue to a neigbor's LRU */
                if (cur_lru == NumLRU - 1) {
                  cur_lru = -1;
                  stop_lru = lru_start;
                }
                continue;
              }
              return reclaimed_page_buf;
            }
            #ifdef PERF_STATS
            syslog(LOG_INFO, "NewPageBuf ps=[%ld] Ran out of cache pages, num_free=[%ld]", PageSize, CountNumBufInLRU());
            #else
            syslog(LOG_INFO, "NewPageBuf ps=[%ld] Ran out of cache pages", PageSize);
            #endif
            throw std::runtime_error("Ran out of cache pages");
          }

          /* TODO */
          bool TryRemoveSlot(const TSlot &slot, TSlot *&slot_to_remove, size_t &out_reclaimed_page_buf, size_t &main_slot_page_id) {
            assert(this);
            /* there are 2 cases:
               - this slot is in the main (pre-allocated) array ... common case
               - this slot is part of a chain, less common */
            std::atomic<size_t> *const main_slot = !slot.MainSlot ? &(slot.PageId) : &(slot.MainSlot->PageId);
            /* try to lock the main slot */
            size_t val = std::atomic_load(main_slot);
            assert(val != EmptySlot);
            if (likely(val != LockedSlot)) {
              if (!std::atomic_compare_exchange_strong(main_slot, &val, LockedSlot)) {
                /* if the atomic no longer holds the value we just read, move on. */
                return true;
              }
              /* now that we've got this slot locked, let's make sure no one is actually looking at it (i.e. refcount = 0) */
              if (slot.RefCount == 0) {
                /* at this point we're locked and we can remove this slot */

                if (slot.MainSlot == nullptr) {
                  /* common case : this is a main slot */
                  if (slot.NextSlot == nullptr) {
                    out_reclaimed_page_buf = std::atomic_load(&slot.BufAddr);
                    /* remove the high order bits from the reclaimed page. */
                    out_reclaimed_page_buf &= All1But3HighestBits;
                    slot_to_remove = const_cast<TSlot *>(&slot);
                    return false;
                  } else {
                    /* if the next slot in our chain has a ref count of 0, we can move it into our main slot. */
                    TSlot &next_slot = *(slot.NextSlot);
                    if (next_slot.RefCount == 0) {
                      assert(slot.RefCount == 0);
                      const_cast<TSlot &>(slot).NextSlot = next_slot.NextSlot;
                      out_reclaimed_page_buf = std::atomic_load(&slot.BufAddr);
                      /* remove the high order bits from the reclaimed page. */
                      out_reclaimed_page_buf &= All1But3HighestBits;
                      std::atomic_store(&const_cast<TSlot &>(slot).BufAddr, std::atomic_load(&(next_slot.BufAddr)));
                      main_slot_page_id = std::atomic_load(&next_slot.PageId);
                      slot_to_remove = const_cast<TSlot *>(&next_slot);
                      return false;
                    }
                    /* We can't swap the next slot (next in chain) into the main slot because the ref count is non-zero */
                    std::atomic_store(main_slot, val);
                    return true;
                  }
                } else {
                  /* uncommon case : this is a chain slot */
                  TSlot *prev_slot = !slot.MainSlot ? const_cast<TSlot *>(&slot) : const_cast<TSlot *>(slot.MainSlot);
                  TSlot *next_slot = const_cast<TSlot *>(prev_slot->NextSlot);
                  for (; next_slot; next_slot = next_slot->NextSlot) {
                    if (next_slot == &slot) {
                      assert(next_slot->RefCount == 0);
                      prev_slot->NextSlot = next_slot->NextSlot;
                      out_reclaimed_page_buf = std::atomic_load(&slot.BufAddr);
                      /* remove the high order bits from the reclaimed page. */
                      out_reclaimed_page_buf &= All1But3HighestBits;
                      slot_to_remove = const_cast<TSlot *>(&slot);
                      main_slot_page_id = val;
                      return false;
                    }
                    prev_slot = next_slot;
                  }
                  throw std::logic_error("Trying to remove slot that does not exist in chain");
                }
              } else {
                /* unlock, move on */
                std::atomic_store(main_slot, val);
                return true;
              }
            }
            return true;
          }

          /* TODO */
          inline TSlot *NewSlot() {
            return new TSlot();
          }

          /* TODO */
          inline void DeleteSlot(TSlot *slot) {
            delete slot;
          }

          /* TODO */
          TVolumeManager *const VolumeManager;

          /* TODO */
          const size_t MaxCacheSize;

          /* TODO */
          const size_t NumSlots;

          /* TODO */
          const size_t NumLRU;

          /* TODO */
          TSlot *SlotArray;

          /* TODO */
          TLRU *LRUArray;

          /* TODO */
          char *PageData;

          /* TODO */
          timespec ReadWait;

          /* TODO */
          std::function<bool (const TSlot &, TSlot *&, size_t &, size_t &)> TryRemoveSlotFunc;

        };  // TCache

      }  // Util

    }  // Disk

  }  // Indy

}  // Stig