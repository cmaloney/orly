/* <stig/atom/transport_arena2.cc>

   Implements <stig/atom/transport_arena2.h>.

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

#include <stig/atom/transport_arena2.h>

#include <cstdlib>
#include <map>
#include <new>

using namespace std;
using namespace Io;
using namespace Stig::Atom;

TTransportArena::TUnexpectedOffset::TUnexpectedOffset()
    : logic_error("encountered an unexpected offset while writing a transport arena") {}

TTransportArena::~TTransportArena() {
  assert(this);
  free(RawData);
}

TTransportArena *TTransportArena::Read(Io::TBinaryInputStream &strm, TCore &core) {
  assert(&strm);
  assert(&core);
  auto transport_arena = new TTransportArena(strm);
  try {
    strm.ReadExactly(&core, sizeof(TCore));
  } catch (...) {
    delete transport_arena;
    throw;
  }
  return transport_arena;
}

void TTransportArena::Write(TBinaryOutputStream &strm, TSuprena *suprena, const TCore &core) {
  assert(&strm);
  assert(suprena);
  assert(&core);
  /* Get the notes from the arena and put them into merge order. */
  std::vector<const TNote *> notes;
  OrderNotes(notes, suprena->GetNotes(), suprena);
  /* Pass once over the notes to build up a map of old offset to new. */
  uint32_t raw_size = 0;
  map<TOffset, TOffset> offset_map;
  set<TOffset> offsets;
  for (const auto *note: notes) {
    offset_map[reinterpret_cast<TOffset>(note)] = raw_size;
    offsets.insert(raw_size);
    raw_size += GetPaddedSize(sizeof(TNote) + note->GetRawSize());
  }
  /* Now we can define a remapper based on what we found. */
  TCore::TRemap remap =
      [&offset_map](TOffset old_offset) {
        auto iter = offset_map.find(old_offset);
        if (iter == offset_map.end()) {
          throw TUnexpectedOffset();
        }
        return iter->second;
      };
  /* Pass over the notes again to write out the remapped notes. */
  strm << offsets << raw_size;
  for (const auto *note: notes) {
    auto copyof_note = TNote::New(note);
    try {
      copyof_note->Remap(remap);
      size_t
          raw_size = copyof_note->GetRawSize(),
          padded_size = GetPaddedSize(raw_size);
      strm.WriteExactly(copyof_note, sizeof(TNote));
      strm.WriteExactly(copyof_note->GetRawData(), raw_size);
      strm.WriteExactly(Padding, padded_size - raw_size);
    } catch (...) {
      delete copyof_note;
      throw;
    }
    delete copyof_note;
  }
  /* Write the remapped core we'll use as the root. */
  TCore temp = core;
  temp.Remap(remap);
  strm.WriteExactly(&temp, sizeof(temp));
}

TTransportArena::TTransportArena(TBinaryInputStream &strm)
    : TCore::TArena(false) {
  assert(&strm);
  uint32_t raw_size;
  strm >> Offsets >> raw_size;
  RawSize = raw_size;
  RawData = static_cast<char *>(malloc(RawSize));
  if (!RawData) {
    throw bad_alloc();
  }
  try {
    strm.ReadExactly(RawData, RawSize);
  } catch (...) {
    free(RawData);
    throw;
  }
}

TTransportArena::TNote *TTransportArena::GetNoteUnsafely(TOffset offset) const {
  assert(this);
  return reinterpret_cast<TNote *>(RawData + offset);
}

void TTransportArena::ReleaseNote(const TNote *, TOffset, void *, void *, void *) {}

const TTransportArena::TNote *TTransportArena::TryAcquireNote(TOffset offset, void *&/*data1*/, void *&/*data2*/, void *&/*data3*/) {
  assert(this);
  auto iter = Offsets.find(offset);
  return (iter != Offsets.end()) ? GetNoteUnsafely(offset) : nullptr;
}

const TTransportArena::TNote *TTransportArena::TryAcquireNote(TOffset offset, size_t /*known_size*/, void *&/*data1*/, void *&/*data2*/, void *&/*data3*/) {
  assert(this);
  auto iter = Offsets.find(offset);
  return (iter != Offsets.end()) ? GetNoteUnsafely(offset) : nullptr;
}

size_t TTransportArena::GetPaddedSize(size_t size) {
  return (size + 7) & ~7;
}

const char TTransportArena::Padding[7] = { 0, 0, 0, 0, 0, 0, 0 };
