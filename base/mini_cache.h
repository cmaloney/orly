/* <base/mini_cache.h>

     A hash based cache implementation that constructs all elements in place
   in pre-allocated storage. This is good for keeping a small cache on the
   stack or heap. It keeps an LRU that is updated on every TryGet() call.
   Once the capacity constraint is met, elements get destroyed in LRU order
   to make room for new inserts.

   example usage:

   using TKey = int64_t;
   using TVal = int64_t;
   TMiniCache<64, TKey, TVal> cache;
   cache.Emplace(std::forward_as_tuple(1L), std::forward_as_tuple(2L));
   const TKey search_key(1L);
   const TVal *val = cache.TryGet(search_key);
   if (val) {
     // do something with val
   } else {
     // the key does not exist in cache
   }

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <base/class_traits.h>
#include <inv_con/unordered_list.h>
#include <inv_con/unordered_multimap.h>

namespace Base {

  /* TODO */
  template <size_t MaxSize, typename TKey, typename TVal>
  class TMiniCache {
    NO_COPY(TMiniCache);
    public:

    /* TODO */
    TMiniCache(size_t bucket_count = MaxSize) : ElemMap(this, bucket_count), LRUList(this), NumElem(0) {}

    /* TODO */
    ~TMiniCache() {
      assert(this);
      // destroy all the elements
      for (size_t i = 0; i < NumElem; ++i) {
        TElem *ptr = &ElemSpace[i];
        ptr->~TElem();
      }
    }

    /* Insert a new TElem in-place using forward constructed args. */
    template <typename... TFwdKeyArgs, typename... TFwdValArgs>
    void inline Emplace(std::tuple<TFwdKeyArgs...> &&key_args, std::tuple<TFwdValArgs...> &&val_args) {
      assert(this);
      assert(TryGet(TElem::template Construct<TKey>(std::move(key_args), std::index_sequence_for<TFwdKeyArgs...>())) == nullptr);
      TElem *ptr;
      if (NumElem < MaxSize) {
        // until the pre-allocated space is full, just construct the TElem in the next slot
        ptr = &ElemSpace[NumElem];
        ++NumElem;
      } else {
        // once the pre-allocated space is full, destroy the elem at the front of the lru, and then construct the new one in that place
        assert(NumElem == MaxSize);
        ptr = LRUList.TryGetFirstMember();
        ptr->~TElem();
      }
      // construct the new element in-place
      new (ptr) TElem(this, std::forward<std::tuple<TFwdKeyArgs...>>(key_args), std::forward<std::tuple<TFwdValArgs...>>(val_args));
    }

    /* Return a pointer to the value, or nullptr if this key is no longer cached. */
    inline TVal *TryGet(const TKey &k) const {
      assert(this);
      TElem *elem = ElemMap.TryGetFirstMember(k);
      if (elem) {
        // put us at the end of the LRU
        elem->LRUMembership.Insert(&LRUList);
        return &(elem->Val);
      }
      return nullptr;
    }

    private:

    /* Forward declarations. */
    class TElem;

    /* TODO */
    using TElemMap = InvCon::UnorderedMultimap::TCollection<TMiniCache, TElem, TKey>;
    using TElemList = InvCon::UnorderedList::TCollection<TMiniCache, TElem>;

    /* TODO */
    class TElem {
      NO_COPY(TElem);
      public:

      /* A helper function that explodes the tuple back into arguments. */
      template <typename T, typename... Ts, std::size_t... Is>
      static inline T Construct(std::tuple<Ts...> &&ts, std::index_sequence<Is...>) {
        return T(std::get<Is>(std::move(ts))...);
      }

      /* TODO */
      using TMapMembership = InvCon::UnorderedMultimap::TMembership<TElem, TMiniCache, TKey>;
      using TListMembership = InvCon::UnorderedList::TMembership<TElem, TMiniCache>;

      /* Automatically insert this TElem into the hash table, and the end of the LRU list. */
      template <typename... TFwdKeyArgs, typename... TFwdValArgs>
      inline TElem(TMiniCache *cache, std::tuple<TFwdKeyArgs...> &&key_args, std::tuple<TFwdValArgs...> &&val_args)
          : CacheMembership(this, &cache->ElemMap, std::move(key_args), std::index_sequence_for<TFwdKeyArgs...>()),
            LRUMembership(this, &cache->LRUList),
            Val(Construct<TVal>(std::move(val_args), std::index_sequence_for<TFwdValArgs...>())) {}

      /* Remove ourselves from the hash map and LRU list. */
      inline ~TElem() {
        assert(this);
        CacheMembership.Remove();
        LRUMembership.Remove();
      }

      private:

      /* TODO */
      typename TMapMembership::TImpl CacheMembership;
      typename TListMembership::TImpl LRUMembership;

      /* TODO */
      TVal Val;

      friend class TMiniCache;

    };  // TElem

    /* TODO */
    mutable typename TElemMap::TImpl ElemMap;

    /* TODO */
    mutable typename TElemList::TImpl LRUList;

    union {
      TElem ElemSpace[MaxSize];
    };

    /* TODO */
    size_t NumElem;

  };  // TMiniCache

  /* Specialization of an empty (non)-cache that saves space. */
  template <>
  template <typename TKey, typename TVal>
  class TMiniCache<0, TKey, TVal> {
    NO_COPY(TMiniCache);
    public:

    /* Do-little. */
    TMiniCache() {}

    /* Do-little. */
    template <typename... TFwdKeyArgs, typename... TFwdValArgs>
    void inline Emplace(std::tuple<TFwdKeyArgs...> &&/*key_args*/, std::tuple<TFwdValArgs...> &&/*val_args*/) {}

    /* Do-little. */
    inline TVal *TryGet(const TKey &/*k*/) const {
      return nullptr;
    }

  }; // TMiniCache

}  // Base
