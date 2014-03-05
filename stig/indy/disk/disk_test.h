/* <stig/indy/disk/disk_test.h>

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

#include <sstream>

#include <base/scheduler.h>
#include <base/uuid.h>
#include <stig/atom/suprena.h>
#include <stig/indy/disk/indy_util_reporter.h>
#include <stig/indy/disk/loop_block_dev.h>
#include <stig/indy/disk/read_file.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      class TMockMem
          : public TMemoryLayer {
        NO_COPY_SEMANTICS(TMockMem);
        public:

        TMockMem() : TMemoryLayer(nullptr) {}

        virtual ~TMockMem() {}

      };

      class TMockUpdate : public TUpdate {
        NO_COPY_SEMANTICS(TMockUpdate);
        public:

        static TMockUpdate *NewMockUpdate(const TOpByKey &op_by_key, const TKey &id, const TKey &metadata, TSequenceNumber seq_num) {
          void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
          return new TMockUpdate(op_by_key, id, metadata, seq_num, state_alloc);
        }

        private:

        TMockUpdate(const TOpByKey &op_by_key, const TKey &id, const TKey &metadata, TSequenceNumber seq_num, void *state_alloc)
            : TUpdate(op_by_key, id, metadata, state_alloc) {
          SetSequenceNumber(seq_num);
        }

      };

      class TReader
          : public TReadFile<Util::LogicalBlockSize, Util::LogicalBlockSize, Util::PhysicalBlockSize, Util::PageCheckedBlock, true> {
        NO_COPY_SEMANTICS(TReader);
        public:

        static constexpr size_t PhysicalCachePageSize = Util::PhysicalBlockSize / (Util::LogicalBlockSize / Util::LogicalBlockSize);

        using TArena = TDiskArena<Util::LogicalBlockSize, Util::LogicalBlockSize, Util::PhysicalBlockSize, Util::PageCheckedBlock, 128, true>;

        TReader(const Base::TCodeLocation &code_location /* DEBUG */, Util::TEngine *engine, const Base::TUuid &file_id, size_t gen_id)
            : TReadFile(code_location, Source::MergeDataFileOther, engine, file_id, Low, gen_id) {
          Arena = std::unique_ptr<TArena>(new TArena(this, engine->GetCache<PhysicalCachePageSize>(), Low));
        }

        virtual ~TReader() {
          assert(this);
          Arena.reset();
        }

        using TReadFile::TIndexFile;

        using TReadFile::GetNumBytesOfArena;
        using TReadFile::GetNumUpdates;
        using TReadFile::GetNumArenaNotes;
        using TReadFile::GetGenId;
        using TReadFile::GetTypeBoundaryOffsetVec;
        using TReadFile::ForEachIndex;
        using TReadFile::FindInHash;

        inline const std::unique_ptr<TArena> &GetArena() const {
          assert(this);
          assert(Arena);
          return Arena;
        }

        private:

        /* TODO */
        std::unique_ptr<TArena> Arena;

      };  // TReader

      void GracefullShutdown() {
        sleep(2);
      }

      template <typename ...TArgs>
      void Insert(TMockMem &mem_layer, TSequenceNumber seq_num, const Base::TUuid &idx_id, int64_t val, const TArgs &...args) {
        Atom::TSuprena arena;
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        std::unique_ptr<TMockUpdate> update(TMockUpdate::NewMockUpdate(TUpdate::TOpByKey{
          { TIndexKey(idx_id, TKey(std::make_tuple(args...), &arena, state_alloc)), TKey(val, &arena, state_alloc)}
          }, TKey(&arena), TKey(Base::TUuid(Base::TUuid::Twister), &arena, state_alloc), seq_num));
        mem_layer.Insert(TUpdate::CopyUpdate(update.get(), state_alloc));
      }

      template <typename ...TArgs>
      void Insert(TMockMem &mem_layer, TSequenceNumber seq_num, const Base::TUuid &idx_id, const TKey &val, const TArgs &...args) {
        Atom::TSuprena arena;
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        std::unique_ptr<TMockUpdate> update(TMockUpdate::NewMockUpdate(TUpdate::TOpByKey{
          { TIndexKey(idx_id, TKey(std::make_tuple(args...), &arena, state_alloc)), val}
          }, TKey(&arena), TKey(Base::TUuid(Base::TUuid::Twister), &arena, state_alloc), seq_num));
        mem_layer.Insert(TUpdate::CopyUpdate(update.get(), state_alloc));
      }

    }  // Disk

  }  // Indy

}  // Stig