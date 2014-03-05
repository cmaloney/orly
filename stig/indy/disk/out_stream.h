/* <stig/indy/disk/out_stream.h>

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

#include <base/no_copy_semantics.h>
#include <base/uuid.h>
#include <inv_con/unordered_list.h>
#include <stig/indy/disk/util/volume_manager.h>
#include <stig/indy/disk/write_group.h>
#include <stig/indy/util/block_vec.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      /* TODO */
      template <size_t PageSize, size_t BlockSize>
      class TBoundaryChecker {
        NO_CONSTRUCTION(TBoundaryChecker);
        public:

        static constexpr size_t PagesInBlock = BlockSize / PageSize;
        static_assert(PagesInBlock > 1, "TBoundaryChecker requires template specialization for PagesInBlock == 1");

        /* TODO */
        static inline bool IsNewPage(const size_t byte_index, size_t &page_in_buf, const TBufBlock *buf) {
          size_t page = (byte_index / PageSize) % PagesInBlock;
          if (page != page_in_buf || !buf) {
            page_in_buf = page;
            return true;
          }
          return false;
        }

      };  // TBoundaryChecker

      /* TODO */
      template <size_t SameSize>
      class TBoundaryChecker<SameSize, SameSize> {
        NO_CONSTRUCTION(TBoundaryChecker);
        public:

        /* TODO */
        static inline bool IsNewPage(const size_t /*byte_index*/, size_t &/*page_in_buf*/, const TBufBlock */*buf*/) {
          return true;
        }
      };  // TBoundaryChecker<SameSize, SameSize>

      /* TODO */
      template <size_t PageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind>
        class TOutStream
            : public TWriteGroup {
          NO_COPY_SEMANTICS(TOutStream);
          public:

          static constexpr size_t PagesInBlock = BlockSize / PageSize;
          static constexpr size_t PhysicalPageSize = PhysicalBlockSize / PagesInBlock;

          /* TODO */
          TOutStream(const Base::TCodeLocation &code_location /* DEBUG */,
                     uint8_t util_src,
                     Util::TVolumeManager *vol_man,
                     size_t byte_offset,
                     Indy::Util::TBlockVec &block_vec,
                     std::unordered_map<size_t, std::shared_ptr<const TBufBlock>> &block_collision_map,
                     TCompletionTrigger &completion_trigger,
                     DiskPriority priority,
                     bool do_cache,
                     #ifndef NDEBUG
                     std::unordered_set<size_t> &written_to_set, /* For debugging blocks that were allocated but not written to */
                     #endif
                     const std::function<size_t (Disk::Util::TVolumeManager *)> &new_block_cb = [](Disk::Util::TVolumeManager *) -> size_t {throw; return 0U;})
              : TWriteGroup(Util::MaxOutBlockGroupSize),
                VolMan(vol_man),
                DoCache(do_cache),
                ByteIndex(byte_offset),
                BlockCollisionMap(block_collision_map),
                BlockVec(block_vec),
                Buf(0),
                BlockInBuf(0),
                PageInBuf(0UL),
                CurBlockIsCollision(false),
                CompletionTrigger(completion_trigger),
                NewBlockCb(new_block_cb),
                #ifndef NDEBUG
                WrittenToSet(written_to_set),
                #endif
                Priority(priority),
                CodeLocation(code_location),
                UtilSrc(util_src) {
          }

          /* TODO */
          virtual ~TOutStream() {
            assert(this);
            if (!CurBlockIsCollision && Buf) {
            //if (!CurBlockIsCollision && Buf && ((ByteIndex % TService::BlockSize) != 0)) {
              while (BlockVec.Size() < BlockInBuf + 1) {
                BlockVec.PushBack(NewBlockCb(VolMan));
              }
              assert(BlockInBuf < BlockVec.Size());
              size_t cur_block_id = BlockVec[BlockInBuf];
              Append(cur_block_id, Buf);
            }
            if (GetSize() > 0) {
              Flush();
            }
          }

          /* TODO */
          inline size_t GetOffset() const {
            assert(this);
            return ByteIndex;
          }

          /* TODO */
          inline virtual void Write(size_t out) {
            assert(this);
            size_t offset = ByteIndex % PageSize;
            if (PageSize - offset >= sizeof(size_t)) {
              CheckBuf();
              *reinterpret_cast<size_t *>(Buf->GetData() + (PageInBuf * PhysicalPageSize) + offset) = out;
              ByteIndex += sizeof(size_t);
            } else {
              Write(&out, sizeof(size_t));
            }
          }

          /* TODO */
          inline virtual void Write(int64_t out) {
            assert(this);
            size_t offset = ByteIndex % PageSize;
            if (PageSize - offset >= sizeof(int64_t)) {
              CheckBuf();
              *reinterpret_cast<int64_t *>(Buf->GetData() + (PageInBuf * PhysicalPageSize) + offset) = out;
              ByteIndex += sizeof(int64_t);
            } else {
              Write(&out, sizeof(int64_t));
            }
          }

          /* TODO */
          inline virtual void Write(uint8_t out) {
            assert(this);
            size_t offset = ByteIndex % PageSize;
            if (PageSize - offset >= sizeof(uint8_t)) {
              CheckBuf();
              *reinterpret_cast<uint8_t *>(Buf->GetData() + (PageInBuf * PhysicalPageSize) + offset) = out;
              ByteIndex += sizeof(uint8_t);
            } else {
              Write(&out, sizeof(uint8_t));
            }
          }

          /* TODO */
          inline virtual void Write(uint16_t out) {
            assert(this);
            size_t offset = ByteIndex % PageSize;
            if (PageSize - offset >= sizeof(uint16_t)) {
              CheckBuf();
              *reinterpret_cast<uint16_t *>(Buf->GetData() + (PageInBuf * PhysicalPageSize) + offset) = out;
              ByteIndex += sizeof(uint16_t);
            } else {
              Write(&out, sizeof(uint16_t));
            }
          }

          /* TODO */
          inline virtual void Write(uint32_t out) {
            assert(this);
            size_t offset = ByteIndex % PageSize;
            if (PageSize - offset >= sizeof(uint32_t)) {
              CheckBuf();
              *reinterpret_cast<uint32_t *>(Buf->GetData() + (PageInBuf * PhysicalPageSize) + offset) = out;
              ByteIndex += sizeof(uint32_t);
            } else {
              Write(&out, sizeof(uint32_t));
            }
          }

          /* TODO */
          inline virtual void Write(bool out) {
            assert(this);
            size_t offset = ByteIndex % PageSize;
            if (PageSize - offset >= sizeof(bool)) {
              CheckBuf();
              *reinterpret_cast<bool *>(Buf->GetData() + (PageInBuf * PhysicalPageSize) + offset) = out;
              ByteIndex += sizeof(bool);
            } else {
              Write(&out, sizeof(bool));
            }
          }

          /* TODO */
          inline virtual void Write(const Base::TUuid &out) {
            assert(this);
            Write(out.GetRaw(), sizeof(uuid_t));
          }

          /* TODO */
          inline virtual void Write(const void *buf, size_t len) {
            assert(this);
            size_t left = len;
            const char *ptr = reinterpret_cast<const char *>(buf);
            while (left > 0) {
              CheckBuf();
              size_t offset = ByteIndex % PageSize;
              size_t do_now = std::min(PageSize - offset, left);
              memcpy(Buf->GetData() + (PageInBuf * PhysicalPageSize) + offset, ptr, do_now);
              ByteIndex += do_now;
              left -= do_now;
              ptr += do_now;
            }
          }

          /* TODO */
          void MakeCurBlockCollisionBlock() {
            assert(this);
            if (!CurBlockIsCollision && Buf /* && (ByteIndex % TService::BlockSize) != 0*/) {
              CurBlockIsCollision = true;
              assert(BlockInBuf < BlockVec.Size());
              auto ret = BlockCollisionMap.insert(std::make_pair(BlockInBuf, nullptr));
              assert(ret.second);
              ret.first->second = std::shared_ptr<const TBufBlock>(new TBufBlock());
              memcpy(ret.first->second->GetData(), Buf, PhysicalBlockSize);
              delete Buf;
              Buf = ret.first->second.get();
            }
          }

          /* TODO */
          void Sync() {
            assert(this);
            if (!CurBlockIsCollision && Buf) {
              while (BlockVec.Size() < BlockInBuf + 1) {
                BlockVec.PushBack(NewBlockCb(VolMan));
              }
              assert(BlockInBuf < BlockVec.Size());
              size_t cur_block_id = BlockVec[BlockInBuf];
              TBufBlock *new_buf = new TBufBlock();
              memcpy(new_buf->GetData(), Buf, PhysicalBlockSize);
              Append(cur_block_id, Buf);
              Buf = new_buf;
            }
            if (GetSize() > 0) {
              Flush();
            }
            CompletionTrigger.Wait();
          }

          private:

          /* TODO */
          inline void CheckBuf() {
            assert(this);
            if (TBoundaryChecker<PageSize, BlockSize>::IsNewPage(ByteIndex, PageInBuf, Buf)) {
              size_t block = ByteIndex / BlockSize;
              if (block != BlockInBuf || !Buf) {
                FetchBlock(block);
              }
            }
          }

          /* TODO */
          inline void FetchBlock(size_t block) {
            assert(this);
            if (!CurBlockIsCollision && Buf) {
              while (BlockVec.Size() < BlockInBuf + 1) {
                BlockVec.PushBack(NewBlockCb(VolMan));
              }
              assert(BlockInBuf < BlockVec.Size());
              size_t cur_block_id = BlockVec[BlockInBuf];
              Append(cur_block_id, Buf);
            }
            auto ret = BlockCollisionMap.find(block);
            if (ret != BlockCollisionMap.end()) {  // this is a collision block
              Buf = ret->second.get();
              CurBlockIsCollision = true;
            } else {  // this is a new block
              Buf = new TBufBlock();
              CurBlockIsCollision = false;
            }
            BlockInBuf = block;
          }

          /* TODO */
          void Flush() {
            assert(this);
            TCompletionTrigger *completion_trigger = &CompletionTrigger;
            /* TODO: implement write groups with new volume manager. */
            for (TWriteCollection::TCursor csr(&WriteCollection); csr; ++csr) {
              #ifndef NDEBUG
              WrittenToSet.insert(csr->GetBlockId());
              #endif
              const TBufBlock *cur_buf = csr->GetBuf();
              VolMan->WriteBlock(CodeLocation, BufKind, UtilSrc, csr->GetBuf()->GetData(), csr->GetBlockId(), Priority, Disk::Util::CacheAll, CompletionTrigger, [cur_buf, completion_trigger](TDiskResult result, const char *err_str) {
                delete cur_buf;
                completion_trigger->Callback(result, err_str);
              });
            }
            TWriteGroup::Flush();
          }

          /* TODO */
          virtual size_t GetLogicalBlockSize() const override {
            assert(this);
            return Util::LogicalBlockSize;
          }

          /* TODO */
          virtual size_t GetPhysicalBlockSize() const override {
            assert(this);
            return Util::PhysicalBlockSize;
          }

          /* TODO */
          Util::TVolumeManager *VolMan;

          /* TODO */
          bool DoCache;

          /* TODO */
          size_t ByteIndex;

          /* TODO */
          std::unordered_map<size_t, std::shared_ptr<const TBufBlock>> &BlockCollisionMap;

          /* TODO */
          Indy::Util::TBlockVec &BlockVec;

          /* TODO */
          const TBufBlock *Buf;

          /* TODO */
          size_t BlockInBuf;
          size_t PageInBuf;

          /* TODO */
          bool CurBlockIsCollision;

          /* TODO */
          TCompletionTrigger &CompletionTrigger;

          /* TODO */
          const std::function<size_t (Disk::Util::TVolumeManager *)> NewBlockCb;

          #ifndef NDEBUG
          /* For debugging blocks that were allocated but not written to */
          std::unordered_set<size_t> &WrittenToSet;
          #endif

          /* TODO */
          DiskPriority Priority;

          /* DEBUG */
          const Base::TCodeLocation CodeLocation;

          /* TODO */
          uint8_t UtilSrc;

        };  // TOutStream

      /* Stream inserters for built-in types. */
      template <size_t PageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind>
      inline TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &operator<<(TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &strm, int64_t that ) { strm.Write(that); return strm; }
      template <size_t PageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind>
      inline TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &operator<<(TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &strm, uint64_t that) { strm.Write(that); return strm; }
      template <size_t PageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind>
      inline TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &operator<<(TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &strm, uint8_t that) { strm.Write(that); return strm; }
      template <size_t PageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind>
      inline TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &operator<<(TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &strm, uint16_t that) { strm.Write(that); return strm; }
      template <size_t PageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind>
      inline TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &operator<<(TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &strm, uint32_t that) { strm.Write(that); return strm; }
      template <size_t PageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind>
      inline TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &operator<<(TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &strm, bool that) { strm.Write(that); return strm; }

      /* Stream inserters for built-in types. */
      template <size_t PageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind>
      inline TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &&operator<<(TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &&strm, int64_t that ) { strm.Write(that); return std::move(strm); }
      template <size_t PageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind>
      inline TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &&operator<<(TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &&strm, uint64_t that) { strm.Write(that); return std::move(strm); }
      template <size_t PageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind>
      inline TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &&operator<<(TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &&strm, uint8_t that) { strm.Write(that); return std::move(strm); }
      template <size_t PageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind>
      inline TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &&operator<<(TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &&strm, uint16_t that) { strm.Write(that); return std::move(strm); }
      template <size_t PageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind>
      inline TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &&operator<<(TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &&strm, uint32_t that) { strm.Write(that); return std::move(strm); }
      template <size_t PageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind>
      inline TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &&operator<<(TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> &&strm, bool that) { strm.Write(that); return std::move(strm); }

    }  // Disk

  }  // Indy

}  // Stig