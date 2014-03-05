/* <stig/atom/core_vector_builder.h>

   Builds a vector of cores to be streamed out.

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
#include <functional>
#include <map>
#include <unordered_set>
#include <vector>

#include <base/no_copy_semantics.h>
#include <io/binary_output_stream.h>
#include <stig/atom/kit2.h>
#include <stig/native/all.h>
#include <stig/sabot/state.h>
#include <stig/sabot/type.h>

namespace Stig {

  namespace Atom {

    /* Builds a vector of cores to be streamed out. */
    class TCoreVectorBuilder final {
      NO_COPY_SEMANTICS(TCoreVectorBuilder);
      public:

      /* Conveniences. */
      using TArena  = TCore::TArena;
      using TNote   = TCore::TNote;
      using TOffset = TCore::TOffset;

      /* Start out empty. */
      TCoreVectorBuilder();

      /* Frees the cores and the arena. */
      ~TCoreVectorBuilder();

      /* The arena to which the cores in the vector refer.  Never null. */
      TArena *GetArena() const {
        assert(this);
        return Arena;
      }

      /* The cores we've accumulated. */
      const std::vector<TCore> &GetCores() const {
        assert(this);
        return Cores;
      }

      /* Append the given value to the vector. */
      template <typename TVal>
      void Push(const TVal &val) {
        assert(this);
        Sabot::State::TAny::TWrapper state(Native::State::New(val, StateBuffer));
        SetDepth(state);
        Cores.emplace_back(Arena, state.get());
      }

      /* Append the value represented by the given state to the vector. */
      void PushState(const Sabot::State::TAny::TWrapper &state) {
        assert(this);
        SetDepth(state);
        Cores.emplace_back(Arena, state.get());
      }

      /* Write the vector to the given stream. */
      void Write(Io::TBinaryOutputStream &strm) const;

      /* TODO */
      inline size_t GetNumArenaBytes() const {
        assert(this);
        assert(Arena);
        return Arena->GetNumBytes();
      }

      private:

      /* An arena in which to do dirty, dirty things. */
      class TDirtyArena final
          : public TCore::TExtensibleArena {
        public:

        /* Do-little. */
        TDirtyArena(const TCoreVectorBuilder *builder);

        /* Destroys all notes when it goes. */
        ~TDirtyArena();

        /* Call by for each note, in order of increasing depth. */
        bool ForEachNote(const std::function<bool (const TNote *)> &cb) const;

        /* See base class. */
        virtual TOffset Propose(TNote *proposed_note) override;

        /* TODO */
        inline size_t GetNumBytes() {
          assert(this);
          return NumBytes;
        }

        private:

        /* See base class.  Does nothing. */
        virtual void ReleaseNote(const TNote */*note*/, TOffset /*offset*/, void */*data1*/, void */*data2*/, void */*data3*/) override;

        /* See base class.  Looks up the requested offset in Notes. */
        virtual const TNote *TryAcquireNote(TOffset offset, void *&/*data1*/, void *&/*data2*/, void *&/*data3*/) override;

        /* See base class.  Looks up the requested offset in Notes. */
        virtual const TNote *TryAcquireNote(TOffset offset, size_t known_size, void *&/*data1*/, void *&/*data2*/, void *&/*data3*/) override;

        /* The builder of which we are a part.  Never null. */
        const TCoreVectorBuilder *Builder;

        /* Our notes in sets of equal depth, from shallowest to deepest. */
        std::map<size_t, std::unordered_set<TNote *>> NotesByDepth;

        size_t NumBytes;

      };  // TDirtyArena

      /* Sets the depth of the core being interned. */
      void SetDepth(const Sabot::State::TAny::TWrapper &state);

      /* Returns the next multiple of 8 bytes. */
      static size_t GetPaddedSize(size_t size);

      /* See accessor. */
      TDirtyArena *Arena;

      /* See accessor. */
      std::vector<TCore> Cores;

      /* The depth of the core being interned. */
      size_t Depth;

      /* Temporary space used while appending a core to the vector. */
      char
          StateBuffer[Sabot::State::GetMaxStateSize()],
          TypeBuffer[Sabot::Type::GetMaxTypeSize()];

      /* Seven bytes of zero, using to pad out oddly sized notes. */
      static const char Padding[7];

    };  // TCoreVectorBuilder

  }  // Atom

}  // Stig
