/* <orly/rt/built_in.h>

   TODO

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

#include <orly/rt/containers.h>
#include <orly/rt/get_size.h>
#include <orly/rt/is_empty.h>
#include <orly/rt/opt.h>
#include <orly/rt/postfix_cast.h>
#include <orly/rt/reduce.h>
#include <orly/rt/reverse.h>
#include <orly/rt/runtime_error.h>
#include <orly/rt/div.h>
#include <orly/rt/slice.h>
#include <orly/rt/sort.h>

namespace Orly {

  namespace Rt {

    template <typename TAddr>
    auto AddrToPair(const TAddr &addr) {
      return std::make_pair(addr.template Get<0>(), addr.template Get<1>());
    }

    template <int size, typename TKey, typename TVal>
    const TDict<TKey, TVal> &DictCtor(const TDict<TKey, TVal> &out) {
      assert(&out);
      if (out.size() != size) {
        throw TSystemError(HERE, "Duplicate keys in a dictionary constructor. Since execution order is arbitrary, this is not allowed.");
      }
      return out;
    }  // DictCtor

    /* TODO */
    DEFINE_ERROR(TAssertionError, TRuntimeError, "Assertion Error");

    inline void Assert(const char *msg, bool result) {
      if (!result) {
        //TODO: probably want an option for logging asserts.
        throw TAssertionError(HERE, msg);
      }
    }

  }  // Rt

}  // Orly
