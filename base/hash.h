/* <base/hash.h>

   TODO

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

#include <array>
#include <map>
#include <set>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <base/no_construction.h>
#include <base/stl_utils.h>
#include <base/tuple_utils.h>

namespace Base {

  template <typename TElem>
  struct THash {
    bool operator()(const TElem &elem, size_t index, size_t &out) {
      assert(&elem);
      assert(&out);
      out ^= Base::RotatedLeft(std::hash<TElem>()(elem), index * 5);
      return true;
    }
  };  // THash

  inline size_t CombineHashes(size_t lhs, size_t rhs) {
    return lhs ^ (lhs + 0x9e3779b9 + (rhs << 6) + (rhs >> 2));
  }

  template <typename... TArgs>
  class HashChainer;

  template <typename TArg, typename... TMoreArgs>
  class HashChainer<TArg, TMoreArgs...> {
    NO_CONSTRUCTION(HashChainer);
    public:

    static size_t Hash(const TArg &arg, const TMoreArgs &... more_args) {
      return CombineHashes(std::hash<TArg>()(arg), HashChainer<TMoreArgs...>::Hash(more_args...));
    }

  };  // HashChainer<TArg, TMoreArgs...>

  template <typename TArg>
  class HashChainer<TArg> {
    NO_CONSTRUCTION(HashChainer);
    public:

    static size_t Hash(const TArg &arg) {
      return std::hash<TArg>()(arg);
    }

  };  // HashChainer<TArg>

  template <typename... TArgs>
  size_t ChainHashes(const TArgs &... args) {
    return HashChainer<TArgs...>::Hash(args...);
  }

}  // Base

namespace std {

  template <typename TElem, size_t size>
  struct hash<array<TElem, size>> {

    typedef size_t result_type;
    typedef array<TElem, size> argument_type;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      result_type result = 0;
      Base::ForEachElem<Base::THash>(that, result);
      return result;
    }

  };  // hash<array<TElems, size>>

  template <typename TKey, typename TVal>
  struct hash<map<TKey, TVal>> {

    typedef size_t result_type;
    typedef map<TKey, TVal> argument_type;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      result_type result = 0;
      for (const auto &elem : that) {
        result ^= hash<pair<TKey, TVal>>()(elem);
      }
      return result;
    }

  };  // hash<map<TKey, TVal>>

  template <typename TLhs, typename TRhs>
  struct hash<pair<TLhs, TRhs>> {

    typedef size_t result_type;
    typedef pair<TLhs, TRhs> argument_type;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      return hash<TLhs>()(that.first) ^ hash<TRhs>()(that.second);
    }

  };  // hash<pair<TLhs, TRhs>>

  template <typename TElem>
  struct hash<set<TElem>> {

    typedef size_t result_type;
    typedef set<TElem> argument_type;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      result_type result = 0;
      for (const auto &elem : that) {
        result ^= hash<TElem>()(elem);
      }
      return result;
    }

  };  // hash<set<TElem>>

  template <typename... TElems>
  struct hash<tuple<TElems...>> {

    typedef size_t result_type;
    typedef tuple<TElems...> argument_type;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      result_type result = 0;
      Base::ForEachElem<Base::THash>(that, result);
      return result;
    }

  };  // hash<tuple<TElems...>>

  template <typename TKey, typename TVal>
  struct hash<unordered_map<TKey, TVal>> {

    typedef size_t result_type;
    typedef unordered_map<TKey, TVal> argument_type;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      result_type result = 0;
      for (const auto &elem : that) {
        result ^= hash<pair<TKey, TVal>>()(elem);
      }
      return result;
    }

  };  // hash<unordered_map<TKey, TVal>>

  template <typename TElem>
  struct hash<unordered_set<TElem>> {

    typedef size_t result_type;
    typedef unordered_set<TElem> argument_type;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      result_type result = 0;
      for (const auto &elem : that) {
        result ^= hash<TElem>()(elem);
      }
      return result;
    }

  };  // hash<unordered_set<TElem>>

  template <typename TElem>
  struct hash<vector<TElem>> {

    typedef size_t result_type;
    typedef vector<TElem> argument_type;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      result_type result = 0;
      for (unsigned int i = 0; i < that.size(); ++i) {
        Base::THash<TElem>()(that[i], i, result);
      }
      return result;
    }

  };  // hash<vector<TElem>>

}  // std
