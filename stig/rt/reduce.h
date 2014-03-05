/* <stig/rt/reduce.h>

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

#include <stig/rt/generator.h>

namespace Stig {

  namespace Rt {

    template <typename TRes, typename TSrc>
    using TReduceFunc = std::function<TRes (const TRes &carry, const TSrc &that)>;


    template <typename TRes, typename TSrc>
    TRes Reduce(const typename TGenerator<TSrc>::TPtr &gen, const TReduceFunc<TRes, TSrc> &reduce_func, TRes start) {
      //NOTE: start becomes res.
      for(auto it = gen->NewCursor(); it; ++it) {
        start = reduce_func(start, *it);
      }
      return start;
    }

    template <typename TRes, typename TSrc>
    using TFastReduceFunc = std::function<void (TRes &carry, const TSrc &that)>;

    template <typename TRes, typename TSrc>
    TRes FastReduce(const typename TGenerator<TSrc>::TPtr &gen, const TFastReduceFunc<TRes, TSrc> &reduce_func, TRes start) {
      //NOTE: start becomes res.
      for(auto it = gen->NewCursor(); it; ++it) {
        reduce_func(start, *it);
      }
      return std::move(start);
    }

  } // Rt

} // Stig