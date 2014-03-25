/* <stig/indy/disk/util/index_manager.h>

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

#include <cassert>

#include <base/no_copy_semantics.h>
#include <inv_con/ordered_list.h>
#include <stig/indy/disk/util/index_sort_file.h>
#include <stig/indy/util/merge_sorter.h>
#include <stig/indy/util/min_heap.h>
#include <stig/indy/util/sorter.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      namespace Util {

        /* TODO */
        template <typename TVal, size_t MemSize, size_t MinParallelSortThreshold, class TComparator = std::less<TVal>>
        class TIndexManager {
          NO_COPY_SEMANTICS(TIndexManager);
          public:

          /* TODO */
          typedef TIndexManager<TVal, MemSize, MinParallelSortThreshold, TComparator> TMe;
          typedef TIndexSortFile<TIndexManager, TVal, MemSize, TComparator> TSortFile;
          typedef InvCon::OrderedList::TCollection<TIndexManager, TSortFile, size_t> TSortFileCollection;

          /* TODO */
          class TCursor {
            NO_COPY_SEMANTICS(TCursor);
            public:

            /* TODO */
            TCursor(TIndexManager *manager, size_t total_read_ahead_slots)
                : Manager(manager) {
              Manager->SortMem();
              size_t num_csr_required = 1UL;
              for (typename TSortFileCollection::TCursor csr(&Manager->SortFileCollection); csr; ++csr, ++num_csr_required) {}
              size_t read_ahead_per_csr = std::max(1UL, total_read_ahead_slots / num_csr_required);
              for (typename TSortFileCollection::TCursor csr(&Manager->SortFileCollection); csr; ++csr) {
                CsrVec.push_back(std::unique_ptr<typename Indy::Util::TSorter<TVal, MemSize>::TCursor>(new typename TSortFile::TCursor(&*csr, read_ahead_per_csr)));
              }
              CsrVec.push_back(std::unique_ptr<typename Indy::Util::TSorter<TVal, MemSize>::TCursor>(new typename Indy::Util::TSorter<TVal, MemSize>::TMemCursor(&Manager->MemSorter)));
              MinHeap = std::unique_ptr<Indy::Util::TMinHeap<TVal, size_t, TComparator>>(new Indy::Util::TMinHeap<TVal, size_t, TComparator>(CsrVec.size(), manager->Comp));
              for (size_t i = 0; i < CsrVec.size(); ++i) {
                typename Indy::Util::TSorter<TVal, MemSize>::TCursor &csr = *CsrVec[i];
                if (csr) {
                  MinHeap->Insert(*csr, i);
                }
              }
            }

            /* TODO */
            ~TCursor() {
              assert(this);
            }

            /* TODO */
            operator bool() const {
              assert(this);
              return static_cast<bool>(*MinHeap);
            }

            /* TODO */
            const TVal &operator*() const {
              assert(this);
              #ifndef NDEBUG
              if (!static_cast<bool>(*MinHeap)) {
                throw std::logic_error("empty minheap");
              }
              #endif
              assert(static_cast<bool>(*MinHeap));
              size_t dummy;
              return MinHeap->Peek(dummy);
            }

            /* TODO */
            const TVal *operator->() const {
              assert(this);
              #ifndef NDEBUG
              if (!static_cast<bool>(*MinHeap)) {
                throw std::logic_error("empty minheap");
              }
              #endif
              assert(static_cast<bool>(*MinHeap));
              size_t dummy;
              return &(MinHeap->Peek(dummy));
            }

            /* TODO */
            TCursor &operator++() {
              assert(this);
              size_t pos = 0U;
              MinHeap->Pop(pos);
              assert(pos < CsrVec.size());
              typename Indy::Util::TSorter<TVal, MemSize>::TCursor &csr = *CsrVec[pos];
              ++csr;
              if (csr) {
                MinHeap->Insert(*csr, pos);
              }
              return *this;
            }

            private:

            /* TODO */
            TIndexManager *Manager;

            /* TODO */
            std::vector<std::unique_ptr<typename Indy::Util::TSorter<TVal, MemSize>::TCursor>> CsrVec;

            /* TODO */
            std::unique_ptr<Indy::Util::TMinHeap<TVal, size_t, TComparator>> MinHeap;

          };  // TCursor

          /* TODO */
          TIndexManager(const Base::TCodeLocation &code_location /* DEBUG */,
                        uint8_t util_src,
                        size_t consolidation_threshold,
                        TVolume::TDesc::TStorageSpeed storage_speed,
                        TEngine *engine,
                        bool do_cache,
                        const TComparator &comp = std::less<TVal>())
              : StorageSpeed(storage_speed),
                Engine(engine),
                VolMan(Engine->GetVolMan()),
                BlockCache(Engine->GetBlockCache()),
                CacheInstr(do_cache ? CacheBlockOnly : ClearBlockOnly),
                Size(0UL),
                SortFileCollection(this),
                MemSorted(false),
                Comp(comp),
                CodeLocation(code_location),
                UtilSrc(util_src),
                ConsolidationThreshold(consolidation_threshold) {
            assert(consolidation_threshold > 0UL);
          }

          /* TODO */
          ~TIndexManager() {
            assert(this);
            SortFileCollection.DeleteEachMember();
          }

          /* TODO */
          template <class... Args>
          void Emplace(Args &&... args) {
            if (MemSorter.IsFull()) {
              //__gnu_parallel::sort(MemSorter.begin(), MemSorter.end(), Comp);
              std::sort(MemSorter.begin(), MemSorter.end(), Comp);
              new TSortFile(CodeLocation, UtilSrc, StorageSpeed, Engine, VolMan, BlockCache, 0UL, MemSorter, SortFileCollection, CacheInstr);
              MemSorter.Clear();
              /* TODO: we can keep a memory-safe data-structure to track our generation sizes */
              size_t gen = -1;
              size_t count = 0UL;
              bool done = false;
              for (typename TSortFileCollection::TCursor csr(&SortFileCollection); csr; ++csr) {
                if (csr.GetMembership()->GetKey() != gen) {
                  if (count >= ConsolidationThreshold) {
                    ConsolidateGeneration(gen, count);
                    done = true;
                    break;
                  }
                  gen = csr.GetMembership()->GetKey();
                  count = 1UL;
                } else {
                  ++count;
                }
              }
              if (!done && count >= ConsolidationThreshold) {
                ConsolidateGeneration(gen, count);
              }
            }
            MemSorted = false;
            MemSorter.Emplace(std::forward<Args>(args)...);
            ++Size;
          }

          /* TODO */
          void Clear() {
            assert(this);
            MemSorter.Clear();
            MemSorted = false;
            SortFileCollection.DeleteEachMember();
            Size = 0UL;
          }

          /* TODO */
          inline size_t GetSize() const {
            return Size;
          }

          private:

          /* TODO */
          void SortMem() {
            assert(this);
            if (!MemSorted) {
              if (Size >= MinParallelSortThreshold) {
                //__gnu_parallel::sort(MemSorter.begin(), MemSorter.end(), Comp);
                std::sort(MemSorter.begin(), MemSorter.end(), Comp);
              } else {
                std::sort(MemSorter.begin(), MemSorter.end(), Comp);
              }
              MemSorted = true;
            }
          }

          /* TODO */
          void ConsolidateGeneration(size_t gen, size_t num) {
            assert(this);
            new TSortFile(CodeLocation, UtilSrc, StorageSpeed, Engine, VolMan, BlockCache, gen, num, SortFileCollection, 10UL, Comp, CacheInstr);
          }

          /* TODO */
          TVolume::TDesc::TStorageSpeed StorageSpeed;

          /* TODO */
          TEngine *Engine;

          /* TODO */
          TVolumeManager *VolMan;

          /* TODO */
          TBlockCache *BlockCache;

          /* TODO */
          TCacheInstr CacheInstr;

          /* TODO */
          size_t Size;

          /* TODO */
          Indy::Util::TSorter<TVal, MemSize> MemSorter;

          /* TODO */
          mutable typename TSortFileCollection::TImpl SortFileCollection;

          /* TODO */
          bool MemSorted;

          /* TODO */
          TComparator Comp;

          /* DEBUG */
          const Base::TCodeLocation CodeLocation;

          /* TODO */
          const uint8_t UtilSrc;

          /* TODO */
          size_t ConsolidationThreshold;

        };  // TIndexManager

      }  // Util

    }  // Disk

  }  // Indy

}  // Stig