/* <stig/rt/built_in.h>

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

#include <stig/rt/containers.h>
#include <stig/rt/get_size.h>
#include <stig/rt/is_empty.h>
#include <stig/rt/opt.h>
#include <stig/rt/postfix_cast.h>
#include <stig/rt/reduce.h>
#include <stig/rt/reverse.h>
#include <stig/rt/runtime_error.h>
#include <stig/rt/div.h>
#include <stig/rt/slice.h>
#include <stig/rt/sort.h>

namespace Stig {

  namespace Rt {

    template <typename TAddr>
    auto AddrToPair(const TAddr &addr)
      DECLTYPE_AUTO(std::make_pair(addr.template Get<0>(), addr.template Get<1>()));

    template <int size, typename TKey, typename TVal>
    const TDict<TKey, TVal> &DictCtor(const TDict<TKey, TVal> &out) {
      assert(&out);
      if (out.size() != size) {
        throw TSystemError(HERE, "Duplicate keys in a dictionary constructor. Since execution order is arbitrary, this is not allowed.");
      }
      return out;
    }  // DictCtor

    /* TODO */
    class TAssertionError : public TRuntimeError, public Base::TFinalError<TAssertionError> {
      public:

      /* Constructor. */
      TAssertionError(const Base::TCodeLocation &loc, const char *msg=0) {
        PostCtor(loc, msg);
      }
    };  // TAssertionError

    inline void Assert(const char *msg, bool result) {
      if (!result) {
        //TODO: probably want an option for logging asserts.
        throw TAssertionError(HERE, msg);
      }
    }

  }  // Rt

}  // Stig