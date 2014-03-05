/* <stig/indy/disk/in_file.h>

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

#include <base/no_copy_semantics.h>
#include <stig/atom/kit2.h>
#include <stig/atom/suprena.h>
#include <stig/indy/disk/util/engine.h>
#include <stig/indy/sequence_number.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      /* Size definition class. */
      class TData {
        NO_CONSTRUCTION(TData);
        public:

        /* TODO */
        static const size_t KeyEntrySize = sizeof(TSequenceNumber) + sizeof(Atom::TCore) + sizeof(Atom::TCore) + sizeof(size_t) + sizeof(size_t);

        /* TODO */
        static const size_t KeyHistorySize = sizeof(TSequenceNumber) + sizeof(Atom::TCore) + sizeof(Atom::TCore);

        /* TODO */
        static const size_t HashEntrySize = sizeof(Atom::TCore) + sizeof(size_t);

        /* TODO */
        static const size_t UpdateEntrySize = sizeof(TSequenceNumber) + sizeof(Atom::TCore) + sizeof(Atom::TCore) + sizeof(size_t) + sizeof(size_t);

        /* TODO */
        static const size_t UpdateKeyPtrSize = sizeof(size_t);

        /* TODO */
        static const size_t NumMetaFields = 10U;
        /*
           1.  # of blocks
           2.  # of meta-blocks (n)
           3.  # of #block / block_id pairings (number of sequential blocks starting at) (m)
           4.  # updates
           5.  # index segments (p)
           6.  # of arena notes
           7.  # of arena bytes
           8.  # of arena type boundaries
           9.  offset of main arena
           10.  offset of update index

        */

        /* TODO */
        static const size_t NumIndexMetaFields = 8U;
        /*
           Offset of Arena
           # arena notes
           # arena bytes
           # arena type boundaries
           # Current Keys
           # History Keys
           Current Key Offset
           # of hash indexes (n)

           (n) (size_t) -> (size_t) hash index offset -> num hash fields pairings
        */

        /* TODO */
        static const uint8_t NullCore[sizeof(Atom::TCore)];

        /* TODO */
        static const Atom::TCore TombstoneCore;

        /* TODO */
        static Atom::TCore GetTombstoneCore() {
          void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
          Atom::TSuprena suprena;
          return Atom::TCore(Native::TTombstone::Tombstone, &suprena, state_alloc);
        }

      };  // TData

      /* TODO */
      class TInFile {
        NO_COPY_SEMANTICS(TInFile);
        public:

        /* TODO */
        virtual size_t GetFileLength() const = 0;

        /* TODO */
        virtual size_t GetStartingBlock() const = 0;

        /* TODO */
        virtual void ReadMeta(size_t offset, size_t &out) const = 0;

        /* TODO */
        virtual size_t FindPageIdOfByte(size_t offset) const = 0;

        protected:

        /* TODO */
        TInFile() {}

        /* TODO */
        virtual ~TInFile() {}

      };  // TInFile

      /* TODO */
      template <size_t CachePageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind, size_t MaxLocalCacheSize, bool ScanAheadAllowed = true>
      class TStream {
        NO_COPY_SEMANTICS(TStream);
        public:

        /* TODO */
        static constexpr size_t DataChunkSize = Util::GetLogicalDataChunkSize<BufKind>();
        static constexpr size_t PhysicalDataChunkSize = Util::GetPhysicalDataChunkSize<BufKind>();
        static constexpr size_t PhysicalCachePageSize = PhysicalBlockSize / (BlockSize / CachePageSize);

        /* TODO */
        TStream(const Base::TCodeLocation &code_location /* DEBUG */, uint8_t util_src, DiskPriority priority, const TInFile *file, Util::TCache<PhysicalCachePageSize> *cache, size_t byte_offset/*, bool scan_ahead_allowed = true*/)
            : TStream(code_location, util_src, priority, file->GetFileLength(), file, cache, byte_offset/*, scan_ahead_allowed*/) {}

        /* TODO */
        TStream(const Base::TCodeLocation &code_location /* DEBUG */, uint8_t util_src, DiskPriority priority, size_t end_of_stream, const TInFile *file, Util::TCache<PhysicalCachePageSize> *cache, size_t byte_offset/*, bool scan_ahead_allowed = true*/)
            : File(file),
              Cache(cache),
              EndOfStream(end_of_stream),
              LoadedPageId(0UL),
              ByteOffset(byte_offset),
              MainSlot(0),
              DataSlot(0),
              BufData(nullptr),
              FetchCount(0U),
              NumSequentialFetch(0UL),
              PrefetchedTo(0UL),
              LastFetched(0UL),
              Priority(priority),
              DiskResult(Success),
              DiskErrStr(nullptr),
              CodeLocation(code_location),
              UtilSrc(util_src) {
          assert(File);
          assert(ByteOffset <= EndOfStream);
          if (ByteOffset < EndOfStream) {
            size_t page_id_of_byte_index = FindPageIdOfByte(ByteOffset);
            OffsetInPage = ByteOffset % CachePageSize;
            ChunkInPage = OffsetInPage / DataChunkSize;
            OffsetInChunk = OffsetInPage % DataChunkSize;
            FetchBuf(page_id_of_byte_index, ByteOffset);
            ++FetchCount;
          }
        }

        /* TODO */
        virtual ~TStream() {
          assert(this);
          AsyncTrigger.Wait();
          if (MaxLocalCacheSize > 0) {
            for (const auto &cache_slot : LocalBufCache) {
              Cache->Release(cache_slot.second.first, cache_slot.first);
            }
          } else if (MainSlot) {
            Cache->Release(MainSlot, LoadedPageId);
          }
        }

        /* TODO */
        void Read(size_t &out) {
          assert(this);
          assert(ByteOffset + sizeof(size_t) <= EndOfStream);
          //if (OffsetInPage <= (CachePageSize - sizeof(size_t))) {
            //out = *reinterpret_cast<const size_t *>(BufData + OffsetInPage);
          if (OffsetInChunk <= (DataChunkSize - sizeof(size_t))) {
            out = *reinterpret_cast<const size_t *>(BufData + (ChunkInPage * PhysicalDataChunkSize) + OffsetInChunk);
            ByteOffset += sizeof(size_t);
            CheckBuf();
            OffsetInPage = ByteOffset % CachePageSize;
            ChunkInPage = OffsetInPage / DataChunkSize;
            OffsetInChunk = OffsetInPage % DataChunkSize;
          } else {
            Read(&out, sizeof(size_t));
          }
        }

        /* TODO */
        void Read(int64_t &out) {
          assert(this);
          assert(ByteOffset + sizeof(int64_t) <= EndOfStream);
          //if (OffsetInPage <= (CachePageSize - sizeof(int64_t))) {
            //out = *reinterpret_cast<const int64_t *>(BufData + OffsetInPage);
          if (OffsetInChunk <= (DataChunkSize - sizeof(int64_t))) {
            out = *reinterpret_cast<const int64_t *>(BufData + (ChunkInPage * PhysicalDataChunkSize) + OffsetInChunk);
            ByteOffset += sizeof(int64_t);
            CheckBuf();
            OffsetInPage = ByteOffset % CachePageSize;
            ChunkInPage = OffsetInPage / DataChunkSize;
            OffsetInChunk = OffsetInPage % DataChunkSize;
          } else {
            Read(&out, sizeof(int64_t));
          }
        }

        /* TODO */
        void Read(uint8_t &out) {
          assert(this);
          assert(ByteOffset + sizeof(uint8_t) <= EndOfStream);
          //if (OffsetInPage <= (CachePageSize - sizeof(uint8_t))) {
            //out = *reinterpret_cast<const uint8_t *>(BufData + OffsetInPage);
          if (OffsetInChunk <= (DataChunkSize - sizeof(uint8_t))) {
            out = *reinterpret_cast<const uint8_t *>(BufData + (ChunkInPage * PhysicalDataChunkSize) + OffsetInChunk);
            ByteOffset += sizeof(uint8_t);
            CheckBuf();
            OffsetInPage = ByteOffset % CachePageSize;
            ChunkInPage = OffsetInPage / DataChunkSize;
            OffsetInChunk = OffsetInPage % DataChunkSize;
          } else {
            Read(&out, sizeof(uint8_t));
          }
        }

        /* TODO */
        void Read(uint16_t &out) {
          assert(this);
          assert(ByteOffset + sizeof(uint16_t) <= EndOfStream);
          //if (OffsetInPage <= (CachePageSize - sizeof(uint16_t))) {
            //out = *reinterpret_cast<const uint16_t *>(BufData + OffsetInPage);
          if (OffsetInChunk <= (DataChunkSize - sizeof(uint16_t))) {
            out = *reinterpret_cast<const uint16_t *>(BufData + (ChunkInPage * PhysicalDataChunkSize) + OffsetInChunk);
            ByteOffset += sizeof(uint16_t);
            CheckBuf();
            OffsetInPage = ByteOffset % CachePageSize;
            ChunkInPage = OffsetInPage / DataChunkSize;
            OffsetInChunk = OffsetInPage % DataChunkSize;
          } else {
            Read(&out, sizeof(uint16_t));
          }
        }

        /* TODO */
        void Read(uint32_t &out) {
          assert(this);
          assert(ByteOffset + sizeof(uint32_t) <= EndOfStream);
          //if (OffsetInPage <= (CachePageSize - sizeof(uint32_t))) {
            //out = *reinterpret_cast<const uint32_t *>(BufData + OffsetInPage);
          if (OffsetInChunk <= (DataChunkSize - sizeof(uint32_t))) {
            out = *reinterpret_cast<const uint32_t *>(BufData + (ChunkInPage * PhysicalDataChunkSize) + OffsetInChunk);
            ByteOffset += sizeof(uint32_t);
            CheckBuf();
            OffsetInPage = ByteOffset % CachePageSize;
            ChunkInPage = OffsetInPage / DataChunkSize;
            OffsetInChunk = OffsetInPage % DataChunkSize;
          } else {
            Read(&out, sizeof(uint32_t));
          }
        }

        /* TODO */
        void Read(bool &out) {
          assert(this);
          assert(ByteOffset + sizeof(bool) <= EndOfStream);
          //if (OffsetInPage <= (CachePageSize - sizeof(bool))) {
            //out = *reinterpret_cast<const bool *>(BufData + OffsetInPage);
          if (OffsetInChunk <= (DataChunkSize - sizeof(bool))) {
            out = *reinterpret_cast<const bool *>(BufData + (ChunkInPage * PhysicalDataChunkSize) + OffsetInChunk);
            ByteOffset += sizeof(bool);
            CheckBuf();
            OffsetInPage = ByteOffset % CachePageSize;
            ChunkInPage = OffsetInPage / DataChunkSize;
            OffsetInChunk = OffsetInPage % DataChunkSize;
          } else {
            Read(&out, sizeof(bool));
          }
        }

        /* TODO */
        void Read(Base::TUuid &out) {
          assert(this);
          assert(ByteOffset + sizeof(uuid_t) <= EndOfStream);
          //if (OffsetInPage <= (CachePageSize - sizeof(uuid_t))) {
          if (OffsetInChunk <= (DataChunkSize - sizeof(uuid_t))) {
            uuid_t id;
            //memcpy(&id, reinterpret_cast<const uuid_t *>(BufData + OffsetInPage), sizeof(uuid_t));
            memcpy(&id, reinterpret_cast<const uuid_t *>(BufData + (ChunkInPage * PhysicalDataChunkSize) + OffsetInChunk), sizeof(uuid_t));
            out = id;
            ByteOffset += sizeof(uuid_t);
            CheckBuf();
            OffsetInPage = ByteOffset % CachePageSize;
            ChunkInPage = OffsetInPage / DataChunkSize;
            OffsetInChunk = OffsetInPage % DataChunkSize;
          } else {
            uuid_t id;
            Read(&id, sizeof(uuid_t));
            out = id;
          }
        }

        /* TODO */
        void Read(void *buf, size_t len) {
          assert(this);
          assert(ByteOffset + len <= EndOfStream);
          size_t left = len;
          char *ptr = reinterpret_cast<char *>(buf);
          while (left > 0) {
            //size_t do_now = std::min(CachePageSize - OffsetInPage, left);
            //memcpy(ptr, BufData + OffsetInPage, do_now);
            size_t do_now = std::min(DataChunkSize - OffsetInChunk, left);
            memcpy(ptr, BufData + (ChunkInPage * PhysicalDataChunkSize) + OffsetInChunk, do_now);
            ByteOffset += do_now;
            CheckBuf();
            OffsetInPage = ByteOffset % CachePageSize;
            ChunkInPage = OffsetInPage / DataChunkSize;
            OffsetInChunk = OffsetInPage % DataChunkSize;
            left -= do_now;
            ptr += do_now;
          }
        }

        /* TODO */
        inline void Skip(size_t num_bytes) {
          assert(this);
          assert(ByteOffset + num_bytes <= EndOfStream);
          ByteOffset += num_bytes;
          CheckBuf();
          OffsetInPage = ByteOffset % CachePageSize;
          ChunkInPage = OffsetInPage / DataChunkSize;
          OffsetInChunk = OffsetInPage % DataChunkSize;
        }

        /* TODO */
        inline void GoTo(size_t offset) {
          assert(this);
          assert(offset <= EndOfStream);
          assert(offset < EndOfStream);
          ByteOffset = offset;
          CheckBuf();
          OffsetInPage = ByteOffset % CachePageSize;
          ChunkInPage = OffsetInPage / DataChunkSize;
          OffsetInChunk = OffsetInPage % DataChunkSize;
        }

        /* TODO */
        inline operator bool() const {
          assert(this);
          return ByteOffset < EndOfStream;
        }

        /* TODO */
        inline size_t GetOffset() const {
          assert(this);
          return ByteOffset;
        }

        /* TODO */
        size_t GetLoadedPageId() const {
          assert(this);
          return LoadedPageId;
        }

        /* TODO */
        Util::TPageCache::TSlot *GetLoadedMainSlot() const {
          assert(this);
          assert(MainSlot);
          return MainSlot;
        }

        /* TODO */
        Util::TPageCache::TSlot *GetLoadedDataSlot() const {
          assert(this);
          assert(DataSlot);
          return DataSlot;
        }

        size_t GetOffsetInChunk() const {
          assert(this);
          return OffsetInChunk;
        }
        size_t GetChunkInPage() const {
          assert(this);
          return ChunkInPage;
        }

        /* TODO */
        size_t GetFetchCount() const {
          assert(this);
          return FetchCount;
        }

        /* TODO */
        inline size_t FindPageIdOfByte(size_t offset) const {
          assert(this);
          return File->FindPageIdOfByte(offset);
        }

        /* TODO */
        const char *GetData() const {
          assert(this);
          //return (BufData + OffsetInPage);
          return (BufData + (ChunkInPage * PhysicalDataChunkSize) + OffsetInChunk);
        }

        /* TODO */
        void ReleaseBuf() {
          assert(this);
          ByteOffset = -1;
          if (MainSlot) {
            Cache->Release(MainSlot, LoadedPageId);
            MainSlot = nullptr;
            DataSlot = nullptr;
          }
        }

        private:

        /* TODO */
        void PrefetchNextPage(size_t offset) {
          ++NumSequentialFetch;
          size_t num_keep_prefetched = 0UL;
          if (NumSequentialFetch > 16) {
            /* 17 - 32 sequential reads; keep 32 ahead */
            num_keep_prefetched = 32UL;
          } else if (NumSequentialFetch > 4) {
            /* 5 - 16 sequential reads; keep 8 ahead */
            num_keep_prefetched = 8UL;
          } else if (NumSequentialFetch > 1) {
            /* 2 - 4 sequential reads; keep 4 ahead */
            num_keep_prefetched = 4UL;
          }
          if (num_keep_prefetched > 0) {
            const size_t starting_offset = std::max(PrefetchedTo, offset / CachePageSize);
            const size_t ending_offset = std::min((offset / CachePageSize) + num_keep_prefetched, (EndOfStream - 1) / CachePageSize);
            const size_t num_pages_to_prefetch = ending_offset - starting_offset;
            if (num_pages_to_prefetch >= num_keep_prefetched / 2) {
              PrefetchedTo = ending_offset;
              /* === We're using these variables to figure out how many async pages we can grab at once === */
              size_t consec_starting_page_id = -1;
              size_t consec_next_page_id = -1;
              size_t num_consec = 0UL;
              /* === We're using these variables to figure out how many async pages we can grab at once === */
              for (size_t i = starting_offset; i < ending_offset; ++i) {
                size_t cur_offset = i * CachePageSize;
                assert(cur_offset < EndOfStream);
                size_t page_id = FindPageIdOfByte(cur_offset);

                if (page_id == consec_next_page_id) {
                  /* this page logically follows the previous one... we could grab them together */
                  ++consec_next_page_id;
                  ++num_consec;
                } else {
                  if (num_consec > 0UL) {
                    Cache->AsyncMultiGet(CodeLocation, Priority, Cache, BufKind, UtilSrc, consec_starting_page_id, num_consec, true, AsyncTrigger);
                  }
                  /* this page does not follow logically, we'll have to do a separate request for this one */
                  num_consec = 0UL;
                  consec_starting_page_id = page_id;
                  consec_next_page_id = consec_starting_page_id + 1UL;
                }
              }
              if (num_consec > 0UL) {
                Cache->AsyncMultiGet(CodeLocation, Priority, Cache, BufKind, UtilSrc, consec_starting_page_id, num_consec, true, AsyncTrigger);
              }
            }
          }
        }

        /* TODO */
        inline void FetchBuf(size_t page_id, size_t offset) {
          assert(this);
          const size_t prev_loaded_page_id = LoadedPageId;
          LoadedPageId = page_id;
          LoadedByteStart = (offset / CachePageSize) * CachePageSize;
          LoadedByteEnd = LoadedByteStart + CachePageSize;
          /* if this offset_block comes sequentially after the one we were on, then we record a sequential access. */
          if (ScanAheadAllowed) {
            if ((offset / CachePageSize) == (LastFetched + 1)) {
              PrefetchNextPage(offset);
            } else {
              NumSequentialFetch = 0UL;
              PrefetchedTo = offset / CachePageSize;
            }
          }
          if (MaxLocalCacheSize > 0) {
            auto pos = LocalBufCache.find(page_id);
            if (pos != LocalBufCache.end()) {
              MainSlot = pos->second.first;
              DataSlot = pos->second.second;
              BufData = DataSlot->KnownGetData(Cache);
            } else {
              if (LocalBufCache.size() >= MaxLocalCacheSize) {
                const auto &cache_slot = LocalBufCache.begin();
                Cache->Release(cache_slot->second.first, cache_slot->first);
                LocalBufCache.erase(LocalBufCache.begin());
              }
              try {
                MainSlot = Cache->Get(page_id, DataSlot);
                BufData = DataSlot->SyncGetData(CodeLocation, Priority, Cache, BufKind, UtilSrc, page_id, SyncTrigger);
                //SyncTrigger.Wait();
                LocalBufCache.emplace(page_id, std::make_pair(MainSlot, DataSlot));
              } catch (const Disk::TDiskFailure &err) {
                MainSlot = nullptr;
                DataSlot = nullptr;
                throw;
              } catch (const Disk::TDiskServiceShutdown &err) {
                MainSlot = nullptr;
                DataSlot = nullptr;
                throw;
              }
            }
          } else {
            if (MainSlot) {
              Cache->Release(MainSlot, prev_loaded_page_id);
            }
            try {
              MainSlot = Cache->Get(page_id, DataSlot);
              BufData = DataSlot->SyncGetData(CodeLocation, Priority, Cache, BufKind, UtilSrc, page_id, SyncTrigger);
              //SyncTrigger.Wait();
            } catch (const Disk::TDiskFailure &err) {
              MainSlot = nullptr;
              DataSlot = nullptr;
              throw;
            } catch (const Disk::TDiskServiceShutdown &err) {
              MainSlot = nullptr;
              DataSlot = nullptr;
              throw;
            }
          }
          LastFetched = offset / CachePageSize;
        }

        /* TODO */
        inline void CheckBuf() {
          assert(this);
          if ((ByteOffset >= LoadedByteEnd || ByteOffset < LoadedByteStart) && ByteOffset < EndOfStream) {
            size_t page_id_of_byte_index = FindPageIdOfByte(ByteOffset);
            FetchBuf(page_id_of_byte_index, ByteOffset);
            ++FetchCount;
          }
        }

        /* TODO */
        const TInFile *File;

        /* TODO */
        Util::TCache<PhysicalCachePageSize> *Cache;

        /* TODO */
        const size_t EndOfStream;

        /* TODO */
        size_t LoadedPageId;

        /* TODO */
        size_t LoadedByteStart;
        size_t LoadedByteEnd;

        /* TODO */
        size_t ByteOffset;

        /* TODO */
        size_t OffsetInPage;
        size_t OffsetInChunk;
        size_t ChunkInPage;

        /* TODO */
        typename Util::TCache<PhysicalCachePageSize>::TSlot *MainSlot;
        typename Util::TCache<PhysicalCachePageSize>::TSlot *DataSlot;
        const char *BufData;

        /* TODO */
        std::unordered_map<size_t,
          std::pair<typename Util::TCache<PhysicalCachePageSize>::TSlot */*main_slot*/,
                    typename Util::TCache<PhysicalCachePageSize>::TSlot */*data_slot*/>> LocalBufCache; /* block_id -> buf cache slot */

        /* TODO */
        size_t FetchCount;

        /* TODO */
        size_t NumSequentialFetch;

        /* TODO */
        size_t PrefetchedTo;

        /* TODO */
        size_t LastFetched;

        /* TODO */
        TCompletionTrigger SyncTrigger;
        TCompletionTrigger AsyncTrigger;

        /* TODO */
        DiskPriority Priority;

        /* TODO */
        TDiskResult DiskResult;
        const char *DiskErrStr;

        /* TODO */
        const Base::TCodeLocation CodeLocation;
        const uint8_t UtilSrc;

      };  // TStream

    }  // Disk

  }  // Indy

}  // Stig