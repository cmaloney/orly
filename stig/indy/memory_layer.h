/* <stig/indy/memory_layer.h>

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
#include <stig/indy/manager_base.h>
#include <stig/indy/update.h>
#include <stig/sabot/all.h>

namespace Stig {

  namespace Server {

    /* Forward Declarations. */
    class TServer;

  }

  namespace Indy {

    /* Forward Declarations. */
    class TRepo;
    class TManager;

    /* TODO */
    class TMemoryLayer
        : public L0::TManager::TRepo::TDataLayer {
      NO_COPY_SEMANTICS(TMemoryLayer);
      public:

      /* TODO */
      typedef InvCon::OrderedList::TCollection<TMemoryLayer, TUpdate, TSequenceNumber> TUpdateCollection;
      typedef InvCon::OrderedList::TCollection<TMemoryLayer, TUpdate::TEntry, TUpdate::TEntry::TEntryKey> TEntryCollection;

      /* TODO */
      TMemoryLayer(L0::TManager *manager);

      /* TODO */
      virtual ~TMemoryLayer();

      /* TODO */
      void Insert(TUpdate *update) NO_THROW;

      /* TODO */
      void ReverseInsert(TUpdate *update) NO_THROW;

      /* TODO */
      inline TEntryCollection *GetEntryCollection() const;

      /* TODO */
      inline TUpdateCollection *GetUpdateCollection() const;

      inline bool IsEmpty() const;

      /* TODO */
      virtual std::unique_ptr<Indy::TPresentWalker> NewPresentWalker(const TIndexKey &from,
                                                                     const TIndexKey &to) const override;

      virtual std::unique_ptr<Indy::TPresentWalker> NewPresentWalker(const TIndexKey &key) const override;

      /* TODO */
      virtual std::unique_ptr<Indy::TUpdateWalker> NewUpdateWalker(TSequenceNumber from) const override;

      /* TODO */
      inline virtual size_t GetSize() const override;

      /* TODO */
      inline virtual TSequenceNumber GetLowestSeq() const override;

      /* TODO */
      inline virtual TSequenceNumber GetHighestSeq() const override;

      private:

      /* TODO */
      class TMatchPresentWalker
          : public Indy::TPresentWalker {
        NO_COPY_SEMANTICS(TMatchPresentWalker);
        public:

        /* TODO */
        TMatchPresentWalker(const TMemoryLayer *layer,
                       const TIndexKey &key);

        /* TODO */
        virtual ~TMatchPresentWalker();

        /* True iff. we have an item. */
        virtual operator bool() const;

        /* The current item. */
        virtual const TItem &operator*() const;

        /* Walk to the next item, if any. */
        virtual TMatchPresentWalker &operator++();

        private:

        /* TODO */
        void Refresh() const;

        /* TODO */
        const TMemoryLayer *const Layer;

        /* TODO */
        const TIndexKey &Key;

        /* TODO */
        mutable TMemoryLayer::TEntryCollection::TCursor Csr;

        /* TODO */
        mutable bool Valid;

        /* TODO */
        mutable bool Cached;

        /* TODO */
        mutable bool PassedMatch;

        /* TODO */
        mutable TItem Item;

      };  // TMatchPresentWalker

      /* TODO */
      class TRangePresentWalker
          : public Indy::TPresentWalker {
        NO_COPY_SEMANTICS(TRangePresentWalker);
        public:

        /* TODO */
        TRangePresentWalker(const TMemoryLayer *layer,
                       const TIndexKey &from,
                       const TIndexKey &to);

        /* TODO */
        virtual ~TRangePresentWalker();

        /* True iff. we have an item. */
        virtual operator bool() const;

        /* The current item. */
        virtual const TItem &operator*() const;

        /* Walk to the next item, if any. */
        virtual TRangePresentWalker &operator++();

        private:

        /* TODO */
        void Refresh() const;

        /* TODO */
        const TMemoryLayer *const Layer;

        /* TODO */
        const TIndexKey &From;

        /* TODO */
        const TIndexKey &To;

        /* TODO */
        mutable TMemoryLayer::TEntryCollection::TCursor Csr;

        /* TODO */
        mutable bool Valid;

        /* TODO */
        mutable bool Cached;

        /* TODO */
        mutable bool PassedMatch;

        /* TODO */
        mutable TItem Item;

      };  // TRangePresentWalker

      /* TODO */
      class TUpdateWalker
          : public Indy::TUpdateWalker {
        NO_COPY_SEMANTICS(TUpdateWalker);
        public:

        /* TODO */
        TUpdateWalker(const TMemoryLayer *layer, TSequenceNumber from);

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
        const TMemoryLayer *const Layer;

        /* TODO */
        TSequenceNumber From;

        /* TODO */
        TMemoryLayer::TUpdateCollection::TCursor Csr;

        /* TODO */
        bool Valid;

        /* TODO */
        mutable TItem Item;

      };  // TUpdateWalker

      /* TODO */
      inline virtual TKind GetKind() const;

      /* TODO */
      void ImporterAppendUpdate(TUpdate *update);

      /* TODO */
      void ImporterAppendEntry(TUpdate::TEntry *entry);

      /* TODO */
      mutable TUpdateCollection::TImpl UpdateCollection;

      /* TODO */
      mutable TEntryCollection::TImpl EntryCollection;

      /* TODO */
      size_t Size;

      friend class Stig::Server::TServer;
      friend class Stig::Indy::TRepo;
      friend class Stig::Indy::TManager;

    };  // TMemoryLayer

    /* TODO */
    inline TMemoryLayer::TEntryCollection *TMemoryLayer::GetEntryCollection() const {
      assert(this);
      return &EntryCollection;
    }

    /* TODO */
    inline TMemoryLayer::TUpdateCollection *TMemoryLayer::GetUpdateCollection() const {
      assert(this);
      return &UpdateCollection;
    }

    /* TODO */
    inline bool TMemoryLayer::IsEmpty() const {
      assert(this);
      return UpdateCollection.TryGetFirstMember() == nullptr;
    }

    /* TODO */
    inline L0::TManager::TRepo::TDataLayer::TKind TMemoryLayer::GetKind() const {
      return L0::TManager::TRepo::TDataLayer::Mem;
    }

    /* TODO */
    inline size_t TMemoryLayer::GetSize() const {
      assert(this);
      return Size;
    }

    /* TODO */
    inline TSequenceNumber TMemoryLayer::GetLowestSeq() const {
      assert(this);
      assert(Size);
      return UpdateCollection.TryGetFirstMember()->GetSequenceNumber();
    }

    /* TODO */
    inline TSequenceNumber TMemoryLayer::GetHighestSeq() const {
      assert(this);
      assert(Size);
      return UpdateCollection.TryGetLastMember()->GetSequenceNumber();
    }

  }  // Indy

}  // Stig

