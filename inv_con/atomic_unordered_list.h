/* <inv_con/atomic_unordered_list.h>

   Classes for maintaining an atomic, invasive, unordered, double-linked list.

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
#include <cassert>
#include <functional>

#include <xmmintrin.h>

#include <base/no_copy_semantics.h>
#include <base/no_default_case.h>

namespace InvCon {

  /* Classes for maintaining an atomic invasive, unordered, double-linked list. */
  namespace AtomicUnorderedList {

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

      /* The collector which owns us.  Never null. */
      TCollector *GetCollector() const {
        assert(this);
        return Collector;
      }

      /* TODO */
      void ForEach(const std::function<bool (const TMember &)> &cb) {
        assert(this);
        bool keep_going = true;
        TTypedMembership *prev_membership = 0;
        while (FirstLock.test_and_set(std::memory_order_acquire)) {/* acquire list first lock */}
        TTypedMembership *next_membership = FirstMembership;
        while (next_membership && keep_going) {
          keep_going = cb(*next_membership->GetMember());
          while (next_membership->NextLock.test_and_set(std::memory_order_acquire)) {/* acquire next lock */}
          if (prev_membership) {
            prev_membership->NextLock.clear(std::memory_order_release);  // release prev
            prev_membership = next_membership;
          } else {
            prev_membership = next_membership;
            FirstLock.clear(std::memory_order_release);  // release prev
          }
          next_membership = next_membership->NextMembership;
        }
        if (prev_membership) {
          prev_membership->NextLock.clear(std::memory_order_release);  // release prev
        } else {
          FirstLock.clear(std::memory_order_release);  // release last
        }
      }

      /* TODO */
      template <typename... TVal>
      void ForEach(const std::function<bool (const TMember &, TVal &...)> &cb, TVal &...out_param) {
        assert(this);
        bool keep_going = true;
        TTypedMembership *prev_membership = 0;
        while (FirstLock.test_and_set(std::memory_order_acquire)) {/* acquire list first lock */}
        TTypedMembership *next_membership = FirstMembership;
        while (next_membership && keep_going) {
          keep_going = cb(*next_membership->GetMember(), out_param...);
          while (next_membership->NextLock.test_and_set(std::memory_order_acquire)) {/* acquire next lock */}
          if (prev_membership) {
            prev_membership->NextLock.clear(std::memory_order_release);  // release prev
            prev_membership = next_membership;
          } else {
            prev_membership = next_membership;
            FirstLock.clear(std::memory_order_release);  // release prev
          }
          next_membership = next_membership->NextMembership;
        }
        if (prev_membership) {
          prev_membership->NextLock.clear(std::memory_order_release);  // release prev
        } else {
          FirstLock.clear(std::memory_order_release);  // release last
        }
      }

      protected:

      /* Pass in a non-null pointer to the collector which owns us. */
      TCollection(TCollector *collector)
          : FirstLock(ATOMIC_FLAG_INIT),
            LastLock(ATOMIC_FLAG_INIT),
            FirstMembership(0),
            LastMembership(0),
            Collector(collector) {
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
      void Insert(TTypedMembership *membership) {
        assert(this);
        assert(membership);
        membership->Insert(this);
      }

      /* Remove each member from the collection but don't delete them. THIS IS NOT ATOMIC...*/
      void RemoveEachMember() {
        assert(this);
        while (FirstMembership) {
          FirstMembership->Remove();
        }
      }

      private:

      /* TODO */
      std::atomic_flag FirstLock, LastLock;

      /* See accessors. */
      TTypedMembership *FirstMembership, *LastMembership;

      /* See accessor. */
      TCollector *const Collector;

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

      protected:

      /* Pass in a non-null pointer to the member which owns us. */
      TMembership(TMember *member)
          : PrevLock(ATOMIC_FLAG_INIT),
            NextLock(ATOMIC_FLAG_INIT),
            Member(member) {
        assert(member);
        ZeroLinkage();
      }

      /* Pass in a non-null pointer to the member which owns us,
         a non-null pointer to a collection to insert into */
      TMembership(TMember *member, TTypedCollection *collection)
          : PrevLock(ATOMIC_FLAG_INIT),
            NextLock(ATOMIC_FLAG_INIT),
            Member(member) {
        assert(member);
        ZeroLinkage();
        Insert(collection);
      }

      /* Automatically removes us from our collection (if any) before destruction. */
      virtual ~TMembership() {
        assert(this);
        if (Collection) {
          Remove();
        }
      }

      /* Insert us last in the given collection. */
      void Insert(TTypedCollection *collection) {
        assert(this);
        assert(!Collection);
        assert(collection);
        _mm_prefetch(collection->LastMembership, _MM_HINT_T0);
        for (;;) {
          while (collection->LastLock.test_and_set(std::memory_order_acquire)) {/* acquire last lock */}
          if (collection->LastMembership) {
            if (collection->LastMembership->NextLock.test_and_set(std::memory_order_acquire)) {  /* try acquire prev's next lock */
              collection->LastLock.clear(std::memory_order_release);  // release last lock
              continue;
            } else {
              break;
            }
          } else {
            if (collection->FirstLock.test_and_set(std::memory_order_acquire)) {  /* try acquire first lock */
              collection->LastLock.clear(std::memory_order_release);  // release last lock
              continue;
            } else {
              break;
            }
          }
        }
        Collection = collection;
        PrevMembership = collection->LastMembership;
        NextMembership = 0;
        FixupLinkage(this, this);
        if (PrevMembership) {
          PrevMembership->NextLock.clear(std::memory_order_release);  // release prev's next
        } else {
          Collection->FirstLock.clear(std::memory_order_release);  // release first
        }
        Collection->LastLock.clear(std::memory_order_release);  // release last
      }

      /* Remove us from our collection.
         If we're not in a collection, this function does nothing. */
      void Remove() {
        assert(this);
        assert(Collection);
        /* Fixup the pointers on either side of us to point around us,
           then go back to the unlinked state. */
        _mm_prefetch(&NextMembership->PrevLock, _MM_HINT_T0);
        _mm_prefetch(reinterpret_cast<uint8_t *>(&NextMembership->Collection), _MM_HINT_T0);
        _mm_prefetch(&PrevMembership->PrevLock, _MM_HINT_T0);
        _mm_prefetch(reinterpret_cast<uint8_t *>(&PrevMembership->Collection), _MM_HINT_T0);
        for (;;) {
          while (PrevLock.test_and_set(std::memory_order_acquire)) {/* acquire prev lock */}
          if (PrevMembership) {
            if (PrevMembership->NextLock.test_and_set(std::memory_order_acquire)) {  /* acquire prev's next lock */
              PrevLock.clear(std::memory_order_release);  // release prev
              continue;
            } else {
              break;
            }
          } else {
            if (Collection->FirstLock.test_and_set(std::memory_order_acquire)) {  /* acquire list first lock */
              PrevLock.clear(std::memory_order_release);  // release prev
              continue;
            } else {
              break;
            }
          }
        }
        while (NextLock.test_and_set(std::memory_order_acquire)) {/* acquire next lock */}
        if (NextMembership) {
          while (NextMembership->PrevLock.test_and_set(std::memory_order_acquire)) {/* acquire next's prev lock */}
        } else {
          while (Collection->LastLock.test_and_set(std::memory_order_acquire)) {/* acquire list last lock */}
        }
        FixupLinkage(NextMembership, PrevMembership);
        if (NextMembership) {
          NextMembership->PrevLock.clear(std::memory_order_release);  // release next's prev
        } else {
          Collection->LastLock.clear(std::memory_order_release);  // release last
        }
        NextLock.clear();  // release next
        if (PrevMembership) {
          PrevMembership->NextLock.clear(std::memory_order_release); // release prev's next
        } else {
          Collection->FirstLock.clear(std::memory_order_release); // release first
        }
        PrevLock.clear(std::memory_order_release); // release prev
        ZeroLinkage();
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

      /* TODO */
      std::atomic_flag PrevLock, NextLock;

      /* See accessors. */
      TMembership *PrevMembership, *NextMembership;

      /* See accessor. */
      TTypedCollection *Collection;

      /* See accessor. */
      TMember *const Member;

      /* For ~TMembership(), Insert(), Remove(), and Member. */
      friend class TCollection<TCollector, TMember>;

    };  // TMembership<TMember, TCollector>

    /* The final versions of the 'one' and 'many' classes. */
    namespace Impl {

      /* The final 'one'. */
      template <typename TCollector, typename TMember>
      class TCollection
          : public AtomicUnorderedList::TCollection<TCollector, TMember> {
        public:

        /* Our base type. */
        typedef AtomicUnorderedList::TCollection<TCollector, TMember> TBase;

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
          : public AtomicUnorderedList::TMembership<TMember, TCollector> {
        public:

        /* Our base type. */
        typedef AtomicUnorderedList::TMembership<TMember, TCollector> TBase;

        /* Do-little. */
        TMembership(TMember *member)
            : TBase(member) {}

        /* Do-little. */
        TMembership(TMember *member, typename TBase::TTypedCollection *collection)
            : TBase(member, collection) {}

        /* Do-little. */
        virtual ~TMembership() {}

        /* Make our base's protected mutators public. */
        using TBase::Insert;
        using TBase::Remove;

      };  // TMembership

    }  // Impl

  }  // UnorderedList

}  // InvCon
