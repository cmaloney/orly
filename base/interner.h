/* <base/interner.h>

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

#include <cassert>
#include <memory>
#include <tuple>
#include <unordered_map>

#include <base/hash.h>
#include <base/no_copy_semantics.h>

namespace Base {

  /* TODO */
  template <typename TObj, typename... TArgs>
  class TInterner {
    NO_COPY_SEMANTICS(TInterner);
    public:

    typedef std::shared_ptr<const TObj> TPtr;
    typedef std::weak_ptr<const TObj> TWeak;

    /* TODO */
    TInterner() {}

    /* TODO */
    template <typename... TCompatArgs>
    TPtr Get(TCompatArgs &&... args) {
      assert(this);

      TWeak &weak = ObjByKey.insert(std::make_pair(TKey(args...), TWeak())).first->second;
      TPtr strong = weak.lock();
      if (!strong) {
        strong = std::shared_ptr<const TObj>(new TObj(std::forward<TCompatArgs>(args)...));
        weak = strong;
      }
      return strong;
    }

    private:

    /* TODO */
    typedef std::tuple<TArgs...> TKey;

    /* TODO */
    std::unordered_map<TKey, TWeak> ObjByKey;

  };  // TInterner<TObj, TArgs...>

}  // Base
