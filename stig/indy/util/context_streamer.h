/* <stig/indy/util/context_streamer.h>

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

#include <algorithm>
#include <cassert>

#include <stig/indy/update.h>
#include <stig/indy/update_walker.h>
#include <io/binary_output_stream.h>

namespace Stig {

  namespace Indy {

    namespace Util {

      /* Forward Declarations. */
      class TContextInputStreamer;

      /* TODO */
      class TContextOutputStreamer {
        NO_COPY_SEMANTICS(TContextOutputStreamer);
        public:

        /* TODO */
        typedef std::unordered_map<Atom::TCore::TOffset, Atom::TCore::TOffset> TTranslationMap;

        /* TODO */
        TContextOutputStreamer() {}

        /* TODO */
        void AppendUpdate(const Base::TUuid &repo_id, const TUpdateWalker::TItem &item) {
          assert(this);
          UpdateVec.push_back(std::move(TUpdate(repo_id, item, Suprena)));
        }

        void Write(Io::TBinaryOutputStream &strm) const {
          assert(this);
          TTranslationMap translation_map;
          /* Writing out the Intern context first */
          strm << Suprena.GetSize();  // (size_t> number of notes
          Atom::TCore::TNote *temp = nullptr;
          translation_map.reserve(Suprena.GetSize());
          auto remapper = [&translation_map](Atom::TCore::TOffset input) {
            return translation_map[input];
          };
          Atom::TCore::TOffset cur_offset = 0U;
          try {

            std::vector<const Atom::TCore::TNote *> sorted_notes;
            Atom::OrderNotes(sorted_notes, Suprena.GetNotes(), &Suprena);
            for (const auto &note : sorted_notes) {
              const size_t note_size = sizeof(Atom::TCore::TNote) + note->GetRawSize();
              strm << note_size;  // (size_t) number of following bytes that represent the TNote
              if ((temp = reinterpret_cast<Atom::TCore::TNote *>(realloc(temp, note_size))) == 0) {
                free(temp);
                throw std::bad_alloc();
              }
              memcpy(temp, note, note_size);
              temp->Remap(remapper);
              strm.WriteExactly(temp, note_size);  // note_size bytes of data
              translation_map[reinterpret_cast<Atom::TCore::TOffset>(note)] = cur_offset;
              cur_offset += note_size;
            }
          } catch (...) {
            free(temp);
            throw;
          }
          free(temp);
          /* Writing out the individual updates next */
          strm << UpdateVec.size();  // (size_t) number of updates
          for (auto iter : UpdateVec) {
            // (Base::TUuid) Repo Id; (TSequenceNumber) Sequence Number of the update; (TCore) Metadata;  (TCore) Id; (size_t) number of key-value pairs to follow
            strm << iter.RepoId << iter.SequenceNumber << Atom::TCore(iter.Metadata, remapper) << Atom::TCore(iter.Id, remapper) << iter.EntryVec.size();
            for (auto entry : iter.EntryVec) {
              strm << entry.first.GetIndexId() << Atom::TCore(entry.first.GetKey().GetCore(), remapper) << Atom::TCore(entry.second, remapper);  // (TUuid) index_id; (TCore) Key; (TCore) Op
            }
          }
        }

        private:

        /* TODO */
        class TUpdate {
          public:

          /* TODO */
          TUpdate() {}

          /* TODO */
          TUpdate(const Base::TUuid &repo_id, const TUpdateWalker::TItem &item, Atom::TSuprena &new_arena)
              : RepoId(repo_id),
                SequenceNumber(item.SequenceNumber) {
            void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
            Metadata = Atom::TCore(&new_arena, state_alloc, item.MainArena, item.Metadata);
            Id = Atom::TCore(&new_arena, state_alloc, item.MainArena, item.Id);
            for (const auto &iter : item.EntryVec) {
              EntryVec.push_back(std::make_pair(TIndexKey(iter.first.GetIndexId(), TKey(Atom::TCore(&new_arena, state_alloc, iter.first.GetKey().GetArena(), iter.first.GetKey().GetCore()), &new_arena)),
                                                Atom::TCore(&new_arena, state_alloc, item.MainArena, iter.second)));
            }
          }

          /* TODO */
          Base::TUuid RepoId;

          /* TODO */
          TSequenceNumber SequenceNumber;

          /* TODO */
          Atom::TCore Metadata;

          /* TODO */
          Atom::TCore Id;

          /* TODO */
          std::vector<std::pair<TIndexKey, Atom::TCore>> EntryVec;

        };  // TUpdate

        /* TODO */
        mutable Atom::TSuprena Suprena;

        /* TODO */
        std::vector<TUpdate> UpdateVec;

        /* TODO */
        friend class TContextInputStreamer;

      };  // TContextOutputStreamer

      /* TODO */
      class TContextInputStreamer {
        public:

        /* TODO */
        typedef TContextOutputStreamer::TTranslationMap TTranslationMap;
        typedef TContextOutputStreamer::TUpdate TUpdate;

        /* TODO */
        TContextInputStreamer()
            : Suprena(std::make_shared<Atom::TSuprena>()) {}

        /* TODO */
        void Read(Io::TBinaryInputStream &strm) {
          assert(this);
          assert(Suprena);
          TTranslationMap translation_map;
          size_t num_elem;
          strm >> num_elem;  // (size_t) number of notes
          size_t note_size;
          Atom::TCore::TOffset new_offset;
          Atom::TCore::TNote *temp = nullptr;
          Atom::TCore::TOffset old_offset = 0U;
          auto remapper = [&translation_map](Atom::TCore::TOffset input) {
            return translation_map[input];
          };
          /* Reading in the Intern context first */
          try {
            for (size_t i = 0; i < num_elem; ++i) {
              strm >> note_size;  // (size_t) number of following bytes that represent the TNote
              if ((temp = reinterpret_cast<Atom::TCore::TNote *>(realloc(temp, note_size))) == 0) {
                throw std::bad_alloc();
              }
              strm.ReadExactly(temp, note_size);  // note_size bytes of data

              temp->Remap(remapper);
              new_offset = Suprena->Propose(Atom::TCore::TNote::New(temp));

              translation_map[old_offset] = new_offset;
              old_offset += note_size;
            }
          } catch (...) {
            free(temp);
            throw;
          }
          free(temp);
          /* Reading in the individual updates next */
          size_t num_updates;
          strm >> num_updates;  // (size_t) number of updates
          UpdateVec.reserve(num_updates);
          Base::TUuid temp_index_id;
          Atom::TCore temp_key;
          Atom::TCore temp_op;
          for (size_t i = 0; i < num_updates; ++i) {
            TUpdate update;
            size_t num_entry;
            // (Base::TUuid) Repo Id; (TSequenceNumber) Sequence Number of the update; (TCore) Metadata;  (TCore) Id; (size_t) number of key-value pairs to follow
            strm >> update.RepoId >> update.SequenceNumber >> update.Metadata >> update.Id >> num_entry;
            update.Metadata.Remap(remapper);
            update.Id.Remap(remapper);
            update.EntryVec.reserve(num_entry);
            for (size_t j = 0; j < num_entry; ++j) {
              // (TUuid) index_id; (TCore) Key; (TCore) Op
              strm >> temp_index_id >> temp_key >> temp_op;
              temp_key.Remap(remapper);
              temp_op.Remap(remapper);
              update.EntryVec.push_back(std::make_pair(TIndexKey(temp_index_id, TKey(temp_key, Suprena.get())), temp_op));
            }
            UpdateVec.push_back(std::move(update));
          }
        }

        /* TODO */
        void Write(Io::TBinaryOutputStream &strm) const {
          assert(this);
          TTranslationMap translation_map;
          /* Writing out the Intern context first */
          strm << Suprena->GetSize();  // (size_t> number of notes
          Atom::TCore::TNote *temp = nullptr;
          translation_map.reserve(Suprena->GetSize());
          auto remapper = [&translation_map](Atom::TCore::TOffset input) {
            return translation_map[input];
          };
          Atom::TCore::TOffset cur_offset = 0U;
          try {
            std::vector<const Atom::TCore::TNote *> sorted_notes;
            Atom::OrderNotes(sorted_notes, Suprena->GetNotes(), Suprena.get());
            for (const auto &note : sorted_notes) {
              const size_t note_size = sizeof(Atom::TCore::TNote) + note->GetRawSize();
              strm << note_size;  // (size_t) number of following bytes that represent the TNote
              if ((temp = reinterpret_cast<Atom::TCore::TNote *>(realloc(temp, note_size))) == 0) {
                free(temp);
                throw std::bad_alloc();
              }
              memcpy(temp, note, note_size);
              temp->Remap(remapper);
              strm.WriteExactly(temp, note_size);  // note_size bytes of data
              translation_map[reinterpret_cast<Atom::TCore::TOffset>(note)] = cur_offset;
              cur_offset += note_size;
            }
          } catch (...) {
            free(temp);
            throw;
          }
          free(temp);
          /* Writing out the individual updates next */
          strm << UpdateVec.size();  // (size_t) number of updates
          for (auto iter : UpdateVec) {
            // (Base::TUuid) Repo Id; (TSequenceNumber) Sequence Number of the update; (TCore) Metadata;  (TCore) Id; (size_t) number of key-value pairs to follow
            strm << iter.RepoId << iter.SequenceNumber << Atom::TCore(iter.Metadata, remapper) << Atom::TCore(iter.Id, remapper) << iter.EntryVec.size();
            for (auto entry : iter.EntryVec) {
              strm << entry.first.GetIndexId() << Atom::TCore(entry.first.GetKey().GetCore(), remapper) << Atom::TCore(entry.second, remapper);  // (TUuid) index_id; (TCore) Key; (TCore) Op
            }
          }
        }

        /* TODO */
        void AppendUpdate(const Base::TUuid &repo_id, const TUpdateWalker::TItem &item) {
          assert(this);
          UpdateVec.push_back(std::move(TUpdate(repo_id, item, *Suprena)));
        }

        /* TODO */
        std::shared_ptr<Atom::TSuprena> Suprena;

        /* TODO */
        std::vector<TUpdate> UpdateVec;

      };  // TContextInputStreamer

      /* Binary streamers for Stig::Indy::Util::TContextInputStreamer */
      inline Io::TBinaryOutputStream &operator<<(Io::TBinaryOutputStream &strm, const TContextInputStreamer &context) { context.Write(strm); return strm; }
      inline Io::TBinaryOutputStream &&operator<<(Io::TBinaryOutputStream &&strm, const TContextInputStreamer &context) { context.Write(strm); return std::move(strm); }
      inline Io::TBinaryInputStream &operator>>(Io::TBinaryInputStream &strm, TContextInputStreamer &context) { context.Read(strm); return strm; }
      inline Io::TBinaryInputStream &&operator>>(Io::TBinaryInputStream &&strm, TContextInputStreamer &context) { context.Read(strm); return std::move(strm); }

      /* Binary streamers for Stig::Indy::Util::TContextOutputStreamer */
      inline Io::TBinaryOutputStream &operator<<(Io::TBinaryOutputStream &strm, const TContextOutputStreamer &context) { context.Write(strm); return strm; }
      inline Io::TBinaryOutputStream &&operator<<(Io::TBinaryOutputStream &&strm, const TContextOutputStreamer &context) { context.Write(strm); return std::move(strm); }

    }  // Util

  }  // Indy

}  // Stig