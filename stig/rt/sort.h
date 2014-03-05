/* <stig/rt/sort.h>

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

#include <algorithm>
#include <vector>

#include <base/no_construction.h>
#include <stig/rt/opt.h>

namespace Stig {

  namespace Rt {

    /* TODO */
    template <typename TVal>
    TVal Sort(const TVal &) = delete;

    /* TODO */
    template <typename TVal>
    std::vector<TVal> Sort(const std::vector<TVal> &val,
          const std::function<bool (const TVal &, const TVal &)> &comp) {
      std::vector<TVal> ret(val);
      std::sort(ret.begin(), ret.end(), comp);
      return ret;
    }

    /* TODO */
    template <typename TVal>
    TOpt<std::vector<TVal>> Sort(const TOpt<std::vector<TVal>> &val,
          const std::function<bool (const TVal &, const TVal &)> &comp) {
      return val.IsKnown() ? TOpt<std::vector<TVal>>(Sort(val.GetVal(), comp))
                           : *TOpt<std::vector<TVal>>::Unknown;
    }

  }  // Rt

}  // Stig