/* <base/iter.h>

   Template for things which are iterable.

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
#include <functional>

#include <base/assert_true.h>
#include <base/stl_utils.h>

namespace Base {

  /* Template for things which are iterable. */
  template <typename TVal_>
  class TIter {
    public:

    typedef TVal_ TVal;

    /* Do-nothing. */
    virtual ~TIter() {}

    /* Return true if we have any more instances of TVal. */
    virtual operator bool() const = 0;

    /* Get the current TVal. */
    virtual TVal &operator*() const = 0;

    /* Move to the next TVal. */
    virtual TIter &operator++() = 0;

    protected:

    /* Do-nothing. */
    TIter() {}
  };

  /* Basically a shared-ptr to a TIter which wraps the iter operators for easy access. You can move TIterHolder but you
     cannot copy it. */
  template <typename TVal>
  class TIterHolder : public TIter<TVal> {
    NO_COPY_SEMANTICS(TIterHolder);
    public:
    TIterHolder(TIter<TVal>* iter) : Iter(Base::AssertTrue(iter)) {}
    ~TIterHolder() {
      if (Iter) {
        delete Iter;
      }
    }
    TIterHolder(TIterHolder &&that) : Iter(that.Iter) {
      that.Iter = 0;
    }

    virtual operator bool() const {
      assert(this);
      assert(Iter);

      return *Iter;
    }

    virtual TVal &operator*() const {
      assert(this);
      assert(Iter);

      return **Iter;
    }

    TVal &Get() const {
      assert(this);
      assert(Iter);
      return **this;
    }

    virtual TIter<TVal> &operator++() {
      assert(this);
      assert(Iter);
      //Note the internal iter __should__ throw for us when advancing past end.
      ++(*Iter);
      return *this;
    }

    private:
    TIter<TVal> *Iter;
  };


  template <typename TVal>
  TIterHolder<TVal> MakeHolder(TIter<TVal> *iter) {
    return TIterHolder<TVal>(iter);
  }


  /* Compare the contents of two iterators.  The two types being iterated must have strict weak ordering based on operator<.  One or both of the iterators will
     be exhausted when this function returns. */
  template <typename TLhs, typename TRhs>
  int Compare(TIter<TLhs> &lhs, TIter<TRhs> &rhs) {
    assert(&lhs);
    assert(&rhs);
    for (; lhs && rhs; ++lhs, ++rhs) {
      if (*lhs < *rhs) {
        return -1;
      }
      if (*rhs < *lhs) {
        return 1;
      }
    }
    return lhs ? 1 : rhs ? -1 : 0;
  }

  /* Gets the hash of the given iterator. The iterator will be exhausted when this function returns. */
  template <typename TVal>
  size_t GetHash(TIter<TVal> &iter) {
    size_t result = 0;
    std::hash<TVal> hasher;

    for (; iter; ++iter) {
      result = Base::RotatedRight(result, 5) ^ hasher(*iter);
    }
    return result;
  }
}