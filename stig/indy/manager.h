/* <stig/indy/manager.h> 

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

#include <mutex>

#include <stig/atom/core_vector.h>
#include <stig/atom/core_vector_builder.h>
#include <stig/indy/disk/durable_manager.h>
#include <stig/indy/failover.h>
#include <stig/indy/replication.h>
#include <stig/indy/transaction_base.h>
#include <stig/indy/util/block_vec.h>

namespace Stig {

  namespace Indy {

    /* The id of the global point of view. */
    const Base::TUuid GlobalPovId("C4EF7C46-28C5-4000-8CCD-C8E799E2C3F3");

    /* The index id of the system repo. */
    const Base::TUuid SystemRepoIndexId("9D3DAB7C-2D75-452C-8200-30180FF584F1");

    /* TODO */
    class TManager
        : public L1::TManager, public DurableManager::TManager {
      NO_COPY_SEMANTICS(TManager);
      public:

      /* TODO */
      enum TState {
        Solo,
        Master,
        SyncSlave,
        Slave
      };

      /* TODO */
      TManager(Disk::Util::TEngine *engine,
               size_t replication_sync_slave_buf_size_mb,
               size_t merge_mem_delay,
               size_t merge_disk_delay,
               size_t layer_cleaning_interval_milliseconds,
               size_t replication_delay,
               TState state,
               bool allow_tailing,
               bool allow_file_sync,
               bool no_realtime,
               Base::TFd &&socket,
               const std::function<void (const std::shared_ptr<std::function<void (const Base::TFd &)>> &)> &wait_for_slave,
               const std::function<void (TState)> &state_change_cb,
               const std::function<void (const Base::TUuid &, const Base::TUuid &, const Base::TUuid &)> &update_replication_notification_cb,
               const std::function<void (const Base::TUuid &idx_id, const Indy::TKey &key, const Indy::TKey &val)> &on_replicate_index_id,
               const std::function<void (const std::function<void (const Base::TUuid &idx_id, const Indy::TKey &key, const Indy::TKey &val)> &)> &for_each_index_cb,
               const std::function<void (const std::function<bool (Fiber::TRunner *)> &)> &for_each_scheduler_cb,
               Base::TScheduler *scheduler,
               Fiber::TRunner *bg_fast_runner,
               size_t block_slots_available_per_merger,
               size_t max_repo_cache_size,
               size_t walker_local_cache_size,
               size_t temp_file_consol_thresh,
               const std::vector<size_t> &merge_mem_cores,
               const std::vector<size_t> &merge_disk_cores,
               bool create_new);

      /* TODO */
      virtual ~TManager();

      /* TODO */
      inline TManager::TPtr<Indy::TRepo> NewSafeRepo(const Base::TUuid &repo_id,
                                                     const Base::TOpt<TTtl> &ttl);

      /* TODO */
      inline TManager::TPtr<Indy::TRepo> NewSafeRepo(const Base::TUuid &repo_id,
                                                      const Base::TOpt<TTtl> &ttl,
                                                      const TManager::TPtr<Indy::TRepo> &parent_repo);

      /* TODO */
      TManager::TPtr<Indy::TRepo> NewSafeRepo(const Base::TUuid &repo_id,
                                              const Base::TOpt<TTtl> &ttl,
                                              const Base::TOpt<TManager::TPtr<L0::TManager::TRepo>> &parent_repo);

      /* TODO */
      inline TManager::TPtr<Indy::TRepo> NewFastRepo(const Base::TUuid &repo_id,
                                                     const Base::TOpt<TTtl> &ttl);

      /* TODO */
      inline TManager::TPtr<Indy::TRepo> NewFastRepo(const Base::TUuid &repo_id,
                                                     const Base::TOpt<TTtl> &ttl,
                                                     const TManager::TPtr<Indy::TRepo> &parent_repo);

      /* TODO */
      TManager::TPtr<Indy::TRepo> NewFastRepo(const Base::TUuid &repo_id,
                                              const Base::TOpt<TTtl> &ttl,
                                              const Base::TOpt<TManager::TPtr<L0::TManager::TRepo>> &parent_repo);

      /* TODO */
      inline TManager::TPtr<Indy::TRepo> GetRepo(const Base::TUuid &repo_id,
                                                 const Base::TOpt<TTtl> &deadline,
                                                 const Base::TOpt<TManager::TPtr<TRepo>> &parent_repo,
                                                 bool is_safe,
                                                 bool create = true);

      /* TODO */
      inline TManager::TPtr<Indy::TRepo> ForceGetRepo(const Base::TUuid &repo_id);

      /* TODO */
      inline TManager::TPtr<Indy::TRepo> GetSystemRepo() const;

      /* TODO */
      virtual void RunReplicationQueue() override;

      /* TODO */
      virtual void RunReplicationWork() override;

      /* TODO */
      virtual void RunReplicateTransaction() override;

      /* TODO */
      inline void SetDurableManager(const std::shared_ptr<Durable::TManager> &durable_manager) {
        assert(this);
        DurableManager = durable_manager;
      }

      /* TODO */
      static const Base::TUuid MinId;
      static const Base::TUuid MaxId;
      static const Base::TUuid SystemRepoId;

      private:

      enum TTransactionPushType {
        Meta,
        Id,
        Entry,
      };

      /* Prepended to sync repo entries. */
      typedef std::tuple<int, Base::TUuid, Base::TUuid, TSequenceNumber, int> TSyncPrepend;

      /* TODO */
      typedef std::tuple<int, Base::TUuid> TSavedRepoKey;
      static const int SavedRepoMagicNumber;

      /* TODO */
      class TSavedRepoObj {
        public:

        /* TODO */
        typedef std::vector<Base::TUuid> TRootPath;
        typedef Base::TOpt<TSequenceNumber> TOptSeq;

        /* TODO */
        static const int Normal = 0;
        static const int Paused = 1;
        static const int Failed = 2;

        /* TODO */
        TSavedRepoObj() {}

        /* TODO */
        TSavedRepoObj(bool is_safe,
                      const TRootPath &root_path,
                      const TOptSeq &lowest_seq,
                      const TOptSeq &highest_seq,
                      TSequenceNumber next_update,
                      TSequenceNumber released_up_to,
                      int state)
            : IsSafe(is_safe),
              RootPath(root_path),
              LowestSequenceNumber(lowest_seq),
              HighestSequenceNumber(highest_seq),
              NextUpdate(next_update),
              ReleasedUpTo(released_up_to),
              State(state) {
          assert(State == Normal || State == Paused || State == Failed);
        }

        /* TODO */
        bool operator==(const TSavedRepoObj &that) const {
          return IsSafe == that.IsSafe &&
            RootPath == that.RootPath &&
            LowestSequenceNumber == that.LowestSequenceNumber &&
            HighestSequenceNumber == that.HighestSequenceNumber &&
            NextUpdate == that.NextUpdate &&
            ReleasedUpTo == that.ReleasedUpTo &&
            State == that.State;
        }

        /* TODO */
        bool IsSafe;

        /* TODO */
        TRootPath RootPath;

        /* TODO */
        TOptSeq LowestSequenceNumber;

        /* TODO */
        TOptSeq HighestSequenceNumber;

        /* TODO */
        TSequenceNumber NextUpdate;

        /* TODO */
        TSequenceNumber ReleasedUpTo;

        /* TODO */
        int State;

      };

      /* TODO */
      class TMaster
          : public TMasterContext {
        NO_COPY_SEMANTICS(TMaster);
        public:

        /* TODO */
        TMaster(TManager *manager, const Base::TFd &fd);

        /* TODO */
        virtual ~TMaster();

        /* TODO */
        virtual bool Queue();

        /* TODO */
        virtual bool Work();

        /* TODO */
        virtual Util::TContextInputStreamer FetchUpdates(const Base::TUuid &repo_id, TSequenceNumber lowest, TSequenceNumber highest);

        /* TODO */
        virtual void NotifyFinishSyncInventory();

        /* TODO */
        virtual TViewDef GetView(const Base::TUuid &repo_id);

        /* TODO */
        virtual TFileSync SyncFile(const Base::TUuid &file_id, size_t gen_id, size_t context);

        /* TODO */
        virtual void Ping();

        private:

        /* TODO */
        TManager *Manager;

      };  // TMaster

      /* TODO */
      class TSlave
          : public TSlaveContext, Indy::Fiber::TRunnable {
        NO_COPY_SEMANTICS(TSlave);
        public:

        /* TODO */
        TSlave(TManager *manager, const Base::TFd &fd);

        /* TODO */
        virtual ~TSlave();

        /* TODO */
        virtual bool Queue() override;

        /* TODO */
        virtual bool Work() override;

        /* TODO */
        virtual void Inventory(const Base::TUuid &repo_id,
                               size_t ttl,
                               const Base::TOpt<Base::TUuid> &parent_repo_id,
                               bool is_safe,
                               const Base::TOpt<TSequenceNumber> &lowest,
                               const Base::TOpt<TSequenceNumber> &highest,
                               TSequenceNumber next_id) override;

        /* TODO */
        virtual void Index(const TIndexMapReplica &index_map_replica) override;

        /* TODO */
        virtual void PushNotifications(const TReplicationStreamer &replication_streamer) override;

        /* TODO */
        size_t ApplyCoreVectorTransactions(const std::vector<Atom::TCore> &core_vec, Atom::TCore::TArena *arena);

        /* TODO */
        virtual void TransitionToSlave() override;

        /* TODO */
        virtual void Ping();

        /* TODO */
        virtual void ScheduleSyncInventory() override;

        private:

        /* TODO */
        void SyncInventory();

        /* TODO */
        void PullUpdateRange(const Base::TUuid &repo_id, TManager::TPtr<Indy::TRepo> &repo, TSequenceNumber from, TSequenceNumber to);

        /* TODO */
        class TFlusher
            : public Io::TOutputConsumer,
              public Io::TInputProducer,
              public Disk::TInFile {
          NO_COPY_SEMANTICS(TFlusher);
          public:

          /* TODO */
          typedef Disk::TStream<Disk::Util::LogicalPageSize, Disk::Util::LogicalBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::CheckedPage, 0UL /*local cache size */> TDataInStream;
          typedef Disk::TOutStream<Disk::Util::LogicalPageSize, Disk::Util::LogicalBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::PageCheckedBlock> TDataOutStream;

          /* TODO */
          TFlusher(Disk::Util::TEngine *engine)
              : Engine(engine),
                OutStream(new TDataOutStream(HERE,
                                             Disk::Source::SlaveSlush,
                                             Engine->GetVolMan(),
                                             0UL,
                                             BlockVec,
                                             CollisionMap,
                                             Trigger,
                                             Disk::Low,
                                             true,
                                             #ifndef NDEBUG
                                             WrittenBlockSet,
                                             #endif
                                             std::bind(&TFlusher::NewBlockCb, this, std::placeholders::_1))),
                Pool(std::make_shared<Io::TPool>(Io::TPool::TArgs())) {}

          /* TODO */
          virtual ~TFlusher() {
            assert(this);
            InStream.reset();
            for (const auto &iter : BlockVec.GetSeqBlockMap()) {
              Engine->FreeSeqBlocks(iter.second.first, iter.second.second);
            }
          }

          /* TODO */
          virtual void ConsumeOutput(const std::shared_ptr<const Io::TChunk> &chunk) override;

          /* TODO */
          virtual std::shared_ptr<const Io::TChunk> TryProduceInput() override;

          /* TODO */
          const std::vector<size_t> &GetOffsetVec() const {
            assert(this);
            return OffsetVec;
          }

          /* TODO */
          void SetStoredOffset() {
            assert(this);
            OffsetVec.push_back(OutStream->GetOffset());
          }

          /* TODO */
          void Flush();

          private:

          /* TODO */
          virtual size_t GetFileLength() const override {
            assert(this);
            return BlockVec.Size() * Disk::Util::LogicalBlockSize;
          }

          /* TODO */
          virtual size_t GetStartingBlock() const override {
            assert(this);
            assert(BlockVec.Size() > 0);
            return BlockVec.Front();
          }

          /* TODO */
          virtual void ReadMeta(size_t /*offset*/, size_t &/*out*/) const override {
            throw std::logic_error("Manager::Flusher::ReadMeta should not be used.");
          }

          /* TODO */
          virtual size_t FindPageIdOfByte(size_t offset) const override {
            assert(this);
            assert(offset <= GetFileLength());
            return ((BlockVec[offset / Disk::Util::LogicalBlockSize]) * Disk::Util::PagesPerBlock) + ((offset % Disk::Util::LogicalBlockSize) / Disk::Util::LogicalPageSize);
          }

          /* TODO */
          size_t NewBlockCb(Disk::Util::TVolumeManager */*vol_man*/);

          /* TODO */
          Disk::Util::TEngine *Engine;

          /* TODO */
          Util::TBlockVec BlockVec;

          /* TODO */
          std::unordered_map<size_t, std::shared_ptr<const Disk::TBufBlock>> CollisionMap;

          /* TODO */
          Disk::TCompletionTrigger Trigger;

          #ifndef NDEBUG
          std::unordered_set<size_t> WrittenBlockSet;
          #endif

          /* TODO */
          std::unique_ptr<TDataOutStream> OutStream;

          /* TODO */
          std::vector<size_t> OffsetVec;

          /* TODO */
          std::shared_ptr<Io::TPool> Pool;

          /* TODO */
          std::unique_ptr<TDataInStream> InStream;

        };  // TFlusher

        /* TODO */
        class TToSync {
          public:

          /* TODO */
          TToSync(const Base::TUuid &repo_id,
                  size_t ttl,
                  const Base::TOpt<Base::TUuid> &parent_repo_id,
                  bool is_safe,
                  const Base::TOpt<TSequenceNumber> &lowest,
                  const Base::TOpt<TSequenceNumber> &highest,
                  TSequenceNumber next_id)
              : RepoId(repo_id),
                Ttl(ttl),
                ParentRepoId(parent_repo_id),
                IsSafe(is_safe),
                Lowest(lowest),
                Highest(highest),
                NextId(next_id) {}

          private:

          /* TODO */
          Base::TUuid RepoId;
          size_t Ttl;
          Base::TOpt<Base::TUuid> ParentRepoId;
          bool IsSafe;
          Base::TOpt<TSequenceNumber> Lowest;
          Base::TOpt<TSequenceNumber> Highest;
          TSequenceNumber NextId;

          friend class TSlave;

        };  // TToSync

        /* TODO */
        TManager *Manager;

        /* queue of repos to synchronize */
        std::vector<TToSync> ToSyncQueue;

        /* A core-vec slush buffer representing the updates we've accumulated */
        std::unique_ptr<Stig::Atom::TCoreVectorBuilder> SlushCoreVec;

        /* TODO */
        std::shared_ptr<TFlusher> Flusher;

      };  // TSlave

      /* TODO */
      virtual TRepo *ConstructRepo(const Base::TUuid &repo_id,
                                   const Base::TOpt<TTtl> &ttl,
                                   const Base::TOpt<TManager::TPtr<TRepo>> &parent_repo,
                                   bool is_safe,
                                   bool create) override;

      /* TODO */
      TRepo *ReconstructRepo(const Base::TUuid &repo_id) override;

      /* TODO */
      virtual bool CanLoad(const L0::TId &id) override;

      /* TODO */
      virtual void Delete(const L0::TId &/*id*/, L0::TSem */*sem*/) override {
        throw std::logic_error("TODO: TManager::Delete()");
      }

      /* TODO */
      virtual void Save(const L0::TId &/*id*/, const L0::TDeadline &/*deadline*/, const std::string &/*blob*/, L0::TSem */*sem*/) override {
        throw std::logic_error("TODO: TManager::Save()");
      }

      /* TODO */
      virtual bool TryLoad(const L0::TId &/*id*/, std::string &/*blob*/) override {
        throw std::logic_error("TODO: TManager::TryLoad()");
      }

      /* TODO */
      virtual void SaveRepo(TRepo *base_repo) override;

      /* TODO */
      void OnSlaveJoin(const Base::TFd &fd);

      /* TODO */
      inline virtual std::mutex &GetReplicationQueueLock() NO_THROW {
        return ReplicationLock;
      }

      /* TODO */
      virtual void Enqueue(TTransactionReplication *transaction_replication, L1::TTransaction::TReplica &&replica) NO_THROW;

      /* TODO */
      virtual void Enqueue(TRepoReplication *repo_replication) NO_THROW;

      /* TODO */
      void Enqueue(TIndexIdReplication *index_replication) NO_THROW;

      /* TODO */
      virtual TTransactionReplication *NewTransactionReplication();

      /* TODO */
      virtual void DeleteTransactionReplication(TTransactionReplication *transaction_replication) NO_THROW override;

      /* TODO */
      virtual void EnqueueDurable(TDurableReplication *durable_replication) NO_THROW override;

      /* TOOD */
      virtual TDurableReplication *NewDurableReplication(const Base::TUuid &id, const TTtl &ttl, const std::string &serialized_form) const override;

      /* TODO */
      virtual void DeleteDurableReplication(TDurableReplication *durable_replication) NO_THROW override;

      /* TODO */
      void Demote();

      /* TODO */
      void PromoteSolo(const Base::TFd &fd);

      /* TODO */
      void PromoteSlave();

      /* TODO */
      void AugmentViewMapWithDiskLayers(TMaster::TViewDef &view_def, const std::unique_ptr<Indy::TRepo::TView> &view) const;

      /* TODO */
      virtual void ForEachScheduler(const std::function<bool (Fiber::TRunner *)> &cb) const {
        assert(this);
        ForEachSchedulerCb(cb);
      }

      /* TODO */
      TManager::TPtr<Indy::TRepo> SystemRepo;

      /* TODO */
      TState State;

      /* TODO */
      std::shared_ptr<TCommonContext> Context;
      std::mutex ContextLock;

      bool SlaveNotifiedFinish;
      std::mutex SlaveNotifyLock;
      std::condition_variable SlaveNotifyCond;

      /* TODO */
      std::function<void (const std::shared_ptr<std::function<void (const Base::TFd &)>> &)> WaitForSlave;

      /* TODO */
      std::function<void (TState)> StateChangeCb;

      /* TODO */
      bool ReplicationRead, ReplicationWork;

      /* TODO */
      TReplicationQueue ReplicationQueue;
      std::mutex ReplicationLock;

      /* TODO */
      Base::TFd ReplicationQueueEpollFd;
      std::mutex ReplicationQueueEpollLock;
      epoll_event ReplicationQueueEvent;
      Base::TEventSemaphore ReplicationQueueSem;

      /* TODO */
      Base::TFd ReplicationWorkEpollFd;
      std::mutex ReplicationWorkEpollLock;
      epoll_event ReplicationWorkEvent;
      Base::TEventSemaphore ReplicationWorkSem;

      /* TODO */
      Base::TFd ReplicationEpollFd;
      std::mutex ReplicationEpollLock;
      epoll_event ReplicationEvent;
      Base::TEventSemaphore ReplicationSem;
      Base::TTime TimeToNextReplication;

      /* TODO */
      size_t ReplicationDelay;

      /* TODO */
      std::function<void (const Base::TUuid &, const Base::TUuid &, const Base::TUuid &)> UpdateReplicationNotificationCb;

      /* TODO */
      std::function<void (const Base::TUuid &idx_id, const Indy::TKey &key, const Indy::TKey &val)> OnReplicateIndexIdCb;

      /* TODO */
      std::function<void (const std::function<void (const Base::TUuid &idx_id, const Indy::TKey &key, const Indy::TKey &val)> &)> ForEachIndexIdCb;

      /* TODO */
      std::function<void (const std::function<bool (Fiber::TRunner *)> &)> ForEachSchedulerCb;

      /* TODO */
      std::shared_ptr<Durable::TManager> DurableManager;

      /* TODO */
      Indy::Fiber::TRunner *BGFastRunner;

      /* TODO */
      size_t ReplicationSyncSlaveBufSizeBytes;

      /* TODO */
      bool AllowFileSync;

      /* TODO */
      std::unordered_map<Base::TUuid, std::unique_ptr<Indy::TRepo::TView>> SlaveSyncViewMap;

      friend class Stig::Server::TServer;

    };  // TManager

    namespace L0 {

      /* TODO */
      template<>
      template<>
      inline TManager::TPtr<Indy::TRepo>::TPtr(const TPtr<L0::TManager::TRepo> &that) {
        assert(&that);
        if ((SomeObj = dynamic_cast<Indy::TRepo *>(that.SomeObj)) != nullptr) {
          SomeObj->OnPtrAcquire();
        }
      }

    }  // L0

    /***************
      *** inline ***
      *************/

    inline TManager::TPtr<Indy::TRepo> TManager::NewSafeRepo(const Base::TUuid &repo_id,
                                                             const Base::TOpt<TTtl> &ttl) {
      return NewSafeRepo(repo_id, ttl, Base::TOpt<TManager::TPtr<L0::TManager::TRepo>>());
    }

    inline TManager::TPtr<Indy::TRepo> TManager::NewSafeRepo(const Base::TUuid &repo_id,
                                                             const Base::TOpt<TTtl> &ttl,
                                                             const TManager::TPtr<Indy::TRepo> &parent_repo) {
      return NewSafeRepo(repo_id, ttl, Base::TOpt<L0::TManager::TPtr<L0::TManager::TRepo>>(parent_repo));
    }

    inline TManager::TPtr<Indy::TRepo> TManager::NewFastRepo(const Base::TUuid &repo_id,
                                                             const Base::TOpt<TTtl> &ttl) {
      return NewFastRepo(repo_id, ttl, Base::TOpt<TManager::TPtr<L0::TManager::TRepo>>());
    }

    inline TManager::TPtr<Indy::TRepo> TManager::NewFastRepo(const Base::TUuid &repo_id,
                                                              const Base::TOpt<TTtl> &ttl,
                                                              const TManager::TPtr<Indy::TRepo> &parent_repo) {
      return NewFastRepo(repo_id, ttl, Base::TOpt<L0::TManager::TPtr<L0::TManager::TRepo>>(parent_repo));
    }

    inline TManager::TPtr<Indy::TRepo> TManager::GetRepo(const Base::TUuid &repo_id,
                                                          const Base::TOpt<TTtl> &ttl,
                                                          const Base::TOpt<TManager::TPtr<L0::TManager::TRepo>> &parent_repo,
                                                          bool is_safe,
                                                          bool create) {
      assert(this);
      return create ? OpenOrCreate(repo_id, ttl, parent_repo, is_safe) : ForceOpenRepo(repo_id);
    }

    inline TManager::TPtr<Indy::TRepo> TManager::ForceGetRepo(const Base::TUuid &repo_id) {
      assert(this);
      return ForceOpenRepo(repo_id);
    }

    inline TManager::TPtr<Indy::TRepo> TManager::GetSystemRepo() const {
      assert(this);
      return SystemRepo;
    }

    inline void TManager::TMaster::Ping() {}

    inline void TManager::TSlave::Ping() {}

  }  // Indy

}  // Stig
