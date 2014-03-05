/* <stig/atom/suprena.h>

   An extensible arena that interns notes in memory.

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
#include <map>
#include <unordered_set>

#include <stig/atom/kit2.h>
#include <stig/atom/note_interner2.h>

namespace Stig {

  namespace Atom {

    /* An extensible arena that interns notes in memory. */
    class TSuprena final
        : public TCore::TExtensibleArena {
      NO_COPY_SEMANTICS(TSuprena);
      public:

      /* Do-little. */
      inline TSuprena();

      /* Destroys all notes when it goes. */
      inline ~TSuprena();

      /* The number of notes we contain. */
      inline size_t GetSize() const;

      /* TODO */
      inline const TNoteInterner::TNotes &GetNotes() const;

      /* See base class. */
      virtual TCore::TOffset Propose(TCore::TNote *proposed_note) final;

      private:

      /* See base class.  Does nothing. */
      virtual void ReleaseNote(const TCore::TNote *note, TCore::TOffset /*offset*/, void *data1, void *data2, void *data3) override;

      /* See base class.  Looks up the requested offset in the interner. */
      virtual const TCore::TNote *TryAcquireNote(TCore::TOffset offset, void *&data1, void *&data2, void *&data3) override;

      /* See base class.  Looks up the requested offset in the interner with a hint for the size. */
      virtual const TCore::TNote *TryAcquireNote(TCore::TOffset offset, size_t known_size, void *&data1, void *&data2, void *&data3) override;

      /* One interner, used for all depths alike. */
      TNoteInterner NoteInterner;

    };  // TSuprena

    /* Inline */

    inline TSuprena::TSuprena()
        : TCore::TArena(false), TCore::TExtensibleArena(false) {}

    inline TSuprena::~TSuprena() {}

    inline size_t TSuprena::GetSize() const {
      assert(this);
      return NoteInterner.GetSize();
    }

    inline const TNoteInterner::TNotes &TSuprena::GetNotes() const {
      assert(this);
      return NoteInterner.GetNotes();
    }

  }  // Atom

}  // Stig
