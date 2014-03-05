/* <stig/asc.h>

   A wrapper providing ascending ordering.

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

  /* A wrapper providing ascending ordering. */
  template <typename TVal>
  class TAsc {
    public:

    /* Do-little. */
    TAsc() {}

    /* Do-little. */
    template <typename TArg>
    TAsc(TArg &&arg)
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

    /* Provide the same ordering as TVal. */
    bool operator==(const TAsc &that) const {
      assert(this);
      return Val == that.Val;
    }

    /* Provide the same ordering as TVal. */
    bool operator!=(const TAsc &that) const {
      assert(this);
      return Val != that.Val;
    }

    /* Provide the same ordering as TVal. */
    bool operator<(const TAsc &that) const {
      assert(this);
      return Val < that.Val;
    }

    /* Provide the same ordering as TVal. */
    bool operator<=(const TAsc &that) const {
      assert(this);
      return Val <= that.Val;
    }

    /* Provide the same ordering as TVal. */
    bool operator>(const TAsc &that) const {
      assert(this);
      return Val > that.Val;
    }

    /* Provide the same ordering as TVal. */
    bool operator>=(const TAsc &that) const {
      assert(this);
      return Val >= that.Val;
    }

    private:

    /* The value we wrap.*/
    TVal Val;

  };  // TAsc

}  // Stig
