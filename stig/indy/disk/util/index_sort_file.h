/* <stig/indy/disk/util/index_sort_file.h>

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
#include <inv_con/ordered_list.h>
#include <inv_con/unordered_list.h>
#include <stig/indy/disk/util/engine.h>
#include <stig/indy/disk/util/snappy.h>
#include <stig/indy/util/block_vec.h>
#include <stig/indy/util/min_heap.h>
#include <stig/indy/util/sorter.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      namespace Util {

        /* TODO */
        template <typename TOwner, typename TVal, size_t MemSize, class TComparator>
        class TIndexSortFile
            : public TInFile {
          NO_COPY_SEMANTICS(TIndexSortFile);
          public:

          /* TODO */
          typedef Util::TIndexSortFile<TOwner, TVal, MemSize, TComparator> TMe;

          /* TODO */
          typedef InvCon::OrderedList::TCollection<TOwner, TMe, size_t> TOwnerCollection;

          /* TODO */
          typedef InvCon::OrderedList::TMembership<TIndexSortFile, TOwner, size_t> TOwnerMembership;

          /* TODO */
          typedef Snappy::TBlockSink<Disk::Util::LogicalCheckedBlockSize, Disk::Util::LogicalCheckedBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::CheckedBlock> TMyBlockSink;
          typedef Snappy::TStreamSource<Disk::Util::LogicalCheckedBlockSize, Disk::Util::LogicalCheckedBlockSize, Disk::Util::PhysicalBlockSize, Disk::Util::CheckedBlock> TMyStreamSource;

          class TCursor;

          /* TODO */
          class TCursor
              : public Stig::Indy::Util::TSorter<TVal, MemSize>::TCursor {
            NO_COPY_SEMANTICS(TCursor);
            public:

            /* TODO */
            TCursor(TIndexSortFile *file, size_t read_ahead_amt = IndexCursorScanAhead)
                : File(file),
                  MetaStream(HERE, File->UtilSrc, Low, File, File->BlockCache, 0UL),
                  DataStream(HERE, File->UtilSrc, Low, File, File->BlockCache, File->NumMetaBytes),
                  NumIntoPage(0U),
                  NumInCurPage(0UL),
                  BufBlock(new TBufBlock()),
                  BufData(BufBlock->GetData()) {
              if (unlikely(read_ahead_amt == 0)) {
                throw std::logic_error("IndexSortFile requires read_ahead_amt >= 1");
              }
              assert(read_ahead_amt > 0);
              assert(DataStream.GetOffset() < File->FileLength);
              Refresh();
            }

            /* TODO */
            ~TCursor() {
              assert(this);
            }

            /* TODO */
            inline virtual operator bool() const {
              assert(this);
              return NumIntoPage != NumInCurPage || DataStream.GetOffset() < File->FileLength;
            }

            /* TODO */
            inline virtual const TVal &operator*() const {
              assert(this);
              assert(BufData);
              return *(reinterpret_cast<const TVal *>(BufData) + NumIntoPage);
            }

            /* TODO */
            inline virtual TCursor &operator++() {
              assert(this);
              assert(static_cast<bool>(*this));
              ++NumIntoPage;
              if (NumIntoPage >= NumInCurPage && DataStream.GetOffset() < File->FileLength) {
                NumIntoPage = 0;
                Refresh();
              }
              return *this;
            }

            private:

            /* TODO */
            inline void Refresh() const {
              assert(this);
              size_t compressed_size = 0UL;
              MetaStream.Read(NumInCurPage);
              MetaStream.Read(compressed_size);
              const size_t start_val = DataStream.GetOffset();
              TMyStreamSource stream_source(DataStream, compressed_size);
              bool res = snappy::RawUncompress(&stream_source, BufBlock->GetData());
              if (unlikely(!res)) {
                throw std::runtime_error("Corrupted compressed data");
              }
              assert(DataStream.GetOffset() - start_val == compressed_size);
            }

            /* TODO */
            TIndexSortFile *File;

            /* TODO */
            mutable TMyStreamSource::TDataInStream MetaStream;
            mutable TMyStreamSource::TDataInStream DataStream;

            /* TODO */
            size_t NumIntoPage;
            mutable size_t NumInCurPage;

            /* TODO */
            mutable std::unique_ptr<TBufBlock> BufBlock;

            /* TODO */
            mutable const char *BufData;

            /* TODO */
            static constexpr size_t IndexCursorScanAhead = 16;

          };  // TCursor

          /* TODO */
          TIndexSortFile(const Base::TCodeLocation &code_location /* DEBUG */,
                         uint8_t util_src,
                         TVolume::TDesc::TStorageSpeed storage_speed,
                         TEngine *engine,
                         TVolumeManager *vol_man,
                         TBlockCache *block_cache,
                         size_t generation_num,
                         const Stig::Indy::Util::TSorter<TVal, MemSize> &sorter,
                         typename TOwnerCollection::TImpl &collection,
                         TCacheInstr cache_instr)
              : StorageSpeed(storage_speed),
                Engine(engine),
                VolMan(vol_man),
                BlockCache(block_cache),
                CacheInstr(cache_instr),
                OwnerMembership(this, generation_num, &collection),
                NumMetaBytes(0UL),
                FileLength(0UL),
                Size(sorter.GetSize()),
                CodeLocation(code_location),
                UtilSrc(util_src) {
            const size_t max_per_block = LogicalCheckedBlockSize / sizeof(TVal);
            const size_t num_compressed_blocks = ceil(static_cast<double>(sorter.GetSize()) / max_per_block);
            /* max blocks = max raw data + [num elem + compressed size] per block
               TODO: use smaller integers for this meta data */
            NumMetaBytes = num_compressed_blocks * 2UL * sizeof(size_t);
            const size_t max_disk_blocks_required = num_compressed_blocks + ((NumMetaBytes / Disk::Util::LogicalCheckedBlockSize));
            std::unique_ptr<TBufBlock> buf_block(new TBufBlock());
            size_t left = sorter.GetSize();
            Engine->AppendReserveBlocks(StorageSpeed, max_disk_blocks_required, BlockVec);
            #ifndef NDEBUG
            std::unordered_set<size_t> written_block_set;
            #endif
            auto iter = sorter.begin();
            std::unordered_map<size_t, std::shared_ptr<const TBufBlock>> collision_map {{(NumMetaBytes / Disk::Util::LogicalCheckedBlockSize), std::shared_ptr<const TBufBlock>(new TBufBlock())}};
            /* scope for out stream */ {
              TMyBlockSink::TDataOutStream meta_stream(HERE,
                                                       util_src,
                                                       vol_man,
                                                       0UL,
                                                       BlockVec,
                                                       collision_map,
                                                       CompletionTrigger,
                                                       Low,
                                                       true,
                                                       #ifndef NDEBUG
                                                       written_block_set,
                                                       #endif
                                                       [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
              TMyBlockSink::TDataOutStream out_stream(HERE,
                                                      util_src,
                                                      vol_man,
                                                      NumMetaBytes,
                                                      BlockVec,
                                                      collision_map,
                                                      CompletionTrigger,
                                                      Low,
                                                      true,
                                                      #ifndef NDEBUG
                                                      written_block_set,
                                                      #endif
                                                      [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
              TMyBlockSink block_sink(out_stream);
              while (left > 0) {
                const size_t do_now = std::min(left, max_per_block);
                memcpy(buf_block->GetData(), &*iter, sizeof(TVal) * do_now);
                Snappy::TBlockSource block_source(buf_block);
                const size_t compressed_size = snappy::Compress(&block_source, &block_sink);
                meta_stream << do_now << compressed_size;
                iter += do_now;
                left -= do_now;
              }
              FileLength = out_stream.GetOffset();
            }  // end out_stream scope
            /* flush collision blocks */ {
              for (auto iter : collision_map) {
                assert(iter.first < BlockVec.Size());
                #ifndef NDEBUG
                written_block_set.insert(BlockVec[iter.first]);
                #endif
                Engine->GetVolMan()->WriteBlock(HERE,
                                                Disk::Util::CheckedBlock,
                                                util_src,
                                                iter.second->GetData(),
                                                BlockVec[iter.first],
                                                Low,
                                                Disk::Util::CacheAll,
                                                CompletionTrigger);
              }
            } /* end flush collision blocks */
            /* now that we know exactly how many bytes we actually used, we can shrink the block vec to free the unused blocks. */
            const size_t end_of_stream = FileLength;
            const size_t actual_blocks_required = ((end_of_stream - 1UL) / Disk::Util::LogicalCheckedBlockSize) + 1UL;
            const size_t num_to_remove = BlockVec.Size() - actual_blocks_required;
            BlockVec.ForEachSeqRangeInRange([&](size_t block_id, size_t num_blocks) -> bool {
              engine->FreeSeqBlocks(block_id, num_blocks);
              return true;
            }, actual_blocks_required, BlockVec.Size());
            BlockVec.Trim(num_to_remove);
            assert(BlockVec.Size() == actual_blocks_required);
            CompletionTrigger.Wait();
          }

          /* TODO */
          TIndexSortFile(const Base::TCodeLocation &code_location /* DEBUG */,
                         uint8_t util_src,
                         TVolume::TDesc::TStorageSpeed storage_speed,
                         TEngine *engine,
                         TVolumeManager *vol_man,
                         TBlockCache *block_cache,
                         size_t generation_num,
                         size_t num_to_merge,
                         typename TOwnerCollection::TImpl &collection,
                         size_t total_read_ahead_slots,
                         const TComparator &comparator,
                         TCacheInstr cache_instr)
              : StorageSpeed(storage_speed),
                Engine(engine),
                VolMan(vol_man),
                BlockCache(block_cache),
                CacheInstr(cache_instr),
                OwnerMembership(this, generation_num + 1),
                NumMetaBytes(0UL),
                FileLength(0UL),
                Size(0UL),
                CodeLocation(code_location),
                UtilSrc(util_src) {
            const size_t max_per_block = LogicalCheckedBlockSize / sizeof(TVal);
            typename InvCon::OrderedList::TCollection<TOwner, TMe, size_t>::TImpl merge_list(collection.GetCollector());
            try {
              const size_t num_csr_required = num_to_merge;
              const size_t read_ahead_per_csr = std::max(1UL, total_read_ahead_slots / num_csr_required);
              std::vector<std::unique_ptr<typename Indy::Util::TSorter<TVal, MemSize>::TCursor>> csr_vec;

              for (InvCon::OrderedList::TMembership<TMe, TOwner, size_t> *membership = collection.TryGetFirstMembership(generation_num); membership && membership->GetKey() == generation_num;) {
                Size += membership->GetMember()->GetSize();
                csr_vec.push_back(std::unique_ptr<typename Indy::Util::TSorter<TVal, MemSize>::TCursor>(new typename TMe::TCursor(membership->GetMember(), read_ahead_per_csr)));
                InvCon::OrderedList::TMembership<TMe, TOwner, size_t> *to_move = membership;
                membership = membership->TryGetNextMembership();
                merge_list.Insert(to_move);
              }
              assert(csr_vec.size() == num_csr_required);
              const size_t num_compressed_blocks = ceil(static_cast<double>(Size) / max_per_block);
              /* max blocks = max raw data + [num elem + compressed size] per block
                 TODO: use smaller integers for this meta data */
              NumMetaBytes = num_compressed_blocks * 2UL * sizeof(size_t);
              const size_t max_disk_blocks_required = num_compressed_blocks + ((NumMetaBytes / Disk::Util::LogicalCheckedBlockSize));
              std::unique_ptr<TBufBlock> buf_block(new TBufBlock());
              Engine->AppendReserveBlocks(StorageSpeed, max_disk_blocks_required, BlockVec);
              #ifndef NDEBUG
              std::unordered_set<size_t> written_block_set;
              #endif
              std::unordered_map<size_t, std::shared_ptr<const TBufBlock>> collision_map {{(NumMetaBytes / Disk::Util::LogicalCheckedBlockSize), std::shared_ptr<const TBufBlock>(new TBufBlock())}};
              /* scope for out stream */ {
                TMyBlockSink::TDataOutStream meta_stream(HERE,
                                                         util_src,
                                                         vol_man,
                                                         0UL,
                                                         BlockVec,
                                                         collision_map,
                                                         CompletionTrigger,
                                                         Low,
                                                         true,
                                                         #ifndef NDEBUG
                                                         written_block_set,
                                                         #endif
                                                         [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
                TMyBlockSink::TDataOutStream out_stream(HERE,
                                                        util_src,
                                                        vol_man,
                                                        NumMetaBytes,
                                                        BlockVec,
                                                        collision_map,
                                                        CompletionTrigger,
                                                        Low,
                                                        true,
                                                        #ifndef NDEBUG
                                                        written_block_set,
                                                        #endif
                                                        [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});



                Indy::Util::TMinHeap<TVal, size_t, TComparator> min_heap(num_csr_required, comparator);
                for (size_t i = 0; i < csr_vec.size(); ++i) {
                  typename Indy::Util::TSorter<TVal, MemSize>::TCursor &csr = *csr_vec[i];
                  if (csr) {
                    min_heap.Insert(*csr, i);
                  }
                }
                std::stringstream ss;
                size_t pos = 0UL;
                TMyBlockSink block_sink(out_stream);
                size_t cur_into_block = 0UL;
                auto flush_func = [&](const size_t num_elem) {
                  Snappy::TBlockSource block_source(buf_block);
                  const size_t compressed_size = snappy::Compress(&block_source, &block_sink);
                  meta_stream << num_elem << compressed_size;
                };
                while (min_heap) {
                  const TVal &v = min_heap.Pop(pos);
                  assert(pos < num_csr_required);
                  memcpy(buf_block->GetData() + cur_into_block * sizeof(TVal), &v, sizeof(TVal));
                  ++cur_into_block;
                  assert(cur_into_block <= max_per_block);
                  if (cur_into_block == max_per_block) {
                    flush_func(cur_into_block);
                    cur_into_block = 0UL;
                  }
                  typename Indy::Util::TSorter<TVal, MemSize>::TCursor &csr = *csr_vec[pos];
                  ++csr;
                  if (csr) {
                    min_heap.Insert(*csr, pos);
                  }
                }
                if (cur_into_block > 0UL) {
                  flush_func(cur_into_block);
                }
                FileLength = out_stream.GetOffset();
              }  // end out_stream scope
              /* flush collision blocks */ {
                for (auto iter : collision_map) {
                  assert(iter.first < BlockVec.Size());
                  #ifndef NDEBUG
                  written_block_set.insert(BlockVec[iter.first]);
                  #endif
                  Engine->GetVolMan()->WriteBlock(HERE,
                                                  Disk::Util::CheckedBlock,
                                                  util_src,
                                                  iter.second->GetData(),
                                                  BlockVec[iter.first],
                                                  Low,
                                                  Disk::Util::CacheAll,
                                                  CompletionTrigger);
                }
              } /* end flush collision blocks */
              /* now that we know exactly how many bytes we actually used, we can shrink the block vec to free the unused blocks. */
              const size_t end_of_stream = FileLength;
              const size_t actual_blocks_required = ((end_of_stream - 1UL) / Disk::Util::LogicalCheckedBlockSize) + 1UL;
              const size_t num_to_remove = BlockVec.Size() - actual_blocks_required;
              BlockVec.ForEachSeqRangeInRange([&](size_t block_id, size_t num_blocks) -> bool {
                engine->FreeSeqBlocks(block_id, num_blocks);
                return true;
              }, actual_blocks_required, BlockVec.Size());
              BlockVec.Trim(num_to_remove);
              assert(BlockVec.Size() == actual_blocks_required);
              CompletionTrigger.Wait();
            } catch (...) {
              merge_list.DeleteEachMember();
              throw;
            }
            merge_list.DeleteEachMember();
            CompletionTrigger.Wait();
            OwnerMembership.Insert(&collection);
          }

          /* TODO */
          ~TIndexSortFile() {
            assert(this);
            for (const auto &iter : BlockVec.GetSeqBlockMap()) {
              Engine->FreeSeqBlocks(iter.second.first, iter.second.second);
            }
          }

          /* TODO */
          virtual size_t GetFileLength() const override {
            assert(this);
            return FileLength;
          }

          /* TODO */
          virtual size_t GetStartingBlock() const override {
            assert(this);
            assert(BlockVec.Size());
            return BlockVec.Front();
          }

          /* TODO */
          virtual void ReadMeta(size_t /*offset*/, size_t &/*out*/) const override {
            throw std::logic_error("ReadMeta is not required");
          }

          /* TODO */
          virtual size_t FindPageIdOfByte(size_t offset) const override {
            assert(offset <= GetFileLength());
            return BlockVec[offset / Disk::Util::LogicalCheckedBlockSize];
          }


          /* TODO */
          virtual size_t GetLogicalBlockSize() const {
            assert(this);
            return LogicalCheckedBlockSize;
          }

          /* TODO */
          virtual size_t GetPhysicalBlockSize() const {
            assert(this);
            return PhysicalBlockSize;
          }

          /* TODO */
          inline TEngine *GetEngine() const {
            assert(this);
            return Engine;
          }

          /* TODO */
          inline TBlockCache *GetBlockCache() const {
            assert(this);
            return BlockCache;
          }

          /* TODO */
          inline size_t GetSize() const {
            assert(this);
            return Size;
          }

          private:

          /* TODO */
          TVolume::TDesc::TStorageSpeed StorageSpeed;

          /* TODO */
          TEngine *Engine;

          /* TODO */
          TVolumeManager *VolMan;

          /* TODO */
          TBlockCache *BlockCache;

          /* TODO */
          TCacheInstr CacheInstr;

          /* TODO */
          typename TOwnerMembership::TImpl OwnerMembership;

          /* TODO */
          Indy::Util::TBlockVec BlockVec;

          /* TODO */
          size_t NumMetaBytes;

          /* TODO */
          size_t FileLength;

          /* TODO */
          size_t Size;

          /* TODO */
          TCompletionTrigger CompletionTrigger;

          /* DEBUG */
          const Base::TCodeLocation CodeLocation;

          /* TODO */
          uint8_t UtilSrc;

        };  // TIndexSortFile

      }  // Util

    }  // Disk

  }  // Indy

}  // Stig