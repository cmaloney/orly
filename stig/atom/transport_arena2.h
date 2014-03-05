/* <stig/atom/transport_arena2.h>

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

#include <set>
#include <stdexcept>

#include <io/binary_input_stream.h>
#include <io/binary_output_stream.h>
#include <stig/atom/kit2.h>
#include <stig/atom/suprena.h>

namespace Stig {

  namespace Atom {

    /* TODO */
    class TTransportArena final
        : public TCore::TArena {
      NO_COPY_SEMANTICS(TTransportArena);
      public:

      /* Conveniences. */
      using TNote   = TCore::TNote;
      using TOffset = TCore::TOffset;

      /* Thrown by Write() if things don't seem to be in the right order or the contents of the notes are messed up. */
      class TUnexpectedOffset
          : public std::logic_error {
        public:

        /* Do-little. */
        TUnexpectedOffset();

      };  // TUnexpectedOffset

      /* A generator of notes in merge order. */
      using TGenerator = std::function<bool (const TNote *)>;

      /* Frees the raw data when it goes. */
      virtual ~TTransportArena();

      /* Stream in.  Constructs a new transport arena as a side-effect. */
      static TTransportArena *Read(Io::TBinaryInputStream &strm, TCore &core);

      /* Stream out. */
      static void Write(Io::TBinaryOutputStream &strm, TSuprena *suprena, const TCore &core);

      private:

      /* Construct from a stream written by Write(). */
      explicit TTransportArena(Io::TBinaryInputStream &strm);

      /* Convert an offset to a note without validation. */
      TNote *GetNoteUnsafely(TOffset offset) const;

      /* See base class.  Does nothing. */
      virtual void ReleaseNote(const TNote *note, TOffset offset, void *data1, void *data2, void *data3) override;

      /* See base class.  Looks up the requested offset in Notes. */
      virtual const TNote *TryAcquireNote(TOffset offset, void *&data1, void *&data2, void *&data3) override;

      /* See base class.  Looks up the requested offset in Notes. */
      virtual const TNote *TryAcquireNote(TOffset offset, size_t known_size, void *&data1, void *&data2, void *&data3) override;

      /* Returns the next multiple of 8 bytes. */
      static size_t GetPaddedSize(size_t size);

      /* Our notes, packed together. */
      char *RawData;

      /* The number of bytes of raw data we contain. */
      size_t RawSize;

      /* The offsets where our notes lie. */
      std::set<TOffset> Offsets;

      /* Seven bytes of zero, using to pad out oddly sized notes. */
      static const char Padding[7];

    };  // TTransportArena

  }  // Atom

}  // Stig
