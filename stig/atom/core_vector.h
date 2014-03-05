/* <stig/atom/core_vector.h>

   Streams in a vector of cores.

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
#include <set>
#include <vector>

#include <base/no_copy_semantics.h>
#include <io/binary_input_stream.h>
#include <stig/atom/kit2.h>

namespace Stig {

  namespace Atom {

    /* Streams in a vector of cores. */
    class TCoreVector final {
      NO_COPY_SEMANTICS(TCoreVector);
      public:

      /* Conveniences. */
      using TArena  = TCore::TArena;
      using TNote   = TCore::TNote;
      using TOffset = TCore::TOffset;

      /* Read from the given stream and hold the entire vector in memory. */
      explicit TCoreVector(Io::TBinaryInputStream &strm);

      /* Frees the cores and the arena. */
      virtual ~TCoreVector();

      /* The arena to which the cores in the vector refer.  Never null. */
      TArena *GetArena() const {
        assert(this);
        return Arena;
      }

      /* The cores we streamed in. */
      const std::vector<TCore> &GetCores() const {
        assert(this);
        return Cores;
      }

      private:

      /* The arena we stream in.  It contains all the notes, packed together. */
      class TPackedArena final
          : public TCore::TArena {
        NO_COPY_SEMANTICS(TPackedArena);
        public:

        /* Read the nodes from the given stream. */
        explicit TPackedArena(Io::TBinaryInputStream &strm);

        /* Frees the notes. */
        virtual ~TPackedArena();

        private:

        /* See base class.  Does nothing. */
        virtual void ReleaseNote(const TNote *note, TOffset offset, void *data1, void *data2, void *data3) override;

        /* See base class.  Looks up the requested offset in Offsets. */
        virtual const TNote *TryAcquireNote(TOffset offset, void *&data1, void *&data2, void *&data3) override;

        /* See base class.  Looks up the requested offset in Offsets. */
        virtual const TNote *TryAcquireNote(TOffset offset, size_t known_size, void *&data1, void *&data2, void *&data3) override;

        /* Our notes, packed together. */
        char *RawData;

        /* The number of bytes of raw data we contain. */
        size_t RawSize;

        /* The offsets where our notes lie. */
        //std::set<TOffset> Offsets;

      };  // TCoreVector::TPackedArena

      /* See accessor. */
      TArena *Arena;

      /* See accessor. */
      std::vector<TCore> Cores;

    };  // TCoreVector

  }  // Atom

}  // Stig
