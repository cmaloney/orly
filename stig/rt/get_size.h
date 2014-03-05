/* <stig/rt/get_size.h>

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

#include <vector>

#include <base/no_construction.h>
#include <stig/rt/containers.h>
#include <stig/rt/opt.h>

namespace Stig {

  namespace Rt {

    /* General GetSize() template that static_asserts */
    template <typename TVal>
    int64_t GetSize(const TVal &) = delete;

    /* size() overload for std::string<TVal> */
    inline int64_t GetSize(const std::string &val) {
      assert(&val);
      return val.size();
    }

    /* size() overload for std::vector<TVal> */
    template <typename TVal>
    int64_t GetSize(const std::vector<TVal> &val) {
      assert(&val);
      return val.size();
    }

    /* size() overload for TSet<TVal> */
    template <typename TVal>
    int64_t GetSize(const TSet<TVal> &val) {
      assert(&val);
      return val.size();
    }

    /* size() overload for TDict<TVal> */
    template <typename TKey, typename TVal>
    int64_t GetSize(const TDict<TKey, TVal> &val) {
      assert(&val);
      return val.size();
    }

    template <typename TAddr, typename TVal>
    int64_t GetSize(const TMutable<TAddr, TVal> &val) {
      assert(&val);
      return GetSize(val.GetVal());
    }

    /* Call existing GetSize with the element in the optional
       Note: If we have nested optionals we'll loop back into this function,
             which is invalid in Stigscript */
    template <typename TVal>
    TOpt<int64_t> GetSize(const TOpt<TVal> &opt_val) {
      assert(&opt_val);
      return opt_val.IsKnown() ? GetSize(opt_val.GetVal()) : *TOpt<int64_t>::Unknown;
    }

  }  // Rt

}  // Stig