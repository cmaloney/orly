/* <stig/indy/disk/read_file.h>

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

#include <cassert>

#include <algorithm>
#include <ostream>

#include <base/no_copy_semantics.h>
#include <inv_con/unordered_multimap.h>
#include <server/daemonize.h>
#include <stig/atom/kit2.h>
#include <stig/indy/disk/in_file.h>
#include <stig/indy/disk/indy_util_reporter.h>
#include <stig/indy/disk/util/cache.h>
#include <stig/indy/disk/util/engine.h>
#include <stig/indy/update.h>
#include <stig/sabot/match_prefix_state.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      /* TODO */
      class TFileKey {
        public:

        /* TODO */
        TFileKey(const Base::TUuid &file_id, size_t gen_id) : FileId(file_id), GenId(gen_id) {}

        /* TODO */
        TFileKey(Base::TUuid &&file_id, size_t gen_id) : FileId(std::move(file_id)), GenId(gen_id) {}

        /* TODO */
        Base::TUuid FileId;

        /* TODO */
        size_t GenId;

        /* TODO */
        inline size_t GetHash() const {
          assert(this);
          return GenId ^ FileId.GetHash();
        }

        /* TODO */
        inline bool operator==(const TFileKey &that) const {
          assert(this);
          assert(&that);
          return FileId == that.FileId && GenId == that.GenId;
        }

        /* TODO */
        inline bool operator!=(const TFileKey &that) const {
          assert(this);
          assert(&that);
          return FileId != that.FileId || GenId != that.GenId;
        }

      };  // TFileKey

    }  // Disk

  }  // Indy

}  // Stig

namespace std {

  /* A standard hasher for Stig::Indy::Disk::TFileKey. */
  template <>
  struct hash<Stig::Indy::Disk::TFileKey> {
    typedef size_t result_type;
    typedef Stig::Indy::Disk::TFileKey argument_type;
    size_t operator()(const Stig::Indy::Disk::TFileKey &that) const {
      return that.GetHash();
    }
  };

}  // std

namespace Stig {

  namespace Indy {

    namespace Disk {

      /* TODO */
      class TArenaInFile
          : public TInFile {
        NO_COPY_SEMANTICS(TArenaInFile);
        public:

        /* TODO */
        virtual size_t GetByteOffsetOfArena() const = 0;

        /* TODO */
        virtual size_t GetNumArenaNotes() const = 0;

        /* TODO */
        virtual Atom::TCore::TOffset GetNumBytesOfArena() const = 0;

        protected:

        /* TODO */
        TArenaInFile() {}

        /* TODO */
        virtual ~TArenaInFile() {}

      };

      /* TODO */
      static constexpr size_t DiskArenaMaxCacheSize = 128;

      /* TODO */
      template <size_t CachePageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind, size_t StreamLocalCacheSize, bool ScanAheadAllowed>
      class TDiskArena
          : public Atom::TCore::TArena {
        NO_COPY_SEMANTICS(TDiskArena);
        private:

        public:

        /* TODO */
        static constexpr size_t DataChunkSize = Util::GetLogicalDataChunkSize<BufKind>();
        static constexpr size_t PhysicalDataChunkSize = Util::GetPhysicalDataChunkSize<BufKind>();
        static constexpr size_t PhysicalCachePageSize = PhysicalBlockSize / (BlockSize / CachePageSize);

        /* TODO */
        class TCursor {
          NO_COPY_SEMANTICS(TCursor);
          public:

          /* TODO */
          TCursor(TDiskArena *context, Atom::TCore::TOffset start_offset, Atom::TCore::TOffset end_offset)
              : Arena(context),
                Pin(nullptr),
                PinAlloc(nullptr),
                Offset(start_offset),
                EndOffset(end_offset) {
            //assert(end_offset <= context->GetNumBytesOfArena());
            PinAlloc = reinterpret_cast<Atom::TCore::TArena::TFinalPin *>(malloc(sizeof(Atom::TCore::TArena::TFinalPin)));
            if (PinAlloc == 0) {
              syslog(LOG_EMERG, "bad alloc in TDiskArena::TCursor");
              throw std::bad_alloc();
            }
            try {
              Refresh();
            } catch(...) {
              if (Pin) {
                Pin->TPin::~TPin();
              }
              free(PinAlloc);
              throw;
            }
          }

          /* TODO */
          ~TCursor() {
            assert(this);
            if (Pin) {
              Pin->TPin::~TPin();
            }
            free(PinAlloc);
          }

          /* TODO */
          operator bool() const {
            assert(this);
            return Pin != nullptr;
          }

          /* TODO */
          const Atom::TCore::TNote *operator*() const {
            assert(this);
            assert(Pin);
            assert(Pin->GetNote());
            return Pin->GetNote();
          }

          /* TODO */
          const Atom::TCore::TNote *operator->() const {
            assert(this);
            assert(Pin);
            assert(Pin->GetNote());
            return Pin->GetNote();
          }

          /* TODO */
          TCursor &operator++() {
            assert(this);
            Refresh();
            return *this;
          }

          /* TODO */
          Atom::TCore::TOffset GetOffset() const {
            assert(this);
            return Offset;
          }

          /* TODO */
          inline TDiskArena *GetArena() const {
            return Arena;
          }

          private:

          /* TODO */
          void Refresh() {
            assert(this);
            if (Pin) {
              Offset += sizeof(Atom::TCore::TNote) + Pin->GetNote()->GetRawSize();
              Pin->TPin::~TPin();
            }
            if (Offset < EndOffset) {
              Pin = Arena->Pin(Offset, PinAlloc);
            } else {
              Pin = nullptr;
            }
          }

          /* TODO */
          TDiskArena *Arena;

          /* TODO */
          TDiskArena::TFinalPin *Pin;

          /* TODO */
          TDiskArena::TFinalPin *PinAlloc;

          /* TODO */
          Atom::TCore::TOffset Offset;

          /* TODO */
          Atom::TCore::TOffset EndOffset;

        };  // TCursor

        /* TODO */
        TDiskArena(TArenaInFile *file, Util::TCache<PhysicalCachePageSize> *cache, DiskPriority priority)
            : TArena(true),
              File(file),
              Priority(priority),
              Cache(cache),
              StartOffset(file->GetByteOffsetOfArena()),
              Stream(HERE, Source::DiskArena, priority, StartOffset + file->GetNumBytesOfArena(), file, cache, StartOffset),
              NumNotes(file->GetNumArenaNotes()) {
          assert(file);
        }

        /* TODO */
        virtual ~TDiskArena() {
          assert(this);
        }

        /* TODO */
        inline virtual void ReleaseNote(const Atom::TCore::TNote *note, Atom::TCore::TOffset offset, void *data1, void *data2, void *data3);

        /* TODO */
        inline virtual const Atom::TCore::TNote *TryAcquireNote(Atom::TCore::TOffset offset, void *&data1, void *&data2, void *&data3);

        /* TODO */
        inline virtual const Atom::TCore::TNote *TryAcquireNote(Atom::TCore::TOffset offset, size_t known_size, void *&data1, void *&data2, void *&data3);

        /* TODO */
        Atom::TCore::TOffset GetNumBytesOfArena() const {
          assert(this);
          return File->GetNumBytesOfArena();
        }

        private:

        /* TODO */
        TArenaInFile *File;

        /* TODO */
        DiskPriority Priority;

        /* TODO */
        Util::TCache<PhysicalCachePageSize> *Cache;

        /* TODO */
        TCompletionTrigger SyncTrigger;

        /* TODO */
        size_t StartOffset;

        /* TODO */
        TStream<CachePageSize, BlockSize, PhysicalBlockSize, BufKind, StreamLocalCacheSize, ScanAheadAllowed> Stream;

        /* TODO */
        size_t NumNotes;

      };  // TDiskArena

      /* TODO */
      template <size_t CachePageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind, bool ScanAheadAllowed = true>
      class TReadFile
          : public TArenaInFile {
        NO_COPY_SEMANTICS(TReadFile);
        public:

        /* TODO */
        static constexpr size_t PagesInBlock = BlockSize / CachePageSize;
        static constexpr size_t PhysicalCachePageSize = PhysicalBlockSize / (BlockSize / CachePageSize);

        /* TODO */
        inline virtual size_t GetFileLength() const {
          assert(this);
          return FileLength;
        }

        /* TODO */
        inline virtual size_t GetStartingBlock() const {
          assert(this);
          return StartingBlockId;
        }

        /* TODO */
        virtual void ReadMeta(size_t , size_t &) const {
          assert(this);
          throw std::logic_error("Deprecated");
        }

        /* TODO */
        inline virtual size_t FindPageIdOfByte(size_t offset) const {
          assert(this);
          assert(offset < FileLength);
          size_t num_blocks_into_file = offset / BlockSize;
          if (num_blocks_into_file == StartingBlockOffset) {
            return (StartingBlockId * PagesInBlock) + (offset % BlockSize) / CachePageSize;
          } else {
            auto upper = OffsetBlockByBlockId.upper_bound(num_blocks_into_file);
            --upper;
            assert(upper != OffsetBlockByBlockId.end());
            return ((upper->second + (num_blocks_into_file - upper->first)) * PagesInBlock) + (offset % BlockSize) / CachePageSize;
          }
        }

        /* TODO */
        static __thread size_t HashHitCount;

        protected:

        /* TODO */
        TReadFile(const Base::TCodeLocation &code_location,
                  uint8_t util_src,
                  Util::TCache<PhysicalCachePageSize> *cache,
                  const Base::TUuid &/*file_id*/,
                  DiskPriority priority,
                  size_t gen_id,
                  size_t starting_block_id,
                  size_t starting_block_offset,
                  size_t file_length)
            : Cache(cache),
              StartingBlockId(starting_block_id),
              StartingBlockOffset(starting_block_offset),
              FileLength(file_length),
              Priority(priority),
              GenId(gen_id),
              CodeLocation(code_location),
              UtilSrc(util_src) {
          Init();
        }

        /* TODO */
        TReadFile(const Base::TCodeLocation &code_location,
                  uint8_t util_src,
                  Util::TEngine *engine,
                  const Base::TUuid &file_id,
                  DiskPriority priority,
                  size_t gen_id)
            : Cache(engine->GetCache<PhysicalCachePageSize>()),
              Priority(priority),
              GenId(gen_id),
              CodeLocation(code_location),
              UtilSrc(util_src) {
          size_t num_keys;
          if (!engine->FindFile(file_id, gen_id, StartingBlockId, StartingBlockOffset, FileLength, num_keys)) {
            std::ostringstream ss;
            ss << file_id;
            syslog(LOG_ERR, "TReadFile() Can not find file %s[%ld]", ss.str().c_str(), gen_id);
            throw std::runtime_error("Could not find file.");
          }
          Init();
        }

        private:

        /* TODO */
        void Init() {
          assert(StartingBlockOffset * BlockSize < FileLength);
          TStream<CachePageSize, BlockSize, PhysicalBlockSize, BufKind, MaxMetaCacheSize> in_stream(CodeLocation, UtilSrc, Priority, this, Cache, StartingBlockOffset * BlockSize);
          //InStream = std::unique_ptr<TStream<CachePageSize, BlockSize, PhysicalBlockSize, BufKind, MaxMetaCacheSize>>(new TStream<CachePageSize, BlockSize, PhysicalBlockSize, BufKind, MaxMetaCacheSize>(CodeLocation, UtilSrc, Priority, this, Cache, StartingBlockOffset * BlockSize));
          /*
            # of blocks
            # of meta-blocks (n)
            # of #block / block_id pairings (number of sequential blocks starting at) (m)
            # updates
            # index segments (p)
            # of arena notes
            # of arena bytes
            # of arena type boundaries
            offset of main arena
            offset of update index
          */
          in_stream.Read(NumBlocks);
          in_stream.Read(NumMetaBlocks);
          in_stream.Read(NumSequentialBlockPairings);
          in_stream.Read(NumUpdates);
          in_stream.Read(NumIndexSegments);
          in_stream.Read(NumMainArenaNotes);
          in_stream.Read(NumMainArenaBytes);
          in_stream.Read(NumMainArenaTypeBoundaries);
          in_stream.Read(ByteOffsetOfMainArena);
          in_stream.Read(ByteOffsetOfUpdateIndex);

          /* meta block ids */
          size_t at_offset_block = NumBlocks - NumMetaBlocks;
          for (size_t i = 0; i < NumMetaBlocks; ++i) {
            size_t block_id;
            in_stream.Read(block_id);
            OffsetBlockByBlockId.insert(std::make_pair(at_offset_block, block_id));
            ++at_offset_block;
          }
          at_offset_block = 0UL;
          /* starting block id pairings */
          for (size_t i = 0; i < NumSequentialBlockPairings; ++i) {
            size_t block_id, num_blocks;
            in_stream.Read(block_id);
            in_stream.Read(num_blocks);
            OffsetBlockByBlockId.insert(std::make_pair(at_offset_block, block_id));
            at_offset_block += num_blocks;
          }
          /* index segment offsets */
          Base::TUuid index_id;
          size_t offset;
          for (size_t i = 0; i < NumIndexSegments; ++i) {
            in_stream.Read(index_id);
            in_stream.Read(offset);
            //std::cout << "Index [" << index_id << "] @ [" << offset << "]" << std::endl;
            IndexOffsetById.emplace(index_id, offset);
            std::unique_ptr<TIndexFile> idx_ptr(new TIndexFile(this, index_id, offset, Priority));
            IndexByOffset.insert(std::make_pair(offset, idx_ptr.get()));
            IndexById.emplace(index_id, std::move(idx_ptr));
          }
          for (size_t i = 0; i < NumMainArenaTypeBoundaries; ++i) {
            in_stream.Read(offset);
            MainArenaTypeBoundaryOffsetVec.emplace_back(offset);
          }
        }

        protected:

        /* TODO */
        virtual ~TReadFile() {}

        /* TODO */
        inline size_t GetNumBlocks() const {
          assert(this);
          return NumBlocks;
        }

        /* TODO */
        inline size_t GetStartingBlockOffset() const {
          assert(this);
          return StartingBlockOffset;
        }

        /* TODO */
        inline size_t GetNumMetaBlocks() const {
          assert(this);
          return NumMetaBlocks;
        }

        /* TODO */
        inline size_t GetNumSequentialBlockPairings() const {
          assert(this);
          return NumSequentialBlockPairings;
        }

        /* TODO */
        inline size_t GetByteOffsetOfArena() const {
          assert(this);
          return ByteOffsetOfMainArena;
        }

        /* TODO */
        inline size_t GetNumArenaNotes() const {
          assert(this);
          return NumMainArenaNotes;
        }

        /* TODO */
        inline size_t GetNumBytesOfArena() const {
          assert(this);
          return NumMainArenaBytes;
        }

        /* TODO */
        inline const std::vector<size_t> &GetTypeBoundaryOffsetVec() const {
          assert(this);
          return MainArenaTypeBoundaryOffsetVec;
        }

        /* TODO */
        inline size_t GetNumUpdates() const {
          assert(this);
          return NumUpdates;
        }

        /* TODO */
        inline size_t GetGenId() const {
          assert(this);
          return GenId;
        }

        /* TODO */
        inline size_t GetByteOffsetOfUpdateIndex() const {
          assert(this);
          return ByteOffsetOfUpdateIndex;
        }

        /* TODO */
        void ForEachIndex(const std::function<void (const Base::TUuid &, size_t)> &cb) const {
          for (const auto &idx : IndexOffsetById) {
            cb(idx.first, idx.second);
          }
        }

        /* TODO */
        bool FindInHash(const Base::TUuid &index_id, const TKey &key, size_t &out_offset) const {
          assert(this);
          auto ret = IndexById.find(index_id);
          if (ret != IndexById.end()) {
            return ret->second->FindInHash(key, out_offset);
          }
          return false;
        }

        public:

        /* TODO */
        class TIndexFile
            : public TArenaInFile {
          NO_COPY_SEMANTICS(TIndexFile);
          public:

          /* TODO */
          using TArena = TDiskArena<CachePageSize, BlockSize, PhysicalBlockSize, BufKind, DiskArenaMaxCacheSize, ScanAheadAllowed>;

          /* TODO */
          using TInStream = TStream<CachePageSize, BlockSize, PhysicalBlockSize, BufKind, 0UL /* local cache size */>;

          /* TODO */
          class TKeyItem {
            public:

            /* TODO */
            TSequenceNumber SeqNum;
            Atom::TCore Key;
            Atom::TCore Value;
            size_t NumHistKeys;
            size_t OffsetOfHistKeys;

          };  // TKeyItem

          /* TODO */
          class THistoryKeyItem {
            public:

            /* TODO */
            TSequenceNumber SeqNum;
            Atom::TCore Key;
            Atom::TCore Value;

          };  // THistoryKeyItem

          /* TODO */
          class TKeyCursor {
            NO_COPY_SEMANTICS(TKeyCursor);
            public:

            /* TODO */
            TKeyCursor(TIndexFile *idx_file, size_t start_num = 0UL)
                : Cur(start_num),
                  Limit(idx_file->NumCurKeys),
                  InStream(idx_file->File->CodeLocation, idx_file->File->UtilSrc, idx_file->File->Priority, idx_file, idx_file->File->Cache, idx_file->ByteOffsetOfKeyIndex + (Cur * TData::KeyEntrySize)) {
              if (Cur < Limit) {
                InStream.Read(&Item, sizeof(Item));
                assert(Item.Key.IsTuple());
              }
            }

            /* True iff. we have an item. */
            operator bool() const {
              assert(this);
              return Cur < Limit;
            }

            /* The current item. */
            const TKeyItem &operator*() const {
              assert(this);
              assert(Cur < Limit);
              return Item;
            }

            /* Walk to the next item, if any. */
            TKeyCursor &operator++() {
              assert(this);
              ++Cur;
              if (likely(Cur < Limit)) {
                InStream.Read(&Item, sizeof(Item));
                assert(Item.Key.IsTuple());
              }
              return *this;
            }

            private:

            /* TODO */
            TKeyItem Item;
            static_assert(sizeof(TKeyItem) == TData::KeyEntrySize, "TKeyItem is not the same size as a current key entry in a data file");

            /* TODO */
            size_t Cur;

            /* TODO */
            size_t Limit;

            /* TODO */
            TStream<CachePageSize, BlockSize, PhysicalBlockSize, BufKind, 0UL /* local cache size */> InStream;

          };  // TKeyCursor

          /* TODO */
          class THistoryKeyCursor {
            NO_COPY_SEMANTICS(THistoryKeyCursor);
            public:

            /* TODO */
            THistoryKeyCursor(TIndexFile *idx_file, size_t start_num = 0UL)
                : Cur(start_num),
                  Limit(idx_file->NumHistKeys),
                  InStream(idx_file->File->CodeLocation,
                           idx_file->File->UtilSrc,
                           idx_file->File->Priority,
                           idx_file,
                           idx_file->File->Cache,
                           idx_file->ByteOffsetOfKeyIndex + (idx_file->NumCurKeys * TData::KeyEntrySize) + (Cur * TData::KeyHistorySize)) {
              if (Cur < Limit) {
                InStream.Read(&Item, sizeof(Item));
                assert(Item.Key.IsTuple());
              }
            }

            /* True iff. we have an item. */
            operator bool() const {
              assert(this);
              return Cur < Limit;
            }

            /* The current item. */
            const THistoryKeyItem &operator*() const {
              assert(this);
              assert(Cur < Limit);
              return Item;
            }

            /* Walk to the next item, if any. */
            THistoryKeyCursor &operator++() {
              assert(this);
              ++Cur;
              if (Cur < Limit) {
                InStream.Read(&Item, sizeof(Item));
                assert(Item.Key.IsTuple());
              }
              return *this;
            }

            private:

            /* TODO */
            THistoryKeyItem Item;
            static_assert(sizeof(THistoryKeyItem) == TData::KeyHistorySize, "THistoryKeyItem is not the same size as a history key entry in a data file");

            /* TODO */
            size_t Cur;

            /* TODO */
            size_t Limit;

            /* TODO */
            TStream<CachePageSize, BlockSize, PhysicalBlockSize, BufKind, 0UL /* local cache size */> InStream;

          };  // THistoryKeyCursor

          TIndexFile(const TReadFile *file, const Base::TUuid &index_id, DiskPriority priority)
              : TIndexFile(file, index_id, file->IndexOffsetById.find(index_id)->second , priority) {}

          /* TODO */
          TIndexFile(const TReadFile *file, const Base::TUuid &index_id, size_t index_meta_offset, DiskPriority /*priority*/)
              : File(file),
                IndexId(index_id) {
            TStream<CachePageSize, BlockSize, PhysicalBlockSize, BufKind, 0UL /* local cache size */> in_stream(File->CodeLocation, File->UtilSrc, File->Priority, File, File->Cache, index_meta_offset);
            in_stream.Read(ArenaByteOffset);
            in_stream.Read(NumArenaNotes);
            in_stream.Read(NumArenaBytes);
            in_stream.Read(NumArenaTypeBoundaries);
            in_stream.Read(NumCurKeys);
            in_stream.Read(NumHistKeys);
            in_stream.Read(ByteOffsetOfKeyIndex);
            in_stream.Read(NumHashTables);
            assert(NumArenaBytes > 0UL);

            size_t offset, num_hash_fields;
            for (size_t i = 0; i < NumHashTables; ++i) {
              in_stream.Read(offset);
              in_stream.Read(num_hash_fields);
              //std::cout << "Hash table @ [" << offset << "] has [" << num_hash_fields <<"] entries" << std::endl;
              NumHashFieldsByOffset.emplace_back(offset, num_hash_fields);
            }
            for (size_t i = 0; i < NumArenaTypeBoundaries; ++i) {
              in_stream.Read(offset);
              ArenaTypeBoundaryByOffset.emplace_back(offset);
            }
          }

          /* TODO */
          bool FindInHash(const TKey &key, size_t &out_offset, TInStream &in_stream, TArena *file_arena) const {
            assert(this);
            assert(key.GetCore().IsTuple());
            const Atom::TCore &core = key.GetCore();
            const Atom::TCore::TOffset *const off = core.TryGetOffset();
            assert(off);
            Atom::TCore::TArena *const arena = key.GetArena();
            void *pin_alloc = alloca(sizeof(Atom::TCore::TArena::TFinalPin));
            Atom::TCore::TArena::TFinalPin::TWrapper pin(arena->Pin(*off,
                                                                    sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * *core.TryGetElemCount()),
                                                                    pin_alloc));
            const size_t num_defined = pin->GetNote()->GetTupleNumNonFree();
            if (num_defined != 0) {
              const std::pair<size_t, size_t> &idx = NumHashFieldsByOffset[num_defined - 1];
              const size_t byte_offset_of_hash_table = idx.first;
              const size_t num_hash_fields = idx.second;
              const size_t hash_to_look_for = key.GetHash();
              const size_t modded_hash = hash_to_look_for % num_hash_fields;

              void *key_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
              void *other_state_alloc = reinterpret_cast<uint8_t *>(key_state_alloc) + Sabot::State::GetMaxStateSize();
              Sabot::State::TAny::TWrapper key_state(core.NewState(arena, key_state_alloc));

              in_stream.GoTo(byte_offset_of_hash_table + (modded_hash * TData::HashEntrySize));
              Atom::TCore cur_core;
              size_t cur_hash = 0;
              for (size_t i = modded_hash; i < num_hash_fields; ++i) {
                in_stream.Read(&cur_core, sizeof(Atom::TCore));
                in_stream.Read(out_offset);
                if (memcmp(&cur_core, &TData::NullCore, sizeof(Atom::TCore)) != 0) {
                  assert(cur_core.IsTuple());
                  cur_hash = cur_core.ForceGetStoredHash();
                  if (cur_hash == hash_to_look_for) {
                    Sabot::TMatchResult res;
                    if (IsPrefixMatch((res = MatchPrefixState(*key_state, *Sabot::State::TAny::TWrapper(cur_core.NewState(file_arena, other_state_alloc)))))) {
                      return true;
                    }
                  } else if (cur_hash % num_hash_fields > modded_hash) {
                    return false;
                  }
                } else {
                  return false;
                }
              }
              in_stream.GoTo(byte_offset_of_hash_table);
              for (size_t i = 0; i < modded_hash; ++i) {
                in_stream.Read(&cur_core, sizeof(Atom::TCore));
                in_stream.Read(out_offset);
                if (memcmp(&cur_core, &TData::NullCore, sizeof(Atom::TCore)) != 0) {
                  cur_hash = cur_core.ForceGetStoredHash();
                  if (cur_hash == hash_to_look_for) {
                    Sabot::TMatchResult res;
                    if (IsPrefixMatch((res = MatchPrefixState(*key_state, *Sabot::State::TAny::TWrapper(cur_core.NewState(file_arena, other_state_alloc)))))) {
                      return true;
                    }
                  } else if (cur_hash % num_hash_fields > modded_hash) {
                    return false;
                  }
                } else {
                  return false;
                }
              }
              syslog(LOG_ERR, "TReadFile::TIndexFile::FindInHash() Should not happen, implies hash table is completely full, modded_hash = [%ld], num_hash_fields[%ld]", modded_hash, num_hash_fields);
              in_stream.GoTo(byte_offset_of_hash_table);
              for (size_t i = 0; i < num_hash_fields; ++i) {
                in_stream.Read(&cur_core, sizeof(Atom::TCore));
                in_stream.Read(out_offset);
                if (memcmp(&cur_core, &TData::NullCore, sizeof(Atom::TCore)) != 0) {
                  std::cout << "[" << i << "] = [" << Indy::TKey(cur_core, file_arena) << "] @ [" << out_offset << "]" << std::endl;
                } else {
                  std::cout << "[" << i << "] = NULL" << std::endl;
                }
              }
              throw std::logic_error("TReadFile::TIndexFile::FindInHash() Should not happen, implies hash table is completely full");
            } else {
              out_offset = ByteOffsetOfKeyIndex;
              return NumCurKeys > 0;
            }
          }

          /* TODO */
          bool BinaryLowerBoundOnKey(const TKey &key, size_t &out_offset, TInStream &in_stream, TArena *file_arena) const {
            assert(this);
            assert(&key);
            assert(&out_offset);
            assert(NumCurKeys > 0);
            size_t first = 0U;
            size_t it = 0;
            size_t step;
            int64_t count = NumCurKeys;
            Atom::TCore core;
            while (count > 0) {
              it = first;
              step = count / 2;
              it += step;
              out_offset = ByteOffsetOfKeyIndex + (it * TData::KeyEntrySize);
              in_stream.GoTo(out_offset + sizeof(TSequenceNumber));
              in_stream.Read(&core, sizeof(Atom::TCore));
              if (TKey(core, file_arena) < key) {
                first = ++it;
                count -= step + 1;
              } else {
                count = step;
              }
            }
            out_offset = ByteOffsetOfKeyIndex + (it * TData::KeyEntrySize);
            return first < NumCurKeys;
          }

          /* TODO */
          Indy::TKey GetKey(size_t n, TInStream &in_stream, TArena *file_arena) const {
            assert(n < NumCurKeys);
            Atom::TCore core;
            in_stream.GoTo(ByteOffsetOfKeyIndex + (n * TData::KeyEntrySize) + sizeof(TSequenceNumber));
            in_stream.Read(&core, sizeof(core));
            return Indy::TKey(core, file_arena);
          }

          /* TODO */
          inline const Base::TUuid &GetIndexId() const {
            assert(this);
            return IndexId;
          }

          /* TODO */
          inline virtual Atom::TCore::TOffset GetNumBytesOfArena() const override {
            assert(this);
            return NumArenaBytes;
          }

          /* TODO */
          inline const std::vector<size_t> &GetTypeBoundaryOffsetVec() const {
            assert(this);
            return ArenaTypeBoundaryByOffset;
          }

          /* TODO */
          inline size_t GetNumCurKeys() const {
            assert(this);
            return NumCurKeys;
          }

          /* TODO */
          inline size_t GetNumHistKeys() const {
            assert(this);
            return NumHistKeys;
          }

          /* TODO */
          inline size_t GetGenId() const {
            assert(this);
            return File->GenId;
          }

          /* TODO */
          inline size_t GetByteOffsetOfKeyIndex() const {
            assert(this);
            return ByteOffsetOfKeyIndex;
          }

          /* TODO */
          inline size_t GetByteOffsetOfHistoryIndex() const {
            assert(this);
            return ByteOffsetOfKeyIndex + (NumCurKeys * TData::KeyEntrySize);
          }

          /* TODO */
          inline const std::vector<std::pair<size_t, size_t>> &GetNumHashFieldsByOffset() const {
            assert(this);
            return NumHashFieldsByOffset;
          }

          private:

          /* TODO */
          inline virtual size_t GetByteOffsetOfArena() const override {
            assert(this);
            return ArenaByteOffset;
          }

          /* TODO */
          inline virtual size_t GetNumArenaNotes() const override {
            assert(this);
            return NumArenaNotes;
          }

          /* TODO */
          inline virtual size_t GetFileLength() const override {
            assert(this);
            assert(File);
            return File->GetFileLength();
          }

          /* TODO */
          inline virtual size_t GetStartingBlock() const override {
            assert(this);
            assert(File);
            return File->GetStartingBlock();
          }

          /* TODO */
          inline virtual void ReadMeta(size_t offset, size_t &out) const override {
            assert(this);
            assert(File);
            return File->ReadMeta(offset, out);
          }

          /* TODO */
          inline virtual size_t FindPageIdOfByte(size_t offset) const override {
            assert(this);
            assert(File);
            return File->FindPageIdOfByte(offset);
          }

          /* TODO */
          const TReadFile *File;

          /* TODO */
          Base::TUuid IndexId;

          /* TODO */
          size_t ArenaByteOffset;
          size_t NumArenaNotes;
          size_t NumArenaBytes;
          size_t NumArenaTypeBoundaries;
          size_t NumCurKeys;
          size_t NumHistKeys;
          size_t ByteOffsetOfKeyIndex;
          size_t NumHashTables;

          /* TODO */
          std::vector<std::pair<size_t, size_t>> NumHashFieldsByOffset;

          /* TODO */
          std::vector<size_t> ArenaTypeBoundaryByOffset;

        };  // TIndexFile

        /* TODO */
        inline const std::unordered_map<Base::TUuid, std::unique_ptr<TIndexFile>> &GetIndexByIdMap() {
          assert(this);
          return IndexById;
        }

        protected:

        /* TODO */
        //Util::TPageCache *PageCache;
        Util::TCache<PhysicalCachePageSize> *Cache;

        /* TODO */
        size_t StartingBlockId;

        /* TODO */
        size_t StartingBlockOffset;

        /* TODO */
        size_t FileLength;

        /* TODO */
        size_t NumBlocks;
        size_t NumMetaBlocks;
        size_t NumSequentialBlockPairings;
        size_t NumUpdates;
        size_t NumIndexSegments;
        size_t NumMainArenaNotes;
        size_t NumMainArenaBytes;
        size_t NumMainArenaTypeBoundaries;
        size_t ByteOffsetOfMainArena;
        size_t ByteOffsetOfUpdateIndex;

        /* TODO */
        static constexpr size_t MaxMetaCacheSize = 64;

        /* TODO */
        std::map<size_t, size_t> OffsetBlockByBlockId;

        /* TODO */
        std::unordered_map<Base::TUuid, size_t> IndexOffsetById;

        /* TODO */
        std::unordered_map<Base::TUuid, std::unique_ptr<TIndexFile>> IndexById;

        /* TODO */
        std::map<size_t, TIndexFile *> IndexByOffset;

        /* TODO */
        std::vector<size_t> MainArenaTypeBoundaryOffsetVec;

        /* TODO */
        DiskPriority Priority;

        /* TODO */
        size_t GenId;

        /* TODO */
        const Base::TCodeLocation CodeLocation;
        const uint8_t UtilSrc;

      };  // TReadFile

      /* TODO */
      template <size_t CachePageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind, bool ScanAheadAllowed = true>
      class TLocalReadFileCache {
        NO_COPY_SEMANTICS(TLocalReadFileCache);
        public:

        /* TODO */
        class TLocalReadFile
            : public TReadFile<CachePageSize, BlockSize, PhysicalBlockSize, BufKind, ScanAheadAllowed> {
          NO_COPY_SEMANTICS(TLocalReadFile);
          public:

          /* TODO */
          TLocalReadFile(Util::TEngine *engine,
                         const Base::TUuid &file_id,
                         size_t gen_id)
              : TReadFile<CachePageSize, BlockSize, PhysicalBlockSize, BufKind, ScanAheadAllowed>(HERE,
                                                                                                  Source::PresentWalk,
                                                                                                  engine,
                                                                                                  file_id,
                                                                                                  RealTime,
                                                                                                  gen_id) {}

          /* TODO */
          virtual ~TLocalReadFile() {}


        };  // TLocalReadFile

        /* TODO */
        TLocalReadFileCache() : LoaderCollection(this) {}

        /* TODO */
        TLocalReadFile *Get(Util::TEngine *engine,
                            const Base::TUuid &file_id,
                            size_t gen_id) {
          assert(this);
          TLoaderObj *loader = LoaderCollection.TryGetFirstMember(TFileKey(file_id, gen_id));
          if (!loader) {
            loader = new TLoaderObj(this, engine, file_id, gen_id);
          }
          return loader->GetFile();
        }

        /* TODO */
        void Clear(const Base::TUuid &file_id, size_t gen_id) {
          TLoaderObj *loader = LoaderCollection.TryGetFirstMember(TFileKey(file_id, gen_id));
          if (loader) {
            delete loader;
          }
        }

        /* TODO */
        static __thread TLocalReadFileCache *Cache;

        private:

        /* TODO */
        class TLoaderObj {
          NO_COPY_SEMANTICS(TLoaderObj);
          public:

          /* TODO */
          typedef InvCon::UnorderedMultimap::TMembership<TLoaderObj, TLocalReadFileCache, TFileKey> TCacheMembership;

          /* TODO */
          TLoaderObj(TLocalReadFileCache *cache, Util::TEngine *engine, const Base::TUuid &file_id, size_t gen_id) : CacheMembership(this, TFileKey(file_id, gen_id), &cache->LoaderCollection) {
            TLocalReadFile *file = new TLocalReadFile(engine, file_id, gen_id);
            File = std::unique_ptr<TLocalReadFile>(file);
            /* if frames have joined the waiting queue, schedule them */
            for (auto frame : FrameWaitingVec) {
              Fiber::TRunner::LocalRunner->Schedule(frame);
            }
            FrameWaitingVec.clear();
          }

          /* TODO */
          TLocalReadFile *GetFile() {
            assert(this);
            if (!File) {
              FrameWaitingVec.emplace_back(Fiber::TFrame::LocalFrame);
              Fiber::Wait();
            }
            return File.get();
          }

          private:

          /* TODO */
          std::unique_ptr<TLocalReadFile> File;

          /* TODO */
          std::vector<Fiber::TFrame *> FrameWaitingVec;

          /* TODO */
          typename TCacheMembership::TImpl CacheMembership;

        };  // TLoaderObj

        /* TODO */
        typedef InvCon::UnorderedMultimap::TCollection<TLocalReadFileCache, TLoaderObj, TFileKey> TLoaderCollection;

        /* TODO */
        mutable typename TLoaderCollection::TImpl LoaderCollection;

      };  // TLocalReadFileCache


      /*** Inline ***/

      template <size_t CachePageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind, size_t LocalCacheSize, bool ScanAheadAllowed>
      inline void TDiskArena<CachePageSize, BlockSize, PhysicalBlockSize, BufKind, LocalCacheSize, ScanAheadAllowed>::ReleaseNote(const Atom::TCore::TNote *note, Atom::TCore::TOffset offset, void *data1, void */*data2*/, void *data3) {
        assert(this);
        if (data1) { /* This data fit in the block, release the block. */
          Cache->Release(reinterpret_cast<typename Util::TCache<PhysicalCachePageSize>::TSlot *>(data1), reinterpret_cast<size_t>(data3));
          //File->GetService()->ReleaseBuf(reinterpret_cast<TPageCache::TObj *>(data));
        } else { /* the data did not fit in the block, free the buffer we allocated. */
          assert(offset / DataChunkSize != (offset + note->GetRawSize() + sizeof(Atom::TCore::TNote)) / DataChunkSize);
          free(const_cast<Atom::TCore::TNote *>(note));
        }
      }

      template <size_t CachePageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind, size_t LocalCacheSize, bool ScanAheadAllowed>
      inline const Atom::TCore::TNote *TDiskArena<CachePageSize, BlockSize, PhysicalBlockSize, BufKind, LocalCacheSize, ScanAheadAllowed>::TryAcquireNote(Atom::TCore::TOffset offset, void *&data1, void *&data2, void *&data3) {
        assert(this);
        const size_t note_offset = StartOffset + offset;
        assert(note_offset < File->GetFileLength());
        Stream.GoTo(note_offset);
        size_t loaded_page_id = Stream.GetLoadedPageId();
        size_t note_size;
        //if (Stream.GetOffsetInPage() <= (CachePageSize - sizeof(Atom::TCore::TNote))) {
        if (Stream.GetOffsetInChunk() <= (DataChunkSize - sizeof(Atom::TCore::TNote))) {
          note_size = sizeof(Atom::TCore::TNote) + reinterpret_cast<const Atom::TCore::TNote *>(Stream.GetData())->GetRawSize();
        } else {
          Atom::TCore::TNote *temp_note = reinterpret_cast<Atom::TCore::TNote *>(alloca(sizeof(Atom::TCore::TNote)));
          Stream.Read(temp_note, sizeof(Atom::TCore::TNote));
          note_size = sizeof(Atom::TCore::TNote) + temp_note->GetRawSize();
        }

        /* try to just point at the memory in the cache block if the note is contiguous. */
        if (note_offset / DataChunkSize == (note_offset + note_size) / DataChunkSize) {
          try {
            typename Util::TCache<PhysicalCachePageSize>::TSlot *data_slot;
            typename Util::TCache<PhysicalCachePageSize>::TSlot *const main_slot = Cache->Get(loaded_page_id, data_slot);
            data1 = main_slot;
            data2 = data_slot;
            data3 = reinterpret_cast<void *>(loaded_page_id);
            data_slot->SyncGetData(HERE, Priority, Cache, BufKind, Source::DiskArena, loaded_page_id, SyncTrigger);
            SyncTrigger.Wait();
          } catch (const Disk::TDiskFailure &err) {
            data1 = nullptr;
            throw;
          } catch (const Disk::TDiskServiceShutdown &err) {
            data1 = nullptr;
            throw;
          }
          assert(note_offset / DataChunkSize == (note_offset + note_size) / DataChunkSize);

          Atom::TCore::TNote *ret = reinterpret_cast<Atom::TCore::TNote *>(const_cast<char *>(reinterpret_cast<typename Util::TCache<PhysicalCachePageSize>::TSlot *>(data2)->KnownGetData(Cache)) + (((note_offset % CachePageSize) / DataChunkSize) * PhysicalDataChunkSize) + (note_offset % DataChunkSize));
          return ret;
        } else {
          data1 = nullptr;
          Atom::TCore::TNote *note_ptr = nullptr;
          if ((note_ptr = reinterpret_cast<Atom::TCore::TNote *>(malloc(note_size))) == 0) {
            syslog(LOG_EMERG, "bad alloc in TDiskArena::TryAcquireNote [%ld]", note_size);
            throw std::bad_alloc();
          }
          try {
            assert(note_offset + note_size <= File->GetFileLength());
            Stream.GoTo(note_offset);
            Stream.Read(note_ptr, note_size);
          } catch (...) {
            free(note_ptr);
            note_ptr = nullptr;
          }
          assert(note_offset / DataChunkSize != (note_offset + note_size) / DataChunkSize);
          return note_ptr;
        }
        throw;
      }

      template <size_t CachePageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind, size_t LocalCacheSize, bool ScanAheadAllowed>
      inline const Atom::TCore::TNote *TDiskArena<CachePageSize, BlockSize, PhysicalBlockSize, BufKind, LocalCacheSize, ScanAheadAllowed>::TryAcquireNote(Atom::TCore::TOffset offset, const size_t note_size, void *&data1, void *&data2, void *&data3) {
        assert(this);
        const size_t note_offset = StartOffset + offset;
        Stream.GoTo(note_offset);
        #ifndef NDEBUG
        size_t stream_note_size;
        if (Stream.GetOffsetInChunk() <= (DataChunkSize - sizeof(Atom::TCore::TNote))) {
          stream_note_size = sizeof(Atom::TCore::TNote) + reinterpret_cast<const Atom::TCore::TNote *>(Stream.GetData())->GetRawSize();
        } else {
          Atom::TCore::TNote *temp_note = reinterpret_cast<Atom::TCore::TNote *>(alloca(sizeof(Atom::TCore::TNote)));
          Stream.Read(temp_note, sizeof(Atom::TCore::TNote));
          stream_note_size = sizeof(Atom::TCore::TNote) + temp_note->GetRawSize();
          Stream.GoTo(note_offset);
        }
        assert(note_size <= stream_note_size); /* this means the hinted note size is larger than the one stored on disk. A smaller size is considered ok as this might be a prefix tuple. */
        #endif
        /* try to just point at the memory in the cache block if the note is contiguous. */
        if (note_offset / DataChunkSize == (note_offset + note_size) / DataChunkSize) {
          try {
            const size_t loaded_page_id = Stream.GetLoadedPageId();
            typename Util::TCache<PhysicalCachePageSize>::TSlot *data_slot;
            typename Util::TCache<PhysicalCachePageSize>::TSlot *const main_slot = Cache->Get(loaded_page_id, data_slot);
            data1 = main_slot;
            data2 = data_slot;
            data3 = reinterpret_cast<void *>(loaded_page_id);
          } catch (const Disk::TDiskFailure &err) {
            data1 = nullptr;
            throw;
          } catch (const Disk::TDiskServiceShutdown &err) {
            data1 = nullptr;
            throw;
          }
          assert(note_offset / DataChunkSize == (note_offset + note_size) / DataChunkSize);
          Atom::TCore::TNote *ret = reinterpret_cast<Atom::TCore::TNote *>(const_cast<char *>(reinterpret_cast<typename Util::TCache<PhysicalCachePageSize>::TSlot *>(data2)->KnownGetData(Cache)) + (((note_offset % CachePageSize) / DataChunkSize) * PhysicalDataChunkSize) + (note_offset % DataChunkSize));
          return ret;
        } else {
          data1 = nullptr;
          Atom::TCore::TNote *note_ptr = nullptr;
          if ((note_ptr = reinterpret_cast<Atom::TCore::TNote *>(malloc(note_size))) == 0) {
            syslog(LOG_EMERG, "bad alloc in TDiskArena::TryAcquireNote [%ld]", note_size);
            throw std::bad_alloc();
          }
          try {
            assert(note_offset + note_size <= File->GetFileLength());
            Stream.GoTo(note_offset);
            Stream.Read(note_ptr, note_size);
          } catch (...) {
            free(note_ptr);
            note_ptr = nullptr;
          }
          assert(note_offset / DataChunkSize != (note_offset + note_size) / DataChunkSize);
          return note_ptr;
        }
        throw;
      }

    }  // Disk

  }  // Indy

}  // Stig