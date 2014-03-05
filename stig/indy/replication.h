/* <stig/indy/replication.h>

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

#include <stig/atom/core_vector.h>
#include <stig/atom/core_vector_builder.h>
#include <stig/indy/transaction_base.h>
#include <stig/time.h>

namespace Stig {

  namespace Indy {

    /* TODO */
    class TIndexMapReplica {
      public:

      /* TODO */
      TIndexMapReplica() {}

      /* TODO */
      void Push(const Base::TUuid &index_id, const Indy::TKey &key, const Indy::TKey &val) {
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        CoreVecBuilder.Push(index_id);
        CoreVecBuilder.PushState(key.GetState(state_alloc));
        CoreVecBuilder.PushState(val.GetState(state_alloc));
      }

      /* TODO */
      void Write(Io::TBinaryOutputStream &stream) const {
        CoreVecBuilder.Write(stream);
      }

      /* TODO */
      void Read(Io::TBinaryInputStream &stream) {
        assert(!CoreVec);
        CoreVec = std::unique_ptr<Atom::TCoreVector>(new Atom::TCoreVector(stream));
      }

      /* TODO */
      const Atom::TCoreVector &GetCoreVec() const {
        assert(this);
        assert(CoreVec);
        return *CoreVec;
      }

      private:

      Atom::TCoreVectorBuilder CoreVecBuilder;
      std::unique_ptr<Atom::TCoreVector> CoreVec;

    };  // TIndexMapReplica

    enum TTransactionAction {
      Push,
      Pop,
      Fail,
      Pause,
      UnPause
    };

    /* TODO */
    class TReplicationQueue {
      NO_COPY_SEMANTICS(TReplicationQueue);
      public:

      /* Forward Declarations. */
      class TReplicationItem;

      /* TODO */
      typedef InvCon::UnorderedList::TCollection<TReplicationQueue, TReplicationItem> TItemCollection;

      /* TODO */
      class TReplicationItem {
        NO_COPY_SEMANTICS(TReplicationItem);
        public:

        /* TODO */
        typedef InvCon::UnorderedList::TMembership<TReplicationItem, TReplicationQueue> TQueueMembership;

        /* TODO */
        enum TKind {
          Repo,
          Durable,
          Transaction,
          IndexId
        };

        /* TODO */
        virtual ~TReplicationItem();

        /* TODO */
        virtual TKind GetKind() const = 0;

        protected:

        /* TODO */
        TReplicationItem();

        private:

        /* TODO */
        TQueueMembership::TImpl QueueMembership;

        /* TODO */
        friend class TReplicationQueue;

      };  // TReplicationItem

      /* TODO */
      TReplicationQueue();

      /* TODO */
      ~TReplicationQueue();

      /* TODO */
      void Insert(TReplicationItem *item) NO_THROW;

      /* TODO */
      void Swap(TReplicationQueue &that);

      /* TODO */
      inline void Clear();

      /* TODO */
      TItemCollection *GetItemCollection() const;

      /* TODO */
      inline bool IsEmpty() const;

      private:

      /* TODO */
      mutable TItemCollection::TImpl ItemCollection;

    };  // TReplicationQueue

    /* TODO */
    class TRepoReplication
        : public TReplicationQueue::TReplicationItem {
      NO_COPY_SEMANTICS(TRepoReplication);
      public:

      /* TODO */
      class TReplica {
        public:

        /* TODO */
        inline TReplica(const Base::TUuid &id, bool is_safe, const TTtl &ttl, const Base::TOpt<Base::TUuid> &parent_id);

        /* TODO */
        inline TReplica(const TRepoReplication &replication_obj);

        /* TODO */
        inline TReplica(const TReplica &that);

        /* TODO */
        inline TReplica(TReplica &&that);

        /* TODO */
        inline const TTtl &GetTtl() const;

        /* TODO */
        inline const Base::TUuid &GetId() const;

        /* TODO */
        inline bool GetIsSafe() const;

        /* TODO */
        inline const Base::TOpt<Base::TUuid> &GetOptParentId() const;

        private:

        /* TODO */
        const TTtl Ttl;

        /* TODO */
        const Base::TUuid Id;

        /* TODO */
        const bool IsSafe;

        /* TODO */
        const Base::TOpt<Base::TUuid> OptParentId;

      };  // TReplica

      /* TODO */
      TRepoReplication(const Base::TUuid &repo_id, bool is_safe, const TTtl &ttl, const Base::TOpt<Base::TUuid> &opt_parent_repo_id);

      /* TODO */
      virtual ~TRepoReplication();

      /* TODO */
      inline virtual TKind GetKind() const;

      /* TODO */
      inline const Base::TUuid &GetId() const;

      /* TODO */
      inline const TTtl &GetTtl() const;

      /* TODO */
      inline bool GetIsSafe() const;

      /* TODO */
      inline const Base::TOpt<Base::TUuid> &GetOptParentRepoId() const;

      private:

      /* TODO */
      const TTtl Ttl;

      /* TODO */
      const Base::TUuid RepoId;

      /* TODO */
      const bool IsSafe;

      /* TODO */
      const Base::TOpt<Base::TUuid> OptParentRepoId;

    };  // TRepoReplication

    /* TODO */
    class TDurableReplication
        : public TReplicationQueue::TReplicationItem {
      NO_COPY_SEMANTICS(TDurableReplication);
      public:

      /* TODO */
      class TReplica {
        public:

        /* TODO */
        inline TReplica(const Base::TUuid &id, const TTtl &ttl, const std::string &serialized_obj);

        /* TODO */
        inline TReplica(const TDurableReplication &replication_obj);

        /* TODO */
        inline TReplica(const TReplica &that);

        /* TODO */
        inline TReplica(TReplica &&that);

        /* TODO */
        inline const TTtl &GetTtl() const;

        /* TODO */
        inline const Base::TUuid &GetId() const;

        /* TODO */
        inline const std::string &GetSerializedObj() const;

        private:

        /* TODO */
        const TTtl Ttl;

        /* TODO */
        const Base::TUuid Id;

        /* TODO */
        const std::string SerializedObj;

      };  // TReplica

      /* TODO */
      TDurableReplication(const Base::TUuid &durable_id, const TTtl &ttl, const std::string &serialized_obj);

      /* TODO */
      virtual ~TDurableReplication();

      /* TODO */
      inline virtual TKind GetKind() const;

      /* TODO */
      inline const Base::TUuid &GetId() const;

      /* TODO */
      inline const TTtl &GetTtl() const;

      /* TODO */
      inline const std::string &GetSerializedObj() const;

      private:

      /* TODO */
      const TTtl DurableTtl;

      /* TODO */
      const Base::TUuid DurableId;

      /* TODO */
      const std::string SerializedObj;

    };  // TDurableReplication

    /* TODO */
    class TIndexIdReplication
        : public TReplicationQueue::TReplicationItem {
      NO_COPY_SEMANTICS(TIndexIdReplication);
      public:

      /* TODO */
      class TReplica {
        public:

        /* TODO */
        inline TReplica(const Base::TUuid &id, const Indy::TKey &key, const Indy::TKey &val);

        /* TODO */
        inline TReplica(const TIndexIdReplication &replication_obj);

        /* TODO */
        inline TReplica(const TReplica &that);

        /* TODO */
        inline TReplica(TReplica &&that);

        /* TODO */
        inline const Base::TUuid &GetId() const;

        /* TODO */
        inline const Indy::TKey &GetKey() const;

        /* TODO */
        inline const Indy::TKey &GetVal() const;

        private:

        /* TODO */
        const Base::TUuid Id;

        /* TODO */
        Indy::TKey Key;
        Indy::TKey Val;

        /* TODO */
        Atom::TSuprena Suprena;

      };  // TReplica

      /* TODO */
      TIndexIdReplication(const Base::TUuid &id, const Indy::TKey &key, const Indy::TKey &val);

      /* TODO */
      virtual ~TIndexIdReplication();

      /* TODO */
      inline virtual TKind GetKind() const;

      /* TODO */
      inline const Base::TUuid &GetId() const;

      /* TODO */
      inline const Indy::TKey &GetKey() const;

      /* TODO */
      inline const Indy::TKey &GetVal() const;

      private:

      /* TODO */
      const Base::TUuid Id;

      /* TODO */
      Atom::TSuprena Suprena;

      /* TODO */
      Indy::TKey Key;
      Indy::TKey Val;

    };  // TIndexIdReplication

    /* TODO */
    class TTransactionReplication
          : public TReplicationQueue::TReplicationItem {
      NO_COPY_SEMANTICS(TTransactionReplication);
      public:

      TTransactionReplication();

      /* TODO */
      TTransactionReplication(L1::TTransaction::TReplica &&replica);

      /* TODO */
      virtual ~TTransactionReplication();

      /* TODO */
      inline void SwapReplica(L1::TTransaction::TReplica &&replica);

      /* TODO */
      inline virtual TKind GetKind() const;

      /* TODO */
      inline const L1::TTransaction::TReplica &GetReplica() const;

      private:

      /* TODO */
      L1::TTransaction::TReplica Replica;

    };  // TTransactionReplication

    class TReplicationStreamer {
      public:

      /* TODO */
      TReplicationStreamer();

      /* TODO */
      ~TReplicationStreamer();

      /* TODO */
      void Write(Io::TBinaryOutputStream &stream) const;

      /* TODO */
      void Read(Io::TBinaryInputStream &stream);

      /* TODO */
      void PushIndexId(const TIndexIdReplication &index_replica);

      /* TODO */
      void PushTransaction(const L1::TTransaction::TReplica &replica);

      /* TODO */
      void PushDurable(const TDurableReplication &durable_replica);

      /* TODO */
      void PushRepo(const TRepoReplication &repo_replica);

      /* TODO */
      inline const Atom::TCoreVector &GetIndexIdVec() const {
        assert(this);
        assert(IndexIdVector);
        return *IndexIdVector;
      }

      /* TODO */
      inline const Atom::TCoreVector &GetRepoVec() const {
        assert(this);
        assert(RepoVector);
        return *RepoVector;
      }

      /* TODO */
      inline const Atom::TCoreVector &GetDurableVec() const {
        assert(this);
        assert(DurableVector);
        return *DurableVector;
      }

      /* TODO */
      inline const Atom::TCoreVector &GetTransactionVec() const {
        assert(this);
        assert(TransactionVector);
        return *TransactionVector;
      }

      /* TODO */
      inline bool IsEmpty() const {
        return IndexIdBuilder.GetCores().empty() && RepoBuilder.GetCores().empty() && DurableBuilder.GetCores().empty() && TransactionBuilder.GetCores().empty();
      }

      private:

      /* TODO */
      Atom::TCoreVectorBuilder IndexIdBuilder;

      /* TODO */
      Atom::TCoreVectorBuilder RepoBuilder;

      /* TODO */
      Atom::TCoreVectorBuilder DurableBuilder;

      /* TODO */
      Atom::TCoreVectorBuilder TransactionBuilder;

      /* TODO */
      std::unique_ptr<Atom::TCoreVector> IndexIdVector;

      /* TODO */
      std::unique_ptr<Atom::TCoreVector> RepoVector;

      /* TODO */
      std::unique_ptr<Atom::TCoreVector> DurableVector;

      /* TODO */
      std::unique_ptr<Atom::TCoreVector> TransactionVector;

    };  // TReplicationStreamer

    /***************
      *** INLINE ***
      *************/

    inline bool TReplicationQueue::IsEmpty() const {
      assert(this);
      return ItemCollection.IsEmpty();
    }

    inline void TReplicationQueue::Insert(TReplicationItem *item) NO_THROW {
      assert(this);
      ItemCollection.Insert(&item->QueueMembership);
    }

    inline void TReplicationQueue::Clear() {
      assert(this);
      ItemCollection.DeleteEachMember();
    }

    inline TReplicationQueue::TItemCollection *TReplicationQueue::GetItemCollection() const {
      assert(this);
      return &ItemCollection;
    }

    inline TReplicationQueue::TReplicationItem::TKind TRepoReplication::GetKind() const {
      assert(this);
      return TKind::Repo;
    }

    inline TReplicationQueue::TReplicationItem::TKind TDurableReplication::GetKind() const {
      assert(this);
      return TKind::Durable;
    }

    inline const Base::TUuid &TDurableReplication::GetId() const {
      assert(this);
      return DurableId;
    }

    inline const TTtl &TDurableReplication::GetTtl() const {
      assert(this);
      return DurableTtl;
    }

    inline const std::string &TDurableReplication::GetSerializedObj() const {
      assert(this);
      return SerializedObj;
    }

    inline TReplicationQueue::TReplicationItem::TKind TIndexIdReplication::GetKind() const {
      assert(this);
      return TKind::IndexId;
    }

    inline const Base::TUuid &TIndexIdReplication::GetId() const {
      assert(this);
      return Id;
    }

    inline const Indy::TKey &TIndexIdReplication::GetKey() const {
      assert(this);
      return Key;
    }

    inline const Indy::TKey &TIndexIdReplication::GetVal() const {
      assert(this);
      return Val;
    }

    inline TReplicationQueue::TReplicationItem::TKind TTransactionReplication::GetKind() const {
      assert(this);
      return TKind::Transaction;
    }

    inline TRepoReplication::TReplica::TReplica(const Base::TUuid &id, bool is_safe, const TTtl &ttl, const Base::TOpt<Base::TUuid> &opt_parent_id)
        : Ttl(ttl), Id(id), IsSafe(is_safe), OptParentId(opt_parent_id) {}

    inline TRepoReplication::TReplica::TReplica(const TRepoReplication &replication_obj)
        : Ttl(replication_obj.Ttl), Id(replication_obj.RepoId), IsSafe(replication_obj.IsSafe), OptParentId(replication_obj.OptParentRepoId) {}

    inline TRepoReplication::TReplica::TReplica(const TReplica &that)
        : Ttl(that.Ttl), Id(that.Id), IsSafe(that.IsSafe), OptParentId(that.OptParentId) {}

    inline TRepoReplication::TReplica::TReplica(TReplica &&that)
        : Ttl(that.Ttl), Id(std::move(that.Id)), IsSafe(that.IsSafe), OptParentId(std::move(that.OptParentId)) {}

    inline const TTtl &TRepoReplication::GetTtl() const {
      return Ttl;
    }

    inline const Base::TUuid &TRepoReplication::GetId() const {
      return RepoId;
    }

    inline bool TRepoReplication::GetIsSafe() const {
      return IsSafe;
    }

    inline const Base::TOpt<Base::TUuid> &TRepoReplication::GetOptParentRepoId() const {
      return OptParentRepoId;
    }

    inline const TTtl &TRepoReplication::TReplica::GetTtl() const {
      return Ttl;
    }

    inline const Base::TUuid &TRepoReplication::TReplica::GetId() const {
      return Id;
    }

    inline bool TRepoReplication::TReplica::GetIsSafe() const {
      return IsSafe;
    }

    inline const Base::TOpt<Base::TUuid> &TRepoReplication::TReplica::GetOptParentId() const {
      return OptParentId;
    }

    inline TDurableReplication::TReplica::TReplica(const Base::TUuid &id, const TTtl &ttl, const std::string &serialized_obj)
        : Ttl(ttl), Id(id), SerializedObj(serialized_obj) {}

    inline TDurableReplication::TReplica::TReplica(const TDurableReplication &replication_obj)
        : Ttl(replication_obj.DurableTtl), Id(replication_obj.DurableId), SerializedObj(replication_obj.SerializedObj) {}

    inline TDurableReplication::TReplica::TReplica(const TReplica &that)
        : Ttl(that.Ttl), Id(that.Id), SerializedObj(that.SerializedObj) {}

    inline TDurableReplication::TReplica::TReplica(TReplica &&that)
        : Ttl(that.Ttl), Id(std::move(that.Id)), SerializedObj(std::move(that.SerializedObj)) {}

    inline const TTtl &TDurableReplication::TReplica::GetTtl() const {
      return Ttl;
    }

    inline const Base::TUuid &TDurableReplication::TReplica::GetId() const {
      return Id;
    }

    inline const std::string &TDurableReplication::TReplica::GetSerializedObj() const {
      return SerializedObj;
    }

    inline const L1::TTransaction::TReplica &TTransactionReplication::GetReplica() const {
      assert(this);
      return Replica;
    }

    inline void TTransactionReplication::SwapReplica(L1::TTransaction::TReplica &&replica) {
      assert(this);
      Replica = std::move(replica);
    }

    /* Binary streamers for Stig::Indy::TReplicationStreamer */
    inline Io::TBinaryOutputStream &operator<<(Io::TBinaryOutputStream &strm, const TReplicationStreamer &streamer) { streamer.Write(strm); return strm; }
    inline Io::TBinaryOutputStream &&operator<<(Io::TBinaryOutputStream &&strm, const TReplicationStreamer &streamer) { streamer.Write(strm); return std::move(strm); }
    inline Io::TBinaryInputStream &operator>>(Io::TBinaryInputStream &strm, TReplicationStreamer &streamer) { streamer.Read(strm); return strm; }
    inline Io::TBinaryInputStream &&operator>>(Io::TBinaryInputStream &&strm, TReplicationStreamer &streamer) { streamer.Read(strm); return std::move(strm); }

    /* Binary streamers for Stig::Indy::TIndexMapReplica */
    inline Io::TBinaryOutputStream &operator<<(Io::TBinaryOutputStream &strm, const TIndexMapReplica &streamer) { streamer.Write(strm); return strm; }
    inline Io::TBinaryOutputStream &&operator<<(Io::TBinaryOutputStream &&strm, const TIndexMapReplica &streamer) { streamer.Write(strm); return std::move(strm); }
    inline Io::TBinaryInputStream &operator>>(Io::TBinaryInputStream &strm, TIndexMapReplica &streamer) { streamer.Read(strm); return strm; }
    inline Io::TBinaryInputStream &&operator>>(Io::TBinaryInputStream &&strm, TIndexMapReplica &streamer) { streamer.Read(strm); return std::move(strm); }

  }  // Indy

}  // Stig