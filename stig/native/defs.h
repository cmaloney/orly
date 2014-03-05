/* <stig/native/defs.h>

   Basic types used in native sabots.

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

#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include <base/no_copy_semantics.h>
#include <stig/desc.h>
#include <stig/sabot/defs.h>

namespace Stig {

  namespace Native {

    /* A placeholder for a decent native blob type. */
    using TBlob = std::basic_string<uint8_t>;

    /* A free value of a specific type.  This is a stateless singleton. */
    template <typename TVal>
    class TFree {
      public:

      /* Our single instance. */
      static const TFree Free;

      /* Do-little. */
      TFree() {}

    };  // TFree<TVal>

    /* See declaration. */
    template <typename TVal>
    const TFree<TVal> TFree<TVal>::Free;

    /* A tombstone where once a value lived.  This is a stateless singleton. */
    class TTombstone {
      NO_COPY_SEMANTICS(TTombstone);
      public:

      /* Our single instance. */
      static const TTombstone Tombstone;

      private:

      /* Do-little. */
      TTombstone() {}

    };  // TTombstone

  }  // Native

}  // Stig
