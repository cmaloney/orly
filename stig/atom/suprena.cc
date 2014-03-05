/* <stig/atom/suprena.cc>

   Implements <stig/atom/suprena.h>.

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

#include <stig/atom/suprena.h>

#include <stig/atom/comparison.h>

using namespace std;
using namespace Stig::Atom;

TCore::TOffset TSuprena::Propose(TCore::TNote *proposed_note) {
  assert(this);
  const TCore::TNote *interned_note = NoteInterner.Propose(proposed_note);
  return reinterpret_cast<TCore::TOffset>(interned_note);
}

void TSuprena::ReleaseNote(const TCore::TNote *, TCore::TOffset, void *, void *, void *) {}

const TCore::TNote *TSuprena::TryAcquireNote(TCore::TOffset offset, void *&/*data1*/, void *&/*data2*/, void *&/*data3*/) {
  assert(this);
  auto note = reinterpret_cast<TCore::TNote *>(offset);
  assert(NoteInterner.IsOwned(note));
  return note;
}

const TCore::TNote *TSuprena::TryAcquireNote(TCore::TOffset offset, size_t known_size, void *&/*data1*/, void *&/*data2*/, void *&/*data3*/) {
  assert(this);
  auto note = reinterpret_cast<TCore::TNote *>(offset);
  assert(note->GetRawSize() + sizeof(Atom::TCore::TNote) >= known_size);
  assert(NoteInterner.IsOwned(note));
  return note;
}
