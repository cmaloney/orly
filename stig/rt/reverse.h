/* <stig/rt/reverse.h>

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
#include <stig/rt/opt.h>

namespace Stig {

  namespace Rt {

    template <typename TVal>
    TVal Reverse(const TVal &) = delete;

    /* TODO */
    template <typename TVal>
    std::vector<TVal> Reverse(const std::vector<TVal> &val) {
      return std::vector<TVal>(val.rbegin(), val.rend());
    }

    /* TODO */
    template <typename TVal>
    TOpt<std::vector<TVal>> Reverse(const TOpt<std::vector<TVal>> &val) {
      return val.IsKnown() ? TOpt<std::vector<TVal>>(Reverse(val.GetVal())) : *TOpt<std::vector<TVal>>::Unknown;
    }

    /* TODO */
    inline std::string Reverse(const std::string &val) {
      return std::string(val.rbegin(), val.rend());
    }

    /* TODO */
    inline TOpt<std::string> Reverse(const TOpt<std::string> &val) {
      return val.IsKnown() ? TOpt<std::string>(Reverse(val.GetVal())) : *TOpt<std::string>::Unknown;
    }

  }  // Rt

}  // Stig