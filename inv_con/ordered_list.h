/* <inv_con/ordered_list.h>

   Classes for maintaining an invasive, ordered, double-linked list.

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
#include <base/no_default_case.h>
#include <inv_con/cursor.h>

namespace InvCon {

  /* Classes for maintaining an invasive, ordered, double-linked list. */
  namespace OrderedList {

    /* Forward declarations of base classes. */
    template <typename TCollector, typename TMember, typename TKey>
    class TCollection;
    template <typename TMember, typename TCollector, typename TKey>
    class TMembership;

    /* Forward declarations of final classes. */
    namespace Impl {
      template <typename TCollector, typename TMember, typename TKey>
      class TCollection;
      template <typename TMember, typename TCollector, typename TKey>
      class TMembership;
    }

    /* The 'one' side of the one-to-many. */
    template <typename TCollector, typename TMember, typename TKey>
    class TCollection {
      NO_COPY_SEMANTICS(TCollection);
      public:

      /* Our corresponding 'many' class. */
      typedef TMembership<TMember, TCollector,TKey> TTypedMembership;

      /* The final version of this class. */
      typedef Impl::TCollection<TCollector, TMember, TKey> TImpl;

      /* Our cursor class. */
      typedef InvCon::Impl::TCursor<TCollector, TCollection, TMember, TTypedMembership> TCursor;

      /* The collector which owns us.  Never null. */
      TCollector *GetCollector() const {
        assert(this);
        return Collector;
      }

      /* The first member of the collection,
         or null if the collection is empty. */
      TMember *TryGetFirstMember() const {
        assert(this);
        return FirstMembership ? FirstMembership->Member : 0;
      }

      /* The first member of the collection which matches the given key,
         or null if the collection contains no match. */
      TMember *TryGetFirstMember(const TKey &key) const {
        assert(this);
        TTypedMembership *membership = TryGetFirstMembership(key);
        return membership ? membership->GetMember() : 0;
      }

      /* The first membership of the collection,
         or null if the collection is empty. */
      TTypedMembership *TryGetFirstMembership() const {
        assert(this);
        return FirstMembership;
      }

      /* The first membership of the collection which matches the given key,
         or null if the collection contains no match. */
      TTypedMembership *TryGetFirstMembership(const TKey &key) const {
        assert(this);
        TTypedMembership *membership = FirstMembership;
        while (membership && membership->Key < key) {
          membership = membership->NextMembership;
        }
        return (membership && !(key < membership->Key)) ? membership : 0;
      }

      /* The last member of the collection,
         or null if the collection is empty. */
      TMember *TryGetLastMember() const {
        assert(this);
        return LastMembership ? LastMembership->Member : 0;
      }

      /* The last member of the collection which matches the given key,
         or null if the collection contains no match. */
      TMember *TryGetLastMember(const TKey &key) const {
        assert(this);
        TTypedMembership *membership = TryGetLastMembership(key);
        return membership ? membership->GetMember() : 0;
      }

      /* The last membership of the collection,
         or null if the collection is empty. */
      TTypedMembership *TryGetLastMembership() const {
        assert(this);
        return LastMembership;
      }

      /* The last membership of the collection which matches the given key,
         or null if the collection contains no match. */
      TTypedMembership *TryGetLastMembership(const TKey &key) const {
        assert(this);
        TTypedMembership *membership = LastMembership;
        while (membership && key < membership->Key) {
          membership = membership->PrevMembership;
        }
        return (membership && !(membership->Key < key)) ? membership : 0;
      }

      /* TODO */
      bool IsEmpty() const {
        assert(this);
        return FirstMembership == 0;
      }

      protected:

      /* Pass in a non-null pointer to the collector which owns us. */
      TCollection(TCollector *collector)
          : Collector(collector), FirstMembership(0), LastMembership(0) {
        assert(collector);
      }

      /* Remove each member from the collection upon destruction. */
      virtual ~TCollection() {
        assert(this);
        RemoveEachMember();
      }

      /* Delete each member. */
      void DeleteEachMember() {
        assert(this);
        while (FirstMembership) {
          TMember *member = FirstMembership->Member;
          FirstMembership->Remove();
          delete member;
        }
      }

      /* Insert the given membership at the correct position.
         If the membership is already at the correct position, do nothing.
         If the membership in a different collection, remove it from that collection before inserting. */
      void Insert(TTypedMembership *membership) {
        assert(this);
        assert(membership);
        membership->Insert(this);
      }

      /* TODO */
      void ReverseInsert(TTypedMembership *membership) {
        assert(this);
        assert(membership);
        membership->ReverseInsert(this);
      }

      /* Remove each member from the collection but don't delete them. */
      void RemoveEachMember() {
        assert(this);
        while (FirstMembership) {
          FirstMembership->Remove();
        }
      }

      private:

      /* See accessor. */
      TCollector *const Collector;

      /* See accessors. */
      TTypedMembership *FirstMembership, *LastMembership;

      /* For Collector, FirstMembership, and LastMembership. */
      friend class TMembership<TMember, TCollector, TKey>;

    };  // TCollection<TCollector, TMember>

    /* The 'many' side of the one-to-many. */
    template <typename TMember, typename TCollector, typename TKey>
    class TMembership {
      NO_COPY_SEMANTICS(TMembership);
      public:

      /* Our corresponding 'one' class. */
      typedef TCollection<TCollector, TMember, TKey> TTypedCollection;

      /* The final version of this class. */
      typedef Impl::TMembership<TMember, TCollector, TKey> TImpl;

      /* Get our key. */
      const TKey &GetKey() const {
        assert(this);
        return Key;
      }

      /* The member which owns us.  Never null. */
      TMember *GetMember() const {
        assert(this);
        return Member;
      }

      /* The collection we're in, if any. */
      TTypedCollection *TryGetCollection() const {
        assert(this);
        return Collection;
      }

      /* The collector whose collection we're in, if any. */
      TCollector *TryGetCollector() const {
        assert(this);
        return Collection ? Collection->Collector : 0;
      }

      /* The member before us in our collection.
         A null here means either we're first in our collection or we're not in a collection. */
      TMember *TryGetPrevMember() const {
        assert(this);
        return PrevMembership ? PrevMembership->Member : 0;
      }

      /* The membership before us in our collection.
         A null here means either we're first in our collection or we're not in a collection. */
      TMembership *TryGetPrevMembership() const {
        assert(this);
        return PrevMembership;
      }

      /* The member before us in our collection.
         A null here means either we're first in our collection or we're not in a collection. */
      TMember *TryGetNextMember() const {
        assert(this);
        return NextMembership ? NextMembership->Member : 0;
      }

      /* The membership before us in our collection.
         A null here means either we're first in our collection or we're not in a collection. */
      TMembership *TryGetNextMembership() const {
        assert(this);
        return NextMembership;
      }

      protected:

      /* Pass in a non-null pointer to the member which owns us,
         and an an optional pointer to a collection to insert into. */
      TMembership(TMember *member)
          : Member(member) {
        assert(member);
        ZeroLinkage();
      }

      /* Pass in a non-null pointer to the member which owns us, a value for our key,
         and an an optional pointer to a collection to insert into. */
      TMembership(TMember *member, const TKey &key, TTypedCollection *collection = 0)
          : Member(member), Key(key) {
        assert(member);
        ZeroLinkage();
        if (collection) {
          Insert(collection);
        }
      }

      /* Pass in a non-null pointer to the member which owns us, a value for our key,
         and an an optional pointer to a collection to insert into. */
      TMembership(TMember *member, const TKey &key, TTypedCollection *collection, TOrient insert_from)
          : Member(member), Key(key) {
        assert(member);
        assert(collection);
        ZeroLinkage();
        switch (insert_from) {
          case TOrient::Fwd: {
            Insert(collection);
            break;
          }
          case TOrient::Rev: {
            ReverseInsert(collection);
            break;
          }
        }
      }

      /* Automatically removes us from our collection (if any) before destruction. */
      virtual ~TMembership() {
        assert(this);
        Remove();
      }

      /* Insert us into the given collection at the correct position.
         If we're already at that position, do nothing.
         If we're already in a different collection, remove us from that collection before inserting. */
      void Insert(TTypedCollection *collection) {
        assert(this);
        assert(collection);
        TMembership
            *prev_membership = 0,
            *next_membership = collection->FirstMembership != this ? collection->FirstMembership : collection->FirstMembership->NextMembership;
        while (next_membership && next_membership->Key <= Key) {
          prev_membership = next_membership;
          next_membership = next_membership->NextMembership != this ? next_membership->NextMembership : next_membership->NextMembership->NextMembership;
        }
        if (Collection != collection || PrevMembership != prev_membership || NextMembership != next_membership) {
          Remove();
          Collection = collection;
          PrevMembership = prev_membership;
          NextMembership = next_membership;
          FixupLinkage(this, this);
        }
      }

      /* TODO */
      void ReverseInsert(TTypedCollection *collection) {
        assert(this);
        assert(collection);
        TMembership
            *next_membership = 0,
            *prev_membership = collection->LastMembership != this ? collection->LastMembership : collection->LastMembership->PrevMembership;
        while (prev_membership && prev_membership->Key > Key) {
          next_membership = prev_membership;
          prev_membership = prev_membership->PrevMembership != this ? prev_membership->PrevMembership : prev_membership->PrevMembership->PrevMembership;
        }
        if (Collection != collection || NextMembership != next_membership || PrevMembership != prev_membership) {
          Remove();
          Collection = collection;
          NextMembership = next_membership;
          PrevMembership = prev_membership;
          FixupLinkage(this, this);
        }
      }

      /* Remove us from our collection.
         If we're not in a collection, this function does nothing. */
      void Remove() {
        assert(this);
        if (Collection) {
          /* Fixup the pointers on either side of us to point around us,
             then go back to the unlinked state. */
          FixupLinkage(NextMembership, PrevMembership);
          ZeroLinkage();
        }
      }

      /* Set our key, relocating us within our collection, if necessary.
         If the new key is the same as the old, do nothing. */
      void SetKey(const TKey &key) {
        assert(this);
        assert(&key);
        if (Key != key) {
          Key = key;
          if (Collection) {
            Insert(Collection);
          }
        }
      }

      private:

      /* The fixup pointers before and after us in our collection.
         We must be in a collection or this is an error. */
      void FixupLinkage(TMembership *prev_fixup, TMembership *next_fixup) {
        assert(this);
        assert(Collection);
        (PrevMembership ? PrevMembership->NextMembership : Collection->FirstMembership) = prev_fixup;
        (NextMembership ? NextMembership->PrevMembership : Collection->LastMembership ) = next_fixup;
      }

      /* Reset all pointers to null.
         This function does no unlinking so make sure we're unlinked first. */
      void ZeroLinkage() {
        assert(this);
        Collection = 0;
        NextMembership = 0;
        PrevMembership = 0;
      }

      /* See accessor. */
      TMember *const Member;

      /* See accessor. */
      TKey Key;

      /* See accessor. */
      TTypedCollection *Collection;

      /* See accessors. */
      TMembership *PrevMembership, *NextMembership;

      /* For ~TMembership(), Insert(), Remove(), and Member. */
      friend class TCollection<TCollector, TMember, TKey>;

    };  // TMembership<TMember, TCollector>

    /* The final versions of the 'one' and 'many' classes. */
    namespace Impl {

      /* The final 'one'. */
      template <typename TCollector, typename TMember, typename TKey>
      class TCollection
          : public OrderedList::TCollection<TCollector, TMember, TKey> {
        public:

        /* Our base type. */
        typedef OrderedList::TCollection<TCollector, TMember, TKey> TBase;

        /* Do-little. */
        TCollection(TCollector *collector)
            : TBase(collector) {}

        /* Do-little. */
        virtual ~TCollection() {}

        /* Make our base's protected mutators public. */
        using TBase::DeleteEachMember;
        using TBase::IsEmpty;
        using TBase::Insert;
        using TBase::ReverseInsert;
        using TBase::RemoveEachMember;

      };  // TCollection

      /* The final 'many'. */
      template <typename TMember, typename TCollector, typename TKey>
      class TMembership
          : public OrderedList::TMembership<TMember, TCollector, TKey> {
        public:

        /* Our base type. */
        typedef OrderedList::TMembership<TMember, TCollector, TKey> TBase;

        /* Do-little. */
        TMembership(TMember *member)
            : TBase(member) {}

        /* Do-little. */
        TMembership(TMember *member, const TKey &key, typename TBase::TTypedCollection *collection = 0)
            : TBase(member, key, collection) {}

        /* Do-little. */
        TMembership(TMember *member, const TKey &key, TOrient insert_from, typename TBase::TTypedCollection *collection)
            : TBase(member, key, collection, insert_from) {}

        /* Do-little. */
        virtual ~TMembership() {}

        /* Make our base's protected mutators public. */
        using TBase::Insert;
        using TBase::ReverseInsert;
        using TBase::Remove;
        using TBase::SetKey;

      };  // TMembership

    }  // Impl

  }  // OrderedList

}  // InvCon
