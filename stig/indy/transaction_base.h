/* <stig/indy/transaction_base.h>

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

#include <stig/indy/manager_base.h>
#include <stig/indy/repo.h>
#include <stig/indy/status.h>

namespace Stig {

  namespace Indy {

    /* Forward Delcarations. */
    class TTransactionReplication;
    class TReplicationStreamer;

    namespace L1 {

      /* Forward Delcarations. */
      class TManager;

      /* TODO */
      class TTransaction {
        NO_COPY_SEMANTICS(TTransaction);
        public:

        /* TODO */
        typedef InvCon::UnorderedList::TMembership<TTransaction, TManager> TManagerMembership;

        /* TODO */
        bool Push(const L0::TManager::TPtr<TRepo> &repo,
                  const std::shared_ptr<TUpdate> &update,
                  const Base::TOpt<TSequenceNumber> &ensure_or_discard = Base::TOpt<TSequenceNumber>());

        /* TODO */
        bool Pop(const L0::TManager::TPtr<TRepo> &repo,
                 const Base::TOpt<TSequenceNumber> &ensure_or_discard = Base::TOpt<TSequenceNumber>());

        /* TODO */
        bool Fail(const L0::TManager::TPtr<TRepo> &repo,
                  const Base::TOpt<TSequenceNumber> &follow_or_discard = Base::TOpt<TSequenceNumber>());

        /* TODO */
        bool Pause(const L0::TManager::TPtr<TRepo> &repo,
                   const Base::TOpt<TSequenceNumber> &follow_or_discard = Base::TOpt<TSequenceNumber>());

        /* TODO */
        bool UnPause(const L0::TManager::TPtr<TRepo> &repo,
                     const Base::TOpt<TSequenceNumber> &follow_or_discard = Base::TOpt<TSequenceNumber>());

        /* TODO */
        const std::shared_ptr<TUpdate> &Peek(const L0::TManager::TPtr<TRepo> &repo);

        /* TODO */
        void Prepare();

        /* Mark the prepared action to be committed.  The commitment will occur when this object destructs.
           If there is no prepared action, do nothing. */
        void CommitAction() NO_THROW;

        /* Discard the prepared action, if any. */
        void DiscardAction() NO_THROW;

        /* TODO */
        class TTransactionCompletion {
          NO_COPY_SEMANTICS(TTransactionCompletion);
          public:

          enum TTransactionResult {
            Completed,
            Failed
          };

          /* TODO */
          typedef InvCon::UnorderedList::TMembership<TTransactionCompletion, TManager> TManagerMembership;

          /* TODO */
          TTransactionCompletion(TManager *manager);

          /* TODO */
          ~TTransactionCompletion();

          /* TODO */
          void RegisterCompletion();

          /* TODO */
          void RegisterFailure();

          /* TODO */
          void SetWaitFor(size_t wait_for);

          /* TODO */
          inline void SetCb(const std::shared_ptr<std::function<void (TTransactionResult)>> &cb);

          private:

          /* TODO */
          size_t WaitFor;

          /* TODO */
          size_t NumCompleted;
          std::mutex Mutex;

          /* TODO */
          std::shared_ptr<std::function<void (TTransactionResult)>> Cb;

          /* TODO */
          TManagerMembership::TImpl ManagerMembership;

        };  // TTransactionCompletion

        /* TODO */
        class TReplica {
          public:

          /* Forward Declarations. */
          class TMutation;

          /* TODO */
          typedef std::list<TMutation> TMutationList;

          /* TODO */
          class TMutation {
            public:

            enum TKind {
              Pusher,
              Popper,
              Failer,
              Pauser,
              UnPauser
            };

            /* TODO */
            class TUpdate {
              public:

              /* TODO */
              typedef std::vector<std::pair<TIndexKey, Atom::TCore>> TOpByKey;

              /* TODO */
              TUpdate();

              /* TODO */
              TUpdate(TUpdate &&update);

              /* TODO */
              TUpdate(const TUpdate &update);

              /* TODO */
              TUpdate(const Stig::Indy::TUpdate *update);

              /* TODO */
              ~TUpdate();

              /* TODO */
              TUpdate &operator=(TUpdate &&that);

              /* TODO */
              TUpdate &operator=(const TUpdate &that);

              /* TODO */
              inline const Atom::TCore &GetMetadata() const;

              /* TODO */
              inline const Atom::TCore &GetId() const;

              /* TODO */
              inline const TOpByKey &GetOpByKey() const;

              /* TODO */
              inline const std::shared_ptr<Atom::TSuprena> &GetSuprena() const;

              /* TODO */
              inline void SetArena(const std::shared_ptr<Atom::TSuprena> &suprena);

              /* TODO */
              void Write(Io::TBinaryOutputStream &strm, const Atom::TCore::TRemap &remap) const;

              /* TODO */
              void Read(Io::TBinaryInputStream &strm, const Atom::TCore::TRemap &remap);

              private:

              /* TODO */
              std::shared_ptr<Atom::TSuprena> Suprena;

              /* TODO */
              Atom::TCore Metadata;

              /* TODO */
              Atom::TCore Id;

              /* TODO */
              TOpByKey OpByKey;

              /* TODO */
              friend class Stig::Indy::TReplicationStreamer;

            };  // TUpdate

            /* TODO */
            TMutation(TMutation &&mutation);

            /* TODO */
            TMutation(const TMutation &mutation);

            /* TODO */
            TMutation(TKind kind, const Base::TUuid &repo_id, const Base::TOpt<TSequenceNumber> &seq_num);

            /* TODO */
            TMutation(TKind kind, const Base::TUuid &repo_id, const Stig::Indy::TUpdate *update, const Base::TOpt<TSequenceNumber> &seq_num);

            /* TODO */
            TMutation(TKind kind, const Base::TUuid &repo_id, TUpdate &&update, const Base::TOpt<TSequenceNumber> &seq_num);

            /* TODO */
            virtual ~TMutation();

            /* TODO */
            TMutation &operator=(TMutation &&that);

            /* TODO */
            TMutation &operator=(const TMutation &that);

            /* TODO */
            inline TKind GetKind() const;

            /* TODO */
            inline const Base::TUuid &GetRepoId() const;

            /* TODO */
            inline const Base::TOpt<TSequenceNumber> &GetSequenceNumber() const;

            /* TODO */
            inline const TUpdate &GetUpdate() const;

            /* TODO */
            inline TUpdate &GetUpdate();

            /* TODO */
            inline void SetSequenceNumber(Base::TOpt<TSequenceNumber> seq_num) NO_THROW;

            /* TODO */
            inline TSequenceNumber &GetNextUpdate() NO_THROW;

            private:

            /* TODO */
            TKind Kind;

            /* TODO */
            Base::TUuid RepoId;

            /* TODO */
            TUpdate Update;

            /* TODO */
            Base::TOpt<TSequenceNumber> SequenceNumber;

            /* TODO */
            TSequenceNumber NextUpdate;

          };  // TMutation

          /* TODO */
          TReplica(TReplica &&replica);

          /* TODO */
          TReplica(const TReplica &replica);

          /* TODO */
          TReplica();

          /* TODO */
          ~TReplica();

          /* TODO */
          TReplica &operator=(TReplica &&that);

          /* TODO */
          TReplica &operator=(const TReplica &that);

          /* TODO */
          TMutation *Push(const Base::TUuid &repo_id, const TUpdate *update);

          /* TODO */
          TMutation *Pop(const Base::TUuid &repo_id, const Base::TOpt<TSequenceNumber> &seq_num);

          /* TODO */
          TMutation *Fail(const Base::TUuid &repo_id, const Base::TOpt<TSequenceNumber> &seq_num);

          /* TODO */
          TMutation *Pause(const Base::TUuid &repo_id, const Base::TOpt<TSequenceNumber> &seq_num);

          /* TODO */
          TMutation *UnPause(const Base::TUuid &repo_id, const Base::TOpt<TSequenceNumber> &seq_num);

          /* TODO */
          void Reset();

          /* TODO */
          inline TMutationList &GetMutationList() const;

          /* TODO */
          inline size_t GetSize() const;

          private:

          /* TODO */
          mutable TMutationList MutationList;

          /* TODO */
          friend class TManager;
          friend class Stig::Indy::TReplicationStreamer;

        };  // TReplica

        /* TODO */
        static void *operator new(size_t size) {
          return Pool.Alloc(size);
        }

        /* TODO */
        static void operator delete(void *ptr, size_t) {
          Pool.Free(ptr);
        }

        /* TODO */
        static void InitTransactionPool(size_t num_obj) {
          Pool.Init(num_obj);
        }

        /* TODO */
        static void InitTransactionMutationPool(size_t num_obj) {
          TMutation::Pool.Init(num_obj);
        }

        private:

        /* Forward Declarations. */
        class TMutation;

        /* TODO */
        typedef InvCon::UnorderedMultimap::TCollection<TTransaction, TMutation, Base::TUuid> TMutationCollection;

        /* TODO */
        TTransaction(TManager *manager, bool should_replicate);

        /* TODO */
        ~TTransaction() NO_THROW;

        /* TODO */
        class TMutation {
          NO_COPY_SEMANTICS(TMutation);
          public:

          enum TKind {
            Pusher,
            Popper,
            StatusChanger
          };

          /* TODO */
          typedef InvCon::UnorderedMultimap::TMembership<TMutation, TTransaction, Base::TUuid> TTransactionMembership;

          /* TODO */
          virtual ~TMutation() NO_THROW;

          /* TODO */
          virtual TKind GetKind() const = 0;

          /* TODO */
          inline const Base::TUuid &GetRepoId() const;

          /* TODO */
          TMutation *TryGetNextMember() const {
            assert(this);
            return TransactionMembership.TryGetNextMember();
          }

          /* TODO */
          static void *operator new(size_t size) {
            return Pool.Alloc(size);
          }

          /* TODO */
          static void operator delete(void *ptr, size_t) {
            Pool.Free(ptr);
          }

          protected:

          /* TODO */
          TMutation(TTransaction *transaction, const L0::TManager::TPtr<TRepo> &repo);

          /* TODO */
          TTransactionMembership::TImpl TransactionMembership;

          /* TODO */
          L0::TManager::TPtr<TRepo> Repo;

          /* TODO */
          mutable TReplica::TMutation *MyMutation;

          /* TODO */
          static Util::TPool Pool;

          /* TODO */
          friend class Server::TIndyReporter;
          friend class TTransaction;

        };  // TMutation

        /* TODO */
        class TPusher
            : public TMutation {
          NO_COPY_SEMANTICS(TPusher);
          public:

          /* TODO */
          TPusher(TTransaction *transaction, const L0::TManager::TPtr<TRepo> &repo, const std::shared_ptr<TUpdate> &update);

          /* TODO */
          virtual ~TPusher() NO_THROW;

          /* TODO */
          virtual TKind GetKind() const;

          private:

          /* TODO */
          TUpdate *Update;

          /* TODO */
          friend class TTransaction;

        };  // TPusher

        /* TODO */
        class TPopper
            : public TMutation {
          NO_COPY_SEMANTICS(TPopper);
          public:

          /* TODO */
          enum TState {
            Peek,
            Pop,
            Fail
          };

          /* TODO */
          TPopper(TTransaction *transaction, const L0::TManager::TPtr<TRepo> &repo, TState state);

          /* TODO */
          virtual ~TPopper() NO_THROW;

          /* TODO */
          virtual TKind GetKind() const;

          /* TODO */
          TState GetState() const;

          /* TODO */
          void DoPeek() const;

          /* TODO */
          void SetState(TState state);

          /* TODO */
          inline const std::shared_ptr<TUpdate> &GetUpdate() const;

          private:

          /* TODO */
          TState State;

          /* TODO */
          mutable std::shared_ptr<TRepo::TView> View;

          /* TODO */
          mutable std::shared_ptr<TUpdate> Update;

          /* TODO */
          friend class TTransaction;

        };  // TPopper

        /* TODO */
        class TStatusChanger
            : public TMutation {
          NO_COPY_SEMANTICS(TStatusChanger);
          public:

          /* TODO */
          TStatusChanger(TTransaction *transaction, const L0::TManager::TPtr<TRepo> &repo, TStatus status);

          /* TODO */
          virtual ~TStatusChanger() NO_THROW;

          /* TODO */
          virtual TKind GetKind() const;

          private:

          /* TODO */
          TStatus Status;

          /* TODO */
          friend class TTransaction;

        };  // TStatusChanger

        /* TODO */
        bool GetCommitFlag() const;

        /* TODO */
        TManager *Manager;

        /* TODO */
        TManagerMembership::TImpl ManagerMembership;

        /* TODO */
        mutable TMutationCollection::TImpl MutationCollection;

        /* TODO */
        bool CommitFlag;

        /* TODO */
        bool ShouldReplicate;

        /* TODO */
        bool Prepared;

        /* TODO */
        bool EnsureOrDiscard;

        /* TODO */
        TReplica Replica;

        /* TODO */
        TTransactionReplication *TransactionReplication;

        /* TODO */
        TTransactionCompletion *TransactionCompletion;

        /* TODO */
        static Util::TPool Pool;

        /* TODO */
        friend class TManager;
        friend class Server::TIndyReporter;

      };  // TTransaction

      /* TODO */
      class TManager
          : public L0::TManager {
        NO_COPY_SEMANTICS(TManager);
        public:

        /* TODO */
        typedef InvCon::UnorderedList::TCollection<TManager, TTransaction> TTransactionCollection;
        typedef InvCon::UnorderedList::TCollection<TManager, TTransaction::TTransactionCompletion> TTransactionCompletionCollection;

        /* TODO */
        std::unique_ptr<TTransaction, std::function<void (TTransaction *)>> NewTransaction(bool should_replicate = true);

        protected:

        /* TODO */
        TManager(Disk::Util::TEngine *engine,
                 size_t merge_mem_delay,
                 size_t merge_disk_delay,
                 bool allow_tailing,
                 bool no_realtime,
                 size_t layer_cleaning_interval_milliseconds,
                 Base::TScheduler *scheduler,
                 size_t block_slots_available_per_merger,
                 size_t max_repo_cache_size,
                 size_t temp_file_consol_thresh,
                 const std::vector<size_t> &merge_mem_cores,
                 const std::vector<size_t> &merge_disk_cores,
                 bool create_new);

        /* TODO */
        virtual ~TManager();

        /* TODO */
        virtual std::mutex &GetReplicationQueueLock() NO_THROW = 0;

        /* TODO */
        virtual void Enqueue(TTransactionReplication *transaction_replication, TTransaction::TReplica &&replica) NO_THROW = 0;

        /* TODO */
        virtual TTransactionReplication *NewTransactionReplication() = 0;

        /* TODO */
        virtual void DeleteTransactionReplication(TTransactionReplication *transaction_replication) NO_THROW = 0;

        private:

        /* TODO */
        void OnCloseTransaction(TTransaction *transaction);

        /* TODO */
        mutable TTransactionCollection::TImpl TransactionCollection;
        std::mutex TransactionLock;

        /* TODO */
        mutable TTransactionCompletionCollection::TImpl TransactionCompletionCollection;
        std::mutex TransactionCompletionLock;

        /* TODO */
        std::function<void (TTransaction *)> OnCloseTransactionCb;

        /* access to Enqueue */
        friend class TTransaction;

      };  // TManager

      /* TODO */
      inline void TTransaction::CommitAction() NO_THROW {
        assert(this);
        CommitFlag = true;
      }

      /* TODO */
      inline void TTransaction::DiscardAction() NO_THROW {
        assert(this);
        CommitFlag = false;
      }

      /* TODO */
      inline const Atom::TCore &TTransaction::TReplica::TMutation::TUpdate::GetMetadata() const {
        assert(this);
        return Metadata;
      }

      /* TODO */
      inline const Atom::TCore &TTransaction::TReplica::TMutation::TUpdate::GetId() const {
        assert(this);
        return Id;
      }

      /* TODO */
      inline const TTransaction::TReplica::TMutation::TUpdate::TOpByKey &TTransaction::TReplica::TMutation::TUpdate::GetOpByKey() const {
        assert(this);
        return OpByKey;
      }

      /* TODO */
      inline const std::shared_ptr<Atom::TSuprena> &TTransaction::TReplica::TMutation::TUpdate::GetSuprena() const {
        assert(this);
        return Suprena;
      }

      /* TODO */
      inline void TTransaction::TReplica::TMutation::TUpdate::SetArena(const std::shared_ptr<Atom::TSuprena> &suprena) {
        assert(this);
        Suprena = suprena;
      }

      /* TODO */
      inline TTransaction::TReplica::TMutation::TKind TTransaction::TReplica::TMutation::GetKind() const {
        assert(this);
        return Kind;
      }

      /* TODO */
      inline const Base::TUuid &TTransaction::TReplica::TMutation::GetRepoId() const {
        assert(this);
        return RepoId;
      }

      /* TODO */
      inline const Base::TOpt<TSequenceNumber> &TTransaction::TReplica::TMutation::GetSequenceNumber() const {
        assert(this);
        return SequenceNumber;
      }

      /* TODO */
      inline const TTransaction::TReplica::TMutation::TUpdate &TTransaction::TReplica::TMutation::GetUpdate() const {
        assert(this);
        assert(Kind == Pusher);
        return Update;
      }

      /* TODO */
      inline TTransaction::TReplica::TMutation::TUpdate &TTransaction::TReplica::TMutation::GetUpdate() {
        assert(this);
        assert(Kind == Pusher);
        return Update;
      }

      /* TODO */
      inline void TTransaction::TReplica::TMutation::SetSequenceNumber(Base::TOpt<TSequenceNumber> seq_num) NO_THROW {
        assert(this);
        SequenceNumber = seq_num;
      }

      /* TODO */
      inline TSequenceNumber &TTransaction::TReplica::TMutation::GetNextUpdate() NO_THROW {
        assert(this);
        return NextUpdate;
      }

      /* TODO */
      inline TTransaction::TReplica::TMutationList &TTransaction::TReplica::GetMutationList() const {
        assert(this);
        return MutationList;
      }

      /* TODO */
      inline size_t TTransaction::TReplica::GetSize() const {
        assert(this);
        size_t count = 0U;
        for (auto iter = MutationList.begin(); iter != MutationList.end(); ++iter, ++count);
        return count;
      }

      /* TODO */
      inline const Base::TUuid &TTransaction::TMutation::GetRepoId() const {
        assert(this);
        return Repo->GetId();
      }

      /* TODO */
      inline TTransaction::TMutation::TKind TTransaction::TPusher::GetKind() const {
        assert(this);
        return TMutation::Pusher;
      }

      /* TODO */
      inline TTransaction::TMutation::TKind TTransaction::TPopper::GetKind() const {
        assert(this);
        return TMutation::Popper;
      }

      /* TODO */
      inline TTransaction::TPopper::TState TTransaction::TPopper::GetState() const {
        assert(this);
        return State;
      }

      /* TODO */
      inline const std::shared_ptr<TUpdate> &TTransaction::TPopper::GetUpdate() const {
        assert(this);
        return Update;
      }

      /* TODO */
      inline void TTransaction::TPopper::SetState(TState state) {
        assert(this);
        State = state;
      }

      /* TODO */
      inline TTransaction::TMutation::TKind TTransaction::TStatusChanger::GetKind() const {
        assert(this);
        return TMutation::StatusChanger;
      }

      /* TODO */
      inline bool TTransaction::GetCommitFlag() const {
        assert(this);
        return CommitFlag;
      }

      /* TODO */
      inline void TTransaction::TTransactionCompletion::SetCb(const std::shared_ptr<std::function<void (TTransactionResult)>> &cb) {
        assert(this);
        Cb = cb;
      }

    }  // L0

  }  // Indy

}  // Stig
