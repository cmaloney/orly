/* <strm/mem/static_out.h>

   A static in-memory buffer which acts as a destination for out-flowing data.

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

#include <strm/out.h>
#include <strm/past_end.h>

namespace Strm {

  namespace Mem {

    /* A static in-memory buffer which acts as a destination for out-flowing
       data.  The object contains the workspace itself.  It doesn't do any
       heap allocation. */
    template <size_t MaxSize_>
    class TStaticOut final
        : public Out::TCons {
      public:

      /* Pulling our template parameter into scope. */
      static constexpr size_t MaxSize = MaxSize_;

      /* Start out empty. */
      TStaticOut()
          : Limit(Workspace) {}

      /* The limit of the data we contain.
         Always >= GetStart().  Never null. */
      const char *GetLimit() const noexcept {
        assert(this);
        return Limit;
      }

      /* The number of bytes we contain.  Can be zero. */
      size_t GetSize() const noexcept {
        assert(this);
        return Limit - Workspace;
      }

      /* The start of the data we contain.
         Always <= GetLimit().  Never null. */
      const char *GetStart() const noexcept {
        assert(this);
        return Workspace;
      }

      /* Go back to being empty. */
      void Reset() const noexcept {
        assert(this);
        Limit = Workspace;
      }

      private:

      /* See Out::TCons. */
      virtual void Cycle(char *cursor, char **start, char **limit) override {
        assert(this);
        if (cursor) {
          assert(cursor >= Limit);
          assert(cursor <= Workspace + MaxSize);
          Limit = cursor;
        }
        if (start) {
          assert(limit);
          *start = Limit;
          *limit = Workspace + MaxSize;
          if (*start >= *limit) {
            throw TPastEnd();
          }
        }
      }

      /* Our one and only workspace. */
      char Workspace[MaxSize];

      /* The limit of the data currently in Workspace.
         Always true: Workspace <= Limit <= (Workspace + MaxSize) */
      char *Limit;

    };  // TStaticOut<MaxSize>

    /* The default static output consumer holds up to 64K. */
    using TDefStaticOut = TStaticOut<0x10000>;

  }  // Mem

}  // Strm
