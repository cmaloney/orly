/* <stig/indy/repo.h>

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

#include <atomic>

#include <base/opt.h>
#include <stig/indy/disk_layer.h>
#include <stig/indy/disk/data_file.h>
#include <stig/indy/disk/merge_data_file.h>
#include <stig/indy/disk/present_walk_file.h>
#include <stig/indy/disk/update_walk_file.h>
#include <stig/indy/manager_base.h>
#include <stig/indy/memory_layer.h>
#include <stig/indy/present_walker.h>
#include <stig/indy/sequence_number.h>
#include <stig/indy/status.h>
#include <stig/indy/update.h>
#include <stig/indy/update_walker.h>
#include <stig/indy/util/merge_sorter.h>
#include <stig/indy/util/min_heap.h>

namespace Stig {

  namespace Indy {

    namespace L1 {

      /* TODO */
      class TTransaction;

    }  // L1

    /* TODO */
    class TRepo
        : public L0::TManager::TRepo {
      NO_COPY_SEMANTICS(TRepo);
      public:

      /* TODO */
      using TParentRepo = L0::TManager::TRepo::TParentRepo;

      /* TODO */
      class TView {
        NO_COPY_SEMANTICS(TView);
        public:

        /* TODO */
        TView(const L0::TManager::TPtr<TRepo> &repo);

        /* TODO */
        TView(TRepo *repo);

        /* TODO */
        ~TView();

        /* TODO */
        const TMemoryLayer *GetCurMem() const;

        /* TODO */
        const TMapping *GetMapping() const;

        /* TODO */
        inline size_t GetNumEntries() const;

        /* TODO */
        inline TSequenceNumber GetNextId() const {
          return NextId;
        }

        /* TODO */
        const Base::TOpt<TSequenceNumber> &GetLower() const;

        /* TODO */
        const Base::TOpt<TSequenceNumber> &GetUpper() const;

        private:

        /* TODO */
        TRepo *Repo;

        /* TODO */
        TMemoryLayer *CurrentMemoryLayer;

        /* TODO */
        TMapping *Mapping;

        /* TODO */
        Base::TOpt<TSequenceNumber> LowerBound;

        /* TODO */
        Base::TOpt<TSequenceNumber> UpperBound;

        /* TODO */
        TSequenceNumber NextId;

      };  // TView

      /* get a snapshot of this repo using the data lock */
      inline void GetSnapshot(Base::TOpt<TSequenceNumber> &seq_num_start,
                              Base::TOpt<TSequenceNumber> &seq_num_limit,
                              TSequenceNumber &next_seq_num);

      /* The sequence number of the oldest unpopped update. */
      inline const Base::TOpt<TSequenceNumber> &GetSequenceNumberStart() const;

      /* The sequence number of the newest update.
         This is also the total number of updates ever pushed to this repo. */
      inline const Base::TOpt<TSequenceNumber> &GetSequenceNumberLimit() const;

      /* TODO */
      inline TSequenceNumber GetNextSequenceNumber() const;

      /* TODO */
      inline void SetNextSequenceNumber(TSequenceNumber next_id);

      /* TODO */
      inline void SetReleasedUpTo(TSequenceNumber released_up_to);

      /* TODO */
      inline TSequenceNumber UseSequenceNumbers(size_t num);

      /* TODO */
      inline TSequenceNumber GetReleasedUpTo() const;

      /* TODO */
      virtual inline const TParentRepo &GetParentRepo() const;

      /* TODO */
      void AddImportLayer(TMemoryLayer *mem_layer, Base::TEventSemaphore &sem, Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed);

      /* TODO */
      void AddFileToRepo(size_t gen_id, TSequenceNumber low_saved, TSequenceNumber high_saved, size_t num_keys);

      /* TODO */
      virtual size_t AddSyncedFileToRepo(size_t starting_block_id,
                                         size_t starting_block_offset,
                                         size_t file_length,
                                         TSequenceNumber low_saved,
                                         TSequenceNumber high_saved,
                                         size_t num_keys) = 0;

      /* Mark the given update for background deletion.
         The sequence number must be older than (that is, less than) that of the oldest unpopped update.
         Marking a non-existent update for deletion is not an error.  It simply does nothing.
         There is no notification when the update is actually deleted. */
      void ReleaseUpdate(TSequenceNumber sequence_number, bool ensure_or_discard);

      /* TODO */
      virtual std::unique_ptr<Indy::TPresentWalker> NewPresentWalker(const std::unique_ptr<TView> &view,
                                                                     const TIndexKey &from,
                                                                     const TIndexKey &to,
                                                                     bool ignore_tombstone = false);

      /* TODO */
      virtual std::unique_ptr<Indy::TPresentWalker> NewPresentWalker(const std::unique_ptr<TView> &view,
                                                                     const TIndexKey &key,
                                                                     bool ignore_tombstone = false);

      /* TODO */
      virtual std::unique_ptr<Indy::TUpdateWalker> NewUpdateWalker(const std::unique_ptr<TView> &view,
                                                                   TSequenceNumber from,
                                                                   const Base::TOpt<TSequenceNumber> &to);

      /* TODO */
      virtual std::unique_ptr<Indy::TUpdateWalker> NewUpdateWalker(const std::unique_ptr<TView> &view,
                                                                   TSequenceNumber from);

      /* TODO */
      virtual void StepTail(size_t block_slots_available) = 0;

      protected:

      /* TODO */
      TRepo(L0::TManager *manager,
            const Base::TUuid &repo_id,
            const TTtl &ttl,
            const Base::TOpt<L0::TManager::TPtr<L0::TManager::TRepo>> &parent_repo);

      /* TODO */
      TRepo(L0::TManager *manager,
            const Base::TUuid &repo_id,
            const TTtl &ttl,
            const Base::TOpt<L0::TManager::TPtr<L0::TManager::TRepo>> &parent_repo,
            const Base::TOpt<TSequenceNumber> &lowest,
            const Base::TOpt<TSequenceNumber> &highest,
            TSequenceNumber next_update,
            TStatus status);

      /* TODO */
      virtual ~TRepo();

      /* TODO */
      Base::TOpt<TSequenceNumber> AppendUpdate(TUpdate *update, TSequenceNumber &next_update) NO_THROW;

      /* TODO */
      Base::TOpt<TSequenceNumber> PopLowest(TSequenceNumber &next_update) NO_THROW;

      /* TODO */
      std::shared_ptr<TUpdate> GetLowestUpdate();

      /* TODO */
      Base::TOpt<TSequenceNumber> ChangeStatus(TStatus, TSequenceNumber &next_update) NO_THROW;

      /* TODO */
      virtual void StepMergeMem() override;

      /* TODO */
      virtual void StepMergeDisk(size_t block_slots_available) = 0;

      /* TODO */
      size_t AddMapping(TDataLayer *layer);

      /* TODO */
      TMapping *AcquireCurrentMapping();

      /* TODO */
      void ReleaseMapping(TMapping *mapping);

      /* TODO */
      std::mutex MemMergeLock;

      /* TODO */
      std::mutex DataLock;

      /* TODO */
      void CheckRemoveDirty();

      /* TODO */
      TMemoryLayer *CurMemoryLayer;

      private:

      /* TODO */
      class TPresentWalker
          : public Indy::TPresentWalker {
        NO_COPY_SEMANTICS(TPresentWalker);
        public:

        /* TODO */
        TPresentWalker(const std::unique_ptr<TView> &view,
                       const TIndexKey &from,
                       const TIndexKey &to,
                       bool ignore_tombstone);

        TPresentWalker(const std::unique_ptr<TView> &view,
                       const TIndexKey &key,
                       bool ignore_tombstone);

        /* TODO */
        inline virtual ~TPresentWalker() {}

        /* True iff. we have an item. */
        inline virtual operator bool() const;

        /* The current item. */
        inline virtual const TItem &operator*() const;

        /* Walk to the next item, if any. */
        inline virtual TPresentWalker &operator++();

        private:

        /* TODO */
        class TRunnablePrep
            : public Indy::Fiber::TRunnable {
          NO_COPY_SEMANTICS(TRunnablePrep);
          public:

          /* TODO */
          TRunnablePrep(TDataLayer *layer, TPresentWalker *walker, Fiber::TSync *sync)
              : Layer(layer), Walker(walker), Sync(sync) {
            Frame = Fiber::TFrame::LocalFramePool->Alloc();
            try {
              Frame->Latch(this, static_cast<Fiber::TRunnable::TFunc>(&TRunnablePrep::Compute));
            } catch (...) {
              Fiber::TFrame::LocalFramePool->Free(Frame);
              throw;
            }
          }

          /* TODO */
          TRunnablePrep(Stig::Indy::TRepo::TPresentWalker::TRunnablePrep &&) {
            throw std::logic_error("Moving TRunnablePrep is not allowed. This means you did not pre-allocate enough space to hold them.");
          }

          /* TODO */
          virtual ~TRunnablePrep() {
            assert(this);
          }

          /* TODO */
          void Compute() {
            //printf("TRunnablePrep::Compute()\n");
            assert(this);
            assert(Walker);
            assert(Layer);
            assert(Sync);
            assert(Fiber::TFrame::LocalFrame == Frame);
            Walker->WalkerVec.emplace_back(Layer->NewPresentWalker(Walker->From));
            Sync->Complete();
            Fiber::FreeMyFrame(Fiber::TFrame::LocalFramePool);
          }

          private:

          /* TODO */
          Fiber::TFrame *Frame;

          /* TODO */
          TDataLayer *Layer;

          /* TODO */
          TPresentWalker *Walker;

          /* TODO */
          Fiber::TSync *Sync;

        };  // TRunnablePrep

        /* TODO */
        inline void Init();

        /* TODO */
        inline void Refresh();

        /* TODO */
        TIndexKey From;

        /* TODO */
        TIndexKey To;

        /* TODO */
        const std::unique_ptr<TView> &View;
        const TSequenceNumber Lower;
        const TSequenceNumber Upper;

        /* TODO */
        std::vector<std::unique_ptr<Indy::TPresentWalker>> WalkerVec;
        std::vector<TRunnablePrep> PrepVec;

        /* TODO */
        Util::TMinHeap<TItem, size_t> MinHeap;

        /* TODO */
        bool Valid;

        /* TODO */
        mutable TItem Item;

        /* TODO */
        const bool IgnoreTombstone;

      };  // TPresentWalker

      /* TODO */
      class TUpdateWalker
          : public Indy::TUpdateWalker {
        NO_COPY_SEMANTICS(TUpdateWalker);
        public:

        /* TODO */
        TUpdateWalker(const std::unique_ptr<TView> &view,
                      TSequenceNumber from,
                      const Base::TOpt<TSequenceNumber> &to);

        /* TODO */
        virtual ~TUpdateWalker();

        /* True iff. we have an item. */
        virtual operator bool() const;

        /* The current item. */
        virtual const Indy::TUpdateWalker::TItem &operator*() const;

        /* Walk to the next item, if any. */
        virtual TUpdateWalker &operator++();

        private:

        /* TODO */
        void Refresh();

        /* TODO */
        TSequenceNumber From;

        /* TODO */
        Base::TOpt<TSequenceNumber> To;

        /* TODO */
        const std::unique_ptr<TView> &View;

        /* TODO */
        std::unordered_map<const TDataLayer *, std::pair<std::unique_ptr<Indy::TUpdateWalker>, size_t>> WalkerMap;

        /* TODO */
        Util::TMergeSorter<TSequenceNumber, const TDataLayer *> MergeSorter;

        /* TODO */
        Util::TMergeSorter<TSequenceNumber, const TDataLayer *>::TMergeElement *SorterAlloc;

        /* TODO */
        bool Valid;

        /* TODO */
        mutable TItem Item;

      };  // TUpdateWalker

      /* TODO */
      TParentRepo ParentRepo;

      /* TODO */
      Base::TOpt<TSequenceNumber> LowestSeqNum;

      /* TODO */
      Base::TOpt<TSequenceNumber> HighestSeqNum;

      /* TODO */
      TSequenceNumber NextUpdate;

      protected:

      /* TODO */
      virtual bool ForEachDependentPtr(const std::function<bool (L0::TManager::TAnyPtr &)> &cb) noexcept override {
        if (ParentRepo) {
          cb(*ParentRepo);
        }
        return true;
      }

      /* TODO */
      TSequenceNumber ReleasedUpTo;

      private:

      /* TODO */
      bool InTetris;

      /* TODO */
      friend class L1::TTransaction;

    };  // TRepo

    /* TODO */
    class TFastRepo
        : public Stig::Indy::TRepo {
      NO_COPY_SEMANTICS(TFastRepo);
      public:

      /* TODO */
      TFastRepo(L0::TManager *manager,
                const Base::TUuid &repo_id,
                const TTtl &ttl,
                const Base::TOpt<L0::TManager::TPtr<L0::TManager::TRepo>> &parent_repo);

      /* TODO */
      TFastRepo(L0::TManager *manager,
                const Base::TUuid &repo_id,
                const TTtl &ttl,
                const Base::TOpt<L0::TManager::TPtr<L0::TManager::TRepo>> &parent_repo,
                const Base::TOpt<TSequenceNumber> &lowest,
                const Base::TOpt<TSequenceNumber> &highest,
                TSequenceNumber next_update,
                TStatus status);

      /* TODO */
      virtual ~TFastRepo();

      private:

      /* TODO */
      virtual bool IsSafeRepo() const override;

      /* TODO */
      virtual void StepMergeDisk(size_t block_slots_available) override;

      /* TODO */
      virtual void StepTail(size_t block_slots_available) override;

      /* TODO */
      virtual size_t AddSyncedFileToRepo(size_t starting_block_id,
                                         size_t starting_block_offset,
                                         size_t file_length,
                                         TSequenceNumber low_saved,
                                         TSequenceNumber high_saved,
                                         size_t num_keys) override;

    };  // TFastRepo

    /* TODO */
    class TSafeRepo
        : public Stig::Indy::TRepo {
      NO_COPY_SEMANTICS(TSafeRepo);
      public:

      /* TODO */
      TSafeRepo(L0::TManager *manager,
                const Base::TUuid &repo_id,
                const TTtl &ttl,
                const Base::TOpt<L0::TManager::TPtr<L0::TManager::TRepo>> &parent_repo);

      /* TODO */
      TSafeRepo(L0::TManager *manager,
                const Base::TUuid &repo_id,
                const TTtl &ttl,
                const Base::TOpt<L0::TManager::TPtr<L0::TManager::TRepo>> &parent_repo,
                const Base::TOpt<TSequenceNumber> &lowest,
                const Base::TOpt<TSequenceNumber> &highest,
                TSequenceNumber next_update,
                TStatus status);

      /* TODO */
      virtual ~TSafeRepo();

      /* TODO */
      virtual void StepMergeDisk(size_t block_slots_available) override;

      /* TODO */
      virtual void StepTail(size_t block_slots_available) override;

      /* TODO */
      static TSafeRepo *ReConstructFromDisk(L0::TManager *manager,
                                            const Base::TUuid &repo_id,
                                            const TDeadline &deadline);

      protected:

      /* TODO */
      inline size_t GetNextGenId();

      private:

      /* TODO */
      virtual bool IsSafeRepo() const override;

      /* TODO */
      virtual size_t MergeFiles(const std::vector<size_t> &gen_id_vec,
                                Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed,
                                size_t max_block_cache_read_slots_allowed,
                                size_t temp_file_consol_thresh,
                                TSequenceNumber &out_saved_low_seq,
                                TSequenceNumber &out_saved_high_seq,
                                size_t &out_num_keys,
                                TSequenceNumber release_up_to,
                                bool can_tail,
                                bool can_tail_tombstone) override;

      /* TODO */
      virtual void RemoveFile(size_t gen_id) override;

      /* TODO */
      virtual size_t WriteFile(TMemoryLayer *memory_layer,
                               Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed,
                               TSequenceNumber &out_saved_low_seq,
                               TSequenceNumber &out_saved_high_seq,
                               size_t &out_num_keys,
                               TSequenceNumber release_up_to) override;

      /* TODO */
      virtual size_t AddSyncedFileToRepo(size_t starting_block_id,
                                         size_t starting_block_offset,
                                         size_t file_length,
                                         TSequenceNumber low_saved,
                                         TSequenceNumber high_saved,
                                         size_t num_keys) override;

      /* TODO */
      virtual std::unique_ptr<Stig::Indy::TPresentWalker> NewPresentWalkerFile(size_t /*gen_id*/,
                                                                               const TIndexKey &/*from*/,
                                                                               const TIndexKey &/*to*/) const override;

      /* TODO */
      virtual std::unique_ptr<Stig::Indy::TPresentWalker> NewPresentWalkerFile(size_t /*gen_id*/,
                                                                               const TIndexKey &/*key*/) const override;

      /* TODO */
      virtual std::unique_ptr<Stig::Indy::TUpdateWalker> NewUpdateWalkerFile(size_t /*gen_id*/, TSequenceNumber /*from*/) const override;

      /* TODO */
      std::atomic<size_t> NextGenId;

      /* TODO */
      std::mutex MergeLock;

    };  // TSafeRepo

    /***************
      *** inline ***
      *************/

    inline size_t TRepo::TView::GetNumEntries() const {
      size_t count = 0UL;
      for (TMapping::TEntryCollection::TCursor mapping_csr(Mapping->GetEntryCollection()); mapping_csr; ++mapping_csr, ++count) {}
      return count;
    }

    inline void TRepo::GetSnapshot(Base::TOpt<TSequenceNumber> &seq_num_start,
                                   Base::TOpt<TSequenceNumber> &seq_num_limit,
                                   TSequenceNumber &next_seq_num) {
      assert(this);
      std::lock_guard<std::mutex> lock(DataLock);
      seq_num_start = LowestSeqNum;
      seq_num_limit = HighestSeqNum;
      next_seq_num = NextUpdate;
    }

    inline const Base::TOpt<TSequenceNumber> &TRepo::GetSequenceNumberStart() const {
      assert(this);
      return LowestSeqNum;
    }

    inline const Base::TOpt<TSequenceNumber> &TRepo::GetSequenceNumberLimit() const {
      assert(this);
      return HighestSeqNum;
    }

    inline TSequenceNumber TRepo::GetNextSequenceNumber() const {
      assert(this);
      return NextUpdate;
    }

    inline void TRepo::SetNextSequenceNumber(TSequenceNumber next_id) {
      assert(this);
      NextUpdate = next_id;
    }

    inline void TRepo::SetReleasedUpTo(TSequenceNumber released_up_to) {
      assert(this);
      std::cout << "SetReleasedUpTo(" << released_up_to << ")" << std::endl;
      ReleasedUpTo = released_up_to;
    }

    inline TSequenceNumber TRepo::UseSequenceNumbers(size_t num) {
      assert(this);
      assert(num);
      std::lock_guard<std::mutex> lock(DataLock);
      TSequenceNumber starting = NextUpdate;
      NextUpdate += num;
      HighestSeqNum = NextUpdate - 1;
      if (!LowestSeqNum) {
        LowestSeqNum = starting;
      }
      return starting;
    }

    inline TSequenceNumber TRepo::GetReleasedUpTo() const {
      assert(this);
      return ReleasedUpTo;
    }

    inline const TRepo::TParentRepo &TRepo::GetParentRepo() const {
      assert(this);
      return ParentRepo;
    }

    inline TRepo::TPresentWalker::operator bool() const {
      assert(this);
      return Valid;
    }

    inline const TPresentWalker::TItem &TRepo::TPresentWalker::operator*() const {
      assert(this);
      assert(Valid);
      return Item;
    }

    inline TRepo::TPresentWalker &TRepo::TPresentWalker::operator++() {
      assert(this);
      assert(Valid);
      Valid = static_cast<bool>(MinHeap);
      Refresh();
      return *this;
    }

    inline void TRepo::TPresentWalker::Init() {
      assert(this);
      bool done = false;
      while (Valid) {
        size_t pos;
        const Indy::TPresentWalker::TItem &cur_item = MinHeap.Pop(pos);
        Indy::TPresentWalker &walker = *WalkerVec[pos];
        assert(cur_item.Key.IsTuple());
        assert((*walker).KeyArena == cur_item.KeyArena);
        assert((*walker).OpArena == cur_item.OpArena);
        assert((*walker).SequenceNumber == cur_item.SequenceNumber);
        assert(Item.KeyArena == nullptr);
        if (cur_item.SequenceNumber >= Lower && cur_item.SequenceNumber <= Upper && (!IgnoreTombstone || !cur_item.Op.IsTombstone())) {
          done = true;
          Item = cur_item;
        }
        ++walker;
        if (walker) {
          MinHeap.Insert(*walker, pos);
        }
        if (done) {
          break;
        } else {
          Valid = static_cast<bool>(MinHeap);
        }
      }
    }

    inline void TRepo::TPresentWalker::Refresh() {
      assert(this);
      bool done = false;
      while (Valid) {
        size_t pos;
        const Indy::TPresentWalker::TItem &cur_item = MinHeap.Pop(pos);
        Indy::TPresentWalker &walker = *WalkerVec[pos];
        assert(cur_item.Key.IsTuple());
        assert((*walker).KeyArena == cur_item.KeyArena);
        assert((*walker).OpArena == cur_item.OpArena);
        assert((*walker).SequenceNumber == cur_item.SequenceNumber);
        assert(Item.KeyArena != nullptr);
        if (cur_item.SequenceNumber >= Lower && cur_item.SequenceNumber <= Upper && Indy::TKey::TupleNeEq(Item.Key, Item.KeyArena, cur_item.Key, cur_item.KeyArena) && (!IgnoreTombstone || !cur_item.Op.IsTombstone())) {
          done = true;
          Item = cur_item;
        }
        ++walker;
        if (walker) {
          MinHeap.Insert(*walker, pos);
        }
        if (done) {
          break;
        } else {
          Valid = static_cast<bool>(MinHeap);
        }
      }
    }

    inline size_t TSafeRepo::GetNextGenId() {
      assert(this);
      return ++NextGenId;
    }

  }  // Indy

}  // Stig