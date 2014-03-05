/* <stig/atom/comparison.h>

   Compare ordered or unordered values.

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
#include <stdexcept>

#include <base/no_default_case.h>

namespace Stig {

  namespace Atom {

    /* The result of comparing two values. */
    enum class TComparison {

      /* The two values are not equal and the first core is ordered before the second. */
      Lt = -1,

      /* The two values are equal. */
      Eq = 0,

      /* The two values are not equal and the first core is ordered after the second. */
      Gt = 1,

      /* The two values are not equal and cannot be ordered relative to each other. */
      Ne = 2

    };  // TComparison

    /* Thrown when an ordered comparision is attempted between unordered values. */
    class TNotOrdered
        : public std::runtime_error {
      public:

      /* Do-little. */
      TNotOrdered();

    };  // TContext

    /* Compare two ordered values.
       bool TLhs::operator<(const TRhs &) must be defined. */
    template <typename TLhs, typename TRhs>
    TComparison CompareOrdered(const TLhs &lhs, const TRhs &rhs) {
      assert(&lhs);
      assert(&rhs);
      return (lhs < rhs) ? TComparison::Lt : (rhs < lhs) ? TComparison::Gt : TComparison::Eq;
    }

    /* The base case for CompareOrderedMembers(), q.v. */
    template <typename TObj>
    TComparison CompareOrderedMembers(const TObj &, const TObj &) {
      return TComparison::Eq;
    }

    /* Compare two objects of the same type (TObj) by comparing their members in the given order. */
    template <typename TObj, typename TVal, typename... TMoreMembers>
    TComparison CompareOrderedMembers(const TObj &lhs, const TObj &rhs, TVal (TObj::*member), TMoreMembers... more_members) {
      assert(&lhs);
      assert(&rhs);
      TComparison result = CompareOrdered(lhs.*member, rhs.*member);
      if (result == TComparison::Eq) {
        result = CompareOrderedMembers(lhs, rhs, more_members...);
      }
      return result;
    }

    /* Compare two unordered values.
       bool TLhs::operator==(const TRhs &) must be defined. */
    template <typename TLhs, typename TRhs>
    TComparison CompareUnordered(const TLhs &lhs, const TRhs &rhs) {
      assert(&lhs);
      assert(&rhs);
      return (lhs == rhs) ? TComparison::Eq : TComparison::Ne;
    }

    /* The result of the 'desc' modifier on the comparison. */
    inline TComparison GetDesc(TComparison comparison) {
      TComparison result;
      switch (comparison) {
        case TComparison::Eq: {
          result = TComparison::Eq;
          break;
        }
        case TComparison::Lt: {
          result = TComparison::Gt;
          break;
        }
        case TComparison::Gt: {
          result = TComparison::Lt;
          break;
        }
        case TComparison::Ne: {
          result = TComparison::Ne;
          break;
        }
        NO_DEFAULT_CASE;
      }
      return result;
    }

    /* True for ==. */
    inline bool IsEq(TComparison comparison) {
      bool result;
      switch (comparison) {
        case TComparison::Eq: {
          result = true;
          break;
        }
        case TComparison::Lt:
        case TComparison::Gt:
        case TComparison::Ne: {
          result = false;
          break;
        }
        NO_DEFAULT_CASE;
      }
      return result;
    }

    /* True for !=. */
    inline bool IsNe(TComparison comparison) {
      bool result;
      switch (comparison) {
        case TComparison::Lt:
        case TComparison::Gt:
        case TComparison::Ne: {
          result = true;
          break;
        }
        case TComparison::Eq: {
          result = false;
          break;
        }
        NO_DEFAULT_CASE;
      }
      return result;
    }

    /* True for <. */
    inline bool IsLt(TComparison comparison) {
      bool result;
      switch (comparison) {
        case TComparison::Lt: {
          result = true;
          break;
        }
        case TComparison::Eq:
        case TComparison::Gt: {
          result = false;
          break;
        }
        case TComparison::Ne: {
          throw TNotOrdered();
        }
        NO_DEFAULT_CASE;
      }
      return result;
    }

    /* True for <=. */
    inline bool IsLe(TComparison comparison) {
      bool result;
      switch (comparison) {
        case TComparison::Lt:
        case TComparison::Eq: {
          result = true;
          break;
        }
        case TComparison::Gt: {
          result = false;
          break;
        }
        case TComparison::Ne: {
          throw TNotOrdered();
        }
        NO_DEFAULT_CASE;
      }
      return result;
    }

    /* True for >. */
    inline bool IsGt(TComparison comparison) {
      bool result;
      switch (comparison) {
        case TComparison::Gt: {
          result = true;
          break;
        }
        case TComparison::Eq:
        case TComparison::Lt: {
          result = false;
          break;
        }
        case TComparison::Ne: {
          throw TNotOrdered();
        }
        NO_DEFAULT_CASE;
      }
      return result;
    }

    /* True for >=. */
    inline bool IsGe(TComparison comparison) {
      bool result;
      switch (comparison) {
        case TComparison::Gt:
        case TComparison::Eq: {
          result = true;
          break;
        }
        case TComparison::Lt: {
          result = false;
          break;
        }
        case TComparison::Ne: {
          throw TNotOrdered();
        }
        NO_DEFAULT_CASE;
      }
      return result;
    }

  }  // Atom

}  // Stig
