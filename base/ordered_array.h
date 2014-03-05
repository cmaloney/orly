/* <base/ordered_array.h>

   An array of key-value pairs, held in sorted order.

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
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <new>
#include <utility>

#include <base/thrower.h>

namespace Base {

  /* An array of key-value pairs, held in sorted order. */
  template <typename TKey, typename TVal>
  class TOrderedArray final {
    public:

    /* The errors we throw. */
    DEFINE_ERROR(TKeyAlreadyInArray, std::runtime_error, "key already in array");
    DEFINE_ERROR(TKeyNotInArray, std::runtime_error, "key not in array");
    DEFINE_ERROR(TMaxSizeReached, std::runtime_error, "array is already at max size");
    DEFINE_ERROR(TMaxSizeTooSmall, std::runtime_error, "max size smaller than actual size");

    /* Provides the ordering of the array. */
    using TComparator = std::function<int (const TKey &, const TKey &)>;

    /* Visits key-value pairs without changing the value. */
    using TReader = std::function<bool (const TKey &key, const TVal &)>;

    /* Visit key-value pairs, possibly changing the value. */
    using TWriter = std::function<bool (const TKey &key, TVal &)>;

    /* Construct a new array containing no key-value pairs and with no capacity to hold any. */
    TOrderedArray() noexcept
        : Start(nullptr), Limit(nullptr), Capacity(nullptr) {}

    /* Construct a new array containing no key-value pairs but with the capacity to hold the given number of them.
       If there is insufficient memory, throw.
       Use the default comparator to order the keys. */
    explicit TOrderedArray(size_t max_size)
        : Start(nullptr), Limit(nullptr), Capacity(nullptr), Comparator(DefaultComparator) {
      SetMaxSize(max_size);
    }

    /* Construct a new array containing no key-value pairs but with the capacity to hold the given number of them.
       If there is insufficient memory, throw.
       Use the given comparator to order the keys. */
    template <typename TComparatorSrc>
    TOrderedArray(size_t max_size, TComparatorSrc &&comparator)
        : Start(nullptr), Limit(nullptr), Capacity(nullptr), Comparator(std::forward<TComparatorSrc>(comparator)) {
      SetMaxSize(max_size);
    }

    /* Move-construct into a new array, leaving the donor in the default-constructed state. */
    TOrderedArray(TOrderedArray &&that) noexcept
        : TOrderedArray() {
      assert(&that);
      Swap(that);
    }

    /* Copy-construct into a new array. */
    TOrderedArray(const TOrderedArray &that)
        : Start(nullptr), Limit(nullptr), Capacity(nullptr), Comparator(that.Comparator) {
      assert(&that);
      SetMaxSize(that.GetMaxSize());
      try {
        for (const TElem *elem = that.Start; elem < that.Limit; ++elem, ++Limit) {
          new (Limit) TElem(*elem);
        }
      } catch (...) {
        this->~TOrderedArray();
        throw;
      }
    }

    /* Destroy all our key-value pairs. */
    ~TOrderedArray() noexcept {
      assert(this);
      RemoveAll();
      free(Start);
    }

    /* Move into this array, leaving the donor in the default-constructed state. */
    TOrderedArray &operator=(TOrderedArray &&that) noexcept {
      assert(this);
      assert(&that);
      Swap(that);
      that.Reset();
      return *this;
    }

    /* Copy into this array. */
    TOrderedArray &operator=(const TOrderedArray &that) {
      assert(this);
      assert(&that);
      TOrderedArray temp(that);
      return Swap(temp);
    }

    /* True iff. we contain at least one key-value pair. */
    operator bool() const noexcept {
      assert(this);
      return Start < Limit;
    }

    /* Find the value for the given key.
       If the array doesn't contain the key, throw. */
    const TVal &operator[](const TKey &key) const {
      assert(this);
      return Find(key);
    }

    /* Find the value for the given key.
       If the array doesn't contain the key, insert it with a default-constructed value. */
    template <typename TKeySrc>
    TVal &operator[](TKeySrc &&key) {
      assert(this);
      TElem *elem;
      if (!Search(key, elem)) {
        InsertAtElem(elem, std::forward<TKeySrc>(key), TVal());
      }
      return elem->second;
    }

    /* True iff. this array contains the given key. */
    bool Contains(const TKey &key) const {
      assert(this);
      TElem *elem;
      return Search(key, elem);
    }

    /* Find the value for the given key.
       If the array doesn't contain the key, throw. */
    const TVal &Find(const TKey &key) const {
      assert(this);
      return FindElem(key)->second;
    }

    /* Find the value for the given key.
       If the array doesn't contain the key, throw. */
    TVal &Find(const TKey &key) {
      assert(this);
      return FindElem(key)->second;
    }

    /* Call back for each key-value pair in the array, in key order. */
    bool ForEach(const TReader &cb) const {
      assert(this);
      assert(&cb);
      for (TElem *elem = Start; elem < Limit; ++elem) {
        if (!cb(elem->first, elem->second)) {
          return false;
        }
      }
      return true;
    }

    /* Call back for each key-value pair in the array, in key order. */
    bool ForEach(const TWriter &cb) {
      assert(this);
      assert(&cb);
      for (TElem *elem = Start; elem < Limit; ++elem) {
        if (!cb(elem->first, elem->second)) {
          return false;
        }
      }
      return true;
    }

    /* Call back for each key-value pair in the array, in key order, starting with the first key >= the given key. */
    bool ForEach(const TKey &key, const TReader &cb) const {
      assert(this);
      assert(&cb);
      TElem *elem;
      Search(key, elem);
      for (; elem < Limit; ++elem) {
        if (!cb(elem->first, elem->second)) {
          return false;
        }
      }
      return true;
    }

    /* Call back for each key-value pair in the array, in key order, starting with the first key >= the given key. */
    bool ForEach(const TKey &key, const TWriter &cb) {
      assert(this);
      assert(&cb);
      TElem *elem;
      Search(key, elem);
      for (; elem < Limit; ++elem) {
        if (!cb(elem->first, elem->second)) {
          return false;
        }
      }
      return true;
    }

    /* The comparator which provided the current sorting of the array. */
    const TComparator &GetComparator() const noexcept {
      assert(this);
      return Comparator;
    }

    /* The maximum number of key-value pairs the array can contain. */
    size_t GetMaxSize() const noexcept {
      assert(this);
      return Capacity - Start;
    }

    /* The number of key-value pairs the array currently contains. */
    size_t GetSize() const noexcept {
      assert(this);
      return Limit - Start;
    }

    /* Insert the given key-value pair into the array.
       If the array already contains the key, throw.
       Return a reference to the value in the array. */
    template <typename TKeySrc, typename TValSrc>
    TVal &Insert(TKeySrc &&key, TValSrc &&val) {
      assert(this);
      TElem *elem;
      if (Search(key, elem)) {
        THROW_ERROR(TKeyAlreadyInArray) << "key = " << key;
      }
      InsertAtElem(elem, std::forward<TKeySrc>(key), std::forward<TValSrc>(val));
      return elem->second;
    }

    /* Insert the given key-value pair into the array.
       If the array already contains the key, replace its value with the given value.
       Return a reference to the value in the array. */
    template <typename TKeySrc, typename TValSrc>
    TVal &InsertOrReplace(TKeySrc &&key, TValSrc &&val) {
      assert(this);
      TElem *elem;
      if (Search(key, elem)) {
        elem->second = std::forward<TValSrc>(val);
      } else {
        InsertAtElem(elem, std::forward<TKeySrc>(key), std::forward<TValSrc>(val));
      }
      return elem->second;
    }

    /* Remove the given key from the array.
       If the array does not contain the key, throw. */
    void Remove(const TKey &key) {
      assert(this);
      TElem *elem = FindElem(key);
      elem->~TElem();
      WiggleClosed(elem);
    }

    /* Remove all key-value pairs from the array, leaving the array empty.
       Leave the array's capacity as-is. */
    void RemoveAll() noexcept {
      assert(this);
      for (TElem *elem = Start; elem < Limit; ++elem) {
        elem->~TElem();
      }
      Limit = Start;
    }

    /* Reset the array to the default-constructed state. */
    TOrderedArray &Reset() noexcept {
      assert(this);
      TOrderedArray temp;
      return Swap(temp);
    }

    /* Allocate storage capacity sufficient to hold the given number of elements.
       If the number is less than the number of elements we currently hold, throw.
       If there is insufficient memory, throw. */
    void SetMaxSize(size_t max_size) {
      assert(this);
      size_t size = GetSize();
      if (max_size < size) {
        THROW_ERROR(TMaxSizeTooSmall) << "max size = " << max_size << ", actual size = " << size;
      }
      if (max_size) {
        Start = static_cast<TElem *>(realloc(Start, sizeof(TElem) * max_size));
        if (!Start) {
          throw std::bad_alloc();
        }
        Limit    = Start + size;
        Capacity = Start + max_size;
        if (!Comparator) {
          try {
            Comparator = DefaultComparator;
          } catch (...) {
            Reset();
            throw;
          }
        }
      } else if (Start) {
        free(Start);
        Start    = nullptr;
        Limit    = nullptr;
        Capacity = nullptr;
      }
    }

    /* Swap this array with that one. */
    TOrderedArray &Swap(TOrderedArray &that) noexcept {
      assert(this);
      assert(&that);
      std::swap(Start,      that.Start     );
      std::swap(Limit,      that.Limit     );
      std::swap(Capacity,   that.Capacity  );
      std::swap(Comparator, that.Comparator);
      return *this;
    }

    /* Try to find the value for the given key.
       If the array contains the key, return a pointer to its value; otherwise, return a null pointer. */
    const TVal *TryFind(const TKey &key) const {
      assert(this);
      TElem *elem = TryFindElem(key);
      return elem ? &(elem->second) : nullptr;
    }

    /* Try to find the value for the given key.
       If the array contains the key, return a pointer to its value; otherwise, return a null pointer. */
    TVal *TryFind(const TKey &key) {
      assert(this);
      TElem *elem = TryFindElem(key);
      return elem ? &(elem->second) : nullptr;
    }

    /* Try to remove the given key from the array.
       If the array contains the key, remove it and return true; otherwise, just return false. */
    bool TryRemove(const TKey &key) {
      assert(this);
      TElem *elem = TryFindElem(key);
      if (elem) {
        elem->~TElem();
        WiggleClosed(elem);
      }
      return elem;
    }

    /* The comparator used when you don't otherwise specify a comparator.
       This one uses operator< to arrange the keys into increasing order. */
    static const TComparator DefaultComparator;

    private:

    /* Each element in the array is a key-value pair. */
    using TElem = std::pair<const TKey, TVal>;

    /* Return a pointer to the element with the given key.
       If the array does not contain the key, throw.
       Never return null. */
    TElem *FindElem(const TKey &key) const {
      assert(this);
      TElem *elem;
      if (!Search(key, elem)) {
        THROW_ERROR(TKeyNotInArray) << "key = " << key;
      }
      assert(elem);
      return elem;
    }

    /* Insert the given key-value pair into the array at the given position.
       If the array is already full, throw. */
    template <typename TKeySrc, typename TValSrc>
    void InsertAtElem(TElem *elem, TKeySrc &&key, TValSrc &&val) {
      assert(this);
      void *mem = WiggleOpen(elem);
      try {
        new (mem) TElem(std::forward<TKeySrc>(key), std::forward<TValSrc>(val));
      } catch (...) {
        WiggleClosed(mem);
        throw;
      }
    }

    /* Binary-search the array and return (via out-parameter) the position of the element for the given key.
       Return true if we actually found the key, false if we merely found the position where they key should be inserted.
       If we return true, then this post-condition holds: Start <= elem < Limit;
       otherwise, this post-condition holds: Start <= elem <= Limit. */
    bool Search(const TKey &key, TElem *&elem) const {
      assert(this);
      assert(&elem);
      assert(Comparator);
      elem = Start;
      TElem *limit = Limit;
      while (elem < limit) {
        TElem *mid = elem + ((limit - elem) / 2);
        int comparison = Comparator(key, mid->first);
        if (comparison < 0) {
          limit = mid;
        } else if (comparison > 0) {
          elem = mid + 1;
        } else {
          elem = mid;
          assert(Start <= elem);
          assert(elem < Limit);
          return true;
        }
      }
      assert(Start <= elem);
      assert(elem <= Limit);
      return false;
    }

    /* Return a pointer to the element with the given key.
       If the array does not contain the key, return null. */
    TElem *TryFindElem(const TKey &key) const {
      assert(this);
      TElem *elem;
      return Search(key, elem) ? elem : nullptr;
    }

    /* Close up the given space in the array. */
    void WiggleClosed(void *mem) {
      assert(this);
      assert(Start <= mem);
      assert(mem < Limit);
      for (TElem *lhs = static_cast<TElem *>(mem), *rhs = lhs + 1; rhs < Limit; ++lhs, ++rhs) {
        new (lhs) TElem(std::move(*rhs));
        rhs->~TElem();
      }
      --Limit;
    }

    /* Open up space in the array at the given position. */
    void *WiggleOpen(TElem *elem) {
      assert(this);
      assert(Start <= elem);
      assert(elem <= Limit);
      if (Limit >= Capacity) {
        THROW_ERROR(TMaxSizeReached) << "max size = " << GetMaxSize();
      }
      if (elem < Limit) {
        for (TElem *lhs = Limit, *rhs = Limit + 1; lhs >= elem; --lhs, --rhs) {
          new (rhs) TElem(std::move(*lhs));
          lhs->~TElem();
        }
      }
      ++Limit;
      return elem;
    }

    /* Pointers to the elements we store.
       Start points to the beginning of our storage space.
       Limit points just past the last initialized element.
       Capacity points just past the end of our storage space.
       These conditions always hold: Start <= Limit <= Capacity.
       It is possible for all three pointers to be null. */
    TElem *Start, *Limit, *Capacity;

    /* See accessor. */
    TComparator Comparator;

  };  // TOrderedArray<TKey, TVal>

  /* See declaration. */
  template <typename TKey, typename TVal>
  const typename TOrderedArray<TKey, TVal>::TComparator TOrderedArray<TKey, TVal>::DefaultComparator =
      [](const TKey &lhs, const TKey &rhs) -> int {
        return (lhs < rhs) ? -1 : (rhs < lhs) ? 1 : 0;
      };

}  // Base
