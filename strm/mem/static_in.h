/* <strm/mem/static_in.h>

   A static in-memory buffer which acts as a source of in-flowing data.

   We point at data which we then serve to a consumer, but we don't own
   the data.  Some other entity must be keeping the data around at least as
   long as we live.

   This buffer implicitly constructs from all kinds of stuff.  This is a
   convenience, as we often serve data out of other data structures.

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
#include <cstring>
#include <string>

#include <strm/in.h>
#include <strm/mem/static_out.h>

namespace Strm {

  namespace Mem {

    /* A static in-memory buffer which acts as a source of in-flowing data.
       We point at data which we then serve to a consumer, but we don't own
       the data. */
    class TStaticIn final
        : public In::TProd {
      public:

      /* By default, we have no data. */
      TStaticIn()
          : Start(nullptr), Limit(nullptr) {}

      /* Point at the data in the given static array. */
      template <size_t Size>
      TStaticIn(const uint8_t (&that)[Size])
          : Start(that), Limit(that + Size) {}

      /* Point at the data in the given c-string. */
      TStaticIn(const char *that)
          : Start(reinterpret_cast<const uint8_t *>(that)),
            Limit(reinterpret_cast<const uint8_t *>(that ? (that + strlen(that)) : that)) {}

      /* Point at the data in the given std string. */
      TStaticIn(const std::string &that)
          : Start(reinterpret_cast<const uint8_t *>(that.data())), Limit(Start + that.size()) {}

      /* Point at the data in the given out-buffer. */
      template <size_t Size>
      TStaticIn(const TStaticOut<Size> &that)
          : Start(that.GetStart()), Limit(that.GetLimit()) {}

      /* Point at the given arbitrary data. */
      TStaticIn(const void *start, const void *limit)
          : Start(static_cast<const uint8_t *>(start)),
            Limit(static_cast<const uint8_t *>(limit)) {
        assert(start <= limit);
      }

      /* Point at the given arbitrary data. */
      TStaticIn(const void *data, size_t size)
          : Start(static_cast<const uint8_t *>(data)), Limit(Start + size) {}

      private:

      /* See In::TProd. */
      virtual bool Cycle(
          size_t release_count,
          const uint8_t **start, const uint8_t **limit) override;

      /* The start and limit of the workspace we will report to the consumer.
         If Start >= Limit (for instance, if both are null), then we have no
         data to report. */
      const uint8_t *Start, *Limit;

    };  // TStaticIn

  }  // Mem

}  // Strm
