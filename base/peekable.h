/* <base/peekable.h>

   Template for changing a non-peekable producer into a peekable iterator.

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
#include <functional>

#include <base/no_copy_semantics.h>
#include <base/no_default_case.h>
#include <base/past_end_error.h>

namespace Base {

  /* TODO */
  template <typename TVal>
  class TPeekable {
    NO_COPY_SEMANTICS(TPeekable);
    public:

    /* TODO */
    typedef std::function<bool (TVal &)> TProducingFunc;

    /* TODO */
    TPeekable(const TProducingFunc &producing_func)
        : ProducingFunc(producing_func), State(Unknown) {}

    /* TODO */
    operator bool() const {
      assert(this);
      return TryRefresh();
    }

    /* TODO */
    TVal &operator*() const {
      assert(this);
      Refresh();
      return Val;
    }

    /* TODO */
    void operator++() {
      assert(this);
      Refresh();
      State = Unknown;
    }

    private:

    /* TODO */
    enum TState { Unknown, Ready, Done };

    /* TODO */
    void Refresh() const {
      assert(this);
      if (!TryRefresh()) {
        throw TPastEndError(HERE);
      }
    }

    /* TODO */
    bool TryRefresh() const {
      assert(this);
      bool is_ready;
      switch (State) {
        case Unknown: {
          is_ready = ProducingFunc(Val);
          State = is_ready ? Ready : Done;
          break;
        }
        case Ready: {
          is_ready = true;
          break;
        }
        case Done: {
          is_ready = false;
          break;
        }
        NO_DEFAULT_CASE;
      }
      return is_ready;
    }

    /* TODO */
    TProducingFunc ProducingFunc;

    /* TODO */
    mutable TState State;

    /* TODO */
    mutable TVal Val;

  };  // TPeekable<TVal>

}  // Base
