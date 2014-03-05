/* <stig/atom/core_vector_builder.cc>

   Implements <stig/atom/core_vector_builder.h>.

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

#include <stig/atom/core_vector_builder.h>

#include <cstdint>
#include <map>
#include <set>

#include <stig/sabot/get_depth.h>

using namespace std;
using namespace Io;
using namespace Stig::Atom;

TCoreVectorBuilder::TCoreVectorBuilder() {
  Arena = new TDirtyArena(this);
}

TCoreVectorBuilder::~TCoreVectorBuilder() {
  assert(this);
  delete Arena;
}

void TCoreVectorBuilder::Write(TBinaryOutputStream &strm) const {
  assert(this);
  assert(&strm);
  /* Pass once over the notes to build up a map of old offset to new. */
  uint32_t raw_size = 0;
  map<TOffset, TOffset> offset_map;
  Arena->ForEachNote(
      [this, &offset_map, &raw_size](const TNote *note) {
        offset_map[reinterpret_cast<TOffset>(note)] = raw_size;
        raw_size += GetPaddedSize(sizeof(TNote) + note->GetRawSize());
        return true;
      }
  );
  /* Now we can define a remapper based on what we found. */
  TCore::TRemap remap =
      [&offset_map](TOffset old_offset) {
        auto iter = offset_map.find(old_offset);
        assert(iter != offset_map.end());
        return iter->second;
      };
  /* Pass over the notes again to write out the remapped notes. */
  strm << raw_size;
  Arena->ForEachNote(
      [this, &strm, &remap](const TNote *note) {
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
        return true;
      }
  );
  /* Write the number of cores in the array, followed by the (remapped) cores themselves. */
  strm << static_cast<uint32_t>(Cores.size());
  for (const auto &core: Cores) {
    TCore temp = core;
    temp.Remap(remap);
    strm.WriteExactly(&temp, sizeof(temp));
  }
}

TCoreVectorBuilder::TDirtyArena::TDirtyArena(const TCoreVectorBuilder *builder)
    : TArena(false), TCore::TExtensibleArena(false), Builder(builder), NumBytes(0UL) {
  assert(builder);
}

TCoreVectorBuilder::TDirtyArena::~TDirtyArena() {
  assert(this);
  for (const auto &item: NotesByDepth) {
    for (TNote *note: item.second) {
      delete note;
    }
  }
}

bool TCoreVectorBuilder::TDirtyArena::ForEachNote(const function<bool (const TNote *)> &cb) const {
  assert(this);
  assert(&cb);
  for (const auto &item: NotesByDepth) {
    for (const TNote *note: item.second) {
      if (!cb(note)) {
        return false;
      }
    }
  }
  return true;
}

TCoreVectorBuilder::TOffset TCoreVectorBuilder::TDirtyArena::Propose(TNote *proposed_note) {
  assert(this);
  assert(proposed_note);
  try {
    static const unordered_set<TNote *> no_notes;
    NotesByDepth.insert(make_pair(Builder->Depth, no_notes)).first->second.insert(proposed_note);
    NumBytes += sizeof(TNote) + proposed_note->GetRawSize();
  } catch (...) {
    delete proposed_note;
    throw;
  }
  return reinterpret_cast<TOffset>(proposed_note);
}

void TCoreVectorBuilder::TDirtyArena::ReleaseNote(const TNote */*note*/, TOffset /*offset*/, void */*data1*/, void */*data2*/, void */*data3*/) {}

const TCoreVectorBuilder::TNote *TCoreVectorBuilder::TDirtyArena::TryAcquireNote(TOffset offset, void *&/*data1*/, void *&/*data2*/, void *&/*data3*/) {
  assert(this);
  return reinterpret_cast<TNote *>(offset);
}

const TCoreVectorBuilder::TNote *TCoreVectorBuilder::TDirtyArena::TryAcquireNote(TOffset offset, size_t /*known_size*/, void *&/*data1*/, void *&/*data2*/, void *&/*data3*/) {
  assert(this);
  return reinterpret_cast<TNote *>(offset);
}

void TCoreVectorBuilder::SetDepth(const Sabot::State::TAny::TWrapper &state) {
  assert(this);
  assert(&state);
  Depth = Sabot::GetDepth(*Sabot::Type::TAny::TWrapper(state->GetType(TypeBuffer)));
}

size_t TCoreVectorBuilder::GetPaddedSize(size_t size) {
  return (size + 7) & ~7;
}

const char TCoreVectorBuilder::Padding[7] = { 0, 0, 0, 0, 0, 0, 0 };
