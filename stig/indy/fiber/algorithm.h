/* <stig/indy/fiber/algorithm.h>

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

#pragma once

#include <algorithm>
#include <type_traits>

#include <base/stl_utils.h>
#include <stig/indy/fiber/fiber.h>

namespace Stig {

  namespace Indy {

    namespace Fiber {

      /* TODO */
      template <
          typename TRandomAccessIterator,
          typename TVal = typename Base::ForIter<TRandomAccessIterator>::TVal,
          typename TComparator = std::less<TVal>>
      class TSubSortRunnable
          : public TRunnable {
        NO_COPY_SEMANTICS(TSubSortRunnable);
        public:

        /* TODO */
        TSubSortRunnable(TRunnerPool &work_pool,
                         const TRandomAccessIterator &begin,
                         const TRandomAccessIterator &end,
                         TSafeSync &safe_sync,
                         const TComparator &comp)
            : Begin(begin), End(end), SafeSync(safe_sync), Comp(comp) {
          SafeSync.WaitForMore(1UL);
          Frame = TFrame::LocalFramePool->Alloc();
          work_pool.Schedule(Frame, this, static_cast<TRunnable::TFunc>(&TSubSortRunnable::DoSort));
        }

        /* TODO */
        ~TSubSortRunnable() {
          assert(this);
          TFrame::LocalFramePool->Free(Frame);
        }

        /* TODO */
        void DoSort() {
          assert(this);
          std::sort(Begin, End, Comp);
          SafeSync.Complete();
        }

        private:

        TFrame *Frame;

        /* TODO */
        const TRandomAccessIterator Begin;
        const TRandomAccessIterator End;
        TSafeSync &SafeSync;
        const TComparator &Comp;

      };  // TSubSortRunnable

      /* TODO */
      template <
          typename TRandomAccessIterator,
          typename TVal = typename Base::ForIter<TRandomAccessIterator>::TVal,
          typename TComparator = std::less<TVal>>
      class TInplaceMergeRunnable
          : public TRunnable {
        NO_COPY_SEMANTICS(TInplaceMergeRunnable);
        public:

        /* TODO */
        TInplaceMergeRunnable(TRunnerPool &work_pool,
                         const TRandomAccessIterator &begin,
                         const TRandomAccessIterator &middle,
                         const TRandomAccessIterator &end,
                         TSafeSync &wait_on_safe_sync,
                         TSafeSync &trigger_to_safe_sync,
                         const TComparator &comp)
            : Begin(begin), Middle(middle), End(end), WaitOnSafeSync(wait_on_safe_sync), TriggerToSafeSync(trigger_to_safe_sync), Comp(comp) {
          TriggerToSafeSync.WaitForMore(1UL);
          Frame = TFrame::LocalFramePool->Alloc();
          work_pool.Schedule(Frame, this, static_cast<TRunnable::TFunc>(&TInplaceMergeRunnable::DoMerge));
        }

        /* TODO */
        ~TInplaceMergeRunnable() {
          assert(this);
          TFrame::LocalFramePool->Free(Frame);
        }

        /* TODO */
        void DoMerge() {
          assert(this);
          WaitOnSafeSync.Sync();
          std::inplace_merge(Begin, Middle, End, Comp);
          TriggerToSafeSync.Complete();
        }

        private:

        TFrame *Frame;

        /* TODO */
        const TRandomAccessIterator Begin;
        const TRandomAccessIterator Middle;
        const TRandomAccessIterator End;
        TSafeSync &WaitOnSafeSync;
        TSafeSync &TriggerToSafeSync;
        const TComparator &Comp;

      };  // TInplaceMergeRunnable

      /* TODO */
      template <
          size_t ParallelThreshold,
          typename TRandomAccessIterator,
          typename TVal = typename Base::ForIter<TRandomAccessIterator>::TVal,
          typename TComparator = std::less<TVal>>
      void Sort(TRunnerPool &work_pool,
                TRandomAccessIterator begin,
                TRandomAccessIterator end,
                const TComparator &comp = std::less<TVal>()) {
        using TSubSortRunnable = TSubSortRunnable<TRandomAccessIterator, TVal, TComparator>;
        using TInplaceMergeRunnable = TInplaceMergeRunnable<TRandomAccessIterator, TVal, TComparator>;
        const size_t num_elem = end - begin;
        if (num_elem >= ParallelThreshold) {
          const size_t worker_count = work_pool.GetWorkerCount();
          const size_t log_2_worker_count = log2(worker_count);
          const size_t num_shard = exp2(log_2_worker_count);
          TSubSortRunnable *sub_sort_array = reinterpret_cast<TSubSortRunnable *>(alloca(sizeof(TSubSortRunnable) * num_shard));
          const size_t elem_per_shard = static_cast<size_t>(ceil(static_cast<double>(num_elem) / num_shard));
          size_t num_elem_assigned = 0UL;
          const size_t total_syncs_required = num_shard;
          TSafeSync *safe_sync_array = reinterpret_cast<TSafeSync *>(alloca(sizeof(TSafeSync) * total_syncs_required));
          for (size_t i = 0; i < total_syncs_required; ++i) {
            new (safe_sync_array + i) TSafeSync();
          }
          size_t safe_sync_idx = 0UL;
          for (size_t i = 0; i < num_shard; ++i) {
            const size_t to_assign = std::min(elem_per_shard, num_elem - num_elem_assigned);
            new (sub_sort_array + i) TSubSortRunnable(work_pool,
                                                      begin + num_elem_assigned,
                                                      begin + num_elem_assigned + to_assign,
                                                      safe_sync_array[safe_sync_idx],
                                                      comp);
            num_elem_assigned += to_assign;
            assert(num_shard > 1);
            if (i % 2 == 1) { /* this means we've assigned 2 sorters to the current sync, move ahead to the next sync */
              ++safe_sync_idx;
            }
          }
          size_t merge_sections = num_shard;
          size_t elem_per_merge = elem_per_shard * 2UL;
          size_t safe_sync_attachment_idx = 0UL;
          size_t num_in_place_merges = 0UL;
          TInplaceMergeRunnable *inplace_merge_array =
              reinterpret_cast<TInplaceMergeRunnable *>(alloca(sizeof(TInplaceMergeRunnable) * (total_syncs_required - 1)));
          for (; merge_sections > 1;) {
            assert(merge_sections % 2 == 0UL);
            const size_t num_to_launch = merge_sections / 2;
            num_elem_assigned = 0UL;
            for (size_t i = 0; i < num_to_launch; ++i) {
              const size_t to_assign = std::min(elem_per_merge, num_elem - num_elem_assigned);
              const size_t sync_to_trigger = safe_sync_idx + i / 2;
              new (inplace_merge_array + num_in_place_merges) TInplaceMergeRunnable(work_pool,
                                                                                    begin + num_elem_assigned,
                                                                                    begin + num_elem_assigned + elem_per_merge / 2,
                                                                                    begin + num_elem_assigned + to_assign,
                                                                                    safe_sync_array[safe_sync_attachment_idx],
                                                                                    safe_sync_array[sync_to_trigger],
                                                                                    comp);
              ++num_in_place_merges;
              num_elem_assigned += to_assign;
              ++safe_sync_attachment_idx;
            }
            safe_sync_idx += num_to_launch / 2;
            merge_sections /= 2;
            elem_per_merge *= 2;
          }
          assert(num_in_place_merges == total_syncs_required - 1);
          /* for the final sync of the entire sort */ {
            safe_sync_array[safe_sync_idx].Sync();
          }
          /* destroy alloca'd (new in place) objects */
          assert(num_shard == total_syncs_required); /* if this fails it means we can use the next for loop to destroy the sub-sorters */
          for (size_t i = 0; i < total_syncs_required; ++i) {
            safe_sync_array[i].~TSafeSync();
            sub_sort_array[i].~TSubSortRunnable();
          }
          for (size_t i = 0; i < total_syncs_required - 1; ++i) {
            inplace_merge_array[i].~TInplaceMergeRunnable();
          }
          assert(safe_sync_attachment_idx == total_syncs_required - 1);
          assert(safe_sync_idx == total_syncs_required - 1);
        } else {
          std::sort(begin, end, comp);
        }
      }

    }  // Fiber

  }  // Indy

}  // Stig
