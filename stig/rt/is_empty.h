/* <stig/rt/is_empty.h>

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
#include <vector>

#include <base/false.h>
#include <stig/rt/containers.h>
#include <stig/rt/mutable.h>
#include <stig/rt/opt.h>

namespace Stig {

  namespace Rt {

    /* General IsEmpty() template on TVal that static_asserts */
    template <typename TVal>
    bool IsEmpty(const TVal &) = delete;

    /* empty() overload for std::string<TVal> */
    inline bool IsEmpty(const std::string &val) {
      assert(&val);
      return val.empty();
    }

    /* empty() overload for std::vector<TVal> */
    template <typename TVal>
    bool IsEmpty(const std::vector<TVal> &val) {
      assert(&val);
      return val.empty();
    }

    /* empty() overload for TSet<TVal> */
    template <typename TVal>
    bool IsEmpty(const TSet<TVal> &val) {
      assert(&val);
      return val.empty();
    }

    /* empty() overload for std::TDict<TVal> */
    template <typename TKey, typename TVal>
    bool IsEmpty(const TDict<TKey, TVal> &val) {
      assert(&val);
      return val.empty();
    }

    /* Call existing IsEmpty with the element in the optional
       Note: If we have nested optionals we'll loop back into this function,
             which is invalid in Stigscript */
    template <typename TVal>
    TOpt<bool> IsEmpty(const TOpt<TVal> &opt) {
      assert(&opt);
      return opt.IsKnown() ? TOpt<bool>(IsEmpty(opt.GetVal())) : *TOpt<bool>::Unknown;
    }

    /* Call existing IsEmpty with the element in the optional
       Note: If we have nested optionals we'll loop back into this function,
             which is invalid in Stigscript */
    template <typename TAddr, typename TVal>
    auto IsEmpty(const TMutable<TAddr, TVal> &mutable_)
      DECLTYPE_AUTO(IsEmpty(mutable_.GetVal()));

  }  // Rt

}  // Stig