/* <base/hash.h>

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

#pragma once

#include <array>
#include <map>
#include <set>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <base/class_traits.h>

#include <util/stl.h>
#include <util/tuple.h>

namespace Base {

  struct TTupleHash {

    explicit TTupleHash(std::size_t &out) : Out(out) {}

    template <std::size_t Idx, typename TElem>
    void operator()(const TElem &elem) {
      assert(this);
      assert(&elem);
      Out ^= Util::RotatedLeft(std::hash<TElem>()(elem), Idx * 5);
    }

    private:

    std::size_t &Out;

  };  // TTupleHash

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

    using result_type = size_t;
    using argument_type = array<TElem, size>;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      result_type result = 0;
      Util::ForEach(that, Base::TTupleHash(result));
      return result;
    }

  };  // hash<array<TElems, size>>

  template <typename TKey, typename TVal>
  struct hash<map<TKey, TVal>> {

    using result_type = size_t;
    using argument_type = map<TKey, TVal>;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      result_type result = 0;
      for (const auto &elem : that) {
        result ^= hash<pair<TKey, TVal>>()(elem);
      }  // for
      return result;
    }

  };  // hash<map<TKey, TVal>>

  template <typename TLhs, typename TRhs>
  struct hash<pair<TLhs, TRhs>> {

    using result_type = size_t;
    using argument_type = pair<TLhs, TRhs>;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      return hash<TLhs>()(that.first) ^ hash<TRhs>()(that.second);
    }

  };  // hash<pair<TLhs, TRhs>>

  template <typename TElem>
  struct hash<set<TElem>> {

    using result_type = size_t;
    using argument_type = set<TElem>;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      result_type result = 0;
      for (const auto &elem : that) {
        result ^= hash<TElem>()(elem);
      }  // for
      return result;
    }

  };  // hash<set<TElem>>

  template <typename... TElems>
  struct hash<tuple<TElems...>> {

    using result_type = size_t;
    using argument_type = tuple<TElems...>;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      result_type result = 0;
      Util::ForEach(that, Base::TTupleHash(result));
      return result;
    }

  };  // hash<tuple<TElems...>>

  template <typename TKey, typename TVal>
  struct hash<unordered_map<TKey, TVal>> {

    using result_type = size_t;
    using argument_type = unordered_map<TKey, TVal>;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      result_type result = 0;
      for (const auto &elem : that) {
        result ^= hash<pair<TKey, TVal>>()(elem);
      }  // for
      return result;
    }

  };  // hash<unordered_map<TKey, TVal>>

  template <typename TElem>
  struct hash<unordered_set<TElem>> {

    using result_type = size_t;
    using argument_type = unordered_set<TElem>;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      result_type result = 0;
      for (const auto &elem : that) {
        result ^= hash<TElem>()(elem);
      }  // for
      return result;
    }

  };  // hash<unordered_set<TElem>>

  template <typename TElem>
  struct hash<vector<TElem>> {

    using result_type = size_t;
    using argument_type = vector<TElem>;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      result_type result = 0;
      for (std::size_t i = 0; i < that.size(); ++i) {
        result ^= Util::RotatedLeft(std::hash<TElem>()(that[i]), i * 5);
      }  // for
      return result;
    }

  };  // hash<vector<TElem>>

}  // std
