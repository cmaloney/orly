/* <stig/rt/collected_by.h>

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

#include <functional>
#include <memory>
#include <tuple>

#include <stig/rt/containers.h>
#include <stig/rt/generator.h>

namespace Stig {

  namespace Rt {

    /* The return type of CollectedBy and the 'dict' local variable must be of same type in order
       for NRVO (Named Return Value Optimization to kick in. */
    template <typename TKey, typename TVal, typename TCollect, typename TRet, typename TLhsRhs>
    TDict<TKey, TCollect> CollectedBy(
          const std::shared_ptr<const TGenerator<std::tuple<TKey, TVal>>> generator,
          const std::function<TRet (const TLhsRhs &, const TLhsRhs &)> &collect) {
      TDict<TKey, TCollect> dict;
      for (auto it = generator->NewCursor(); it; ++it) {
        const auto &item = *it;
        const auto &key = std::get<0>(item);
        const auto &val = std::get<1>(item);
        /* This approach was chosen over the 'insert everytime and modify if the key already exists'
           because of the extra calls being made to std::make_pair(). This approach only constructs
           the pair if we need to, this proved to be almost 2x faster over a large loop. The insert
           is also just as efficient since we do 'insert with a hint'.*/
        auto iter = dict.find(key);
        if (iter == std::end(dict)) {
          dict.insert(iter, std::make_pair(key, val));
        } else {
          iter->second = collect(iter->second, val);
        }  // if
      }  // for
      return dict;
    }

  }  // namespace Rt

}  // namespace Stig
