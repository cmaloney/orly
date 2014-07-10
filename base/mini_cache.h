/* <base/mini_cache.h>

   TODO

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
      for (size_t i = 0; i < NumElem; ++i) {
        TElem *ptr = reinterpret_cast<TElem *>(ElemSpace + sizeof(TElem) * i);
        ptr->~TElem();
      }
    }

    /* TODO */
    template <typename... TFwdKeyArgs, typename... TFwdValArgs>
    void inline Emplace(std::tuple<TFwdKeyArgs...> &&key_args, std::tuple<TFwdValArgs...> &&val_args) {
      assert(this);
      assert(TryGet(TElem::template Construct<TKey>(std::move(key_args), std::index_sequence_for<TFwdKeyArgs...>())) == nullptr);
      TElem *ptr;
      if (NumElem < MaxSize) {
        ptr = reinterpret_cast<TElem *>(ElemSpace + sizeof(TElem) * NumElem);
        ++NumElem;
      } else {
        assert(NumElem == MaxSize);
        ptr = LRUList.TryGetFirstMember();
        ptr->~TElem();
      }
      new (ptr) TElem(this, std::forward<std::tuple<TFwdKeyArgs...>>(key_args), std::forward<std::tuple<TFwdValArgs...>>(val_args));
    }

    /* TODO */
    inline TVal *TryGet(const TKey &k) const {
      assert(this);
      TElem *elem = ElemMap.TryGetFirstMember(k);
      if (elem) {
        elem->LRUMembership.Remove();
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

      /* TODO */
      template <typename T, typename... Ts, std::size_t... Is>
      static inline T Construct(std::tuple<Ts...> &&ts, std::index_sequence<Is...>) {
        return T(std::get<Is>(std::move(ts))...);
      }

      /* TODO */
      using TMapMembership = InvCon::UnorderedMultimap::TMembership<TElem, TMiniCache, TKey>;
      using TListMembership = InvCon::UnorderedList::TMembership<TElem, TMiniCache>;

      /* TODO */
      template <typename... TFwdKeyArgs, typename... TFwdValArgs>
      inline TElem(TMiniCache *cache, std::tuple<TFwdKeyArgs...> &&key_args, std::tuple<TFwdValArgs...> &&val_args)
          : CacheMembership(this, &cache->ElemMap, std::move(key_args), std::index_sequence_for<TFwdKeyArgs...>()),
            LRUMembership(this, &cache->LRUList),
            Val(Construct<TVal>(std::move(val_args), std::index_sequence_for<TFwdValArgs...>())) {}

      /* TODO */
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

    /* TODO */
    char ElemSpace[sizeof(TElem) * MaxSize];

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
