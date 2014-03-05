/* <inv_con/unordered_multimap.h>

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
#include <functional>

#include <base/no_copy_semantics.h>
#include <base/no_default_case.h>
#include <inv_con/cursor.h>

namespace InvCon {

  /* Classes for maintaining an invasive, ordered, double-linked list. */
  namespace UnorderedMultimap {

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

      /* Call back once for each unique key in the collection.  If more than one member has
         the same key, call back for the first one in the collection.  You can use TryGetNextMemberWithSameKey()
         to spin through the rest.  If the callback returns false, stop generating immediately. */
      bool ForEachUniqueMember(const std::function<bool (TMember *)> &cb) const {
        assert(this);
        assert(&cb);
        auto csr = TryGetFirstMembership();
        bool result = true;
        while (csr) {
          if (!cb(csr->GetMember())) {
            result = false;
            break;
          }
          const TKey &key = csr->GetKey();
          do {
            csr = csr->TryGetNextMembership();
          } while (csr && csr->GetKey() == key);
        }
        return result;
      }

      /* TODO */
      static size_t GetBucketSize() {
        return sizeof(TBucket);
      }

      /* The collector which owns us.  Never null. */
      TCollector *GetCollector() const {
        assert(this);
        return Collector;
      }

      /* The first member of the collection,
         or null if the collection is empty. */
      TMember *TryGetFirstMember() const {
        assert(this);
        TTypedMembership *membership = TryGetFirstMembership();
        return membership ? membership->GetMember() : 0;
      }

      /* The first member of the collection which matches the given key,
         or null if the collection contains no match. */
      TMember *TryGetFirstMember(const TKey &key) const {
        assert(this);
        TTypedMembership *membership = TryGetFirstMembership(key);
        assert(membership == 0 || membership->GetKey() == key);
        return membership ? membership->GetMember() : 0;
      }

      /* The first membership of the collection,
         or null if the collection is empty. */
      TTypedMembership *TryGetFirstMembership() const {
        assert(this);
        TBucket *bucket = TBucket::GetFirstNonEmptyBucket(this);
        return bucket ? bucket->TryGetFirstMembership() : 0;
      }

      /* The first membership of the collection which matches the given key,
         or null if the collection contains no match. */
      TTypedMembership *TryGetFirstMembership(const TKey &key) const {
        assert(this);
        return GetBucket(key)->TryGetFirstMembership(key);
      }

      /* The last member of the collection,
         or null if the collection is empty. */
      TMember *TryGetLastMember() const {
        assert(this);
        TTypedMembership *membership = TryGetLastMembership();
        return membership ? membership->GetMember() : 0;
      }

      /* The last member of the collection which matches the given key,
         or null if the collection contains no match. */
      TMember *TryGetLastMember(const TKey &key) const {
        assert(this);
        TTypedMembership *membership = TryGetLastMembership(key);
        assert(membership == 0 || membership->GetKey() == key);
        return membership ? membership->GetMember() : 0;
      }

      /* The last membership of the collection,
         or null if the collection is empty. */
      TTypedMembership *TryGetLastMembership() const {
        assert(this);
        TBucket *bucket = TBucket::GetLastNonEmptyBucket(this);
        return bucket ? bucket->TryGetLastMembership() : 0;
      }

      /* The last membership of the collection which matches the given key,
         or null if the collection contains no match. */
      TTypedMembership *TryGetLastMembership(const TKey &key) const {
        assert(this);
        return GetBucket(key)->TryGetLastMembership(key);
      }

      /* TODO */
      bool IsEmpty() const {
        assert(this);
        for (size_t i = 0; i < BucketCount; ++i) {
          if (!BucketArray[i].IsEmpty()) {
            return false;
          }
        }
        return true;
      }

      protected:

      /* Pass in a non-null pointer to the collector which owns us
         and the (prime) number of hash buckets to make. */
      TCollection(TCollector *collector, size_t bucket_count, void *allocation = 0)
          : Collector(collector) {
        assert(collector);
        assert(bucket_count);
        if (allocation) {
          BucketArray = new (allocation) TBucket[bucket_count];
        } else {
          BucketArray = new TBucket[bucket_count];
        }
        BucketCount = bucket_count;
        for (size_t i = 0; i < BucketCount; ++i) {
          BucketArray[i].Init(this, i);
        }
      }

      /* Removes each member from the collection upon destruction. */
      virtual ~TCollection() {
        assert(this);
        delete [] BucketArray;
      }

      /* Delete each member. */
      void DeleteEachMember() {
        assert(this);
        for (size_t i = 0; i < BucketCount; ++i) {
          BucketArray[i].DeleteEachMember();
        }
      }

      /* Destroy each member. */
      void DestroyEachMember() {
        assert(this);
        for (size_t i = 0; i < BucketCount; ++i) {
          BucketArray[i].DestroyEachMember();
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

      /* Remove each member from the collection but don't delete them. */
      void RemoveEachMember() {
        assert(this);
        for (size_t i = 0; i < BucketCount; ++i) {
          BucketArray[i].RemoveEachMember();
        }
      }

      private:

      /* TODO */
      class TBucket {
        NO_COPY_SEMANTICS(TBucket);
        public:

        /* TODO */
        TBucket()
            : Collection(0), Idx(0), FirstMembership(0), LastMembership(0) {}

        /* TODO */
        ~TBucket() {
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

        /* Destroy each member. */
        void DestroyEachMember() {
          assert(this);
          while (FirstMembership) {
            TMember *member = FirstMembership->Member;
            FirstMembership->Remove();
            member->~TMember();
          }
        }

        /* TODO */
        TBucket *GetPrevNonEmptyBucket() {
          assert(this);
          TBucket *bucket = this;
          for (;;) {
            if (bucket->Idx == 0) {
              bucket = 0;
              break;
            }
            --bucket;
            if (bucket->FirstMembership) {
              break;
            }
          }
          return bucket;
        }

        /* TODO */
        TBucket *GetNextNonEmptyBucket() {
          assert(this);
          TBucket *bucket = this;
          for (;;) {
            if (bucket->Idx == Collection->BucketCount - 1) {
              bucket = 0;
              break;
            }
            ++bucket;
            if (bucket->FirstMembership) {
              break;
            }
          }
          return bucket;
        }

        /* TODO */
        void Init(TCollection *collection, size_t idx) {
          assert(this);
          Collection = collection;
          Idx = idx;
        }

        bool IsEmpty() {
          assert(this);
          return FirstMembership == nullptr;
        }

        /* Remove each member from the bucket but don't delete them. */
        void RemoveEachMember() {
          assert(this);
          while (FirstMembership) {
            FirstMembership->Remove();
          }
        }

        /* The first membership of the bucket, or null if the bucket is empty. */
        TTypedMembership *TryGetFirstMembership() const {
          assert(this);
          return FirstMembership;
        }

        /* The first membership of the bucket which matches the given key,
           or null if the bucket has no match. */
        TTypedMembership *TryGetFirstMembership(const TKey &key) const {
          assert(this);
          TTypedMembership *membership = FirstMembership;
          while (membership && !(membership->Key == key)) {
            membership = membership->NextMembership;
          }
          return membership;
        }

        /* The last membership of the bucket, or null if the bucket is empty. */
        TTypedMembership *TryGetLastMembership() const {
          assert(this);
          return LastMembership;
        }

        /* The last membership of the bucket which matches the given key,
           or null if the bucket has no match. */
        TTypedMembership *TryGetLastMembership(const TKey &key) const {
          assert(this);
          TTypedMembership *membership = LastMembership;
          while (membership && !(membership->Key == key)) {
            membership = membership->PrevMembership;
          }
          return membership;
        }

        /* TODO */
        static TBucket *GetFirstNonEmptyBucket(const TCollection *collection) {
          assert(collection);
          TBucket *bucket = collection->BucketArray;
          if (!bucket->FirstMembership) {
            bucket = bucket->GetNextNonEmptyBucket();
          }
          return bucket;
        }

        /* TODO */
        static TBucket *GetLastNonEmptyBucket(const TCollection *collection) {
          assert(collection);
          TBucket *bucket = collection->BucketArray + (collection->BucketCount - 1);
          if (!bucket->FirstMembership) {
            bucket = bucket->GetPrevNonEmptyBucket();
          }
          return bucket;
        }

        private:

        /* TODO */
        TCollection *Collection;

        /* TODO */
        size_t Idx;

        /* See accessors. */
        TTypedMembership *FirstMembership, *LastMembership;

        /* For Collection, FirstMembership, and LastMembership. */
        friend class TMembership<TMember, TCollector, TKey>;

      };  // TBucket

      /* TODO */
      TBucket *GetBucket(const TKey &key) const {
        assert(this);
        return BucketArray + (Hash(key) % BucketCount);
      }

      /* See accessor. */
      TCollector *const Collector;

      /* Our hash buckets. */
      TBucket *BucketArray;

      /* The number of buckets in BucketArray. */
      size_t BucketCount;

      /* TODO */
      std::hash<TKey> Hash;

      /* For Collector, GetBucket(), and TBucket. */
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
        return Bucket ? Bucket->Collection : 0;
      }

      /* The collector whose collection we're in, if any. */
      TCollector *TryGetCollector() const {
        assert(this);
        TTypedCollection *collection = TryGetCollection();
        return collection ? collection->Collector : 0;
      }

      /* The member before us in our collection.
         A null here means either we're first in our collection or we're not in a collection. */
      TMember *TryGetPrevMember() const {
        assert(this);
        TMembership *membership = TryGetPrevMembership();
        return membership ? membership->Member : 0;
      }

      /* The member before us in our collection, iff. that member has the same key as we have. */
      TMember *TryGetPrevMemberWithSameKey() const {
        assert(this);
        TMembership *membership = TryGetPrevMembership();
        return (membership && Key == membership->Key) ? membership->Member : 0;
      }

      /* The membership before us in our collection.
         A null here means either we're first in our collection or we're not in a collection. */
      TMembership *TryGetPrevMembership() const {
        assert(this);
        TMembership *membership = PrevMembership;
        if (!membership && Bucket) {
          TBucket *bucket = Bucket->GetPrevNonEmptyBucket();
          membership = bucket ? bucket->LastMembership : 0;
        }
        return membership;
      }

      /* The member after us in our collection.
         A null here means either we're last in our collection or we're not in a collection. */
      TMember *TryGetNextMember() const {
        assert(this);
        TMembership *membership = TryGetNextMembership();
        return membership ? membership->Member : 0;
      }

      /* The member after us in our collection, iff. that member has the same key as we have. */
      TMember *TryGetNextMemberWithSameKey() const {
        assert(this);
        TMembership *membership = TryGetNextMembership();
        return (membership && Key == membership->Key) ? membership->Member : 0;
      }

      /* The membership after us in our collection.
         A null here means either we're last in our collection or we're not in a collection. */
      TMembership *TryGetNextMembership() const {
        assert(this);
        TMembership *membership = NextMembership;
        if (!membership && Bucket) {
          TBucket *bucket = Bucket->GetNextNonEmptyBucket();
          membership = bucket ? bucket->FirstMembership : 0;
        }
        return membership;
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
      TMembership(TMember *member, const TKey &key, TTypedCollection *collection)
          : Member(member), Key(key) {
        assert(member);
        ZeroLinkage();
        if (collection) {
          Insert(collection);
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
        TBucket *bucket = collection->GetBucket(Key);
        TMembership
            *prev_membership = 0,
            *next_membership = bucket->FirstMembership;
        bool is_found = false;
        while (next_membership) {
          bool is_match = (Key == next_membership->Key);
          if (!is_found) {
            is_found = is_match;
          } else if (!is_match) {
            break;
          }
          prev_membership = next_membership;
          next_membership = next_membership->NextMembership;
        }
        if (Bucket != bucket || PrevMembership != prev_membership || NextMembership != next_membership) {
          Remove();
          Bucket = bucket;
          PrevMembership = prev_membership;
          NextMembership = next_membership;
          FixupLinkage(this, this);
        }
      }

      /* Remove us from our collection.
         If we're not in a collection, this function does nothing. */
      void Remove() {
        assert(this);
        if (Bucket) {
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
          if (Bucket) {
            Insert(Bucket->Collection);
          }
        }
      }

      private:

      /* TODO */
      typedef typename TTypedCollection::TBucket TBucket;

      /* The fixup pointers before and after us in our collection.
         We must be in a collection or this is an error. */
      void FixupLinkage(TMembership *prev_fixup, TMembership *next_fixup) {
        assert(this);
        assert(Bucket);
        (PrevMembership ? PrevMembership->NextMembership : Bucket->FirstMembership) = prev_fixup;
        (NextMembership ? NextMembership->PrevMembership : Bucket->LastMembership ) = next_fixup;
      }

      /* Reset all pointers to null.
         This function does no unlinking so make sure we're unlinked first. */
      void ZeroLinkage() {
        assert(this);
        Bucket = 0;
        NextMembership = 0;
        PrevMembership = 0;
      }

      /* See accessor. */
      TMember *const Member;

      /* See accessor. */
      TKey Key;

      /* See accessor. */
      TBucket *Bucket;

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
          : public UnorderedMultimap::TCollection<TCollector, TMember, TKey> {
        public:

        /* Our base type. */
        typedef UnorderedMultimap::TCollection<TCollector, TMember, TKey> TBase;

        /* Do-little. */
        TCollection(TCollector *collector, size_t bucket_count = 1023, void *allocation = 0)
            : TBase(collector, bucket_count, allocation) {}

        /* Do-little. */
        virtual ~TCollection() {}

        /* Make our base's protected mutators public. */
        using TBase::DeleteEachMember;
        using TBase::DestroyEachMember;
        using TBase::Insert;
        using TBase::RemoveEachMember;

      };  // TCollection

      /* The final 'many'. */
      template <typename TMember, typename TCollector, typename TKey>
      class TMembership
          : public UnorderedMultimap::TMembership<TMember, TCollector, TKey> {
        public:

        /* Our base type. */
        typedef UnorderedMultimap::TMembership<TMember, TCollector, TKey> TBase;

        /* Do-little. */
        TMembership(TMember *member)
            : TBase(member) {}

        /* Do-little. */
        TMembership(TMember *member, const TKey &key, typename TBase::TTypedCollection *collection = 0)
            : TBase(member, key, collection) {}

        /* Do-little. */
        virtual ~TMembership() {}

        /* Make our base's protected mutators public. */
        using TBase::Insert;
        using TBase::Remove;

      };  // TMembership

    }  // Impl

  }  // UnorderedMultimap

}  // InvCon
