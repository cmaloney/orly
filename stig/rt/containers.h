/* <stig/rt/containers.h>

   The containers runtime value. eg. TSet, TDict

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
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include <stig/rt/mutable.h>
#include <stig/rt/opt.h>
#include <base/stl_utils.h>

namespace Stig {

  namespace Rt {

    /* Match */
    template <typename TVal>
    bool Match(const TVal &lhs, const TVal &rhs) {
      assert(&lhs);
      assert(&rhs);
      return lhs == rhs;
    }

    /* MatchLess */
    template <typename TVal>
    bool MatchLess(const TVal &lhs, const TVal &rhs) {
      assert(&lhs);
      assert(&rhs);
      return lhs < rhs;
    }

    /* TMatch */
    template <typename TVal>
    struct TMatch {

      /* TODO */
      bool operator()(const TVal &lhs, const TVal &rhs) const { return Match(lhs, rhs); }

    };

    /* TMatchLess */
    template <typename TVal>
    struct TMatchLess {

      /* TODO */
      bool operator()(const TVal &lhs, const TVal &rhs) const { return MatchLess(lhs, rhs); }

    };

    /* TODO */
    template <typename TKey, typename TVal>
    //using TDict = std::unordered_map<TKey, TVal, std::hash<TKey>, TMatch<TKey>>;
    using TDict = std::map<TKey, TVal, TMatchLess<TKey>>;

    /* TODO */
    template <typename TVal>
    //using TSet = std::unordered_set<TVal, std::hash<TVal>, TMatch<TVal>>;
    using TSet = std::set<TVal, TMatchLess<TVal>>;

    /* Match an TOpt<TVal> and TOpt<TVal> */
    template <typename TVal>
    bool Match(const TOpt<TVal> &lhs, const TOpt<TVal> &rhs) {
      assert(&lhs);
      assert(&rhs);
      return lhs.IsKnown() && rhs.IsKnown() ? Match(lhs.GetVal(), rhs.GetVal())
        : lhs.IsUnknown() && rhs.IsUnknown() ? true : false;
    }

    /* Match an TOpt<TVal> and TOpt<TVal> */
    template <typename TVal>
    bool MatchLess(const TOpt<TVal> &lhs, const TOpt<TVal> &rhs) {
      assert(&lhs);
      assert(&rhs);
      return lhs.IsKnown() && rhs.IsKnown() ? MatchLess(lhs.GetVal(), rhs.GetVal())
        : lhs.IsUnknown() && rhs.IsUnknown() ? false : lhs.IsUnknown() ? true : false;
    }

    /* Match an TList<TVal> and TList<TVal> */
    template <typename TVal>
    bool Match(const std::vector<TVal> &lhs, const std::vector<TVal> &rhs) {
      assert(&lhs);
      assert(&rhs);
      if (lhs.size() != rhs.size()) {
        return false;
      }
      auto lhs_iter = lhs.begin();
      auto rhs_iter = rhs.begin();
      for (; lhs_iter != lhs.end() && rhs_iter != rhs.end(); ++lhs_iter, ++rhs_iter) {
        if (!Match(*lhs_iter, *rhs_iter)) {
          return false;
        }
      }
      return true;
    }

    /* Match an TList<TVal> and TList<TVal> */
    template <typename TVal>
    bool MatchLess(const std::vector<TVal> &lhs, const std::vector<TVal> &rhs) {
      assert(&lhs);
      assert(&rhs);
      auto lhs_iter = lhs.begin();
      auto rhs_iter = rhs.begin();
      for (; lhs_iter != lhs.end() && rhs_iter != rhs.end(); ++lhs_iter, ++rhs_iter) {
        if (MatchLess(*lhs_iter, *rhs_iter)) {
          return true;
        } else if (!Match(*lhs_iter, *rhs_iter)) {
          return false;
        }
      }
      if (lhs_iter == lhs.end() && rhs_iter == rhs.end()) {
        return false;
      }
      return lhs_iter == lhs.end();
    }

    /* Match an TSet<TVal> and TSet<TVal> */
    template <typename TVal>
    bool Match(const TSet<TVal> &lhs, const TSet<TVal> &rhs) {
      assert(&lhs);
      assert(&rhs);
      if (lhs.size() != rhs.size()) {
        return false;
      }
      for (auto iter : lhs) {
        if (!Base::Contains(rhs, iter)) {
          return false;
        }
      }
      return true;
    }

    /* Match an TDict<TVal> and TDict<TVal> */
    template <typename TKey, typename TVal>
    bool Match(const TDict<TKey, TVal> &lhs, const TDict<TKey, TVal> &rhs) {
      assert(&lhs);
      assert(&rhs);
      if (lhs.size() != rhs.size()) {
        return false;
      }
      for (auto iter : lhs) {
        auto pos = rhs.find(iter.first);
        if (pos == rhs.end() || !Match(pos->second, iter.second)) {
          return false;
        }
      }
      return true;
    }

    template <typename TAddr, typename TVal>
    bool Match(const TMutable<TAddr, TVal> &lhs, const TMutable<TAddr, TVal> &rhs) {
      return Match(lhs.GetVal(), rhs.GetVal());
    }

  }  // Rt

}  // Stig

namespace std {

  /* A standard hasher for Rt::TDict<TKey, TVal>. */
  template <typename TKey, typename TVal>
  struct hash<Stig::Rt::TDict<TKey, TVal>> {

    typedef size_t result_type;
    typedef Stig::Rt::TDict<TKey, TVal> argument_type;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      result_type result = 0;
      for (const auto &elem : that) {
        result ^= hash<pair<TKey, TVal>>()(elem);
      }
      return result;
    }

  };  // hash<Stig::Rt::TDict<TKey, TVal>>

  /* A standard hasher for Rt::TSet<TVal>. */
  template <typename TVal>
  struct hash<Stig::Rt::TSet<TVal>> {

    typedef size_t result_type;
    typedef Stig::Rt::TSet<TVal> argument_type;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      result_type result = 0;
      for (const auto &elem : that) {
        result ^= hash<TVal>()(elem);
      }
      return result;
    }

  };  // hash<Stig::Rt::TSet<TVal>>

}  // std

/* Add : dict + dict */
template <typename TKey, typename TVal>
Stig::Rt::TDict<TKey, TVal> operator+(
      const Stig::Rt::TDict<TKey, TVal> &lhs,
      const Stig::Rt::TDict<TKey, TVal> &rhs) {
  Stig::Rt::TDict<TKey, TVal> temp(lhs);
  // We do it this way to over-ride conflicts
  for (auto iter : rhs) {
    auto ret = temp.insert(iter);
    if (!ret.second) {
      ret.first->second = iter.second;
    }
  }
  return temp;
}

/* Sub : dict - set */
template <typename TKey, typename TVal>
Stig::Rt::TDict<TKey, TVal> operator-(
      const Stig::Rt::TDict<TKey, TVal> &lhs,
      const Stig::Rt::TSet<TKey> &rhs) {
  Stig::Rt::TDict<TKey, TVal> temp(lhs);
  for (auto iter : rhs) {
    temp.erase(iter);
  }
  return temp;
}

/* Add : list + list */
template <typename TVal>
std::vector<TVal> operator+(const std::vector<TVal> &lhs, const std::vector<TVal> &rhs) {
  std::vector<TVal> ret(lhs);
  ret.insert(ret.end(), rhs.begin(), rhs.end());
  return ret;
}

/* Sub : set - set */
template <typename TVal>
Stig::Rt::TSet<TVal> operator-(const Stig::Rt::TSet<TVal> &lhs, const Stig::Rt::TSet<TVal> &rhs) {
  Stig::Rt::TSet<TVal> result(lhs);
  for (auto elem : rhs) {
    result.erase(elem);
  }
  return result;
}

/* Intersection : set & set */
template <typename TVal>
Stig::Rt::TSet<TVal> operator&(const Stig::Rt::TSet<TVal> &lhs, const Stig::Rt::TSet<TVal> &rhs) {
  Stig::Rt::TSet<TVal> result;
  for (auto elem : lhs) {
    if (Base::Contains(rhs, elem)) {
      result.insert(elem);
    }
  }
  return result;
}

/* Union : set | set */
template <typename TVal>
Stig::Rt::TSet<TVal> operator|(const Stig::Rt::TSet<TVal> &lhs, const Stig::Rt::TSet<TVal> &rhs) {
  Stig::Rt::TSet<TVal> result(lhs);
  // We do it this way to over-ride conflicts
  for (auto elem : rhs) {
    result.insert(elem);
  }
  return result;
}

/* SymmetricDiff : set ^ set */
template <typename TVal>
Stig::Rt::TSet<TVal> operator^(const Stig::Rt::TSet<TVal> &lhs, const Stig::Rt::TSet<TVal> &rhs) {
  Stig::Rt::TSet<TVal> result;
  for (auto elem : lhs) {
    if (!Base::Contains(rhs, elem)) {
      result.insert(elem);
    }
  }
  for (auto elem : rhs) {
    if (!Base::Contains(lhs, elem)) {
      result.insert(elem);
    }
  }
  return result;
}
