/* <stig/indy/update.h>

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
#include <stig/atom/kit2.h>
#include <stig/atom/suprena.h>
#include <stig/indy/key.h>
#include <stig/indy/sequence_number.h>
#include <stig/indy/util/pool.h>
#include <stig/sabot/all.h>
#include <stig/sabot/assert_tuple.h>

namespace Stig {

  namespace Server {

    /* Forward Declarations. */
    class TServer;
    class TIndyReporter;

  }

  namespace Indy {

    /* Forward Declarations. */
    class TMemoryLayer;

    namespace L1 {

      /* Forward Declarations. */
      class TTransaction;

    }  // L1

    /* TODO */
    class TUpdate {
      NO_COPY_SEMANTICS(TUpdate);
      public:

      /* Forward Declarations. */
      class TEntry;

      /* TODO */
      typedef std::map<TIndexKey, TKey> TOpByKey;

      /* TODO */
      typedef InvCon::OrderedList::TCollection<TUpdate, TEntry, TKey> TEntryCollection;

      /* TODO */
      ~TUpdate();

      /* TODO */
      class TPersistenceNotification {
        NO_COPY_SEMANTICS(TPersistenceNotification);
        public:

        enum TResult {
          Completed,
          Failed
        };

        /* TODO */
        TPersistenceNotification(const std::function<void (TResult)> &cb);

        /* TODO */
        ~TPersistenceNotification();

        /* TODO */
        inline void Call(TUpdate::TPersistenceNotification::TResult result);

        private:

        /* TODO */
        std::function<void (TResult)> Cb;

      };  // TPersistenceNotification

      /* TODO */
      class TEntry {
        NO_COPY_SEMANTICS(TEntry);
        public:

        /* TODO */
        inline const TKey &GetKey() const;

        /* TODO */
        inline const TIndexKey &GetIndexKey() const;

        /* TODO */
        inline const Atom::TCore &GetOp() const;

        /* TODO */
        inline TSequenceNumber GetSequenceNumber() const;

        /* TODO */
        inline const Atom::TCore &GetMetadata() const;

        /* TODO */
        inline const Atom::TCore &GetId() const;

        /* TODO */
        inline Atom::TSuprena &GetSuprena() const;

        /* TODO */
        inline const TUpdate *GetUpdate() const;

        /* TODO */
        static void *operator new(size_t size) {
          return Pool.Alloc(size);
        }

        /* TODO */
        static void operator delete(void *ptr, size_t) {
          Pool.Free(ptr);
        }

        private:

        /* TODO */
        class TEntryKey {
          public:

          /* TODO */
          TEntryKey(const TEntry *entry);

          /* TODO */
          bool operator==(const TEntryKey &that) const;

          /* TODO */
          bool operator!=(const TEntryKey &that) const;

          /* TODO */
          bool operator<=(const TEntryKey &that) const;

          /* TODO */
          bool operator>(const TEntryKey &that) const;

          private:

          /* TODO */
          const TEntry *Entry;

        };  // TKey

        /* TODO */
        typedef InvCon::OrderedList::TMembership<TEntry, TUpdate, TKey> TUpdateMembership;
        typedef InvCon::OrderedList::TMembership<TEntry, TMemoryLayer, TEntryKey> TMemoryLayerMembership;

        /* TODO */
        TEntry(TUpdate *update, const TIndexKey &key, const TKey &op, void *state_alloc);

        /* TODO */
        inline const TEntryKey &GetEntryKey() const;

        /* TODO */
        TIndexKey IndexKey;

        /* TODO */
        TUpdateMembership::TImpl UpdateMembership;

        /* TODO */
        TMemoryLayerMembership::TImpl MemoryLayerMembership;

        /* TODO */
        Atom::TCore Op;

        /* TODO */
        static Util::TPool Pool;

        /* TODO */
        friend class TMemoryLayer;
        friend class TManager;
        friend class TUpdate;
        friend class Stig::Server::TServer;
        friend class Stig::Server::TIndyReporter;

      };  // TEntry

      /* TODO */
      inline TSequenceNumber GetSequenceNumber() const;

      /* TODO */
      inline const Atom::TCore &GetId() const;

      /* TODO */
      inline const Atom::TCore &GetMetadata() const;

      /* TODO */
      inline Atom::TSuprena &GetSuprena() const;

      /* TODO */
      inline TEntryCollection *GetEntryCollection() const;

      /* TODO */
      inline const std::shared_ptr<TPersistenceNotification> &GetPersistenceNotification() const;

      /* TODO */
      inline void SetSequenceNumber(TSequenceNumber seq_num);

      /* TODO */
      inline void SetMetadata(const TKey &metadata);

      /* TODO */
      inline void SetId(const TKey &id);

      /* TODO */
      inline void SetPersistenceNotification(const std::function<void (TPersistenceNotification::TResult)> &cb);

      /* TODO */
      inline TEntry *AddEntry(const TIndexKey &key, const TKey &op);

      /* TODO */
      static std::shared_ptr<TUpdate> NewUpdate(const TOpByKey &op_by_key, const TKey &metadata, const TKey &id);

      /* TODO */
      static std::shared_ptr<TUpdate> ReconstructUpdate(TSequenceNumber seq_num);

      /* TODO */
      static TUpdate *CopyUpdate(TUpdate *that, void *state_alloc);

      /* TODO */
      static TUpdate *ShallowCopy(TUpdate *that, void *state_alloc);

      /* TODO */
      static void *operator new(size_t size) {
        return Pool.Alloc(size);
      }

      /* TODO */
      static void operator delete(void *ptr, size_t) {
        Pool.Free(ptr);
      }

      /* TODO */
      static void InitUpdatePool(size_t num_obj) {
        Pool.Init(num_obj);
      }

      /* TODO */
      static void InitEntryPool(size_t num_obj) {
        TEntry::Pool.Init(num_obj);
      }

      protected:

      /* TODO */
      TUpdate(const TOpByKey &op_by_key, const TKey &metadata, const TKey &id, void *state_alloc);

      /* TODO */
      explicit TUpdate(const TUpdate *that, void *state_alloc);

      /* Shallow Copy! */
      explicit TUpdate(void *state_alloc, const TUpdate *that);

      /* TODO */
      explicit TUpdate(TSequenceNumber);

      private:

      /* TODO */
      typedef InvCon::OrderedList::TMembership<TUpdate, TMemoryLayer, TSequenceNumber> TMemoryLayerMembership;

      /* TODO */
      mutable Atom::TSuprena Suprena;

      /* TODO */
      mutable TEntryCollection::TImpl EntryCollection;

      /* TODO */
      TMemoryLayerMembership::TImpl MemoryLayerMembership;

      /* TODO */
      Atom::TCore Metadata;

      /* TODO */
      Atom::TCore Id;

      /* TODO */
      std::shared_ptr<TPersistenceNotification> PersistenceNotification;

      /* TODO */
      static Util::TPool Pool;

      /* TODO */
      friend class TMemoryLayer;
      friend class TManager;
      friend class L1::TTransaction;
      friend class Stig::Server::TServer;
      friend class Stig::Server::TIndyReporter;

    };  // TUpdate

    /* TODO */
    inline void TUpdate::TPersistenceNotification::Call(TUpdate::TPersistenceNotification::TResult result) {
      assert(this);
      Cb(result);
    }

    /* TODO */
    inline TSequenceNumber TUpdate::GetSequenceNumber() const {
      assert(this);
      return MemoryLayerMembership.GetKey();
    }

    /* TODO */
    inline const Atom::TCore &TUpdate::GetId() const {
      assert(this);
      return Id;
    }

    /* TODO */
    inline const Atom::TCore &TUpdate::GetMetadata() const {
      assert(this);
      return Metadata;
    }

    /* TODO */
    inline Atom::TSuprena &TUpdate::GetSuprena() const {
      assert(this);
      return Suprena;
    }

    /* TODO */
    inline void TUpdate::SetSequenceNumber(TSequenceNumber seq_num) {
      assert(this);
      MemoryLayerMembership.SetKey(seq_num);
    }

    /* TODO */
    inline void TUpdate::SetMetadata(const TKey &metadata) {
      assert(this);
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      Metadata = Atom::TCore(&Suprena, Sabot::State::TAny::TWrapper(metadata.GetCore().NewState(metadata.GetArena(), state_alloc)));
    }

    /* TODO */
    inline void TUpdate::SetId(const TKey &id) {
      assert(this);
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      Id = Atom::TCore(&Suprena, Sabot::State::TAny::TWrapper(id.GetCore().NewState(id.GetArena(), state_alloc)));
    }

    /* TODO */
    inline void TUpdate::SetPersistenceNotification(const std::function<void (TPersistenceNotification::TResult)> &cb) {
      assert(this);
      PersistenceNotification = std::make_shared<TPersistenceNotification>(cb);
    }

    /* TODO */
    inline TUpdate::TEntry *TUpdate::AddEntry(const TIndexKey &index_key, const TKey &op) {
      assert(this);
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      return new TEntry(this, index_key, op, state_alloc);
    }

    /* TODO */
    inline const TKey &TUpdate::TEntry::GetKey() const {
      assert(this);
      return IndexKey.GetKey();
    }

    /* TODO */
    inline const TIndexKey &TUpdate::TEntry::GetIndexKey() const {
      assert(this);
      return IndexKey;
    }

    /* TODO */
    inline const Atom::TCore &TUpdate::TEntry::GetOp() const {
      assert(this);
      return Op;
    }

    /* TODO */
    inline TSequenceNumber TUpdate::TEntry::GetSequenceNumber() const {
      assert(this);
      assert(UpdateMembership.TryGetCollector());
      return UpdateMembership.TryGetCollector()->MemoryLayerMembership.GetKey();
    }

    /* TODO */
    inline const Atom::TCore &TUpdate::TEntry::GetMetadata() const {
      assert(this);
      assert(UpdateMembership.TryGetCollector());
      return UpdateMembership.TryGetCollector()->Metadata;
    }

    /* TODO */
    inline const Atom::TCore &TUpdate::TEntry::GetId() const {
      assert(this);
      assert(UpdateMembership.TryGetCollector());
      return UpdateMembership.TryGetCollector()->Id;
    }

    /* TODO */
    inline Atom::TSuprena &TUpdate::TEntry::GetSuprena() const {
      assert(this);
      assert(UpdateMembership.TryGetCollector());
      return UpdateMembership.TryGetCollector()->Suprena;
    }

    /* TODO */
    inline const TUpdate *TUpdate::TEntry::GetUpdate() const {
      assert(this);
      assert(UpdateMembership.TryGetCollector());
      return UpdateMembership.TryGetCollector();
    }

    inline const TUpdate::TEntry::TEntryKey &TUpdate::TEntry::GetEntryKey() const {
      assert(this);
      return MemoryLayerMembership.GetKey();
    }

    /* TODO */
    inline TUpdate::TEntryCollection *TUpdate::GetEntryCollection() const {
      assert(this);
      return &EntryCollection;
    }

    /* TODO */
    inline const std::shared_ptr<TUpdate::TPersistenceNotification> &TUpdate::GetPersistenceNotification() const {
      assert(this);
      return PersistenceNotification;
    }

  }  // Indy

}  // Stig