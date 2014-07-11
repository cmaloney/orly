/* <base/array_view.h>

   A view of a contiguous array of elements.  We don't own the data; we just
   look at it.

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

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace Base {

  /* A view of a contiguous array of elements. */
  template <typename TElem>
  class TArrayView final {
    public:

    /* The non-constant version of TElem. */
    using TNonConstElem = typename std::remove_const<TElem>::type;

    /* The constant here is true iff. we can construct from the TOther. */
    template <typename TOther>
    using From = typename std::integral_constant<
        bool,
        std::is_same<TOther, nullptr_t>::value ||
            std::is_convertible<TOther*, TElem*>::value>::type;

    /* Construct an empty view. */
    TArrayView() noexcept
        : Start(nullptr), Limit(nullptr) {}

    /* Construct from an array view of a compatible type. */
    template <
        typename TOther,
        typename = typename std::enable_if<From<TOther>::value>::type>
    TArrayView(const TArrayView<TOther> &that) noexcept
        : Start(that.GetStart()), Limit(that.GetLimit()) {}

    /* Implicitly construct a view of the given static array. */
    template <
        typename TOther,
        size_t Count,
        typename = typename std::enable_if<From<TOther>::value>::type>
    TArrayView(TOther (&that)[Count]) noexcept
        : Start(that), Limit(that + Count) {}

    /* Implicitly construct a view of the given std vector. */
    template <
        typename TOther, typename TAlloc,
        typename = typename std::enable_if<From<TOther>::value>::type>
    TArrayView(const std::vector<TOther, TAlloc> &that) noexcept
        : Start(that.data()), Limit(Start + that.size()) {}

    /* Implicitly construct a view of the given std basic string. */
    template <
        class TOther, class TTraits, class TAlloc,
        typename = typename std::enable_if<From<TOther>::value>::type>
    TArrayView(
        const std::basic_string<TOther, TTraits, TAlloc> &that) noexcept
        : Start(that.data()), Limit(Start + that.size()) {}

    /* Construct a view of the given range elements. */
    template <
        typename TOther,
        typename = typename std::enable_if<From<TOther>::value>::type>
    TArrayView(TOther *start, size_t size)
        : Start(start), Limit(start + size) {}

    /* Construct a view of the given range elements. */
    template <
        typename TOther,
        typename = typename std::enable_if<From<TOther>::value>::type>
    TArrayView(TOther *start, TOther *limit)
        : Start(start), Limit(limit) {
      assert(start <= limit);
    }

    /* True iff. we're non-empty. */
    operator bool() const noexcept {
      assert(this);
      return Start < Limit;
    }

    /* A pointer to just past the last element in the view. */
    TElem *GetLimit() const noexcept {
      assert(this);
      return Limit;
    }

    /* The number of elements in the view.
       NOTE: This is NOT the number of bytes unless sizeof(TElem) == 1. */
    size_t GetSize() const noexcept {
      assert(this);
      return Limit - Start;
    }

    /* A pointer to the first element in the view. */
    TElem *GetStart() const noexcept {
      assert(this);
      return Start;
    }

    /* The first element in the view.  The view must be non-empty. */
    TElem &Peek() const noexcept {
      assert(this);
      assert(Start < Limit);
      return *Start;
    }

    /* Pop the first element from the view and shorten the view as per Skip().
       The view must be non-empty.  This makes a copy of the element. */
    TNonConstElem Pop() noexcept {
      assert(this);
      assert(Start < Limit);
      return *Start++;
    }

    /* Shorten the view by advancing the start past the one element.
       The view must be non-empty. */
    TArrayView &Skip() noexcept {
      assert(this);
      assert(Start < Limit);
      ++Start;
      return *this;
    }

    private:

    /* See accessors.  Start <= Limit.  Start can be null and, if it is,
       Limit can also be null. */
    TElem *Start, *Limit;

  };  // TArrayView<TElem>

  /* Some common aliases. */
  using TBytes = TArrayView<uint8_t>;
  using TConstBytes = TArrayView<const uint8_t>;

}  // Base
