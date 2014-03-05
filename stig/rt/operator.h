/* <stig/rt/operator.h>

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

#include <c14/lang.h>
#include <cassert>
#include <vector>

#include <base/no_construction.h>
#include <stig/rt/containers.h>
#include <stig/rt/mutable.h>
#include <stig/rt/opt.h>

namespace Stig {

  namespace Rt {

    /* Default behaviour for EqEq. */
    template <typename TLhs, typename TRhs>
    struct EqEqStruct {
      NO_CONSTRUCTION(EqEqStruct);

      static bool Do(const TLhs &lhs, const TRhs &rhs) {
        assert(&lhs);
        assert(&rhs);
        return lhs == rhs;
      }

    };  // EqEqStruct

    /* Default behaviour for Neq. */
    template <typename TLhs, typename TRhs>
    struct NeqStruct {
      NO_CONSTRUCTION(NeqStruct);

      static bool Do(const TLhs &lhs, const TRhs &rhs) {
        assert(&lhs);
        assert(&rhs);
        return lhs != rhs;
      }

    };  // NeqStruct

    /* Default behaviour for Lt. */
    template <typename TLhs, typename TRhs>
    struct LtStruct {
      NO_CONSTRUCTION(LtStruct);

      static bool Do(const TLhs &lhs, const TRhs &rhs) {
        assert(&lhs);
        assert(&rhs);
        return lhs < rhs;
      }

    };  // LtStruct

    /* Default behaviour for LtEq. */
    template <typename TLhs, typename TRhs>
    struct LtEqStruct {
      NO_CONSTRUCTION(LtEqStruct);

      static bool Do(const TLhs &lhs, const TRhs &rhs) {
        assert(&lhs);
        assert(&rhs);
        return lhs <= rhs;
      }

    };  // LtEqStruct

    /* Default behaviour for Gt. */
    template <typename TLhs, typename TRhs>
    struct GtStruct {
      NO_CONSTRUCTION(GtStruct);

      static bool Do(const TLhs &lhs, const TRhs &rhs) {
        assert(&lhs);
        assert(&rhs);
        return lhs > rhs;
      }

    };  // GtStruct

    /* Default behaviour for GtEq. */
    template <typename TLhs, typename TRhs>
    struct GtEqStruct {
      NO_CONSTRUCTION(GtEqStruct);

      static bool Do(const TLhs &lhs, const TRhs &rhs) {
        assert(&lhs);
        assert(&rhs);
        return lhs >= rhs;
      }

    };  // GtEqStruct

    /* The following functions are simply wrapper functions that we use in code gen.
       This is nice to have because this way we can get the C++ compiler to deduce the types for us,
       which means we don't have to specify the type in every one of these calls. */

    /* Wrapper function for EqEq. */
    template <typename TLhs, typename TRhs>
    auto EqEq(const TLhs &lhs, const TRhs &rhs)
      DECLTYPE_AUTO((EqEqStruct<TLhs, TRhs>::Do(lhs, rhs)));

    /* Wrapper function for Neq. */
    template <typename TLhs, typename TRhs>
    auto Neq(const TLhs &lhs, const TRhs &rhs)
      DECLTYPE_AUTO((NeqStruct<TLhs, TRhs>::Do(lhs, rhs)));

    /* Wrapper function for Lt. */
    template <typename TLhs, typename TRhs>
    auto Lt(const TLhs &lhs, const TRhs &rhs)
      DECLTYPE_AUTO((LtStruct<TLhs, TRhs>::Do(lhs, rhs)));

    /* Wrapper function for LtEq. */
    template <typename TLhs, typename TRhs>
    auto LtEq(const TLhs &lhs, const TRhs &rhs)
      DECLTYPE_AUTO((LtEqStruct<TLhs, TRhs>::Do(lhs, rhs)));

    /* Wrapper function for Gt. */
    template <typename TLhs, typename TRhs>
    auto Gt(const TLhs &lhs, const TRhs &rhs)
      DECLTYPE_AUTO((GtStruct<TLhs, TRhs>::Do(lhs, rhs)));

    /* Wrapper function for GtEq. */
    template <typename TLhs, typename TRhs>
    auto GtEq(const TLhs &lhs, const TRhs &rhs)
      DECLTYPE_AUTO((GtEqStruct<TLhs, TRhs>::Do(lhs, rhs)));

    /* The following are helpful functions that allow generic handling of optional cases.
       NOTE: In situations where you explicitly know that the value is an optional,
             prefer not to use these functions.  These are useful when you don't care
             if the value will turn out to be an optional or non-optional. */

    /* Returns true since we know the valus is known. */
    template <typename TVal>
    bool IsKnown(const TVal &) {
      return true;
    }

    /* Return true if the optional value is known. */
    template <typename TVal>
    bool IsKnown(const TOpt<TVal> &opt_val) {
      assert(&opt_val);
      return opt_val.IsKnown();
    }

    /* Return false since we know the value is known. */
    template <typename TVal>
    bool IsUnknown(const TVal &) {
      return false;
    }

    /* Returns true if the optional value is unknown. */
    template <typename TVal>
    bool IsUnknown(const TOpt<TVal> &opt_val) {
      assert(&opt_val);
      return opt_val.IsUnknown();
    }

    /* Pass-through. */
    template <typename TVal>
    const TVal &GetVal(const TVal &val) {
      assert(&val);
      return val;
    }

    /* Returns the value that the optional contains. */
    template <typename TVal>
    const TVal &GetVal(const TOpt<TVal> &opt_val) {
      assert(&opt_val);
      return opt_val.GetVal();
    }

    /* Returns true if the value is known and is equal to true. */
    inline bool IsKnownTrue(const TOpt<bool> &opt_val) {
      assert(&opt_val);
      return opt_val.IsKnown() && opt_val.GetVal() == true;
    }

    /* Returns true if the value is known and is equal to false. */
    inline bool IsKnownFalse(const TOpt<bool> &opt_val) {
      assert(&opt_val);
      return opt_val.IsKnown() && opt_val.GetVal() == false;
    }

    /* Logical operators. */

    inline bool And(bool lhs, bool rhs) { return lhs && rhs; }

    template <typename TLhs, typename TRhs>
    TOpt<bool> And(TLhs &&lhs, TRhs &&rhs) {
      assert(&lhs);
      assert(&rhs);
      return IsKnownFalse(lhs) || IsKnownFalse(rhs) ? false
          : IsKnown(lhs) && IsKnown(rhs) ? And(GetVal(lhs), GetVal(rhs)) : *TOpt<bool>::Unknown;
    }

    template <typename TLhs, typename TRhsFunction>
    auto AndThen(TLhs &&lhs, TRhsFunction &&rhs)
      DECLTYPE_AUTO(IsKnownFalse(lhs) ? false : And(lhs, rhs()));

    inline bool Not(bool val) { return !val; }

    inline TOpt<bool> Not(const TOpt<bool> &opt_val) {
      assert(&opt_val);
      return opt_val.IsKnown() ? Not(opt_val.GetVal()) : *TOpt<bool>::Unknown;
    }

    inline bool Or(bool lhs, bool rhs) { return lhs || rhs; }

    template <typename TLhs, typename TRhs>
    TOpt<bool> Or(TLhs &&lhs, TRhs &&rhs) {
      assert(&lhs);
      assert(&rhs);
      return IsKnownTrue(lhs) || IsKnownTrue(rhs) ? true
          : IsKnown(lhs) && IsKnown(rhs) ? Or(GetVal(lhs), GetVal(rhs)) : *TOpt<bool>::Unknown;
    }

    template <typename TLhs, typename TRhsFunction>
    auto OrElse(TLhs &&lhs, TRhsFunction &&rhs)
      DECLTYPE_AUTO(IsKnownTrue(lhs) ? true : Or(lhs, rhs()));

    inline bool Xor(bool lhs, bool rhs) { return lhs ^ rhs; }

    template <typename TLhs, typename TRhs>
    TOpt<bool> Xor(TLhs &&lhs, TRhs &&rhs) {
      assert(&lhs);
      assert(&rhs);
      return IsKnown(lhs) && IsKnown(rhs) ? Xor(GetVal(lhs), GetVal(rhs)) : *TOpt<bool>::Unknown;
    }

    /* TODO */
    template <typename TVal>
    auto IsKnownExpr(const TOpt<TVal> &opt_val, const TVal &val)
        -> decltype(And(IsKnown(opt_val), EqEq(GetVal(opt_val), val))) {
      assert(&opt_val);
      assert(&val);
      return AndThen(IsKnown(opt_val), [&opt_val, &val]() { return EqEq(GetVal(opt_val), val); });
    }

    namespace Opt {

      /* Compare an Rt::TOpt and an Rt::TOpt */
      template <typename TLhs, typename TRhs, typename TComp>
      static TOpt<bool> Compare(TLhs &&lhs, TRhs &&rhs, TComp &&comp) {
        assert(&lhs);
        assert(&rhs);
        assert(&comp);
        assert(comp);
        return IsKnown(lhs) && IsKnown(rhs) ? comp(GetVal(lhs), GetVal(rhs)) : *TOpt<bool>::Unknown;
      }

    }  // Opt

    namespace Dict {

      /* TODO */
      template <typename TLhsKey, typename TLhsVal, typename TRhsKey, typename TRhsVal>
      auto Equal(const TDict<TLhsKey, TLhsVal> &lhs, const TDict<TRhsKey, TRhsVal> &rhs, const bool is_eqeq)
              -> decltype(And(EqEq(lhs.begin()->first , rhs.begin()->first),
                              EqEq(lhs.begin()->second, rhs.begin()->second))) {
        assert(&lhs);
        assert(&rhs);
        if (lhs.size() != rhs.size()) {
          return !is_eqeq;
        }
        bool unknown = false;
        for (auto lhs_iter : lhs) {
          auto key_result = Contains(rhs, lhs_iter.first);
          if (IsKnownTrue(key_result)) {
            auto val_result = EqEq(rhs.find(lhs_iter.first)->second, lhs_iter.second);
            if (IsKnownFalse(val_result)) {
              return !is_eqeq;
            }
            if (IsUnknown(val_result)) {
              unknown = true;
            }
          }
          if (IsKnownFalse(key_result)) {
            return !is_eqeq;
          }
          if (IsUnknown(key_result)) {
            unknown = true;
            if (IsKnown(lhs_iter.second)) {
              bool all_diff = true;
              for (auto rhs_iter : rhs) {
                auto val_result = EqEq(rhs_iter.second, lhs_iter.second);
                if (!IsKnownFalse(val_result)) {
                  all_diff = false;
                  break;
                }
              }
              if (all_diff) {
                return !is_eqeq;
              }
            }
          }
        }
        return unknown ? decltype(And(EqEq(lhs.begin()->first , rhs.begin()->first),
                                      EqEq(rhs.begin()->second, rhs.begin()->second)))() : is_eqeq;
      }

    }  // Dict

    namespace List {

      /* TODO */
      template <typename TLhs, typename TRhs>
      auto Equal(const std::vector<TLhs> &lhs, const std::vector<TRhs> &rhs, const bool is_eqeq)
            -> decltype(EqEq(*lhs.begin(), *rhs.begin())) {
        assert(&lhs);
        assert(&rhs);
        if (lhs.size() != rhs.size()) {
          return !is_eqeq;
        }
        bool unknown = false;
        auto lhs_iter = lhs.begin();
        auto rhs_iter = rhs.begin();
        for (; lhs_iter != lhs.end() && rhs_iter != rhs.end(); ++lhs_iter, ++rhs_iter) {
          auto result = EqEq(*lhs_iter, *rhs_iter);
          if (IsKnownFalse(result)) {
            return !is_eqeq;
          }
          if (IsUnknown(result)) {
            unknown = true;
          }
        }
        return unknown ? decltype(EqEq(*lhs.begin(), *rhs.begin()))() : is_eqeq;
      }

      /* TODO */
      template <typename TLhs, typename TRhs, typename TElemComp, typename TSizeComp>
      auto Compare(
            const std::vector<TLhs> &lhs,
            const std::vector<TRhs> &rhs,
            const TElemComp &elem_comp,
            const TSizeComp &size_comp) -> decltype(elem_comp(*lhs.begin(), *rhs.begin())) {
        assert(&lhs);
        assert(&rhs);
        assert(&elem_comp);
        assert(elem_comp);
        assert(&size_comp);
        assert(size_comp);
        auto lhs_iter = lhs.begin();
        auto rhs_iter = rhs.begin();
        for (; lhs_iter != lhs.end() && rhs_iter != rhs.end(); ++lhs_iter, ++rhs_iter) {
          if (IsKnownTrue(EqEq(*lhs_iter, *rhs_iter))) {
            continue;
          }
          return elem_comp(*lhs_iter, *rhs_iter);
        }
        return size_comp(lhs.size(), rhs.size());
      }

    }  // List

    namespace Set {

      /* TODO */
      template <typename TLhs, typename TRhs>
      auto Equal(const TSet<TLhs> &lhs, const TSet<TRhs> &rhs, const bool is_eqeq)
            -> decltype(Contains(rhs, *lhs.begin())) {
        assert(&lhs);
        assert(&rhs);
        if (lhs.size() != rhs.size()) {
          return !is_eqeq;
        }
        bool unknown = false;
        for (auto iter : lhs) {
          auto result = Contains(rhs, iter);
          if (IsKnownFalse(result)) {
            return !is_eqeq;
          }
          if (IsUnknown(result)) {
            unknown = true;
          }
        }
        return unknown ? decltype(Contains(rhs, *lhs.begin()))() : is_eqeq;
      }

      template <typename TLhs, typename TRhs, typename TSizeComp>
      auto Subset(
            const TSet<TLhs> &lhs,
            const TSet<TRhs> &rhs,
            const TSizeComp &size_comp) -> decltype(Contains(rhs, *lhs.begin())) {
        assert(&lhs);
        assert(&rhs);
        assert(&size_comp);
        assert(size_comp);
        if (!size_comp(lhs.size(), rhs.size())) {
          return false;
        }
        bool unknown = false;
        for (auto iter : lhs) {
          auto result = Contains(rhs, iter);
          if (IsKnownFalse(result)) {
            return false;
          }
          if (IsUnknown(result)) {
            unknown = true;
          }
        }
        return unknown ? decltype(Contains(rhs, *lhs.begin()))() : true;
      }

    }  // Set

    /* Dict comparators */

    /* EqEq : dict == dict */
    template <typename TLhsKey, typename TLhsVal, typename TRhsKey, typename TRhsVal>
    struct EqEqStruct<TDict<TLhsKey, TLhsVal>, TDict<TRhsKey, TRhsVal>> {
      NO_CONSTRUCTION(EqEqStruct);

      static auto Do(const TDict<TLhsKey, TLhsVal> &lhs, const TDict<TRhsKey, TRhsVal> &rhs)
        DECLTYPE_AUTO(Dict::Equal(lhs, rhs, true));

    };  // EqEqStruct

    /* Neq : dict != dict */
    template <typename TLhsKey, typename TLhsVal, typename TRhsKey, typename TRhsVal>
    struct NeqStruct<TDict<TLhsKey, TLhsVal>, TDict<TRhsKey, TRhsVal>> {
      NO_CONSTRUCTION(NeqStruct);

      static auto Do(const TDict<TLhsKey, TLhsVal> &lhs, const TDict<TRhsKey, TRhsVal> &rhs)
        DECLTYPE_AUTO(Dict::Equal(lhs, rhs, false));

    };  // NeqStruct

    /* List comparators */

    /* EqEq : list == list */
    template <typename TLhs, typename TRhs>
    struct EqEqStruct<std::vector<TLhs>, std::vector<TRhs>> {
      NO_CONSTRUCTION(EqEqStruct);

      static auto Do(const std::vector<TLhs> &lhs, const std::vector<TRhs> &rhs)
        DECLTYPE_AUTO(List::Equal(lhs, rhs, true));

    };

    /* Neq : list != list */
    template <typename TLhs, typename TRhs>
    struct NeqStruct<std::vector<TLhs>, std::vector<TRhs>> {
      NO_CONSTRUCTION(NeqStruct);

      static auto Do(const std::vector<TLhs> &lhs, const std::vector<TRhs> &rhs)
        DECLTYPE_AUTO(List::Equal(lhs, rhs, false));

    };

    /* Lt : list < list */
    template <typename TLhs, typename TRhs>
    struct LtStruct<std::vector<TLhs>, std::vector<TRhs>> {
      NO_CONSTRUCTION(LtStruct);

      static auto Do(const std::vector<TLhs> &lhs, const std::vector<TRhs> &rhs)
        DECLTYPE_AUTO(List::Compare(lhs, rhs, LtStruct<TLhs, TRhs>::Do, LtStruct<size_t, size_t>::Do));

    };

    /* LtEq : list <= list */
    template <typename TLhs, typename TRhs>
    struct LtEqStruct<std::vector<TLhs>, std::vector<TRhs>> {
      NO_CONSTRUCTION(LtEqStruct);

      static auto Do(const std::vector<TLhs> &lhs, const std::vector<TRhs> &rhs)
        DECLTYPE_AUTO(List::Compare(lhs, rhs, LtEqStruct<TLhs, TRhs>::Do, LtEqStruct<size_t, size_t>::Do));

    };

    /* Gt : list > list */
    template <typename TLhs, typename TRhs>
    struct GtStruct<std::vector<TLhs>, std::vector<TRhs>> {
      NO_CONSTRUCTION(GtStruct);

      static auto Do(const std::vector<TLhs> &lhs, const std::vector<TRhs> &rhs)
        DECLTYPE_AUTO(List::Compare(lhs, rhs, GtStruct<TLhs, TRhs>::Do, GtStruct<size_t, size_t>::Do));

    };

    /* GtEq : list >= list */
    template <typename TLhs, typename TRhs>
    struct GtEqStruct<std::vector<TLhs>, std::vector<TRhs>> {
      NO_CONSTRUCTION(GtEqStruct);

      static auto Do(const std::vector<TLhs> &lhs, const std::vector<TRhs> &rhs)
        DECLTYPE_AUTO(List::Compare(lhs, rhs, GtEqStruct<TLhs, TRhs>::Do, GtEqStruct<size_t, size_t>::Do));

    };

    /* Mutable comparators */

    /* EqEq : mutable<addr, lhs> == rhs. */
    template <typename TAddr, typename TLhs, typename TRhs>
    struct EqEqStruct<TMutable<TAddr, TLhs>, TRhs> {
      NO_CONSTRUCTION(EqEqStruct);

      static auto Do(const TMutable<TAddr, TLhs> &lhs, const TRhs &rhs)
        DECLTYPE_AUTO(EqEq(lhs.GetVal(), rhs));
    };

    /* EqEq : lhs == mutable<addr, rhs> */
    template <typename TLhs, typename TAddr, typename TRhs>
    struct EqEqStruct<TLhs, TMutable<TAddr, TRhs>> {
      NO_CONSTRUCTION(EqEqStruct);

      static auto Do(const TLhs &lhs, const TMutable<TAddr, TRhs> &rhs)
        DECLTYPE_AUTO(EqEq(lhs, rhs.GetVal()));
    };

    /* EqEq : opt<lhs> == mutable<addr, rhs> */
    template <typename TLhs, typename TAddr, typename TRhs>
    struct EqEqStruct<TOpt<TLhs>, TMutable<TAddr, TRhs>> {
      NO_CONSTRUCTION(EqEqStruct);

      static auto Do(const TOpt<TLhs> &lhs, const TMutable<TAddr, TRhs> &rhs)
        DECLTYPE_AUTO(EqEq(lhs, rhs.GetVal()));
    };

    /* EqEq : mutable<addr, lhs> == opt<rhs>. */
    template <typename TAddr, typename TLhs, typename TRhs>
    struct EqEqStruct<TMutable<TAddr, TLhs>, TOpt<TRhs>> {
      NO_CONSTRUCTION(EqEqStruct);

      static auto Do(const TMutable<TAddr, TLhs> &lhs, const TOpt<TRhs> &rhs)
        DECLTYPE_AUTO(EqEq(lhs.GetVal(), rhs));
    };

    /* EqEq : mutable<lhs_addr, lhs> == mutable<rhs_addr, rhs> */
    template <typename TLhsAddr, typename TLhs, typename TRhsAddr, typename TRhs>
    struct EqEqStruct<TMutable<TLhsAddr, TLhs>, TMutable<TRhsAddr, TRhs>> {
      NO_CONSTRUCTION(EqEqStruct);

      static auto Do(const TMutable<TLhsAddr, TLhs> &lhs, const TMutable<TRhsAddr, TRhs> &rhs)
        DECLTYPE_AUTO(EqEq(lhs.GetVal(), rhs.GetVal()));
    };

    /* Neq : mutable<addr, lhs> != rhs. */
    template <typename TAddr, typename TLhs, typename TRhs>
    struct NeqStruct<TMutable<TAddr, TLhs>, TRhs> {
      NO_CONSTRUCTION(NeqStruct);

      static auto Do(const TMutable<TAddr, TLhs> &lhs, const TRhs &rhs)
        DECLTYPE_AUTO(Neq(lhs.GetVal(), rhs));
    };

    /* Neq : lhs != mutable<addr, rhs> */
    template <typename TLhs, typename TAddr, typename TRhs>
    struct NeqStruct<TLhs, TMutable<TAddr, TRhs>> {
      NO_CONSTRUCTION(NeqStruct);

      static auto Do(const TLhs &lhs, const TMutable<TAddr, TRhs> &rhs)
        DECLTYPE_AUTO(Neq(lhs, rhs.GetVal()));
    };

    /* Neq : opt<lhs> != mutable<addr, rhs> */
    template <typename TLhs, typename TAddr, typename TRhs>
    struct NeqStruct<TOpt<TLhs>, TMutable<TAddr, TRhs>> {
      NO_CONSTRUCTION(NeqStruct);

      static auto Do(const TOpt<TLhs> &lhs, const TMutable<TAddr, TRhs> &rhs)
        DECLTYPE_AUTO(Neq(lhs, rhs.GetVal()));
    };

    /* Neq : mutable<addr, lhs> != opt<rhs>. */
    template <typename TAddr, typename TLhs, typename TRhs>
    struct NeqStruct<TMutable<TAddr, TLhs>, TOpt<TRhs>> {
      NO_CONSTRUCTION(NeqStruct);

      static auto Do(const TMutable<TAddr, TLhs> &lhs, const TOpt<TRhs> &rhs)
        DECLTYPE_AUTO(Neq(lhs.GetVal(), rhs));
    };

    /* Neq : mutable<lhs_addr, lhs> != mutable<rhs_addr, rhs> */
    template <typename TLhsAddr, typename TLhs, typename TRhsAddr, typename TRhs>
    struct NeqStruct<TMutable<TLhsAddr, TLhs>, TMutable<TRhsAddr, TRhs>> {
      NO_CONSTRUCTION(NeqStruct);

      static auto Do(const TMutable<TLhsAddr, TLhs> &lhs, const TMutable<TRhsAddr, TRhs> &rhs)
        DECLTYPE_AUTO(Neq(lhs.GetVal(), rhs.GetVal()));
    };

    /* Lt : mutable<addr, lhs> < rhs. */
    template <typename TAddr, typename TLhs, typename TRhs>
    struct LtStruct<TMutable<TAddr, TLhs>, TRhs> {
      NO_CONSTRUCTION(LtStruct);

      static auto Do(const TMutable<TAddr, TLhs> &lhs, const TRhs &rhs)
        DECLTYPE_AUTO(Lt(lhs.GetVal(), rhs));
    };

    /* Lt : lhs < mutable<addr, rhs> */
    template <typename TLhs, typename TAddr, typename TRhs>
    struct LtStruct<TLhs, TMutable<TAddr, TRhs>> {
      NO_CONSTRUCTION(LtStruct);

      static auto Do(const TLhs &lhs, const TMutable<TAddr, TRhs> &rhs)
        DECLTYPE_AUTO(Lt(lhs, rhs.GetVal()));
    };

    /* Lt : opt<lhs> < mutable<addr, rhs> */
    template <typename TLhs, typename TAddr, typename TRhs>
    struct LtStruct<TOpt<TLhs>, TMutable<TAddr, TRhs>> {
      NO_CONSTRUCTION(LtStruct);

      static auto Do(const TOpt<TLhs> &lhs, const TMutable<TAddr, TRhs> &rhs)
        DECLTYPE_AUTO(Lt(lhs, rhs.GetVal()));
    };

    /* Lt : mutable<addr, lhs> < opt<rhs>. */
    template <typename TAddr, typename TLhs, typename TRhs>
    struct LtStruct<TMutable<TAddr, TLhs>, TOpt<TRhs>> {
      NO_CONSTRUCTION(LtStruct);

      static auto Do(const TMutable<TAddr, TLhs> &lhs, const TOpt<TRhs> &rhs)
        DECLTYPE_AUTO(Lt(lhs.GetVal(), rhs));
    };

    /* Lt : mutable<lhs_addr, lhs> < mutable<rhs_addr, rhs> */
    template <typename TLhsAddr, typename TLhs, typename TRhsAddr, typename TRhs>
    struct LtStruct<TMutable<TLhsAddr, TLhs>, TMutable<TRhsAddr, TRhs>> {
      NO_CONSTRUCTION(LtStruct);

      static auto Do(const TMutable<TLhsAddr, TLhs> &lhs, const TMutable<TRhsAddr, TRhs> &rhs)
        DECLTYPE_AUTO(Lt(lhs.GetVal(), rhs.GetVal()));
    };

    /* LtEq : mutable<addr, lhs> <= rhs. */
    template <typename TAddr, typename TLhs, typename TRhs>
    struct LtEqStruct<TMutable<TAddr, TLhs>, TRhs> {
      NO_CONSTRUCTION(LtEqStruct);

      static auto Do(const TMutable<TAddr, TLhs> &lhs, const TRhs &rhs)
        DECLTYPE_AUTO(LtEq(lhs.GetVal(), rhs));
    };

    /* LtEq : lhs <= mutable<addr, rhs> */
    template <typename TLhs, typename TAddr, typename TRhs>
    struct LtEqStruct<TLhs, TMutable<TAddr, TRhs>> {
      NO_CONSTRUCTION(LtEqStruct);

      static auto Do(const TLhs &lhs, const TMutable<TAddr, TRhs> &rhs)
        DECLTYPE_AUTO(LtEq(lhs, rhs.GetVal()));
    };

    /* LtEq : opt<lhs> <= mutable<addr, rhs> */
    template <typename TLhs, typename TAddr, typename TRhs>
    struct LtEqStruct<TOpt<TLhs>, TMutable<TAddr, TRhs>> {
      NO_CONSTRUCTION(LtEqStruct);

      static auto Do(const TOpt<TLhs> &lhs, const TMutable<TAddr, TRhs> &rhs)
        DECLTYPE_AUTO(LtEq(lhs, rhs.GetVal()));
    };

    /* LtEq : mutable<addr, lhs> <= opt<rhs>. */
    template <typename TAddr, typename TLhs, typename TRhs>
    struct LtEqStruct<TMutable<TAddr, TLhs>, TOpt<TRhs>> {
      NO_CONSTRUCTION(LtEqStruct);

      static auto Do(const TMutable<TAddr, TLhs> &lhs, const TOpt<TRhs> &rhs)
        DECLTYPE_AUTO(LtEq(lhs.GetVal(), rhs));
    };

    /* LtEq : mutable<lhs_addr, lhs> <= mutable<rhs_addr, rhs> */
    template <typename TLhsAddr, typename TLhs, typename TRhsAddr, typename TRhs>
    struct LtEqStruct<TMutable<TLhsAddr, TLhs>, TMutable<TRhsAddr, TRhs>> {
      NO_CONSTRUCTION(LtEqStruct);

      static auto Do(const TMutable<TLhsAddr, TLhs> &lhs, const TMutable<TRhsAddr, TRhs> &rhs)
        DECLTYPE_AUTO(LtEq(lhs.GetVal(), rhs.GetVal()));
    };

    /* Gt : mutable<addr, lhs> > rhs. */
    template <typename TAddr, typename TLhs, typename TRhs>
    struct GtStruct<TMutable<TAddr, TLhs>, TRhs> {
      NO_CONSTRUCTION(GtStruct);

      static auto Do(const TMutable<TAddr, TLhs> &lhs, const TRhs &rhs)
        DECLTYPE_AUTO(Gt(lhs.GetVal(), rhs));
    };

    /* Gt : lhs > mutable<addr, rhs> */
    template <typename TLhs, typename TAddr, typename TRhs>
    struct GtStruct<TLhs, TMutable<TAddr, TRhs>> {
      NO_CONSTRUCTION(GtStruct);

      static auto Do(const TLhs &lhs, const TMutable<TAddr, TRhs> &rhs)
        DECLTYPE_AUTO(Gt(lhs, rhs.GetVal()));
    };

    /* Gt : opt<lhs> > mutable<addr, rhs> */
    template <typename TLhs, typename TAddr, typename TRhs>
    struct GtStruct<TOpt<TLhs>, TMutable<TAddr, TRhs>> {
      NO_CONSTRUCTION(GtStruct);

      static auto Do(const TOpt<TLhs> &lhs, const TMutable<TAddr, TRhs> &rhs)
        DECLTYPE_AUTO(Gt(lhs, rhs.GetVal()));
    };

    /* Gt : mutable<addr, lhs> > opt<rhs>. */
    template <typename TAddr, typename TLhs, typename TRhs>
    struct GtStruct<TMutable<TAddr, TLhs>, TOpt<TRhs>> {
      NO_CONSTRUCTION(GtStruct);

      static auto Do(const TMutable<TAddr, TLhs> &lhs, const TOpt<TRhs> &rhs)
        DECLTYPE_AUTO(Gt(lhs.GetVal(), rhs));
    };

    /* Gt : mutable<lhs_addr, lhs> > mutable<rhs_addr, rhs> */
    template <typename TLhsAddr, typename TLhs, typename TRhsAddr, typename TRhs>
    struct GtStruct<TMutable<TLhsAddr, TLhs>, TMutable<TRhsAddr, TRhs>> {
      NO_CONSTRUCTION(GtStruct);

      static auto Do(const TMutable<TLhsAddr, TLhs> &lhs, const TMutable<TRhsAddr, TRhs> &rhs)
        DECLTYPE_AUTO(Gt(lhs.GetVal(), rhs.GetVal()));
    };

    /* GtEq : mutable<addr, lhs> >= rhs. */
    template <typename TAddr, typename TLhs, typename TRhs>
    struct GtEqStruct<TMutable<TAddr, TLhs>, TRhs> {
      NO_CONSTRUCTION(GtEqStruct);

      static auto Do(const TMutable<TAddr, TLhs> &lhs, const TRhs &rhs)
        DECLTYPE_AUTO(GtEq(lhs.GetVal(), rhs));
    };

    /* GtEq : lhs >= mutable<addr, rhs> */
    template <typename TLhs, typename TAddr, typename TRhs>
    struct GtEqStruct<TLhs, TMutable<TAddr, TRhs>> {
      NO_CONSTRUCTION(GtEqStruct);

      static auto Do(const TLhs &lhs, const TMutable<TAddr, TRhs> &rhs)
        DECLTYPE_AUTO(GtEq(lhs, rhs.GetVal()));
    };

    /* GtEq : opt<lhs> >= mutable<addr, rhs> */
    template <typename TLhs, typename TAddr, typename TRhs>
    struct GtEqStruct<TOpt<TLhs>, TMutable<TAddr, TRhs>> {
      NO_CONSTRUCTION(GtEqStruct);

      static auto Do(const TOpt<TLhs> &lhs, const TMutable<TAddr, TRhs> &rhs)
        DECLTYPE_AUTO(GtEq(lhs, rhs.GetVal()));
    };

    /* GtEq : mutable<addr, lhs> >= opt<rhs>. */
    template <typename TAddr, typename TLhs, typename TRhs>
    struct GtEqStruct<TMutable<TAddr, TLhs>, TOpt<TRhs>> {
      NO_CONSTRUCTION(GtEqStruct);

      static auto Do(const TMutable<TAddr, TLhs> &lhs, const TOpt<TRhs> &rhs)
        DECLTYPE_AUTO(GtEq(lhs.GetVal(), rhs));
    };

    /* GtEq : mutable<lhs_addr, lhs> >= mutable<rhs_addr, rhs> */
    template <typename TLhsAddr, typename TLhs, typename TRhsAddr, typename TRhs>
    struct GtEqStruct<TMutable<TLhsAddr, TLhs>, TMutable<TRhsAddr, TRhs>> {
      NO_CONSTRUCTION(GtEqStruct);

      static auto Do(const TMutable<TLhsAddr, TLhs> &lhs, const TMutable<TRhsAddr, TRhs> &rhs)
        DECLTYPE_AUTO(GtEq(lhs.GetVal(), rhs.GetVal()));
    };

    /* Set comparators */

    /* EqEq : set == set */
    template <typename TLhs, typename TRhs>
    struct EqEqStruct<TSet<TLhs>, TSet<TRhs>> {
      NO_CONSTRUCTION(EqEqStruct);

      static auto Do(const TSet<TLhs> &lhs, const TSet<TRhs> &rhs)
        DECLTYPE_AUTO(Set::Equal(lhs, rhs, true));

    };

    /* Neq : set != set */
    template <typename TLhs, typename TRhs>
    struct NeqStruct<TSet<TLhs>, TSet<TRhs>> {
      NO_CONSTRUCTION(NeqStruct);

      static auto Do(const TSet<TLhs> &lhs, const TSet<TRhs> &rhs)
        DECLTYPE_AUTO(Set::Equal(lhs, rhs, false));

    };

    /* Lt : set < set */
    template <typename TLhs, typename TRhs>
    struct LtStruct<TSet<TLhs>, TSet<TRhs>> {
      NO_CONSTRUCTION(LtStruct);

      static auto Do(const TSet<TLhs> &lhs, const TSet<TRhs> &rhs)
        DECLTYPE_AUTO(Set::Subset(lhs, rhs, LtStruct<size_t, size_t>::Do));

    };

    /* LtEq : set <= set */
    template <typename TLhs, typename TRhs>
    struct LtEqStruct<TSet<TLhs>, TSet<TRhs>> {
      NO_CONSTRUCTION(LtEqStruct);

      static auto Do(const TSet<TLhs> &lhs, const TSet<TRhs> &rhs)
        DECLTYPE_AUTO(Set::Subset(lhs, rhs, LtEqStruct<size_t, size_t>::Do));

    };

    /* Gt : set > set */
    template <typename TLhs, typename TRhs>
    struct GtStruct<TSet<TLhs>, TSet<TRhs>> {
      NO_CONSTRUCTION(GtStruct);

      static auto Do(const TSet<TLhs> &lhs, const TSet<TRhs> &rhs)
        DECLTYPE_AUTO(Set::Subset(rhs, lhs, LtStruct<size_t, size_t>::Do));

    };

    /* GtEq : set >= set */
    template <typename TLhs, typename TRhs>
    struct GtEqStruct<TSet<TLhs>, TSet<TRhs>> {
      NO_CONSTRUCTION(GtEqStruct);

      static auto Do(const TSet<TLhs> &lhs, const TSet<TRhs> &rhs)
        DECLTYPE_AUTO(Set::Subset(rhs, lhs, LtEqStruct<size_t, size_t>::Do));

    };

    /* Opt comparators */

    /* EqEq : opt<lhs> == rhs */
    template <typename TLhs, typename TRhs>
    struct EqEqStruct<TOpt<TLhs>, TRhs> {
      NO_CONSTRUCTION(EqEqStruct);

      static TOpt<bool> Do(const TOpt<TLhs> &lhs, const TRhs &rhs) {
        return Opt::Compare(lhs, rhs, EqEqStruct<TLhs, TRhs>::Do);
      }

    };

    /* EqEq : lhs == opt<rhs> */
    template <typename TLhs, typename TRhs>
    struct EqEqStruct<TLhs, TOpt<TRhs>> {
      NO_CONSTRUCTION(EqEqStruct);

      static TOpt<bool> Do(const TLhs &lhs, const TOpt<TRhs> &rhs) {
        return Opt::Compare(lhs, rhs, EqEqStruct<TLhs, TRhs>::Do);
      }

    };

    /* EqEq : opt<lhs> == opt<rhs> */
    template <typename TLhs, typename TRhs>
    struct EqEqStruct<TOpt<TLhs>, TOpt<TRhs>> {
      NO_CONSTRUCTION(EqEqStruct);

      static TOpt<bool> Do(const TOpt<TLhs> &lhs, const TOpt<TRhs> &rhs) {
        return Opt::Compare(lhs, rhs, EqEqStruct<TLhs, TRhs>::Do);
      }

    };

    /* Neq : opt<lhs> != rhs */
    template <typename TLhs, typename TRhs>
    struct NeqStruct<TOpt<TLhs>, TRhs> {
      NO_CONSTRUCTION(NeqStruct);

      static TOpt<bool> Do(const TOpt<TLhs> &lhs, const TRhs &rhs) {
        return Opt::Compare(lhs, rhs, NeqStruct<TLhs, TRhs>::Do);
      }

    };

    /* Neq : lhs != opt<rhs> */
    template <typename TLhs, typename TRhs>
    struct NeqStruct<TLhs, TOpt<TRhs>> {
      NO_CONSTRUCTION(NeqStruct);

      static TOpt<bool> Do(const TLhs &lhs, const TOpt<TRhs> &rhs) {
        return Opt::Compare(lhs, rhs, NeqStruct<TLhs, TRhs>::Do);
      }

    };

    /* Neq : opt<lhs> != opt<rhs> */
    template <typename TLhs, typename TRhs>
    struct NeqStruct<TOpt<TLhs>, TOpt<TRhs>> {
      NO_CONSTRUCTION(NeqStruct);

      static TOpt<bool> Do(const TOpt<TLhs> &lhs, const TOpt<TRhs> &rhs) {
        return Opt::Compare(lhs, rhs, NeqStruct<TLhs, TRhs>::Do);
      }

    };

    /* Lt : opt<lhs> < rhs */
    template <typename TLhs, typename TRhs>
    struct LtStruct<TOpt<TLhs>, TRhs> {
      NO_CONSTRUCTION(LtStruct);

      static TOpt<bool> Do(const TOpt<TLhs> &lhs, const TRhs &rhs) {
        return Opt::Compare(lhs, rhs, LtStruct<TLhs, TRhs>::Do);
      }

    };

    /* Lt : lhs < opt<rhs> */
    template <typename TLhs, typename TRhs>
    struct LtStruct<TLhs, TOpt<TRhs>> {
      NO_CONSTRUCTION(LtStruct);

      static TOpt<bool> Do(const TLhs &lhs, const TOpt<TRhs> &rhs) {
        return Opt::Compare(lhs, rhs, LtStruct<TLhs, TRhs>::Do);
      }

    };

    /* Lt : opt<lhs> < opt<rhs> */
    template <typename TLhs, typename TRhs>
    struct LtStruct<TOpt<TLhs>, TOpt<TRhs>> {
      NO_CONSTRUCTION(LtStruct);

      static TOpt<bool> Do(const TOpt<TLhs> &lhs, const TOpt<TRhs> &rhs) {
        return Opt::Compare(lhs, rhs, LtStruct<TLhs, TRhs>::Do);
      }

    };

    /* LtEq : opt<lhs> <= rhs */
    template <typename TLhs, typename TRhs>
    struct LtEqStruct<TOpt<TLhs>, TRhs> {
      NO_CONSTRUCTION(LtEqStruct);

      static TOpt<bool> Do(const TOpt<TLhs> &lhs, const TRhs &rhs) {
        return Opt::Compare(lhs, rhs, LtEqStruct<TLhs, TRhs>::Do);
      }

    };

    /* LtEq : lhs <= opt<rhs> */
    template <typename TLhs, typename TRhs>
    struct LtEqStruct<TLhs, TOpt<TRhs>> {
      NO_CONSTRUCTION(LtEqStruct);

      static TOpt<bool> Do(const TLhs &lhs, const TOpt<TRhs> &rhs) {
        return Opt::Compare(lhs, rhs, LtEqStruct<TLhs, TRhs>::Do);
      }

    };

    /* LtEq : opt<lhs> <= opt<rhs> */
    template <typename TLhs, typename TRhs>
    struct LtEqStruct<TOpt<TLhs>, TOpt<TRhs>> {
      NO_CONSTRUCTION(LtEqStruct);

      static TOpt<bool> Do(const TOpt<TLhs> &lhs, const TOpt<TRhs> &rhs) {
        return Opt::Compare(lhs, rhs, LtEqStruct<TLhs, TRhs>::Do);
      }

    };

    /* Gt : opt<lhs> > rhs */
    template <typename TLhs, typename TRhs>
    struct GtStruct<TOpt<TLhs>, TRhs> {
      NO_CONSTRUCTION(GtStruct);

      static TOpt<bool> Do(const TOpt<TLhs> &lhs, const TRhs &rhs) {
        return Opt::Compare(lhs, rhs, GtStruct<TLhs, TRhs>::Do);
      }

    };

    /* Gt : lhs > opt<rhs> */
    template <typename TLhs, typename TRhs>
    struct GtStruct<TLhs, TOpt<TRhs>> {
      NO_CONSTRUCTION(GtStruct);

      static TOpt<bool> Do(const TLhs &lhs, const TOpt<TRhs> &rhs) {
        return Opt::Compare(lhs, rhs, GtStruct<TLhs, TRhs>::Do);
      }

    };

    /* Gt : opt<lhs> > opt<rhs> */
    template <typename TLhs, typename TRhs>
    struct GtStruct<TOpt<TLhs>, TOpt<TRhs>> {
      NO_CONSTRUCTION(GtStruct);

      static TOpt<bool> Do(const TOpt<TLhs> &lhs, const TOpt<TRhs> &rhs) {
        return Opt::Compare(lhs, rhs, GtStruct<TLhs, TRhs>::Do);
      }

    };

    /* GtEq : opt<lhs> >= rhs */
    template <typename TLhs, typename TRhs>
    struct GtEqStruct<TOpt<TLhs>, TRhs> {
      NO_CONSTRUCTION(GtEqStruct);

      static TOpt<bool> Do(const TOpt<TLhs> &lhs, const TRhs &rhs) {
        return Opt::Compare(lhs, rhs, GtEqStruct<TLhs, TRhs>::Do);
      }

    };

    /* GtEq : lhs >= opt<rhs> */
    template <typename TLhs, typename TRhs>
    struct GtEqStruct<TLhs, TOpt<TRhs>> {
      NO_CONSTRUCTION(GtEqStruct);

      static TOpt<bool> Do(const TLhs &lhs, const TOpt<TRhs> &rhs) {
        return Opt::Compare(lhs, rhs, GtEqStruct<TLhs, TRhs>::Do);
      }

    };

    /* GtEq : opt<lhs> >= opt<rhs> */
    template <typename TLhs, typename TRhs>
    struct GtEqStruct<TOpt<TLhs>, TOpt<TRhs>> {
      NO_CONSTRUCTION(GtEqStruct);

      static TOpt<bool> Do(const TOpt<TLhs> &lhs, const TOpt<TRhs> &rhs) {
        return Opt::Compare(lhs, rhs, GtEqStruct<TLhs, TRhs>::Do);
      }

    };

    /* To protect from -Wsign-compare */

    /* EqEqStruct */
    template <>
    struct EqEqStruct<size_t, int64_t> {
      NO_CONSTRUCTION(EqEqStruct);

      static bool Do(const size_t &lhs, const int64_t &rhs) {
        return EqEqStruct<size_t, size_t>::Do(lhs, rhs);
      }

    };

    /* EqEqStruct */
    template <>
    struct EqEqStruct<int64_t, size_t> {
      NO_CONSTRUCTION(EqEqStruct);

      static bool Do(const int64_t &lhs, const size_t &rhs) {
        return EqEqStruct<size_t, size_t>::Do(lhs, rhs);
      }

    };

    /* NeqStruct */
    template <>
    struct NeqStruct<size_t, int64_t> {
      NO_CONSTRUCTION(NeqStruct);

      static bool Do(const size_t &lhs, const int64_t &rhs) {
        return NeqStruct<size_t, size_t>::Do(lhs, rhs);
      }

    };

    /* NeqStruct */
    template <>
    struct NeqStruct<int64_t, size_t> {
      NO_CONSTRUCTION(NeqStruct);

      static bool Do(const int64_t &lhs, const size_t &rhs) {
        return NeqStruct<size_t, size_t>::Do(lhs, rhs);
      }

    };

    /* LtStruct */
    template <>
    struct LtStruct<size_t, int64_t> {
      NO_CONSTRUCTION(LtStruct);

      static bool Do(const size_t &lhs, const int64_t &rhs) {
        return LtStruct<size_t, size_t>::Do(lhs, rhs);
      }

    };

    /* LtStruct */
    template <>
    struct LtStruct<int64_t, size_t> {
      NO_CONSTRUCTION(LtStruct);

      static bool Do(const int64_t &lhs, const size_t &rhs) {
        return LtStruct<size_t, size_t>::Do(lhs, rhs);
      }

    };

    /* LtEqStruct */
    template <>
    struct LtEqStruct<size_t, int64_t> {
      NO_CONSTRUCTION(LtEqStruct);

      static bool Do(const size_t &lhs, const int64_t &rhs) {
        return LtEqStruct<size_t, size_t>::Do(lhs, rhs);
      }

    };

    /* LtEqStruct */
    template <>
    struct LtEqStruct<int64_t, size_t> {
      NO_CONSTRUCTION(LtEqStruct);

      static bool Do(const int64_t &lhs, const size_t &rhs) {
        return LtEqStruct<size_t, size_t>::Do(lhs, rhs);
      }

    };

    /* GtStruct */
    template <>
    struct GtStruct<size_t, int64_t> {
      NO_CONSTRUCTION(GtStruct);

      static bool Do(const size_t &lhs, const int64_t &rhs) {
        return GtStruct<size_t, size_t>::Do(lhs, rhs);
      }

    };

    /* GtStruct */
    template <>
    struct GtStruct<int64_t, size_t> {
      NO_CONSTRUCTION(GtStruct);

      static bool Do(const int64_t &lhs, const size_t &rhs) {
        return GtStruct<size_t, size_t>::Do(lhs, rhs);
      }

    };

    /* GtEqStruct */
    template <>
    struct GtEqStruct<size_t, int64_t> {
      NO_CONSTRUCTION(GtEqStruct);

      static bool Do(const size_t &lhs, const int64_t &rhs) {
        return GtEqStruct<size_t, size_t>::Do(lhs, rhs);
      }

    };

    /* GtEqStruct */
    template <>
    struct GtEqStruct<int64_t, size_t> {
      NO_CONSTRUCTION(GtEqStruct);

      static bool Do(const int64_t &lhs, const size_t &rhs) {
        return GtEqStruct<size_t, size_t>::Do(lhs, rhs);
      }

    };  // GtEqStruct<int64_t, size_t>

    /* TODO */
    template <typename TContainer, typename TVal>
    bool Contains(const TContainer &, const TVal &) = delete;

    /* TODO */
    inline bool Contains(const std::string &container, const std::string &val) {
      assert(&container);
      assert(&val);
      return container.find(val) != std::string::npos;
    }

    /* TODO */
    template <typename TLhs, typename TRhs>
    auto Contains(const std::vector<TLhs> &container, const TRhs &val)
          -> decltype(EqEq(*container.begin(), val)) {
      assert(&container);
      assert(&val);
      bool unknown = false;
      auto iter = container.begin();
      for (; iter != container.end(); ++iter) {
        auto result = EqEq(*iter, val);
        if (IsKnownTrue(result)) {
          return true;
        }
        if (IsUnknown(result)) {
          unknown = true;
        }
      }
      return unknown ? decltype(EqEq(*iter, val))() : false;
    }

    /* TODO */
    template <typename TLhs, typename TRhs>
    TOpt<bool> Contains(const TSet<TOpt<TLhs>> &container, const TRhs &val) {
      assert(&container);
      assert(&val);
      if (container.empty()) {
        return TOpt<bool>(false);
      }
      if (IsUnknown(val)) {
        return *TOpt<bool>::Unknown;
      }
      if (container.count(val)) {
        return TOpt<bool>(true);
      }
      if (container.count(*TOpt<TLhs>::Unknown)) {
        return *TOpt<bool>::Unknown;
      }
      return TOpt<bool>(false);
    }

    /* TODO */
    template <typename TLhs, typename TRhs>
    auto Contains(const TSet<TLhs> &container, const TRhs &val)
          -> decltype(EqEq(*container.begin(), val)) {
      assert(&container);
      assert(&val);
      if (container.empty()) {
        return false;
      }
      if (IsUnknown(val)) {
        return decltype(EqEq(*container.begin(), val))();
      }
      return container.count(GetVal(val));
    }

    /* TODO */
    template <typename TLhsKey, typename TLhsVal, typename TRhsKey>
    TOpt<bool> Contains(const TDict<TOpt<TLhsKey>, TLhsVal> &container, const TRhsKey &key) {
      assert(&container);
      assert(&key);
      if (container.empty()) {
        return TOpt<bool>(false);
      }
      if (IsUnknown(key)) {
        return *TOpt<bool>::Unknown;
      }
      if (container.count(key)) {
        return TOpt<bool>(true);
      }
      if (container.count(*TOpt<TLhsKey>::Unknown)) {
        return *TOpt<bool>::Unknown;
      }
      return TOpt<bool>(false);
    }

    /* TODO */
    template <typename TLhsKey, typename TLhsVal, typename TRhsKey>
    auto Contains(const TDict<TLhsKey, TLhsVal> &container, const TRhsKey &key)
          -> decltype(EqEq(container.begin()->first, key)) {
      assert(&container);
      assert(&key);
      if (container.empty()) {
        return false;
      }
      if (IsUnknown(key)) {
        return decltype(EqEq(container.begin()->first, key))();
      }
      return container.count(GetVal(key));
    }

    /* TODO */
    template <typename TContainer, typename TVal>
    TOpt<bool> Contains(const TOpt<TContainer> &container, const TVal &val) {
      assert(&container);
      return container.IsKnown() ? Contains(container.GetVal(), val) : *TOpt<bool>::Unknown;
    }

    /* TODO */
    template <typename TKey, typename TVal>
    TOpt<bool> Contains(const TOpt<TDict<TKey, TVal>> &container, const TKey &val) {
      assert(&container);
      return container.IsKnown() ? Contains(container.GetVal(), val) : *TOpt<bool>::Unknown;
    }

  }  // Rt

}  // Stig