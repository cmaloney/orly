/* <stig/atom/note_interner2.cc>

   Implements <stig/atom/note_interner2.h>.

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

#include <stig/atom/note_interner2.h>

using namespace std;
using namespace Stig::Atom;

TNoteInterner::~TNoteInterner() {
  assert(this);
  for (auto note: Notes) {
    delete const_cast<TCore::TNote *>(note);
  }
}

bool TNoteInterner::IsKnown(const TCore::TNote *note) const {
  assert(this);
  return Notes.find(note) != Notes.end();
}

bool TNoteInterner::IsOwned(const TCore::TNote *note) const {
  assert(this);
  auto iter = Notes.find(note);
  return iter != Notes.end() && *iter == note;
}

const TCore::TNote *TNoteInterner::Propose(TCore::TNote *proposed_note) {
  assert(this);
  assert(proposed_note);
  const TCore::TNote *interned_note;
  try {
    interned_note = *(Notes.insert(proposed_note).first);
  } catch (...) {
    delete proposed_note;
    throw;
  }
  if (interned_note != proposed_note) {
    delete proposed_note;
  }
  return interned_note;
}

void Stig::Atom::OrderNotes(std::vector<const TCore::TNote *> &out_vec, const TNoteInterner::TNotes &notes, TCore::TArena *arena) {
  assert(out_vec.empty());
  out_vec.assign(notes.begin(), notes.end());
  void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
  void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
  void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
  void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
  std::sort(out_vec.begin(), out_vec.end(), std::bind(NoteCmp,
                                                      std::placeholders::_1,
                                                      arena,
                                                      std::placeholders::_2,
                                                      arena,
                                                      lhs_state_alloc,
                                                      rhs_state_alloc,
                                                      lhs_type_alloc,
                                                      rhs_type_alloc));
}