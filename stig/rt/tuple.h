/* <stig/rt/tuple.h>

   The 'tuple' runtime value.

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

#include <base/no_construction.h>
#include <stig/desc.h>
#include <stig/rt/operator.h>

namespace Stig {

  namespace Rt {

    /* Match two tuples. */
    template <typename... TArgs>
    bool Match(const std::tuple<TArgs...> &/*lhs*/, const std::tuple<TArgs...> &/*rhs*/) {
      throw std::runtime_error("TODO: Match for tuple");
    }

    /* MatchLess two tuples. */
    template <typename... TArgs>
    bool MatchLess(const std::tuple<TArgs...> &/*lhs*/, const std::tuple<TArgs...> &/*rhs*/) {
      throw std::runtime_error("TODO: MatchLess for tuple");
    }

    template <typename TVal>
    inline size_t GetTupleSize(const TVal &) {
      return std::tuple_size<TVal>::value;
    }
#if 0
    template <size_t Idx, typename... TArgs>
    class TupleHelper;

    template <size_t Idx>
    class TupleHelper<Idx> {
      NO_CONSTRUCTION(TupleHelper);
      public:

      using TPartialTuple = std::_Tuple_impl<Idx>;

      static bool EqEq(const TPartialTuple &, const TPartialTuple &) {
        return true;
      }

      static bool Neq(const TPartialTuple &, const TPartialTuple &) {
        return false;
      }

      static bool Lt(const TPartialTuple &, const TPartialTuple &) {
        return false;
      }

      static bool LtEq(const TPartialTuple &, const TPartialTuple &) {
        return true;
      }

      static bool Gt(const TPartialTuple &, const TPartialTuple &) {
        return false;
      }

      static bool GtEq(const TPartialTuple &, const TPartialTuple &) {
        return true;
      }

    };

    template <size_t Idx, typename TElem, typename... TRest>
    class TupleHelper<Idx, TElem, TRest...> {
      NO_CONSTRUCTION(TupleHelper);
      public:

      using TPartialTuple = std::_Tuple_impl<Idx, TElem, TRest...>;

      static auto EqEq(const TPartialTuple &lhs, const TPartialTuple &rhs)
        DECLTYPE_AUTO(And(Stig::Rt::EqEq(TPartialTuple::_M_head(lhs), TPartialTuple::_M_head(rhs)), TupleHelper<Idx + 1, TRest...>::EqEq(TPartialTuple::_M_tail(lhs), TPartialTuple::_M_tail(rhs))));

      static auto Neq(const TPartialTuple &lhs, const TPartialTuple &rhs)
        DECLTYPE_AUTO((Stig::Rt::Neq(TPartialTuple::_M_head(lhs), TPartialTuple::_M_head(rhs)), TupleHelper<Idx + 1, TRest...>::Neq(TPartialTuple::_M_tail(lhs), TPartialTuple::_M_tail(rhs))));

      /* a < that.a || (a == that.a && (b < that.b || (b == that.b && c < that.c)) ) */
      static auto Lt(const TPartialTuple &lhs, const TPartialTuple &rhs)
        DECLTYPE_AUTO(Or(Stig::Rt::Lt(TPartialTuple::_M_head(lhs), TPartialTuple::_M_head(rhs)), And(Stig::Rt::EqEq(TPartialTuple::_M_head(lhs), TPartialTuple::_M_head(rhs)), TupleHelper<Idx + 1, TRest...>::Lt(TPartialTuple::_M_tail(lhs), TPartialTuple::_M_tail(rhs)))));

      static auto LtEq(const TPartialTuple &lhs, const TPartialTuple &rhs)
        DECLTYPE_AUTO(Or(Lt(lhs, rhs), EqEq(lhs, rhs)));

      static auto Gt(const TPartialTuple &lhs, const TPartialTuple &rhs)
        DECLTYPE_AUTO(Or(Stig::Rt::Gt(TPartialTuple::_M_head(lhs), TPartialTuple::_M_head(rhs)), And(Stig::Rt::EqEq(TPartialTuple::_M_head(lhs), TPartialTuple::_M_head(rhs)), TupleHelper<Idx + 1, TRest...>::Gt(TPartialTuple::_M_tail(lhs), TPartialTuple::_M_tail(rhs)))));

      static auto GtEq(const TPartialTuple &lhs, const TPartialTuple &rhs)
        DECLTYPE_AUTO(Or(Gt(lhs, rhs), EqEq(lhs, rhs)));

    };
    template <typename... TArgs>
    struct EqEqStruct<std::tuple<TArgs...>, std::tuple<TArgs...>> {
      NO_CONSTRUCTION(EqEqStruct);

      static auto Do(const std::tuple<TArgs...> &lhs, const std::tuple<TArgs...> &rhs)
        DECLTYPE_AUTO((TupleHelper<0, TArgs...>::EqEq(lhs, rhs)));
    };

    template <typename... TArgs>
    struct NeqStruct<std::tuple<TArgs...>, std::tuple<TArgs...>> {
      NO_CONSTRUCTION(NeqStruct);
      static auto Do(const std::tuple<TArgs...> &lhs, const std::tuple<TArgs...> &rhs)
        DECLTYPE_AUTO((Stig::Rt::TupleHelper<0, TArgs...>::Neq(lhs, rhs)));
    };

    template <typename... TArgs>
    struct LtStruct<std::tuple<TArgs...>, std::tuple<TArgs...>> {
      NO_CONSTRUCTION(LtStruct);
      static auto Do(const std::tuple<TArgs...> &lhs, const std::tuple<TArgs...> &rhs)
        DECLTYPE_AUTO((Stig::Rt::TupleHelper<0, TArgs...>::Lt(lhs, rhs)));
    };

    template <typename... TArgs>
    struct LtEqStruct<std::tuple<TArgs...>, std::tuple<TArgs...>> {
      NO_CONSTRUCTION(LtEqStruct);

      static auto Do(const std::tuple<TArgs...> &lhs, const std::tuple<TArgs...> &rhs)
        DECLTYPE_AUTO((TupleHelper<0, TArgs...>::LtEq(lhs, rhs)));
    };

    template <typename... TArgs>
    struct GtStruct<std::tuple<TArgs...>, std::tuple<TArgs...>> {
      NO_CONSTRUCTION(GtStruct);
      static auto Do(const std::tuple<TArgs...> &lhs, const std::tuple<TArgs...> &rhs)
        DECLTYPE_AUTO((Stig::Rt::TupleHelper<0, TArgs...>::Gt(lhs, rhs)));
    };

    template <typename... TArgs>
    struct GtEqStruct<std::tuple<TArgs...>, std::tuple<TArgs...>> {
      NO_CONSTRUCTION(GtEqStruct);
      static auto Do(const std::tuple<TArgs...> &lhs, const std::tuple<TArgs...> &rhs)
        DECLTYPE_AUTO((Stig::Rt::TupleHelper<0, TArgs...>::GtEq(lhs, rhs)));
    };

    template <>
    struct EqEqStruct<std::tuple<>, std::tuple<>> {
      NO_CONSTRUCTION(EqEqStruct);

      static bool Do(const std::tuple<> &, const std::tuple<> &) {
        return true;
      }
    };

    template <>
    struct NeqStruct<std::tuple<>, std::tuple<>> {
      NO_CONSTRUCTION(NeqStruct);

      static bool Do(const std::tuple<> &, const std::tuple<> &) {
        return false;
      }
    };

    template <>
    struct LtStruct<std::tuple<>, std::tuple<>> {
      NO_CONSTRUCTION(LtStruct);

      static bool Do(const std::tuple<> &, const std::tuple<> &) {
        return false;
      }
    };

    template <>
    struct LtEqStruct<std::tuple<>, std::tuple<>> {
      NO_CONSTRUCTION(LtEqStruct);

      static bool Do(const std::tuple<> &, const std::tuple<> &) {
        return true;
      }
    };

    template <>
    struct GtStruct<std::tuple<>, std::tuple<>> {
      NO_CONSTRUCTION(GtStruct);

      static bool Do(const std::tuple<> &, const std::tuple<> &) {
        return false;
      }
    };

    template <>
    struct GtEqStruct<std::tuple<>, std::tuple<>> {
      NO_CONSTRUCTION(GtEqStruct);

      static bool Do(const std::tuple<> &, const std::tuple<> &) {
        return true;
      }
    };
#endif

  }  // Rt

}  // Stig