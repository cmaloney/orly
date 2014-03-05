/* <inv_con/unordered_list.h>

   Classes for maintaining an invasive, unordered, double-linked list.

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
#include <inv_con/orient.h>
#include <inv_con/cursor.h>

namespace InvCon {

  /* Classes for maintaining an invasive, unordered, double-linked list. */
  namespace UnorderedList {

    /* Forward declarations of base classes. */
    template <typename TCollector, typename TMember>
    class TCollection;
    template <typename TMember, typename TCollector>
    class TMembership;

    /* Forward declarations of final classes. */
    namespace Impl {
      template <typename TCollector, typename TMember>
      class TCollection;
      template <typename TMember, typename TCollector>
      class TMembership;
    }

    /* The 'one' side of the one-to-many. */
    template <typename TCollector, typename TMember>
    class TCollection {
      NO_COPY_SEMANTICS(TCollection);
      public:

      /* Our corresponding 'many' class. */
      typedef TMembership<TMember, TCollector> TTypedMembership;

      /* The final version of this class. */
      typedef Impl::TCollection<TCollector, TMember> TImpl;

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

      /* The first membership of the collection,
         or null if the collection is empty. */
      TTypedMembership *TryGetFirstMembership() const {
        assert(this);
        return FirstMembership;
      }

      /* The last member of the collection,
         or null if the collection is empty. */
      TMember *TryGetLastMember() const {
        assert(this);
        return LastMembership ? LastMembership->Member : 0;
      }

      /* The last membership of the collection,
         or null if the collection is empty. */
      TTypedMembership *TryGetLastMembership() const {
        assert(this);
        return LastMembership;
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

      /* Insert the given membership such that it will be at the requested position in this collection.
         If the membership is already in that position, do nothing.
         If the membership in a collection, remove it from that collection before inserting. */
      void Insert(TTypedMembership *membership, TOrient orient = Fwd) {
        assert(this);
        assert(membership);
        membership->Insert(this, orient);
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
      friend class TMembership<TMember, TCollector>;

    };  // TCollection<TCollector, TMember>

    /* The 'many' side of the one-to-many. */
    template <typename TMember, typename TCollector>
    class TMembership {
      NO_COPY_SEMANTICS(TMembership);
      public:

      /* Our corresponding 'one' class. */
      typedef TCollection<TCollector, TMember> TTypedCollection;

      /* The final version of this class. */
      typedef Impl::TMembership<TMember, TCollector> TImpl;

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

      /* Pass in a non-null pointer to the member which owns us. */
      TMembership(TMember *member)
          : Member(member) {
        assert(member);
        ZeroLinkage();
      }

      /* Pass in a non-null pointer to the member which owns us,
         a non-null pointer to a collection to insert into, and the position at which to insert. */
      TMembership(TMember *member, TTypedCollection *collection, TOrient orient)
          : Member(member) {
        assert(member);
        ZeroLinkage();
        Insert(collection, orient);
      }

      /* Pass in a non-null pointer to the member which owns us,
         a non-null pointer to a sibling to insert beside, and the position at which to insert. */
      TMembership(TMember *member, TMembership *sibling, TOrient orient)
          : Member(member) {
        assert(member);
        ZeroLinkage();
        Insert(sibling, orient);
      }

      /* Automatically removes us from our collection (if any) before destruction. */
      virtual ~TMembership() {
        assert(this);
        Remove();
      }

      /* Insert us last (orient == Fwd) or first (orient == Rev) in the given collection.
         If we're already in that position, do nothing.
         If we're already in a collection, remove us from that collection before inserting. */
      void Insert(TTypedCollection *collection, TOrient orient = Fwd) {
        assert(this);
        assert(collection);
        switch (orient) {
          case Fwd: {
            if (collection->LastMembership != this) {
              SetLinkage(collection, collection->LastMembership, 0);
            }
            break;
          }
          case Rev: {
            if (collection->FirstMembership != this) {
              SetLinkage(collection, 0, collection->FirstMembership);
            }
            break;
          }
          NO_DEFAULT_CASE;
        }
      }

      /* Insert us after (orient == Fwd) or before (orient = Rev) 'sibling'.
         If we're already in that position, do nothing.
         If we're already in a collection, remove us from that collection before inserting.
         The sibling must already be in a collection or this is an error.
         It is also an error to try to insert ourself next to ourself. */
      void Insert(TMembership *sibling, TOrient orient = Fwd) {
        assert(this);
        assert(sibling && sibling != this);
        assert(sibling->Collection);
        switch (orient) {
          case Fwd: {
            if (sibling->NextMembership != this) {
              SetLinkage(sibling->Collection, sibling, sibling->NextMembership);
            }
            break;
          }
          case Rev: {
            if (sibling->PrevMembership != this) {
              SetLinkage(sibling->Collection, sibling->PrevMembership, sibling);
            }
            break;
          }
          NO_DEFAULT_CASE;
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

      private:

      /* The fixup pointers before and after us in our collection.
         We must be in a collection or this is an error. */
      void FixupLinkage(TMembership *prev_fixup, TMembership *next_fixup) {
        assert(this);
        assert(Collection);
        (PrevMembership ? PrevMembership->NextMembership : Collection->FirstMembership) = prev_fixup;
        (NextMembership ? NextMembership->PrevMembership : Collection->LastMembership ) = next_fixup;
      }

      /* Insert us in the given collection, just between the 'prev_membership' and 'next_membership' siblings.
         If we're is already in a collection, remove us from there before inserting. */
      void SetLinkage(TTypedCollection *collection, TMembership *prev_membership, TMembership *next_membership) {
        assert(this);
        assert(collection);
        /* 'prev_membership' is null (this is, we're inserting as first)
           or 'next_membership' comes immediately after it in the collection's collection. */
        assert(
            !prev_membership ||
            (prev_membership->Collection == collection && prev_membership->NextMembership == next_membership));
        /* 'next_membership' is null (this is, we're inserting as last)
           or 'prev_membership' comes immediately before it in the collection's collection. */
        assert(
            !next_membership ||
            (next_membership->Collection == collection && next_membership->PrevMembership == prev_membership));
        /* Free up our linkage pointers, then point apporpriately. */
        Remove();
        Collection = collection;
        PrevMembership = prev_membership;
        NextMembership = next_membership;
        FixupLinkage(this, this);
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
      TTypedCollection *Collection;

      /* See accessors. */
      TMembership *PrevMembership, *NextMembership;

      /* For ~TMembership(), Insert(), Remove(), and Member. */
      friend class TCollection<TCollector, TMember>;

    };  // TMembership<TMember, TCollector>

    /* The final versions of the 'one' and 'many' classes. */
    namespace Impl {

      /* The final 'one'. */
      template <typename TCollector, typename TMember>
      class TCollection
          : public UnorderedList::TCollection<TCollector, TMember> {
        public:

        /* Our base type. */
        typedef UnorderedList::TCollection<TCollector, TMember> TBase;

        /* Do-little. */
        TCollection(TCollector *collector)
            : TBase(collector) {}

        /* Do-little. */
        virtual ~TCollection() {}

        /* Make our base's protected mutators public. */
        using TBase::DeleteEachMember;
        using TBase::Insert;
        using TBase::RemoveEachMember;

      };  // TCollection

      /* The final 'many'. */
      template <typename TMember, typename TCollector>
      class TMembership
          : public UnorderedList::TMembership<TMember, TCollector> {
        public:

        /* Our base type. */
        typedef UnorderedList::TMembership<TMember, TCollector> TBase;

        /* Do-little. */
        TMembership(TMember *member)
            : TBase(member) {}

        /* Do-little. */
        TMembership(TMember *member, typename TBase::TTypedCollection *collection, TOrient orient = Fwd)
            : TBase(member, collection, orient) {}

        /* Do-little. */
        TMembership(TMember *member, TBase *sibling, TOrient orient = Fwd)
            : TBase(member, sibling, orient) {}

        /* Do-little. */
        virtual ~TMembership() {}

        /* Make our base's protected mutators public. */
        using TBase::Insert;
        using TBase::Remove;

      };  // TMembership

    }  // Impl

  }  // UnorderedList

}  // InvCon
