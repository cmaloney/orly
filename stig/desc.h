/* <stig/desc.h>

   A wrapper providing descending ordering.

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

namespace Stig {

  /* A wrapper providing descending ordering. */
  template <typename TVal>
  class TDesc {
    public:

    /* Do-little. */
    TDesc() {}

    /* Do-little. */
    template <typename TArg>
    TDesc(TArg &&arg)
        : Val(arg) {}

    /* Behave like a smart pointer. */
    const TVal &operator*() const {
      assert(this);
      return Val;
    }

    /* Behave like a smart pointer. */
    TVal &operator*() {
      assert(this);
      return Val;
    }

    /* Behave like a smart pointer. */
    const TVal *operator->() const {
      assert(this);
      return &Val;
    }

    /* Behave like a smart pointer. */
    TVal *operator->() {
      assert(this);
      return &Val;
    }

    #if 0
    /* TODO */
    operator const TVal &() const {
      assert(this);
      return Val;
    }
    #endif

    /* Provide the opposite ordering of TVal. */
    bool operator==(const TDesc &that) const {
      assert(this);
      return Val == that.Val;
    }

    /* Provide the opposite ordering of TVal. */
    bool operator!=(const TDesc &that) const {
      assert(this);
      return Val != that.Val;
    }

    /* Provide the opposite ordering of TVal. */
    bool operator<(const TDesc &that) const {
      assert(this);
      return Val > that.Val;
    }

    /* Provide the opposite ordering of TVal. */
    bool operator<=(const TDesc &that) const {
      assert(this);
      return Val >= that.Val;
    }

    /* Provide the opposite ordering of TVal. */
    bool operator>(const TDesc &that) const {
      assert(this);
      return Val < that.Val;
    }

    /* Provide the opposite ordering of TVal. */
    bool operator>=(const TDesc &that) const {
      assert(this);
      return Val <= that.Val;
    }

    private:

    /* The value we wrap.*/
    TVal Val;

  };  // TDesc

}  // Stig
