/* <stig/indy/disk/merge_data_file.cc>

   Implements <stig/indy/disk/merge_data_file.h>.

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

#include <stig/indy/disk/merge_data_file.h>

#include <stig/indy/disk/util/hash_util.h>
#include <stig/indy/util/block_vec.h>
#include <stig/indy/util/min_heap.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::Atom;
using namespace Stig::Indy;
using namespace Stig::Indy::Disk;
using namespace Stig::Indy::Disk::Util;
using namespace Stig::Indy::Util;

/* TODO */
template <bool CanTail, bool CanTailTombstones>
class TMergeDataFileImpl {
  NO_COPY_SEMANTICS(TMergeDataFileImpl);
  public:

  template <size_t LocalCacheSize>
  using TDataInStream = TDataFile::TDataInStream<LocalCacheSize>;

  using TDataOutStream = TDataFile::TDataOutStream;

  /* TODO */
  TMergeDataFileImpl(TEngine *engine,
                     Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed,
                     const Base::TUuid &file_uuid,
                     const std::vector<size_t> &gen_vec,
                     const Base::TUuid &file_uid,
                     size_t gen_id,
                     TSequenceNumber /*release_up_to*/,
                     DiskPriority priority,
                     size_t max_block_cache_read_slots_allowed,
                     size_t temp_file_consol_thresh)
      : Engine(engine),
        StorageSpeed(storage_speed),
        UpdateIndexStorageSpeed(TVolume::TDesc::TStorageSpeed::Slow),
        SorterStorageSpeed(TVolume::TDesc::TStorageSpeed::Slow),
        Priority(priority),
        MaxBlockCacheReadSlotsAllowed(max_block_cache_read_slots_allowed),
        MainArenaByteOffset(0UL),
        NumUpdates(0UL),
        NumKeys(0UL),
        LowestSeq(0UL),
        HighestSeq(0UL),
        TempFileConsolThresh(temp_file_consol_thresh),
        UpdateCollector(HERE, Source::MergeDataFileUpdateIndex, TempFileConsolThresh, SorterStorageSpeed, Engine, true) {
    assert(!CanTailTombstones || gen_vec.size() == 1);
    try {
      for (const auto &iter : gen_vec) {
        ReadFileVec.emplace_back(new TReader(HERE, Source::MergeDataFileScan, Engine, file_uuid, iter));
      }
      std::unordered_map<Base::TUuid, std::vector<std::tuple<TReader *, size_t, size_t>>> source_index_map; /* reader, byte offset, original idx in reader vec */
      std::unordered_map<Base::TUuid, std::unique_ptr<TMergeIndexFile>> index_map;
      auto index_cb = [&source_index_map](const Base::TUuid &index_id, size_t byte_offset, TReader *reader, size_t orig_pos) -> void {
        auto ret = source_index_map.emplace(index_id, std::vector<std::tuple<TReader *, size_t, size_t>>{});
        ret.first->second.emplace_back(reader, byte_offset, orig_pos);
      };
      std::vector<std::unique_ptr<TArenaKeeperSorter>> main_arena_keeper_vec;
      /* build the source map */ {
        size_t orig_pos = 0UL;
        for (const auto &reader : ReadFileVec) {
          main_arena_keeper_vec.emplace_back(new TArenaKeeperSorter(HERE, Source::MergeDataFileTailIndex, TempFileConsolThresh, SorterStorageSpeed, Engine, true));
          std::function<void (const Base::TUuid &, size_t)> cb = std::bind(index_cb, std::placeholders::_1, std::placeholders::_2, reader.get(), orig_pos);
          reader->ForEachIndex(cb);
          ++orig_pos;
        }
      }
      std::unique_ptr<TSeqKeeperSorter> seq_keeper(CanTail ? new TSeqKeeperSorter(HERE, Source::MergeDataFileTailIndex, TempFileConsolThresh, SorterStorageSpeed, Engine, true) : nullptr);
      /* for each index, build the merge index file */ {
        for (const auto &idx : source_index_map) {
          const Base::TUuid &index_id = idx.first;
          const std::vector<std::tuple<TReader *, size_t, size_t>> &reader_vec = idx.second;
          std::vector<std::unique_ptr<typename TReader::TIndexFile>> idx_file_vec;
          std::vector<std::unique_ptr<TDataDiskArena<true>>> disk_arena_vec;
          std::vector<std::vector<size_t>> type_boundary_offset_vec;
          std::vector<std::unique_ptr<TRemapSorter>> remap_sorter_vec;
          size_t max_arena_bytes = 0UL;
          for (const auto &idx_f : reader_vec) {
            remap_sorter_vec.push_back(std::unique_ptr<TRemapSorter>(new TRemapSorter(HERE, Source::MergeDataFileRemapIndex, TempFileConsolThresh, SorterStorageSpeed, Engine, true)));
            idx_file_vec.emplace_back(new typename TReader::TIndexFile(std::get<0>(idx_f), idx.first, std::get<1>(idx_f), Priority));
            typename TReader::TIndexFile &idx_file = *idx_file_vec.back();
            disk_arena_vec.emplace_back(new TDataDiskArena<true>(&idx_file, Engine->GetCache<TDataDiskArena<true>::PhysicalCachePageSize>(), Priority));
            type_boundary_offset_vec.emplace_back(idx_file.GetTypeBoundaryOffsetVec());
            max_arena_bytes += idx_file.GetNumBytesOfArena();
          }
          assert(idx_file_vec.size());
          /* mini scope for small stream */ {
            TDataInStream<0UL> in_stream(HERE, Source::MergeDataFileRemapIndex, RealTime, idx_file_vec[0].get(), engine->GetCache<TDataInStream<0UL>::PhysicalCachePageSize>(), 0);
            index_map.emplace(index_id, std::unique_ptr<TMergeIndexFile>(new TMergeIndexFile(index_id,
                                                                                             Engine,
                                                                                             StorageSpeed,
                                                                                             gen_id,
                                                                                             Priority,
                                                                                             MaxBlockCacheReadSlotsAllowed,
                                                                                             TempFileConsolThresh,
                                                                                             &BlockVec,
                                                                                             MainArenaRemapIndex,
                                                                                             #ifndef NDEBUG
                                                                                             WrittenBlockSet,
                                                                                             #endif
                                                                                             idx_file_vec[0]->GetKey(0UL, in_stream, disk_arena_vec[0].get()))));
          }
          TMergeIndexFile &merge_idx_file = *index_map[index_id];
          assert(idx_file_vec.size() == disk_arena_vec.size());
          assert(idx_file_vec.size() == remap_sorter_vec.size());
          std::swap(merge_idx_file.SourceFileVec, idx_file_vec);
          std::swap(merge_idx_file.DiskArenaVec, disk_arena_vec);
          std::swap(merge_idx_file.ArenaRemapSorterVec, remap_sorter_vec);
          std::swap(merge_idx_file.TypeBoundaryOffsetVec, type_boundary_offset_vec);
          std::swap(merge_idx_file.MaxArenaBytes, max_arena_bytes);
          if (CanTail) {
            for (size_t i = 0; i < merge_idx_file.SourceFileVec.size(); ++i) {
              merge_idx_file.ArenaKeeperVec.emplace_back(new TArenaKeeperSorter(HERE, Source::MergeDataFileTailIndex, TempFileConsolThresh, SorterStorageSpeed, Engine, true));
              merge_idx_file.HistoryKeeperFilterVec.emplace_back(std::vector<bool>(merge_idx_file.SourceFileVec[i]->GetNumHistKeys()));
            }
          }
          for (const auto &idx_f : reader_vec) {
            merge_idx_file.OrigReadVecPos.push_back(std::get<2>(idx_f));
          }
        }
      }
      if (CanTail) { /* find out which sequence numbers we want to keep */
        /* push all the sequence numbers of current keys in the source files. We will use these later to figure out if anything in the history
           section is something we want to keep so we don't split a transaction. */
        for (const auto &idx_pair : index_map) {
          TMergeIndexFile &merge_idx_file = *idx_pair.second;
          size_t source_idx = 0UL;
          for (const auto &source_file : merge_idx_file.SourceFileVec) {
            TArenaKeeperSorter &key_arena_keeper = *(merge_idx_file.ArenaKeeperVec[source_idx]);
            TArenaKeeperSorter &main_arena_keeper = *(main_arena_keeper_vec[merge_idx_file.OrigReadVecPos[source_idx]]);
            for (typename TReader::TIndexFile::TKeyCursor key_cursor(source_file.get()); key_cursor; ++key_cursor) {
              const typename TReader::TIndexFile::TKeyItem &item = *key_cursor;
              if (!CanTailTombstones || !item.Value.IsTombstone() || item.NumHistKeys > 0) {
                /* add the sequence number */
                seq_keeper->Emplace(item.SeqNum);
                /* put the key arena offset into the index specific keeper filter */
                const Atom::TCore::TOffset *offset = item.Key.TryGetOffset();
                if (offset) {
                  key_arena_keeper.Emplace(*offset);
                }
                /* put the valuearena offset into the main arean keeper filter */
                offset = item.Value.TryGetOffset();
                if (offset) {
                  main_arena_keeper.Emplace(*offset);
                }
              }
            }
            ++source_idx;
          }
        }
        /* now for each history key, if it exists in the seq filter, also add its arena references to the arena keeper filter */
        for (const auto &idx_pair : index_map) {
          TMergeIndexFile &merge_idx_file = *idx_pair.second;
          size_t source_idx = 0UL;
          for (const auto &source_file : merge_idx_file.SourceFileVec) {
            TArenaKeeperSorter &key_arena_keeper = *(merge_idx_file.ArenaKeeperVec[source_idx]);
            TArenaKeeperSorter &main_arena_keeper = *(main_arena_keeper_vec[merge_idx_file.OrigReadVecPos[source_idx]]);
            /* build the access pattern for history sequence numbers */
            TMergeDataFileImpl::TSeqAccessSorter seq_access(HERE, Source::MergeDataFileTailIndex, TempFileConsolThresh, SorterStorageSpeed, Engine, true);
            TMergeDataFileImpl::TSeqResolvedSorter seq_resolved(HERE, Source::MergeDataFileTailIndex, TempFileConsolThresh, SorterStorageSpeed, Engine, true);
            size_t idx = 0UL;
            for (typename TReader::TIndexFile::THistoryKeyCursor history_cursor(source_file.get()); history_cursor; ++history_cursor) {
              const typename TReader::TIndexFile::THistoryKeyItem &item = *history_cursor;
              seq_access.Emplace(++idx, item.SeqNum);
            }
            TMergeDataFileImpl::ResolveSeqFilter(MaxBlockCacheReadSlotsAllowed, seq_access, *seq_keeper, seq_resolved);

            assert(source_idx < merge_idx_file.HistoryKeeperFilterVec.size());
            std::vector<bool> &hist_filter_vec = merge_idx_file.HistoryKeeperFilterVec[source_idx];
            typename TMergeDataFileImpl::TSeqResolvedSorter::TCursor hist_filter_csr(&seq_resolved, MaxBlockCacheReadSlotsAllowed);
            size_t cur_hist_offset = 0UL;
            for (typename TReader::TIndexFile::THistoryKeyCursor history_cursor(source_file.get()); history_cursor; ++history_cursor, ++hist_filter_csr, ++cur_hist_offset) {
              assert(cur_hist_offset < hist_filter_vec.size());
              assert(hist_filter_csr);
              const typename TReader::TIndexFile::THistoryKeyItem &item = *history_cursor;
              assert(hist_filter_csr->OldKey == item.SeqNum);
              /* if this history key is part of a transaction we want to keep, then add its key / value arena offsets to the arena keeper filters */
              if (hist_filter_csr->NewKey) {
                hist_filter_vec[cur_hist_offset] = true;
                const Atom::TCore::TOffset *offset = item.Key.TryGetOffset();
                if (offset) {
                  key_arena_keeper.Emplace(*offset);
                }
                /* put the valuearena offset into the main arean keeper filter */
                offset = item.Value.TryGetOffset();
                if (offset) {
                  main_arena_keeper.Emplace(*offset);
                }
              } else {
                hist_filter_vec[cur_hist_offset] = false;
                assert(!hist_filter_vec[cur_hist_offset]);
              }
            }
            ++source_idx;
          }
        }
      }
      /* now merge each arena */ {
        for (auto &idx_pair : index_map) {
          TMergeIndexFile &merge_idx_file = *idx_pair.second;
          try {
            merge_idx_file.ConstructArena(merge_idx_file.TypeBoundaryOffsetVec,
                                          merge_idx_file.MaxArenaBytes);
          } catch (const std::exception &ex) {
            syslog(LOG_ERR, "MergeDataFile caught error [%s]", ex.what());

            throw;
          }
        }
      }
      size_t num_main_arena_notes = 0UL;
      size_t num_main_arena_bytes = 0UL;
      std::vector<std::unique_ptr<TRemapSorter>> main_remap_sorter_vec;
      try {
        /* merge the main arenas */ {
          std::vector<std::unique_ptr<TDataDiskArena<true>>> disk_arena_vec;
          std::vector<std::vector<size_t>> type_boundary_offset_vec;
          size_t max_arena_bytes = 0UL;
          for (const auto &reader : ReadFileVec) {
            main_remap_sorter_vec.push_back(std::unique_ptr<TRemapSorter>(new TRemapSorter(HERE, Source::MergeDataFileRemapIndex, TempFileConsolThresh, SorterStorageSpeed, Engine, true)));
            disk_arena_vec.emplace_back(new typename TReader::TArena(reader.get(), Engine->GetCache<TReader::PhysicalCachePageSize>(), Low));
            type_boundary_offset_vec.emplace_back(reader->GetTypeBoundaryOffsetVec());
            max_arena_bytes += reader->GetNumBytesOfArena();
          }
          if (max_arena_bytes) {
            MainArenaByteOffset = MakeArena(Engine,
                                            StorageSpeed,
                                            SorterStorageSpeed,
                                            Priority,
                                            MaxBlockCacheReadSlotsAllowed,
                                            TempFileConsolThresh,
                                            disk_arena_vec,
                                            type_boundary_offset_vec,
                                            MainArenaTypeBoundaryOffsetVec,
                                            main_remap_sorter_vec,
                                            main_arena_keeper_vec,
                                            BlockVec,
                                            #ifndef NDEBUG
                                            WrittenBlockSet,
                                            #endif
                                            max_arena_bytes,
                                            num_main_arena_notes,
                                            num_main_arena_bytes);
          }
        }
      } catch (const std::exception &ex) {
        std::cerr << "Main Arena error " << ex.what() << std::endl;
        throw;
      } catch (...) {
        std::cerr << "Main Arena error" << std::endl;
        throw;
      }
      /* for each index, merge the keys, write out the hashes */ {
        for (const auto &idx : index_map) {
          TMergeIndexFile &idx_file = *idx.second;
          size_t max_key_count = 0UL;
          const std::vector<std::unique_ptr<TDataDiskArena<true>>> &disk_arena_vec = idx_file.DiskArenaVec;
          const std::vector<std::unique_ptr<typename TReader::TIndexFile>> &source_file_vec = idx_file.SourceFileVec;
          assert(disk_arena_vec.size() == source_file_vec.size());
          std::vector<std::unique_ptr<typename TReader::TIndexFile::TKeyCursor>> cur_key_cursor_vec;
          std::vector<std::unique_ptr<typename TReader::TIndexFile::THistoryKeyCursor>> hist_key_cursor_vec;
          std::vector<size_t> history_key_cur_idx_vec(source_file_vec.size(), 0UL);
          Stig::Indy::Util::TMinHeap<TSortedKey, size_t> min_heap(source_file_vec.size() * 2);
          std::vector<TSortedKey> sorted_key_vec(source_file_vec.size() * 2);

          std::vector<std::unique_ptr<TMergeDataFileImpl::TRemapAccessSorter>> key_access_sorter_vec;
          std::vector<std::unique_ptr<TMergeDataFileImpl::TRemapResolvedSorter>> key_resolved_sorter_vec;
          std::vector<std::unique_ptr<typename TMergeDataFileImpl::TRemapResolvedSorter::TCursor>> key_resolved_sorter_cursor_vec;
          std::vector<std::unique_ptr<TMergeDataFileImpl::TRemapAccessSorter>> val_access_sorter_vec;
          std::vector<std::unique_ptr<TMergeDataFileImpl::TRemapResolvedSorter>> val_resolved_sorter_vec;
          std::vector<std::unique_ptr<typename TMergeDataFileImpl::TRemapResolvedSorter::TCursor>> val_resolved_sorter_cursor_vec;
          std::vector<size_t> read_file_vec_pos_by_source_pos;
          for (const auto &source_file : source_file_vec) {
            for (size_t i = 0; i < ReadFileVec.size(); ++i) {
              if (source_file->GetGenId() == ReadFileVec[i]->GetGenId()) {
                read_file_vec_pos_by_source_pos.emplace_back(i);
                break;
              }
            }
          }
          size_t pos = 0;
          /* build the key access pattern */ {
            size_t max_block_cache_read_slot_per_sub_cursor = MaxBlockCacheReadSlotsAllowed / (source_file_vec.size() * 4UL);
            for (const auto &source_file : source_file_vec) {
              /* current keys */ {
                key_access_sorter_vec.push_back(std::unique_ptr<TMergeDataFileImpl::TRemapAccessSorter>(new TMergeDataFileImpl::TRemapAccessSorter(HERE, Source::MergeDataFileScan, TempFileConsolThresh, SorterStorageSpeed, Engine, true)));
                key_resolved_sorter_vec.push_back(std::unique_ptr<TMergeDataFileImpl::TRemapResolvedSorter>(new TMergeDataFileImpl::TRemapResolvedSorter(HERE, Source::MergeDataFileScan, TempFileConsolThresh, SorterStorageSpeed, Engine, true)));
                val_access_sorter_vec.push_back(std::unique_ptr<TMergeDataFileImpl::TRemapAccessSorter>(new TMergeDataFileImpl::TRemapAccessSorter(HERE, Source::MergeDataFileScan, TempFileConsolThresh, SorterStorageSpeed, Engine, true)));
                val_resolved_sorter_vec.push_back(std::unique_ptr<TMergeDataFileImpl::TRemapResolvedSorter>(new TMergeDataFileImpl::TRemapResolvedSorter(HERE, Source::MergeDataFileScan, TempFileConsolThresh, SorterStorageSpeed, Engine, true)));
                TMergeDataFileImpl::TRemapAccessSorter &key_access_sorter = *key_access_sorter_vec.back();
                TMergeDataFileImpl::TRemapResolvedSorter &key_resolved_sorter = *key_resolved_sorter_vec.back();
                TMergeDataFileImpl::TRemapSorter &key_remap_sorter = *idx_file.ArenaRemapSorterVec[pos / 2];
                TMergeDataFileImpl::TRemapAccessSorter &val_access_sorter = *val_access_sorter_vec.back();
                TMergeDataFileImpl::TRemapResolvedSorter &val_resolved_sorter = *val_resolved_sorter_vec.back();
                TMergeDataFileImpl::TRemapSorter &val_remap_sorter = *main_remap_sorter_vec[read_file_vec_pos_by_source_pos[pos / 2]];
                size_t idx = 0UL;
                if (CanTail) {
                  for (typename TReader::TIndexFile::TKeyCursor key_cursor(source_file.get()); key_cursor; ++key_cursor) {
                    const typename TReader::TIndexFile::TKeyItem &item = *key_cursor;
                    if (!CanTailTombstones || !item.Value.IsTombstone() || item.NumHistKeys > 0) {
                      const Atom::TCore::TOffset *key_off = item.Key.TryGetOffset();
                      if (key_off) {
                        key_access_sorter.Emplace(++idx, *key_off);
                      }
                      const Atom::TCore::TOffset *val_off = item.Value.TryGetOffset();
                      if (val_off) {
                        val_access_sorter.Emplace(++idx, *val_off);
                      }
                    }
                  }
                } else {
                  for (typename TReader::TIndexFile::TKeyCursor key_cursor(source_file.get()); key_cursor; ++key_cursor) {
                    const typename TReader::TIndexFile::TKeyItem &item = *key_cursor;
                    const Atom::TCore::TOffset *key_off = item.Key.TryGetOffset();
                    if (key_off) {
                      key_access_sorter.Emplace(++idx, *key_off);
                    }
                    const Atom::TCore::TOffset *val_off = item.Value.TryGetOffset();
                    if (val_off) {
                      val_access_sorter.Emplace(++idx, *val_off);
                    }
                  }
                }
                TMergeDataFileImpl::ResolveRemap(MaxBlockCacheReadSlotsAllowed, key_access_sorter, key_remap_sorter, key_resolved_sorter);
                TMergeDataFileImpl::ResolveRemap(MaxBlockCacheReadSlotsAllowed, val_access_sorter, val_remap_sorter, val_resolved_sorter);
                key_access_sorter_vec.back().reset(); /* reset the access sorter so we release the resources. */
                val_access_sorter_vec.back().reset(); /* reset the access sorter so we release the resources. */
                key_resolved_sorter_cursor_vec.push_back(std::unique_ptr<typename TMergeDataFileImpl::TRemapResolvedSorter::TCursor>(new typename TMergeDataFileImpl::TRemapResolvedSorter::TCursor(&key_resolved_sorter, max_block_cache_read_slot_per_sub_cursor)));
                val_resolved_sorter_cursor_vec.push_back(std::unique_ptr<typename TMergeDataFileImpl::TRemapResolvedSorter::TCursor>(new typename TMergeDataFileImpl::TRemapResolvedSorter::TCursor(&val_resolved_sorter, max_block_cache_read_slot_per_sub_cursor)));
              } /* finish access pattern for current keys */

              ++pos;

              /* history keys */ {
                key_access_sorter_vec.push_back(std::unique_ptr<TMergeDataFileImpl::TRemapAccessSorter>(new TMergeDataFileImpl::TRemapAccessSorter(HERE, Source::MergeDataFileScan, TempFileConsolThresh, SorterStorageSpeed, Engine, true)));
                key_resolved_sorter_vec.push_back(std::unique_ptr<TMergeDataFileImpl::TRemapResolvedSorter>(new TMergeDataFileImpl::TRemapResolvedSorter(HERE, Source::MergeDataFileScan, TempFileConsolThresh, SorterStorageSpeed, Engine, true)));
                val_access_sorter_vec.push_back(std::unique_ptr<TMergeDataFileImpl::TRemapAccessSorter>(new TMergeDataFileImpl::TRemapAccessSorter(HERE, Source::MergeDataFileScan, TempFileConsolThresh, SorterStorageSpeed, Engine, true)));
                val_resolved_sorter_vec.push_back(std::unique_ptr<TMergeDataFileImpl::TRemapResolvedSorter>(new TMergeDataFileImpl::TRemapResolvedSorter(HERE, Source::MergeDataFileScan, TempFileConsolThresh, SorterStorageSpeed, Engine, true)));
                TMergeDataFileImpl::TRemapAccessSorter &key_access_sorter = *key_access_sorter_vec.back();
                TMergeDataFileImpl::TRemapResolvedSorter &key_resolved_sorter = *key_resolved_sorter_vec.back();
                TMergeDataFileImpl::TRemapSorter &key_remap_sorter = *idx_file.ArenaRemapSorterVec[pos / 2];
                TMergeDataFileImpl::TRemapAccessSorter &val_access_sorter = *val_access_sorter_vec.back();
                TMergeDataFileImpl::TRemapResolvedSorter &val_resolved_sorter = *val_resolved_sorter_vec.back();
                TMergeDataFileImpl::TRemapSorter &val_remap_sorter = *main_remap_sorter_vec[read_file_vec_pos_by_source_pos[pos / 2]];
                size_t idx = 0UL;
                if (CanTail) {
                  std::vector<bool> &hist_filter_vec = idx_file.HistoryKeeperFilterVec[pos / 2];
                  assert(hist_filter_vec.size() == source_file->GetNumHistKeys());
                  size_t cur_hist_offset = 0UL;
                  for (typename TReader::TIndexFile::THistoryKeyCursor history_cursor(source_file.get()); history_cursor; ++history_cursor, ++cur_hist_offset) {
                    assert(cur_hist_offset < hist_filter_vec.size());
                    if (hist_filter_vec[cur_hist_offset]) {
                      const typename TReader::TIndexFile::THistoryKeyItem &item = *history_cursor;
                      const Atom::TCore::TOffset *key_off = item.Key.TryGetOffset();
                      if (key_off) {
                        key_access_sorter.Emplace(++idx, *key_off);
                      }
                      const Atom::TCore::TOffset *val_off = item.Value.TryGetOffset();
                      if (val_off) {
                        val_access_sorter.Emplace(++idx, *val_off);
                      }
                    }
                  }
                } else {
                  for (typename TReader::TIndexFile::THistoryKeyCursor history_cursor(source_file.get()); history_cursor; ++history_cursor) {
                    const typename TReader::TIndexFile::THistoryKeyItem &item = *history_cursor;
                    const Atom::TCore::TOffset *key_off = item.Key.TryGetOffset();
                    if (key_off) {
                      key_access_sorter.Emplace(++idx, *key_off);
                    }
                    const Atom::TCore::TOffset *val_off = item.Value.TryGetOffset();
                    if (val_off) {
                      val_access_sorter.Emplace(++idx, *val_off);
                    }
                  }
                }
                TMergeDataFileImpl::ResolveRemap(MaxBlockCacheReadSlotsAllowed, key_access_sorter, key_remap_sorter, key_resolved_sorter);
                TMergeDataFileImpl::ResolveRemap(MaxBlockCacheReadSlotsAllowed, val_access_sorter, val_remap_sorter, val_resolved_sorter);
                key_access_sorter_vec.back().reset(); /* reset the access sorter so we release the resources. */
                val_access_sorter_vec.back().reset(); /* reset the access sorter so we release the resources. */
                key_resolved_sorter_cursor_vec.push_back(std::unique_ptr<typename TMergeDataFileImpl::TRemapResolvedSorter::TCursor>(new typename TMergeDataFileImpl::TRemapResolvedSorter::TCursor(&key_resolved_sorter, max_block_cache_read_slot_per_sub_cursor)));
                val_resolved_sorter_cursor_vec.push_back(std::unique_ptr<typename TMergeDataFileImpl::TRemapResolvedSorter::TCursor>(new typename TMergeDataFileImpl::TRemapResolvedSorter::TCursor(&val_resolved_sorter, max_block_cache_read_slot_per_sub_cursor)));
              } /* finish access pattern for history keys */

              ++pos;
            }
          }
          std::swap(idx_file.ResolvedKeySorterCursorVec, key_resolved_sorter_cursor_vec);
          std::swap(idx_file.ResolvedValSorterCursorVec, val_resolved_sorter_cursor_vec);

          pos = 0;
          for (const auto &source_file : source_file_vec) {
            max_key_count += source_file->GetNumCurKeys();
            max_key_count += source_file->GetNumHistKeys();
            cur_key_cursor_vec.emplace_back(new typename TReader::TIndexFile::TKeyCursor(source_file.get()));
            hist_key_cursor_vec.emplace_back(new typename TReader::TIndexFile::THistoryKeyCursor(source_file.get()));
            typename TReader::TIndexFile::TKeyCursor &cur_csr = *cur_key_cursor_vec.back();
            if (CanTail) {
              for (; cur_csr; ++cur_csr) {
                const typename TReader::TIndexFile::TKeyItem &item = *cur_csr;
                if (!CanTailTombstones || !item.Value.IsTombstone() || item.NumHistKeys > 0) {
                  TSortedKey &k = sorted_key_vec[pos];
                  k.Core = item.Key;
                  k.Arena = disk_arena_vec[pos / 2].get();
                  k.SeqNum = item.SeqNum;
                  min_heap.Insert(k, pos);
                  break;
                }
              }
            } else {
              if (cur_csr) {
                const typename TReader::TIndexFile::TKeyItem &item = *cur_csr;
                TSortedKey &k = sorted_key_vec[pos];
                k.Core = item.Key;
                k.Arena = disk_arena_vec[pos / 2].get();
                k.SeqNum = item.SeqNum;
                min_heap.Insert(k, pos);
              }
            }
            ++pos;
            typename TReader::TIndexFile::THistoryKeyCursor &hist_csr = *hist_key_cursor_vec.back();
            if (CanTail) {
              size_t &cur_hist_offset = history_key_cur_idx_vec[pos / 2];
              std::vector<bool> &hist_filter_vec = idx_file.HistoryKeeperFilterVec[pos / 2];
              for (; hist_csr; ++hist_csr, ++cur_hist_offset) {
                assert(cur_hist_offset < hist_filter_vec.size());
                if (hist_filter_vec[cur_hist_offset]) {
                  const typename TReader::TIndexFile::THistoryKeyItem &item = *hist_csr;
                  TSortedKey &k = sorted_key_vec[pos];
                  k.Core = item.Key;
                  k.Arena = disk_arena_vec[pos / 2].get();
                  k.SeqNum = item.SeqNum;
                  min_heap.Insert(k, pos);
                  ++cur_hist_offset;
                  break;
                }
              }
            } else {
              if (hist_csr) {
                const typename TReader::TIndexFile::THistoryKeyItem &item = *hist_csr;
                TSortedKey &k = sorted_key_vec[pos];
                k.Core = item.Key;
                k.Arena = disk_arena_vec[pos / 2].get();
                k.SeqNum = item.SeqNum;
                min_heap.Insert(k, pos);
              }
            }
            ++pos;
          }
          idx_file.PrepKeyRange(max_key_count, &UpdateCollector);
          Base::TOpt<TKey> last_written;
          while (min_heap) {
            size_t pos;
            const TSortedKey &k = min_heap.Pop(pos);
            if (pos % 2 == 0) { /* came from a current source */
              typename TReader::TIndexFile::TKeyCursor &cur_csr = *cur_key_cursor_vec[pos / 2];
              assert(cur_csr);
              const typename TReader::TIndexFile::TKeyItem &cur_item = *cur_csr;
              TKey cur_key(cur_item.Key, k.Arena);
              if (!last_written || *last_written != cur_key) { /* new key */
                idx_file.MyPos = pos;
                Atom::TCore key_core = cur_item.Key;
                Atom::TCore val_core = cur_item.Value;
                key_core.Remap(idx_file.KeyRemapper);
                val_core.Remap(idx_file.ValRemapper);
                idx_file.PushCurKey(cur_item.SeqNum, key_core, val_core);
                last_written = cur_key;
              } else { /* history key */
                idx_file.MyPos = pos;
                Atom::TCore key_core = cur_item.Key;
                Atom::TCore val_core = cur_item.Value;
                key_core.Remap(idx_file.KeyRemapper);
                val_core.Remap(idx_file.ValRemapper);
                idx_file.PushHistKey(cur_item.SeqNum, key_core, val_core);
              }
              ++cur_csr;
              if (CanTail) {
                for (; cur_csr; ++cur_csr) {
                  const typename TReader::TIndexFile::TKeyItem &item = *cur_csr;
                  if (!CanTailTombstones || !item.Value.IsTombstone() || item.NumHistKeys > 0) {
                    TSortedKey &k = sorted_key_vec[pos];
                    k.Core = item.Key;
                    /* the arena should already be correct k.Arena = ... */
                    k.SeqNum = item.SeqNum;
                    min_heap.Insert(k, pos);
                    break;
                  }
                }
              } else {
                if (cur_csr) {
                  const typename TReader::TIndexFile::TKeyItem &item = *cur_csr;
                  TSortedKey &k = sorted_key_vec[pos];
                  k.Core = item.Key;
                  /* the arena should already be correct k.Arena = ... */
                  k.SeqNum = item.SeqNum;
                  min_heap.Insert(k, pos);
                }
              }
            } else { /* came from a history source */
              typename TReader::TIndexFile::THistoryKeyCursor &hist_csr = *hist_key_cursor_vec[pos / 2];
              assert(hist_csr);
              const typename TReader::TIndexFile::THistoryKeyItem &cur_item = *hist_csr;
              idx_file.MyPos = pos;
              Atom::TCore key_core = cur_item.Key;
              Atom::TCore val_core = cur_item.Value;
              key_core.Remap(idx_file.KeyRemapper);
              val_core.Remap(idx_file.ValRemapper);
              idx_file.PushHistKey(cur_item.SeqNum, key_core, val_core);
              ++hist_csr;
              if (CanTail) {
                size_t &cur_hist_offset = history_key_cur_idx_vec[pos / 2];
                std::vector<bool> &hist_filter_vec = idx_file.HistoryKeeperFilterVec[pos / 2];
                for (; hist_csr; ++hist_csr, ++cur_hist_offset) {
                  if (hist_filter_vec[cur_hist_offset]) {
                    const typename TReader::TIndexFile::THistoryKeyItem &item = *hist_csr;
                    TSortedKey &k = sorted_key_vec[pos];
                    k.Core = item.Key;
                    /* the arena should already be correct k.Arena = ... */
                    k.SeqNum = item.SeqNum;
                    min_heap.Insert(k, pos);
                    ++cur_hist_offset;
                    break;
                  }
                }
              } else {
                if (hist_csr) {
                  const typename TReader::TIndexFile::THistoryKeyItem &item = *hist_csr;
                  TSortedKey &k = sorted_key_vec[pos];
                  k.Core = item.Key;
                  /* the arena should already be correct k.Arena = ... */
                  k.SeqNum = item.SeqNum;
                  min_heap.Insert(k, pos);
                }
              }
            }
          } /* end of min-heap */
          idx_file.FlushHistory();
          /* now that we've finished writing the current + history keys, we can remove the unused blocks. */
          const size_t end_of_stream = idx_file.EndOfHistoryStream;
          const size_t actual_blocks_required = ((end_of_stream - 1UL) / Disk::Util::LogicalBlockSize) + 1UL;
          const size_t num_to_remove = BlockVec.Size() - actual_blocks_required;
          BlockVec.ForEachSeqRangeInRange([&](size_t block_id, size_t num_blocks) -> bool {
            Engine->FreeSeqBlocks(block_id, num_blocks);
            return true;
          }, actual_blocks_required, BlockVec.Size());
          BlockVec.Trim(num_to_remove);
          assert(BlockVec.Size() == actual_blocks_required);
          idx_file.FlushHashes();
        }
      }
      /* calculate how many updates we have */ {
        for (const auto &read_file : ReadFileVec) {
          /* we should be more clever about this. for now we're just grabbing all of them. */
          NumUpdates += read_file->GetNumUpdates();
        }
      }
      TCompletionTrigger completion_trigger;
      const size_t num_bytes_required_for_update_idx = (NumUpdates * TData::UpdateEntrySize) + (UpdateCollector.GetSize() * TData::UpdateKeyPtrSize);
      const size_t byte_offset_of_update_entries = BlockVec.Size() * LogicalBlockSize;
      const size_t byte_offset_of_bucket_entries = byte_offset_of_update_entries + (NumUpdates * TData::UpdateEntrySize);
      /* write out the update index */ {
        size_t blocks_required = ceil(static_cast<double>(num_bytes_required_for_update_idx) / LogicalBlockSize);
        const size_t total_blocks_required = BlockVec.Size() + blocks_required;
        Engine->AppendReserveBlocks(UpdateIndexStorageSpeed, blocks_required, BlockVec);
        assert(BlockVec.Size() == total_blocks_required);
        std::unordered_map<size_t, std::shared_ptr<const TBufBlock>> collision_map {};
        auto ret = collision_map.insert(std::make_pair(byte_offset_of_bucket_entries / LogicalBlockSize, nullptr));
        if (ret.second) { // fresh insert
          ret.first->second = std::shared_ptr<const TBufBlock>(new TBufBlock());
        }
        /* update stream life-time */ {
          TMergeDataFileImpl::TDataOutStream seq_stream(HERE,
                                                    Source::MergeDataFileUpdate,
                                                    Engine->GetVolMan(),
                                                    byte_offset_of_update_entries,
                                                    BlockVec,
                                                    collision_map,
                                                    completion_trigger,
                                                    Priority,
                                                    true /* do_cache */,
                                                    #ifndef NDEBUG
                                                    WrittenBlockSet,
                                                    #endif
                                                    [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
          TMergeDataFileImpl::TDataOutStream ptr_stream(HERE,
                                                    Source::MergeDataFileUpdate,
                                                    Engine->GetVolMan(),
                                                    byte_offset_of_bucket_entries,
                                                    BlockVec,
                                                    collision_map,
                                                    completion_trigger,
                                                    Priority,
                                                    true /* do_cache */,
                                                    #ifndef NDEBUG
                                                    WrittenBlockSet,
                                                    #endif
                                                    [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
          typename TUpdateCollector::TCursor update_csr(&UpdateCollector, MaxBlockCacheReadSlotsAllowed);
          assert(update_csr);
          TSequenceNumber cur_seq_num = (*update_csr).SequenceNumber;
          Atom::TCore cur_meta, cur_id;
          size_t bucket_ptr = byte_offset_of_bucket_entries;
          size_t num_accum = 0U;
          LowestSeq = (*update_csr).SequenceNumber;
          while (update_csr) {
            const TUpdateObj &obj = *update_csr;
            if (obj.SequenceNumber != cur_seq_num) {
              seq_stream << cur_seq_num;  // Sequence Number
              seq_stream.Write(&cur_meta, sizeof(cur_meta));  // Metadata
              seq_stream.Write(&cur_id, sizeof(cur_id));  // Id
              seq_stream << bucket_ptr;  // Byte offset of bucket
              seq_stream << num_accum;  // number of key ptrs in bucket
              cur_seq_num = obj.SequenceNumber;
              /* TODO: merge in cur_meta and cur_id */
              #if 0
              cur_meta = obj.Metadata;
              cur_id = obj.Id;
              #endif
              bucket_ptr += num_accum * TData::UpdateKeyPtrSize;
              num_accum = 0U;
            }
            if (obj.IsCurrent) {  /* current key, NO need to re-adjust offset */
              ptr_stream << obj.Offset;  // key_ptr
            } else {  /* history key, need to re-adjust offset */
              ptr_stream << static_cast<size_t>(index_map[obj.IndexId]->ByteOffsetOfHistory + obj.Offset);
            }
            ++num_accum;
            ++update_csr;
          }
          HighestSeq = cur_seq_num;
          assert(HighestSeq >= LowestSeq);
          seq_stream << cur_seq_num;  // Sequence Number
          seq_stream.Write(&cur_meta, sizeof(cur_meta));  // Metadata
          seq_stream.Write(&cur_id, sizeof(cur_id));  // Id
          seq_stream << bucket_ptr;  // Byte offset of bucket
          seq_stream << num_accum;  // number of key ptrs in bucket
        }
        /* flush collision blocks */ {
          for (auto iter : collision_map) {
            assert(iter.first < BlockVec.Size());
            #ifndef NDEBUG
            WrittenBlockSet.insert(BlockVec[iter.first]);
            #endif
            Engine->GetVolMan()->WriteBlock(HERE,
                                            Disk::Util::PageCheckedBlock,
                                            Source::MergeDataFileOther,
                                            iter.second->GetData(),
                                            BlockVec[iter.first],
                                            Priority,
                                            Disk::Util::CacheAll,
                                            completion_trigger);
          }
          /* wait for the io to finish since the collision map is about to go out of scope. */
          completion_trigger.Wait();
        }
      }
      size_t num_blocks = 0UL;
      size_t num_meta_blocks = 0UL;
      /* compute / write out the meta-data */ {
        /*
          # of blocks
          # of meta-blocks (n)
          # of #block / block_id pairings (number of sequential blocks starting at) (m)
          # updates
          # index segments (p)
          # of arena notes
          # of arena bytes
          offset of main arena
          offset of update index

          n (size_t) metablock block_id(s)
          m (size_t) -> (size_t) #block -> starting_block_id pairings
          p (Base::TUuid) -> (size_t) offsets to index segment(s)
        */

        const TBlockVec::TBlockMap seq_block_map_copy = BlockVec.GetSeqBlockMap();
        const size_t num_sequential_block_pairings = seq_block_map_copy.size();

        const size_t start_of_meta_data = BlockVec.Size() * LogicalBlockSize;

        size_t bytes_required_for_meta_data = TData::NumMetaFields * sizeof(size_t);  // meta fields
        bytes_required_for_meta_data += num_sequential_block_pairings * sizeof(size_t) * 2UL;  // num_sequential_block_pairings
        bytes_required_for_meta_data += index_map.size() * (sizeof(Base::TUuid) + sizeof(size_t));  // offsets to index segment(s)
        bytes_required_for_meta_data += MainArenaTypeBoundaryOffsetVec.size() * sizeof(size_t);  // offsets to type boundaries in arena

        num_blocks = BlockVec.Size();
        size_t num_bytes_for_meta = bytes_required_for_meta_data;
        num_meta_blocks = ceil(static_cast<double>(num_bytes_for_meta) / LogicalBlockSize);
        num_bytes_for_meta = bytes_required_for_meta_data + (num_meta_blocks * sizeof(size_t));
        num_meta_blocks = ceil(static_cast<double>(num_bytes_for_meta) / LogicalBlockSize);
        num_bytes_for_meta = bytes_required_for_meta_data + (num_meta_blocks * sizeof(size_t));
        num_meta_blocks = ceil(static_cast<double>(num_bytes_for_meta) / LogicalBlockSize);

        num_blocks += num_meta_blocks;

        const size_t total_blocks_required = BlockVec.Size() + num_meta_blocks;
        Engine->AppendReserveBlocks(StorageSpeed, num_meta_blocks, BlockVec);
        assert(BlockVec.Size() == total_blocks_required);
        std::unordered_map<size_t, std::shared_ptr<const TBufBlock>> collision_map {};
        TMergeDataFileImpl::TDataOutStream stream(HERE,
                                              Source::MergeDataFileMeta,
                                              Engine->GetVolMan(),
                                              start_of_meta_data,
                                              BlockVec,
                                              collision_map,
                                              completion_trigger,
                                              Priority,
                                              true /* do_cache */,
                                              #ifndef NDEBUG
                                              WrittenBlockSet,
                                              #endif
                                              [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
        stream << BlockVec.Size();  // # of blocks
        stream << num_meta_blocks;  // # of meta-blocks
        stream << num_sequential_block_pairings;  // # of #block / block_id pairings
        stream << NumUpdates;  // # of updates
        stream << index_map.size();  // # of index segments
        stream << num_main_arena_notes;  // # of arena notes
        stream << num_main_arena_bytes;  // # of arena bytes
        stream << MainArenaTypeBoundaryOffsetVec.size();  // # of arena type boundaries
        stream << MainArenaByteOffset;  // byte offset of main arena
        stream << byte_offset_of_update_entries;  // offset of update index

        /* write out the meta-block ids */
        for (size_t i = 0; i < num_meta_blocks; ++i) {
          stream << BlockVec[BlockVec.Size() - num_meta_blocks + i];
        }
        /* write out the sequential block pairings */
        static_assert(std::is_same<TBlockVec::TBlockMap::mapped_type, std::pair<size_t, size_t>>::value, "BlockVec mapped type must be the same as expected types for streaming block meta info for MergeDataFile");
        for (const auto &iter : seq_block_map_copy) {
          stream << iter.second.first << iter.second.second;
        }
        /* write out the index offsets */
        for (const auto &index : index_map) {
          stream.Write(index.first);
          stream << index.second->ByteOffsetOfIndexMeta;
        }
        for (const auto &offset : MainArenaTypeBoundaryOffsetVec) {
          stream << offset;
        }

      }
      /* compute the total number of current keys */ {
        for (const auto &idx : index_map) {
          NumKeys += idx.second->NumCurKeys;
        }
      }
      completion_trigger.Wait();
      StartingBlockId = BlockVec[BlockVec.Size() - num_meta_blocks];
      StartingBlockOffset = BlockVec.Size() - num_meta_blocks;
      FileLength = num_blocks * LogicalBlockSize;
      size_t total_num_keys = 0UL;
      for (const auto &index : index_map) {
        total_num_keys += index.second->NumCurKeys;
      }
      index_map.clear();
      /* sync this file to disk */ {
        std::vector<std::pair<size_t, size_t>> block_id_to_num_seq_blocks;
        /* compute the sequential block map */ {
          std::pair<size_t, size_t> seq_group = make_pair(BlockVec.Front(), 0UL);
          for (auto iter: BlockVec) {
            if (iter == (seq_group.first + seq_group.second)) {
              ++seq_group.second;
            } else {
              block_id_to_num_seq_blocks.push_back(seq_group);
              seq_group.first = iter;
              seq_group.second = 1UL;
            }
          }
          block_id_to_num_seq_blocks.push_back(seq_group);
        }
        Engine->GetVolMan()->SyncToDisk(block_id_to_num_seq_blocks);
      } /* done sync file to disk */
      /* wait for file entry to flush */ {
        Engine->InsertFile(file_uid, TFileObj::TKind::DataFile, gen_id, StartingBlockId, StartingBlockOffset, FileLength, total_num_keys, LowestSeq, HighestSeq, completion_trigger);
        completion_trigger.Wait();
      }
    } catch (const std::exception &ex) {
      syslog(LOG_ERR, "MergeDataFile gen [%ld] caught error [%s]", gen_id, ex.what());
      throw;
    }
    /* Let's make sure that all the blocks we have allocated were written to */
    #ifndef NDEBUG
    for (size_t block_id : BlockVec) {
      if (WrittenBlockSet.find(block_id) == WrittenBlockSet.end()) {
        syslog(LOG_ERR, "TMergeDataFileImpl allocated blocks that did not get written to [%ld]", block_id);
        throw std::logic_error("TMergeDataFileImpl allocated blocks that did not get written to");
      }
    }
    #endif
  }

  /* TODO */
  class TUpdateObj {
    public:

    /* TODO */
    TUpdateObj(TSequenceNumber seq_num, size_t offset, bool is_current, const Base::TUuid &index_id)
        : SequenceNumber(seq_num),
          Offset(offset),
          IsCurrent(is_current),
          IndexId(index_id) {}

    /* TODO */
    bool operator<(const TUpdateObj &that) const {
      assert(this);
      assert(&that);
      return SequenceNumber < that.SequenceNumber;
    }

    /* TODO */
    TSequenceNumber SequenceNumber;

    /* TODO */
    Atom::TCore Metadata;

    /* TODO */
    Atom::TCore Id;

    /* TODO */
    size_t Offset;

    /* TODO */
    bool IsCurrent;

    /* TODO */
    Base::TUuid IndexId;

  };  // TUpdateObj

  /* TODO */
  typedef Disk::Util::TIndexManager<TUpdateObj, Disk::Util::SortBufSize, Disk::Util::SortBufMinParallelSize> TUpdateCollector;
  using TTypeBoundaryOffsetVec = TDataFile::TTypeBoundaryOffsetVec;

  /* TODO */
  typedef Indy::Util::TBlockVec TBlockVec;
  typedef std::unordered_set<TRemapObj> TRemapIndex;

  /* TODO */
  class TL0MergeNote {
    public:

    /* TODO */
    TL0MergeNote()
        : Arena(nullptr), Offset(0UL), Note(nullptr) {}

    /* TODO */
    TL0MergeNote(Atom::TCore::TArena *arena, Atom::TCore::TOffset offset, const Atom::TCore::TNote *note)
        : Arena(arena), Offset(offset), Note(note) {}

    /* TODO */
    bool operator<(const TL0MergeNote &that) const {
      assert(this);
      assert(&that);
      assert(Arena);
      assert(that.Arena);
      assert(Note);
      assert(that.Note);
      /* we know the depths are the same, L0 */
      Atom::TCore
        lhs_core(Offset, Note),
        rhs_core(that.Offset, that.Note);

      void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
      void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
      Sabot::State::TAny::TWrapper
        lhs(lhs_core.NewState(Arena, lhs_state_alloc)),
        rhs(rhs_core.NewState(that.Arena, rhs_state_alloc));
      void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
      void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
      return Atom::IsLt(Sabot::CompareStates(*lhs, *rhs, lhs_type_alloc, rhs_type_alloc));
    }

    /* TODO */
    bool operator==(const TL0MergeNote &that) const {
      assert(this);
      assert(&that);
      assert(Arena);
      assert(that.Arena);
      assert(Note);
      assert(that.Note);
      Atom::TCore
        lhs_core(Offset, Note),
        rhs_core(that.Offset, that.Note);
      void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
      void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
      Sabot::State::TAny::TWrapper
        lhs(lhs_core.NewState(Arena, lhs_state_alloc)),
        rhs(rhs_core.NewState(that.Arena, rhs_state_alloc));
      void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
      void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
      return Atom::IsEq(Sabot::CompareStates(*lhs, *rhs, lhs_type_alloc, rhs_type_alloc));
    }

    /* TODO */
    bool operator!=(const TL0MergeNote &that) const {
      assert(this);
      assert(&that);
      assert(Arena);
      assert(that.Arena);
      assert(Note);
      assert(that.Note);
      Atom::TCore
        lhs_core(Offset, Note),
        rhs_core(that.Offset, that.Note);
      void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
      void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
      Sabot::State::TAny::TWrapper
        lhs(lhs_core.NewState(Arena, lhs_state_alloc)),
        rhs(rhs_core.NewState(that.Arena, rhs_state_alloc));
      void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
      void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
      return Atom::IsNe(Sabot::CompareStates(*lhs, *rhs, lhs_type_alloc, rhs_type_alloc));
    }

    /* TODO */
    Atom::TCore::TArena *GetArena() const {
      assert(this);
      return Arena;
    }

    /* TODO */
    Atom::TCore::TOffset GetOffset() const {
      assert(this);
      return Offset;
    }

    /* TODO */
    const Atom::TCore::TNote *GetNote() const {
      assert(this);
      return Note;
    }

    private:

    /* TODO */
    Atom::TCore::TArena *Arena;

    /* TODO */
    Atom::TCore::TOffset Offset;

    /* TODO */
    const Atom::TCore::TNote *Note;

  };  // TL0MergeNote

  /* TODO */
  class TMergeNote {
    public:

    /* TODO */
    TMergeNote()
        : Arena(nullptr), Offset(0UL), Note(nullptr) {}

    /* TODO */
    TMergeNote(Atom::TCore::TArena *arena, Atom::TCore::TOffset offset, const Atom::TCore::TNote *note)
        : Arena(arena), Offset(offset), Note(note) {}

    /* TODO */
    bool operator<(const TMergeNote &/*that*/) const {
      throw std::logic_error("TMergeNote::operator < should not get used.");
    }

    /* TODO */
    bool operator==(const TMergeNote &that) const {
      assert(this);
      assert(&that);
      assert(Arena == that.Arena);
      assert(Note);
      assert(that.Note);
      const size_t lhs_note_size = sizeof(Atom::TCore::TNote) + Note->GetRawSize();
      const size_t rhs_note_size = sizeof(Atom::TCore::TNote) + that.Note->GetRawSize();
      return lhs_note_size == rhs_note_size && memcmp(Note, that.Note, lhs_note_size) == 0;
    }

    /* TODO */
    bool operator!=(const TMergeNote &that) const {
      assert(this);
      assert(&that);
      assert(Arena == that.Arena);
      assert(Note);
      assert(that.Note);
      const size_t lhs_note_size = sizeof(Atom::TCore::TNote) + Note->GetRawSize();
      const size_t rhs_note_size = sizeof(Atom::TCore::TNote) + that.Note->GetRawSize();
      return lhs_note_size != rhs_note_size || memcmp(Note, that.Note, lhs_note_size) != 0;
    }

    /* TODO */
    Atom::TCore::TArena *GetArena() const {
      assert(this);
      return Arena;
    }

    /* TODO */
    Atom::TCore::TOffset GetOffset() const {
      assert(this);
      return Offset;
    }

    /* TODO */
    const Atom::TCore::TNote *GetNote() const {
      assert(this);
      return Note;
    }

    private:

    /* TODO */
    Atom::TCore::TArena *Arena;

    /* TODO */
    Atom::TCore::TOffset Offset;

    /* TODO */
    const Atom::TCore::TNote *Note;

  };  // TMergeNote

  /* TODO */
  class TTypeNote {
    public:

    /* TODO */
    TTypeNote(Atom::TCore::TArena *arena, Atom::TCore::TOffset offset)
        : Arena(arena),
          Offset(offset){}

    /* TODO */
    inline bool operator<(const TTypeNote &that) const {
      return Atom::IsLt(Compare(that));
    }

    /* TODO */
    inline bool operator!=(const TTypeNote &that) const {
      return Atom::IsNe(Compare(that));
    }

    /* TODO */
    inline Atom::TCore::TArena *GetArena() const {
      assert(this);
      return Arena;
    }

    /* TODO */
    inline Atom::TCore::TOffset GetOffset() const {
      assert(this);
      return Offset;
    }

    private:

    /* TODO */
    inline Atom::TComparison Compare(const TTypeNote &that) const {
      void *lhs_pin_alloc = alloca(sizeof(Atom::TCore::TArena::TFinalPin) * 2);
      void *rhs_pin_alloc = reinterpret_cast<uint8_t *>(lhs_pin_alloc) + sizeof(Atom::TCore::TArena::TFinalPin);
      void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
      void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
      Atom::TCore::TArena::TFinalPin::TWrapper
        lhs_pin(Arena->Pin(Offset, lhs_pin_alloc)),
        rhs_pin(that.Arena->Pin(that.Offset, rhs_pin_alloc));
      Atom::TCore
        lhs_core(Offset, lhs_pin->GetNote()),
        rhs_core(that.Offset, rhs_pin->GetNote());
      Sabot::Type::TAny::TWrapper
        lhs_wrapper(lhs_core.GetType(Arena, lhs_type_alloc)),
        rhs_wrapper(rhs_core.GetType(that.Arena, rhs_type_alloc));
      size_t
        lhs_depth = Sabot::GetDepth(*lhs_wrapper),
        rhs_depth = Sabot::GetDepth(*rhs_wrapper);
      Atom::TComparison ret = Atom::CompareOrdered(lhs_depth, rhs_depth);
      if (Atom::IsEq(ret)) {
        ret = Sabot::CompareTypes(*lhs_wrapper, *rhs_wrapper);
      }
      return ret;
    }

    /* TODO */
    Atom::TCore::TArena *Arena;

    /* TODO */
    Atom::TCore::TOffset Offset;

  };  // TTypeNote

  class TMyMergeArena
      : public TArenaInFile {
    NO_COPY_SEMANTICS(TMyMergeArena);
    public:

    TMyMergeArena(TEngine *engine, const TBlockVec &block_vec, size_t arena_byte_offset, size_t arena_num_notes, size_t arena_num_bytes)
        : Engine(engine),
          BlockVec(block_vec),
          ArenaByteOffset(arena_byte_offset),
          NumArenaNotes(arena_num_notes),
          NumArenaBytes(arena_num_bytes) {}

    virtual ~TMyMergeArena() {}

    private:

    virtual size_t GetFileLength() const override {
      return BlockVec.Size() * Disk::Util::LogicalBlockSize;
    }

    virtual size_t GetStartingBlock() const override {
      throw std::logic_error("TODO: implement TAbc::GetStartingBlock");
    }

    virtual void ReadMeta(size_t /*offset*/, size_t &/*out*/) const override {
      throw std::logic_error("TODO: implement TAbc::ReadMeta");
    }

    virtual size_t FindPageIdOfByte(size_t offset) const override {
      assert(offset <= GetFileLength());
      //return ((BlockVec[offset / Disk::Util::LogicalBlockSize]) * Disk::Util::PagesPerBlock) + ((offset % Disk::Util::LogicalBlockSize) / Disk::Util::LogicalPageSize);
      return BlockVec[offset / Disk::Util::LogicalBlockSize];
    }

    virtual size_t GetByteOffsetOfArena() const override {
      assert(this);
      return ArenaByteOffset;
    }

    virtual size_t GetNumArenaNotes() const override {
      assert(this);
      return NumArenaNotes;
    }

    virtual Atom::TCore::TOffset GetNumBytesOfArena() const override {
      assert(this);
      return NumArenaBytes;
    }

    TEngine *Engine;
    const TBlockVec &BlockVec;
    size_t ArenaByteOffset;
    size_t NumArenaNotes;
    size_t NumArenaBytes;

  };

  /* TODO */
  class TNewRemapObj {
    public:

    /* TODO */
    TNewRemapObj(Atom::TCore::TOffset old_key, Atom::TCore::TOffset new_key) : OldKey(old_key), NewKey(new_key) {}

    /* TODO */
    bool operator<(const TNewRemapObj &that) const {
      return OldKey < that.OldKey;
    }

    /* TODO */
    Atom::TCore::TOffset OldKey;

    /* TODO */
    Atom::TCore::TOffset NewKey;

  };  // TNewRemapObj

  /* TODO */
  typedef TIndexManager<TNewRemapObj, Disk::Util::NewRemapBufSize, Disk::Util::SortBufMinParallelSize> TRemapSorter;

  /* TODO */
  template <typename TKey>
  class TAccessObj {
    public:

    /* TODO */
    TAccessObj(size_t idx, TKey key) : Idx(idx), Key(key) {}

    /* TODO */
    bool operator<(const TAccessObj &that) const {
      return Key < that.Key;
    }

    /* TODO */
    size_t Idx;

    /* TODO */
    TKey Key;

  };  // TAccessObj

  /* TODO */
  template <typename TOldKey, typename TNewKey>
  class TResolvedObj {
    public:

    /* TODO */
    TResolvedObj(size_t idx, TOldKey old_key, TNewKey new_key) : Idx(idx), OldKey(old_key), NewKey(new_key) {}

    /* TODO */
    bool operator<(const TResolvedObj &that) const {
      return Idx < that.Idx;
    }

    /* TODO */
    size_t Idx;

    /* TODO */
    TOldKey OldKey;

    /* TODO */
    TNewKey NewKey;

  };  // TResolvedObj

  /* TODO */
  template <typename TKey>
  using TAccessSorter = TIndexManager<TAccessObj<TKey>, Disk::Util::AccessBufSize, Disk::Util::SortBufMinParallelSize>;

  /* TODO */
  template <typename TOldKey, typename TNewKey>
  using TResolvedSorter = TIndexManager<TResolvedObj<TOldKey, TNewKey>, Disk::Util::AccessBufSize, Disk::Util::SortBufMinParallelSize>;

  /* TODO */
  typedef TIndexManager<TSequenceNumber, Disk::Util::SeqKeeperBufSize, Disk::Util::SortBufMinParallelSize> TSeqKeeperSorter;

  /* TODO */
  typedef TIndexManager<Atom::TCore::TOffset, Disk::Util::ArenaKeeperBufSize, Disk::Util::SortBufMinParallelSize> TArenaKeeperSorter;

  /* TODO */
  typedef TAccessObj<Atom::TCore::TOffset> TRemapAccessObj;
  typedef TAccessSorter<Atom::TCore::TOffset> TRemapAccessSorter;
  typedef TResolvedObj<Atom::TCore::TOffset, Atom::TCore::TOffset> TRemapResolvedObj;
  typedef TResolvedSorter<Atom::TCore::TOffset, Atom::TCore::TOffset> TRemapResolvedSorter;

  /* TODO */
  typedef TAccessObj<TSequenceNumber> TSeqAccessObj;
  typedef TAccessSorter<TSequenceNumber> TSeqAccessSorter;
  typedef TResolvedObj<TSequenceNumber, bool> TSeqResolvedObj;
  typedef TResolvedSorter<TSequenceNumber, bool> TSeqResolvedSorter;

  /* TODO */
  static void ResolveRemap(size_t max_block_cache_read_slots_allowed,
                           TRemapAccessSorter &access_sorter,
                           TRemapSorter &remap_sorter,
                           TRemapResolvedSorter &resolved_sorter) {
    typename TRemapSorter::TCursor remap_csr(&remap_sorter, max_block_cache_read_slots_allowed / 2);
    for (typename TRemapAccessSorter::TCursor access_csr(&access_sorter, max_block_cache_read_slots_allowed / 2); access_csr; ++access_csr) {
      assert(remap_csr);
      #ifndef NDEBUG
      try {
        for (; remap_csr->OldKey != access_csr->Key; ++remap_csr) {}
      } catch (const std::exception &ex) {
        syslog(LOG_ERR, "Error remapping [%ld] [%s]", access_csr->Key, ex.what());
        throw;
      }
      #else
      for (; remap_csr->OldKey != access_csr->Key; ++remap_csr) {}
      #endif
      assert(remap_csr);
      resolved_sorter.Emplace(access_csr->Idx, remap_csr->OldKey, remap_csr->NewKey);
    }
  }

  /* TODO */
  static void ResolveSeqFilter(size_t max_block_cache_read_slots_allowed,
                           TSeqAccessSorter &access_sorter,
                           TSeqKeeperSorter &keep_filter_sorter,
                           TSeqResolvedSorter &resolved_sorter) {
    TSeqKeeperSorter::TCursor filter_csr(&keep_filter_sorter, max_block_cache_read_slots_allowed / 2);
    for (typename TSeqAccessSorter::TCursor access_csr(&access_sorter, max_block_cache_read_slots_allowed / 2); access_csr; ++access_csr) {
      for (; filter_csr && (*filter_csr < access_csr->Key); ++filter_csr) {}
      if (filter_csr) {
        resolved_sorter.Emplace(access_csr->Idx, access_csr->Key, *filter_csr == access_csr->Key);
      } else {
        resolved_sorter.Emplace(access_csr->Idx, access_csr->Key, false);
      }
    }
  }

  static inline void AdvanceFilterCursor(typename TArenaKeeperSorter::TCursor &csr) {
    size_t cur_val = *csr;
    ++csr;
    for (; csr && *csr == cur_val; ++csr) {}
  }

  static size_t MakeArena(TEngine *engine,
                          TVolume::TDesc::TStorageSpeed storage_speed,
                          TVolume::TDesc::TStorageSpeed sorter_storage_speed,
                          DiskPriority priority,
                          size_t max_block_cache_read_slots_allowed,
                          size_t temp_file_consol_thresh,
                          const std::vector<std::unique_ptr<TDataDiskArena<true>>> &disk_arena_vec,
                          const std::vector<std::vector<size_t>> &type_boundary_offset_vec_by_file,
                          typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TTypeBoundaryOffsetVec &type_boundary_offset_out_vec,
                          const std::vector<std::unique_ptr<typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TRemapSorter>> &remap_sorter_vec,
                          const std::vector<std::unique_ptr<typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TArenaKeeperSorter>> &arena_keeper_vec,
                          typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TBlockVec &block_vec,
                          #ifndef NDEBUG
                          std::unordered_set<size_t> &written_block_set,
                          #endif
                          size_t max_total_note_bytes,
                          size_t &num_notes_out,
                          size_t &num_bytes_out) {
    /* build a map from boundary to sorted keeper filter for each file */
    std::vector<std::map<size_t, std::unique_ptr<typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TArenaKeeperSorter>>> sorted_keeper_map_vec;
    if (CanTail) {
      for (const auto &type_vec : type_boundary_offset_vec_by_file) {
        std::map<size_t, std::unique_ptr<typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TArenaKeeperSorter>> temp_map;
        for (size_t offset : type_vec) {
          temp_map.insert(make_pair(offset, std::unique_ptr<typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TArenaKeeperSorter>(new typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TArenaKeeperSorter(HERE, Source::MergeDataFileTailIndex, temp_file_consol_thresh, sorter_storage_speed, engine, true))));
        }
        sorted_keeper_map_vec.emplace_back(std::move(temp_map));
      }
      /* from the original source keeper filter, place the values into the sorted_keeper_maps */
      assert(arena_keeper_vec.size() == sorted_keeper_map_vec.size());
      for (size_t i = 0; i < arena_keeper_vec.size(); ++i) {
        for (typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TArenaKeeperSorter::TCursor arena_keeper_csr(arena_keeper_vec[i].get(), max_block_cache_read_slots_allowed); arena_keeper_csr; ++arena_keeper_csr) {
          auto &m = sorted_keeper_map_vec[i];
          auto upper_b = m.upper_bound(*arena_keeper_csr);
          --upper_b;
          assert(upper_b != m.end());
          upper_b->second->Emplace(*arena_keeper_csr);
        }
      }
      /* now we go backwards over each type range. we filter this sequence by the sorted values in the keeper filter. if it's a keeper, we add all the indirections of this value. */ {
        for (size_t i = 0; i < disk_arena_vec.size(); ++i) {
          auto &m = sorted_keeper_map_vec[i];
          auto add_to_filter_cb = [&m](Atom::TCore::TOffset offset) {
            auto upper_b = m.upper_bound(offset);
            --upper_b;
            assert(upper_b != m.end());
            upper_b->second->Emplace(offset);
          };
          //std::cout << "backwards [" << i << "] << arena [" << disk_arena_vec[i] << "]" << std::endl;
          size_t end_offset = disk_arena_vec[i]->GetNumBytesOfArena();
          for (auto iter = type_boundary_offset_vec_by_file[i].rbegin(); iter != type_boundary_offset_vec_by_file[i].rend(); ++iter) {
            //std::cout << "\t" << *iter << std::endl;
            const auto &filter = sorted_keeper_map_vec[i][*iter];
            typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TArenaKeeperSorter::TCursor filter_csr(filter.get(), max_block_cache_read_slots_allowed);
            for (TDataDiskArena<true>::TCursor note_cursor(disk_arena_vec[i].get(), *iter, end_offset); note_cursor; ++note_cursor) {
              for (; filter_csr && *filter_csr < note_cursor.GetOffset(); ++filter_csr) {}
              if (filter_csr && *filter_csr == note_cursor.GetOffset()) {
                //std::cout << "\t\t" << note_cursor.GetOffset() << std::endl;
                /* we're going to keep this one, so we insert the sub-indirections into the lower level keeper filters. */
                note_cursor->ForOffset(add_to_filter_cb);
              } else {
                //std::cout << "\t\tskipping " << note_cursor.GetOffset() << std::endl;
              }
            }
            end_offset = *iter;
          }
        }
      }
    }
    assert(disk_arena_vec.size() == type_boundary_offset_vec_by_file.size());
    const size_t arena_byte_offset = block_vec.Size() * LogicalBlockSize;

    size_t max_blocks_required = ceil(static_cast<double>(max_total_note_bytes) / LogicalBlockSize);
    const size_t total_blocks_required = block_vec.Size() + max_blocks_required;
    engine->AppendReserveBlocks(storage_speed, max_blocks_required, block_vec);
    #ifndef NDEBUG
    /* TEMP DEBUG: validate that no one is looking at these cache pages */ {
      for (size_t i = block_vec.Size() - max_blocks_required; i < block_vec.Size(); ++i) {
        assert(engine->GetPageCache()->AssertNoRefCount(block_vec[i] * 16));
        assert(engine->GetBlockCache()->AssertNoRefCount(block_vec[i]));
      }
    }
    #endif
    assert(block_vec.Size() == total_blocks_required);

    TCompletionTrigger completion_trigger;
    std::unordered_map<size_t, std::shared_ptr<const TBufBlock>> arena_collision_map {};
    /* arena stream life-span */ {
      TDataOutStream arena_stream(HERE,
                                  Source::MergeDataFileArena,
                                  engine->GetVolMan(),
                                  arena_byte_offset,
                                  block_vec,
                                  arena_collision_map,
                                  completion_trigger,
                                  priority,
                                  true /* do_cache */,
                                  #ifndef NDEBUG
                                  written_block_set,
                                  #endif
                                  [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
      Atom::TCore::TOffset cur_disk_offset = 0UL;

      /* let's get some information about what types are involved... */ {
        size_t num_total_types = 0UL;
        for (const auto &type_boundary_offset_vec : type_boundary_offset_vec_by_file) {
          num_total_types += type_boundary_offset_vec.size();
        }
        std::vector<TTypeNote> type_note_vec;
        type_note_vec.reserve(num_total_types);
        Stig::Indy::Util::TMinHeap<TTypeNote, size_t> min_heap(num_total_types);
        for (size_t i = 0; i < disk_arena_vec.size(); ++i) {
          TDataDiskArena<true> *const arena = disk_arena_vec[i].get();
          const std::vector<size_t> &type_boundary_offset_vec = type_boundary_offset_vec_by_file[i];
          for (const size_t off : type_boundary_offset_vec) {
            type_note_vec.emplace_back(arena, off);
            min_heap.Insert(type_note_vec.back(), i);
          }
        }

        void *pin_alloc = alloca(sizeof(Atom::TCore::TArena::TFinalPin));
        void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
        Base::TOpt<TTypeNote> prev_type;
        std::vector<std::pair<size_t, size_t>> offset_by_file_to_merge;

        auto merge_func = [&]() {
          type_boundary_offset_out_vec.push_back(cur_disk_offset);
          Atom::TCore::TArena::TFinalPin::TWrapper pin(prev_type->GetArena()->Pin(prev_type->GetOffset(), pin_alloc));
          Atom::TCore temp_core(prev_type->GetOffset(), pin->GetNote());
          Sabot::Type::TAny::TWrapper type_wrapper(temp_core.GetType(prev_type->GetArena(), type_alloc));
          size_t type_depth = Sabot::GetDepth(*type_wrapper);
          if (type_depth == 0) {
            MergeTypeRangeDepth0(arena_stream,
                                 max_block_cache_read_slots_allowed,
                                 disk_arena_vec,
                                 type_boundary_offset_vec_by_file,
                                 sorted_keeper_map_vec,
                                 offset_by_file_to_merge,
                                 remap_sorter_vec,
                                 cur_disk_offset,
                                 num_notes_out,
                                 num_bytes_out);
          } else {
            if (num_notes_out > 0) {
              /* flush last collision block */ {
                /* find the max block */
                size_t max_block = 0UL;
                bool found = false;
                for (const auto &iter : arena_collision_map) {
                  found = true;
                  max_block = std::max(max_block, iter.first);
                }
                if (found) {
                  assert(max_block < block_vec.Size());
                  #ifndef NDEBUG
                  written_block_set.insert(block_vec[max_block]);
                  #endif
                  engine->GetVolMan()->WriteBlock(HERE,
                                                  Disk::Util::PageCheckedBlock,
                                                  Source::MergeDataFileArena,
                                                  arena_collision_map[max_block]->GetData(),
                                                  block_vec[max_block],
                                                  priority,
                                                  Disk::Util::CacheAll,
                                                  completion_trigger);
                }
              }
              try {
                arena_stream.Sync();
              } catch (const TDiskError &err) {
                syslog(LOG_EMERG, "Caught MergeDataFile Disk err : [%s]", err.what());
                throw;
              }
              arena_stream.MakeCurBlockCollisionBlock();
              typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TMyMergeArena my_merge_arena(engine, block_vec, arena_byte_offset, num_notes_out, num_bytes_out);
              TDataDiskArena<false> my_arena(&my_merge_arena, engine->GetCache<TDataDiskArena<false>::PhysicalCachePageSize>(), priority);
              MergeTypeRange(engine,
                             storage_speed,
                             sorter_storage_speed,
                             max_block_cache_read_slots_allowed,
                             temp_file_consol_thresh,
                             arena_stream,
                             &my_arena,
                             disk_arena_vec,
                             type_boundary_offset_vec_by_file,
                             sorted_keeper_map_vec,
                             offset_by_file_to_merge,
                             remap_sorter_vec,
                             cur_disk_offset,
                             num_notes_out,
                             num_bytes_out);
            } else {
              MergeTypeRange(engine,
                             storage_speed,
                             sorter_storage_speed,
                             max_block_cache_read_slots_allowed,
                             temp_file_consol_thresh,
                             arena_stream,
                             static_cast<TDataDiskArena<false> *const>(nullptr),
                             disk_arena_vec,
                             type_boundary_offset_vec_by_file,
                             sorted_keeper_map_vec,
                             offset_by_file_to_merge,
                             remap_sorter_vec,
                             cur_disk_offset,
                             num_notes_out,
                             num_bytes_out);
            }
          }
        };

        while (min_heap) {
          size_t file;
          const typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TTypeNote &type_note = min_heap.Pop(file);

          if (!prev_type || type_note != *prev_type) {
            if (prev_type) { /* we have collected all the ranges of this type, time to merge */
              merge_func();
            }
            offset_by_file_to_merge.clear();
            prev_type = type_note;
          }
          offset_by_file_to_merge.emplace_back(file, type_note.GetOffset());
        }
        if (prev_type) {
          merge_func();
        }
      }
    }
    /* flush last collision block */ {
      /* find the max block */
      size_t max_block = 0UL;
      bool found = false;
      for (const auto &iter : arena_collision_map) {
        found = true;
        max_block = std::max(max_block, iter.first);
      }
      if (found) {
        assert(max_block < block_vec.Size());
        #ifndef NDEBUG
        written_block_set.insert(block_vec[max_block]);
        #endif
        engine->GetVolMan()->WriteBlock(HERE,
                                        Disk::Util::PageCheckedBlock,
                                        Source::MergeDataFileArena,
                                        arena_collision_map[max_block]->GetData(),
                                        block_vec[max_block],
                                        priority,
                                        Disk::Util::CacheAll,
                                        completion_trigger);
      }
    }
    /* wait for the arena to flush */ {
      completion_trigger.Wait();
    }
    /* now that we know exactly how many bytes we actually used, we can shrink the block vec to free the unused blocks. */
    const size_t end_of_stream = arena_byte_offset + num_bytes_out;
    const size_t actual_blocks_required = ((end_of_stream - 1UL) / Disk::Util::LogicalBlockSize) + 1UL;
    const size_t num_to_remove = block_vec.Size() - actual_blocks_required;
    /* let's make sure that the max_block that we just flushed isn't in the section that we're freeing... that would be a logic error :-) */
    block_vec.ForEachSeqRangeInRange([&](size_t block_id, size_t num_blocks) -> bool {
      engine->FreeSeqBlocks(block_id, num_blocks);
      return true;
    }, actual_blocks_required, block_vec.Size());
    block_vec.Trim(num_to_remove);
    assert(block_vec.Size() == actual_blocks_required);
    return arena_byte_offset;
  }

  template <bool ScanAheadAllowed>
  static void MergeTypeRangeDepth0(TDataOutStream &arena_stream,
                                   size_t max_block_cache_read_slots_allowed,
                                   const std::vector<std::unique_ptr<TDataDiskArena<ScanAheadAllowed>>> &disk_arena_vec,
                                   const std::vector<std::vector<size_t>> &type_boundary_offset_vec_by_file,
                                   const std::vector<std::map<size_t, std::unique_ptr<TArenaKeeperSorter>>> &sorted_keeper_map_vec,
                                   const std::vector<std::pair<size_t, size_t>> &offset_by_file_to_merge,
                                   const std::vector<std::unique_ptr<TRemapSorter>> &remap_sorter_vec,
                                   Atom::TCore::TOffset &cur_disk_offset,
                                   size_t &num_notes_out,
                                   size_t &num_bytes_out) {
    try {
      std::vector<std::unique_ptr<typename TDataDiskArena<ScanAheadAllowed>::TCursor>> note_cursor_vec;
      std::vector<std::unique_ptr<typename TArenaKeeperSorter::TCursor>> keeper_filter_csr_vec;
      std::vector<size_t> file_id_by_pos_vec;
      for (const auto &matched_type_range : offset_by_file_to_merge) {
        const size_t file = matched_type_range.first;
        const size_t from_offset = matched_type_range.second;
        const auto &type_boundary_offset_vec = type_boundary_offset_vec_by_file[file];
        TDataDiskArena<ScanAheadAllowed> *disk_arena = disk_arena_vec[file].get();
        size_t to_offset = disk_arena->GetNumBytesOfArena();
        for (const size_t off : type_boundary_offset_vec) {
          if (off > from_offset) {
            to_offset = off;
            break;
          }
        }
        assert(to_offset > from_offset);
        file_id_by_pos_vec.emplace_back(file);
        note_cursor_vec.emplace_back(new typename TDataDiskArena<ScanAheadAllowed>::TCursor(disk_arena, from_offset, to_offset));
      }
      std::vector<TL0MergeNote> merge_note_vec(note_cursor_vec.size());
      Stig::Indy::Util::TMinHeap<TL0MergeNote, size_t> min_heap(note_cursor_vec.size());
      size_t pos = 0UL;
      for (const auto &csr_ptr : note_cursor_vec) {
        typename TDataDiskArena<ScanAheadAllowed>::TCursor &csr = *csr_ptr;
        const auto &matched_type_range = offset_by_file_to_merge[pos];
        const size_t file = matched_type_range.first;
        const size_t from_offset = matched_type_range.second;
        /* set up the tailing filter */
        if (CanTail) {
          assert(file < sorted_keeper_map_vec.size());
          const auto &keeper_filter = sorted_keeper_map_vec[file].find(from_offset)->second;
          keeper_filter_csr_vec.emplace_back(new typename TArenaKeeperSorter::TCursor(keeper_filter.get(), max_block_cache_read_slots_allowed));
        }
        /* add the first elements we're interested in to the min-heap */
        if (CanTail) {
          typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TArenaKeeperSorter::TCursor &filter_csr = *keeper_filter_csr_vec[pos];
          for (; csr && filter_csr && csr.GetOffset() < *filter_csr; ++csr) {}
          if (csr && filter_csr) {
            merge_note_vec[pos] = TL0MergeNote(csr.GetArena(), csr.GetOffset(), *csr);
            min_heap.Insert(merge_note_vec[pos], pos);
            AdvanceFilterCursor(filter_csr);
          }
        } else {
          if (csr) {
            merge_note_vec[pos] = TL0MergeNote(csr.GetArena(), csr.GetOffset(), *csr);
            min_heap.Insert(merge_note_vec[pos], pos);
          }
        }
        ++pos;
      }
      Base::TOpt<typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TL0MergeNote> prev_note;
      Atom::TCore::TOffset prev_disk_offset = cur_disk_offset;
      Atom::TCore::TNote *temp_note = nullptr;
      size_t max_temp_note_size = getpagesize();
      temp_note = reinterpret_cast<Atom::TCore::TNote *>(malloc(max_temp_note_size));
      if (!temp_note) {
        throw std::bad_alloc();
      }
      try {
        while (min_heap) {
          const TL0MergeNote &merge_note = min_heap.Pop(pos);
          typename TDataDiskArena<ScanAheadAllowed>::TCursor &csr = *note_cursor_vec[pos];
          const size_t file = file_id_by_pos_vec[pos];
          typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TRemapSorter &remap_sorter = *(remap_sorter_vec[file]);
          const Atom::TCore::TNote *const note = merge_note.GetNote();
          const size_t note_size = sizeof(Atom::TCore::TNote) + note->GetRawSize();

          if (!prev_note || merge_note != *prev_note) {  // new note
            if (note_size > max_temp_note_size) {
              if ((temp_note = reinterpret_cast<Atom::TCore::TNote *>(realloc(temp_note, note_size))) == 0) {
                free(temp_note);
                temp_note = nullptr;
                syslog(LOG_EMERG, "bad alloc in MergeDataFile MergeTypeRangeDepth0 note realloc [%ld]", note_size);
                throw std::bad_alloc();
              }
              max_temp_note_size = note_size;
            }
            memcpy(temp_note, merge_note.GetNote(), note_size);

            prev_note = TL0MergeNote(merge_note.GetArena(), merge_note.GetOffset(), temp_note);

            remap_sorter.Emplace(merge_note.GetOffset(), cur_disk_offset);

            prev_disk_offset = cur_disk_offset;
            cur_disk_offset += note_size;

            /* we don't need to remap L0 notes */

            arena_stream.Write(note, note_size);
            num_bytes_out += note_size;
            ++num_notes_out;

          } else {  // same note as previous
            remap_sorter.Emplace(merge_note.GetOffset(), prev_disk_offset);
          }
          ++csr;
          if (CanTail) {
            typename TArenaKeeperSorter::TCursor &filter_csr = *keeper_filter_csr_vec[pos];
            for (; csr && filter_csr && csr.GetOffset() < *filter_csr; ++csr) {}
            if (csr && filter_csr) {
              assert(csr.GetOffset() == *filter_csr);
              merge_note_vec[pos] = TL0MergeNote(csr.GetArena(), csr.GetOffset(), *csr);
              min_heap.Insert(merge_note_vec[pos], pos);
              AdvanceFilterCursor(filter_csr);
            }
          } else {
            if (csr) {
              merge_note_vec[pos] = TL0MergeNote(csr.GetArena(), csr.GetOffset(), *csr);
              min_heap.Insert(merge_note_vec[pos], pos);
            }
          }
        }
        free(temp_note);
        temp_note = nullptr;
      } catch (const std::exception &ex) {
        free(temp_note);
        temp_note = nullptr;
        throw;
      }
    } catch (const std::exception &ex) {
      syslog(LOG_ERR, "MergeTypeRangeDepth0 caught error [%s]", ex.what());
      throw;
    }
  }

  template <bool MyArenaScanAheadAllowed, bool DiskArenaVecScanAheadAllowed>
  static void MergeTypeRange(TEngine *engine,
                             TVolume::TDesc::TStorageSpeed /*storage_speed*/,
                             TVolume::TDesc::TStorageSpeed sorter_storage_speed,
                             size_t max_block_cache_read_slots_allowed,
                             size_t temp_file_consol_thresh,
                             TDataOutStream &arena_stream,
                             TDataDiskArena<MyArenaScanAheadAllowed> *const my_arena,
                             const std::vector<std::unique_ptr<TDataDiskArena<DiskArenaVecScanAheadAllowed>>> &disk_arena_vec,
                             const std::vector<std::vector<size_t>> &type_boundary_offset_vec_by_file,
                             const std::vector<std::map<size_t, std::unique_ptr<TArenaKeeperSorter>>> &sorted_keeper_map_vec,
                             const std::vector<std::pair<size_t, size_t>> &offset_by_file_to_merge,
                             const std::vector<std::unique_ptr<TRemapSorter>> &remap_sorter_vec,
                             Atom::TCore::TOffset &cur_disk_offset,
                             size_t &num_notes_out,
                             size_t &num_bytes_out) {
    //std::cout << "MergeTypeRange" << std::endl;
    try {
      assert(remap_sorter_vec.size() == disk_arena_vec.size());
      std::vector<size_t> file_id_by_pos_vec;
      std::vector<std::unique_ptr<typename TDataDiskArena<DiskArenaVecScanAheadAllowed>::TCursor>> note_cursor_vec;
      std::vector<std::unique_ptr<typename TArenaKeeperSorter::TCursor>> keeper_filter_csr_vec;
      std::vector<std::unique_ptr<TRemapAccessSorter>> access_sorter_vec;
      std::vector<std::unique_ptr<TRemapResolvedSorter>> resolved_sorter_vec;
      std::vector<std::unique_ptr<typename TRemapResolvedSorter::TCursor>> resolved_sorter_cursor_vec;

      size_t my_pos = 0;
      auto note_remapper = [&resolved_sorter_cursor_vec, &my_pos](Atom::TCore::TOffset offset) {
        const TRemapResolvedObj &obj = **resolved_sorter_cursor_vec[my_pos];
        assert(offset == obj.OldKey);
        Atom::TCore::TOffset ret = obj.NewKey;
        assert(resolved_sorter_cursor_vec[my_pos]);
        ++(*resolved_sorter_cursor_vec[my_pos]);
        return ret;
      };

      size_t max_block_cache_read_slots_allowed_per_subcursor = max_block_cache_read_slots_allowed / offset_by_file_to_merge.size();
      for (size_t i = 0; i < offset_by_file_to_merge.size(); ++i) {
        const auto &matched_type_range = offset_by_file_to_merge[i];
        const size_t file = matched_type_range.first;
        const size_t from_offset = matched_type_range.second;
        const auto &type_boundary_offset_vec = type_boundary_offset_vec_by_file[file];
        TDataDiskArena<DiskArenaVecScanAheadAllowed> *disk_arena = disk_arena_vec[file].get();
        size_t to_offset = disk_arena->GetNumBytesOfArena();
        for (const size_t off : type_boundary_offset_vec) {
          if (off > from_offset) {
            to_offset = off;
            break;
          }
        }
        assert(to_offset > from_offset);
        file_id_by_pos_vec.emplace_back(file);

        /* build an access pattern list */ {
          access_sorter_vec.push_back(std::unique_ptr<TRemapAccessSorter>(new TRemapAccessSorter(HERE, Source::MergeDataFileRemapIndex, temp_file_consol_thresh, sorter_storage_speed, engine, true)));
          resolved_sorter_vec.push_back(std::unique_ptr<TRemapResolvedSorter>(new TRemapResolvedSorter(HERE, Source::MergeDataFileRemapIndex, temp_file_consol_thresh, sorter_storage_speed, engine, true)));
          TRemapAccessSorter &access_sorter = *access_sorter_vec.back();
          TRemapResolvedSorter &resolved_sorter = *resolved_sorter_vec.back();
          TRemapSorter &remap_sorter = *remap_sorter_vec[file];
          size_t idx = 0UL;
          auto pusher = [&access_sorter, &idx](Atom::TCore::TOffset off) {
            access_sorter.Emplace(++idx, off);
          };
          if (CanTail) {
            assert(file < sorted_keeper_map_vec.size());
            const auto &keeper_filter = sorted_keeper_map_vec[file].find(from_offset)->second;
            typename TArenaKeeperSorter::TCursor filter_csr(keeper_filter.get(), max_block_cache_read_slots_allowed);
            keeper_filter_csr_vec.emplace_back(new typename TArenaKeeperSorter::TCursor(keeper_filter.get(), max_block_cache_read_slots_allowed));
            for (typename TDataDiskArena<DiskArenaVecScanAheadAllowed>::TCursor csr(disk_arena, from_offset, to_offset); csr; ++csr) {
              for (; csr && filter_csr && csr.GetOffset() < *filter_csr; ++csr) {}
              if (csr && filter_csr && csr.GetOffset() == *filter_csr) {
                /* keep this one */
                csr->ForOffset(pusher);
                AdvanceFilterCursor(filter_csr);
              }
            }
          } else {
            for (typename TDataDiskArena<DiskArenaVecScanAheadAllowed>::TCursor csr(disk_arena, from_offset, to_offset); csr; ++csr) {
              if (csr) {
                csr->ForOffset(pusher);
              }
            }
          }

          //std::cout << "ResolveRemap file [" << file << "]" << std::endl;
          ResolveRemap(max_block_cache_read_slots_allowed, access_sorter, remap_sorter, resolved_sorter);
          resolved_sorter_cursor_vec.push_back(std::unique_ptr<typename TRemapResolvedSorter::TCursor>(new typename TRemapResolvedSorter::TCursor(&resolved_sorter, max_block_cache_read_slots_allowed_per_subcursor)));
        }
        access_sorter_vec.back().reset();
        note_cursor_vec.emplace_back(new typename TDataDiskArena<DiskArenaVecScanAheadAllowed>::TCursor(disk_arena, from_offset, to_offset));
      }
      std::vector<TMergeNote> merge_note_vec(note_cursor_vec.size());
      Atom::TCore::TNote::TOrderedArenaCompare note_comp(my_arena);
      Stig::Indy::Util::TMinHeap<Atom::TCore::TNote, size_t, Atom::TCore::TNote::TOrderedArenaCompare> min_heap(note_cursor_vec.size(), note_comp);
      std::vector<std::pair<Atom::TCore::TNote *, size_t>> temp_note_vec(note_cursor_vec.size(), std::make_pair(nullptr, 0UL));
      //std::cout << "=== Create MinHeap ===" << std::endl;
      try {
        size_t pos = 0UL;
        for (const auto &csr_ptr : note_cursor_vec) {
          typename TDataDiskArena<DiskArenaVecScanAheadAllowed>::TCursor &csr = *csr_ptr;

          if (CanTail) {
            typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TArenaKeeperSorter::TCursor &filter_csr = *keeper_filter_csr_vec[pos];
            for (; csr && filter_csr && csr.GetOffset() < *filter_csr; ++csr) {}
            if (csr && filter_csr) {
              //std::cout << "csr.GetOffset() [" << csr.GetOffset() << "] vs. *filter_csr [" << *filter_csr << "]" << std::endl;
              assert(csr.GetOffset() == *filter_csr);
              const Atom::TCore::TNote *const note = *csr;
              const size_t note_size = sizeof(Atom::TCore::TNote) + note->GetRawSize();
              const size_t cur_max_note_size = std::max(note_size, static_cast<size_t>(getpagesize()));
              Atom::TCore::TNote *t_note = reinterpret_cast<Atom::TCore::TNote *>(malloc(cur_max_note_size));
              if (!t_note) {
                throw std::bad_alloc();
              }
              memcpy(t_note, note, note_size);
              assert(disk_arena_vec[file_id_by_pos_vec[pos]].get() == csr.GetArena());
              my_pos = pos;
              t_note->Remap(note_remapper);
              temp_note_vec[pos].first = t_note;
              temp_note_vec[pos].second = cur_max_note_size;
              merge_note_vec[pos] = TMergeNote(my_arena, csr.GetOffset(), t_note);
              min_heap.Insert(*merge_note_vec[pos].GetNote(), pos);
              AdvanceFilterCursor(filter_csr);
            }
          } else {
            if (csr) {
              const Atom::TCore::TNote *const note = *csr;
              const size_t note_size = sizeof(Atom::TCore::TNote) + note->GetRawSize();
              const size_t cur_max_note_size = std::max(note_size, static_cast<size_t>(getpagesize()));
              Atom::TCore::TNote *t_note = reinterpret_cast<Atom::TCore::TNote *>(malloc(cur_max_note_size));
              if (!t_note) {
                throw std::bad_alloc();
              }
              memcpy(t_note, note, note_size);
              assert(disk_arena_vec[file_id_by_pos_vec[pos]].get() == csr.GetArena());
              my_pos = pos;
              t_note->Remap(note_remapper);
              temp_note_vec[pos].first = t_note;
              temp_note_vec[pos].second = cur_max_note_size;
              merge_note_vec[pos] = TMergeNote(my_arena, csr.GetOffset(), t_note);
              min_heap.Insert(*merge_note_vec[pos].GetNote(), pos);
            }
          }

          ++pos;
        }
        Atom::TCore::TNote *temp_note = nullptr;
        size_t max_temp_note_size = getpagesize();
        temp_note = reinterpret_cast<Atom::TCore::TNote *>(malloc(max_temp_note_size));
        if (!temp_note) {
          throw std::bad_alloc();
        }
        try {
          Base::TOpt<TMergeNote> prev_note;
          Atom::TCore::TOffset prev_disk_offset = cur_disk_offset;
          while (min_heap) {
            min_heap.Pop(pos);
            const typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TMergeNote &merge_note = merge_note_vec[pos];
            typename TDataDiskArena<DiskArenaVecScanAheadAllowed>::TCursor &csr = *note_cursor_vec[pos];
            const size_t file = file_id_by_pos_vec[pos];
            TRemapSorter &remap_sorter = *(remap_sorter_vec[file]);
            const Atom::TCore::TNote *const note = merge_note.GetNote();
            const size_t note_size = sizeof(Atom::TCore::TNote) + note->GetRawSize();
            if (!prev_note || merge_note != *prev_note) {  // new note
              if (note_size > max_temp_note_size) {
                if ((temp_note = reinterpret_cast<Atom::TCore::TNote *>(realloc(temp_note, note_size))) == 0) {
                  free(temp_note);
                  temp_note = nullptr;
                  syslog(LOG_EMERG, "bad alloc in MergeDataFile MergeTypeRange note realloc [%ld]", note_size);
                  throw std::bad_alloc();
                }
                max_temp_note_size = note_size;
              }
              memcpy(temp_note, merge_note.GetNote(), note_size);
              prev_note = TMergeNote(my_arena, merge_note.GetOffset(), temp_note);
              remap_sorter.Emplace(merge_note.GetOffset(), cur_disk_offset);
              prev_disk_offset = cur_disk_offset;
              cur_disk_offset += note_size;
              arena_stream.Write(temp_note, note_size);
              num_bytes_out += note_size;
              ++num_notes_out;
            } else {  // same note as previous
              remap_sorter.Emplace(merge_note.GetOffset(), prev_disk_offset);
            }
            ++csr;
            if (CanTail) {
              typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TArenaKeeperSorter::TCursor &filter_csr = *keeper_filter_csr_vec[pos];
              for (; csr && filter_csr && csr.GetOffset() < *filter_csr; ++csr) {}
              if (csr && filter_csr) {
                //std::cout << "csr.GetOffset() [" << csr.GetOffset() << "] vs. *filter_csr [" << *filter_csr << "]" << std::endl;
                assert(csr.GetOffset() == *filter_csr);
                std::pair<Atom::TCore::TNote *, size_t> &mem_note = temp_note_vec[pos];
                const Atom::TCore::TNote *const note = *csr;
                const size_t note_size = sizeof(Atom::TCore::TNote) + note->GetRawSize();
                const size_t cur_max_note_size = std::max(note_size, mem_note.second);
                if (cur_max_note_size > mem_note.second) {
                  mem_note.first = reinterpret_cast<Atom::TCore::TNote *>(realloc(mem_note.first, cur_max_note_size));
                  if (!mem_note.first) {
                    throw std::bad_alloc();
                  }
                  mem_note.second = cur_max_note_size;
                }
                memcpy(mem_note.first, note, note_size);
                my_pos = pos;
                mem_note.first->Remap(note_remapper);
                merge_note_vec[pos] = TMergeNote(my_arena, csr.GetOffset(), mem_note.first);
                min_heap.Insert(*merge_note_vec[pos].GetNote(), pos);
                AdvanceFilterCursor(filter_csr);
              }
            } else {
              if (csr) {
                std::pair<Atom::TCore::TNote *, size_t> &mem_note = temp_note_vec[pos];
                const Atom::TCore::TNote *const note = *csr;
                const size_t note_size = sizeof(Atom::TCore::TNote) + note->GetRawSize();
                const size_t cur_max_note_size = std::max(note_size, mem_note.second);
                if (cur_max_note_size > mem_note.second) {
                  mem_note.first = reinterpret_cast<Atom::TCore::TNote *>(realloc(mem_note.first, cur_max_note_size));
                  if (!mem_note.first) {
                    throw std::bad_alloc();
                  }
                  mem_note.second = cur_max_note_size;
                }
                memcpy(mem_note.first, note, note_size);
                my_pos = pos;
                mem_note.first->Remap(note_remapper);
                merge_note_vec[pos] = TMergeNote(my_arena, csr.GetOffset(), mem_note.first);
                min_heap.Insert(*merge_note_vec[pos].GetNote(), pos);
              }
            }
          }
          free(temp_note);
        } catch (const std::exception &/*ex*/) {
          free(temp_note);
          temp_note = nullptr;
          throw;
        }
        for (auto &mem_note : temp_note_vec) {
          free(mem_note.first);
          mem_note.first = nullptr;
        }
      } catch (const std::exception &ex) {
        for (auto &mem_note : temp_note_vec) {
          free(mem_note.first);
          mem_note.first = nullptr;
        }
        throw;
      }
    } catch (const std::exception &ex) {
      syslog(LOG_ERR, "MergeTypeRange caught error [%s]", ex.what());
      throw;
    }
  }

  /* TODO */
  inline size_t GetNumKeys() const {
    assert(this);
    return NumKeys;
  }

  /* TODO */
  inline TSequenceNumber GetLowestSequence() const {
    assert(this);
    return LowestSeq;
  }

  /* TODO */
  inline TSequenceNumber GetHighestSequence() const {
    assert(this);
    return HighestSeq;
  }

  private:

  /* TODO */
  class TSortedKey {
    public:

    /* TODO */
    TSortedKey() : Arena(nullptr), SeqNum(0UL) {}

    /* TODO */
    inline bool operator<(const TSortedKey &that) const {
      assert(this);
      Atom::TComparison comp;
      if (Arena && that.Arena && Core.TryQuickOrderComparison(Arena, that.Core, that.Arena, comp)) {
      } else {
        void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
        void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
        comp = Sabot::OrderStates(*Sabot::State::TAny::TWrapper(Core.NewState(Arena, lhs_state_alloc)),
                                  *Sabot::State::TAny::TWrapper(that.Core.NewState(that.Arena, rhs_state_alloc)));
      }
      return Atom::IsLt(comp) || (Atom::IsEq(comp) && SeqNum >= that.SeqNum);
    }

    /* TODO */
    Atom::TCore Core;
    Atom::TCore::TArena *Arena;
    TSequenceNumber SeqNum;

  };  // TSortedKey

  /* TODO */
  class TReader
      : public TReadFile<LogicalBlockSize, LogicalBlockSize, PhysicalBlockSize, PageCheckedBlock, true> {
    NO_COPY_SEMANTICS(TReader);
    public:

    static constexpr size_t PhysicalCachePageSize = PhysicalBlockSize / (LogicalBlockSize / LogicalBlockSize);

    using TArena = TDiskArena<LogicalBlockSize, LogicalBlockSize, PhysicalBlockSize, PageCheckedBlock, 128, true>;

    /* TODO */
    TReader(const Base::TCodeLocation &code_location /* DEBUG */, uint8_t util_src, TEngine *engine, const Base::TUuid &file_id, size_t gen_id)
        : TReadFile(code_location, util_src, engine, file_id, Low, gen_id) {
      Arena = std::unique_ptr<TArena>(new TArena(this, engine->GetCache<PhysicalCachePageSize>(), Low));
    }

    /* TODO */
    virtual ~TReader() {
      assert(this);
      Arena.reset();
    }

    /* TODO */
    using TReadFile::TIndexFile;

    /* TODO */
    using TReadFile::GetNumBytesOfArena;
    using TReadFile::GetNumUpdates;
    using TReadFile::GetNumArenaNotes;
    using TReadFile::GetGenId;
    using TReadFile::GetTypeBoundaryOffsetVec;
    using TReadFile::ForEachIndex;

    /* TODO */
    inline const std::unique_ptr<TArena> &GetArena() const {
      assert(this);
      assert(Arena);
      return Arena;
    }

    private:

    /* TODO */
    std::unique_ptr<TArena> Arena;

  };  // TReader

  class TMergeIndexFile
      : public TArenaInFile {
    NO_COPY_SEMANTICS(TMergeIndexFile);
    public:

    /*
    template <bool ScanAheadAllowed>
    using TDataDiskArena = typename TMergeDataFileImpl<CanTail, CanTailTombstones>::TDataDiskArena;
    */

    class THashObj {
      public:

      THashObj(const Atom::TCore &core, size_t hash, size_t offset)
          : Core(core), Hash(hash), Offset(offset) {}

      bool operator<(const THashObj &that) const {
        assert(this);
        assert(&that);
        return Hash < that.Hash;
      }

      Atom::TCore Core;
      size_t Hash;
      size_t Offset;

    };

    typedef Disk::Util::TIndexManager<THashObj, Disk::Util::SortBufSize, Disk::Util::SortBufMinParallelSize> THashCollector;

    TMergeIndexFile(const Base::TUuid &index_id,
                    TEngine *engine,
                    Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed,
                    size_t gen_id,
                    DiskPriority priority,
                    size_t max_block_cache_read_slots_allowed,
                    size_t temp_file_consol_thresh,
                    TBlockVec *block_vec,
                    TRemapIndex &main_arena_remap_index,
                    #ifndef NDEBUG
                    std::unordered_set<size_t> &written_block_set,
                    #endif
                    const TKey &example_key)
        : IndexId(index_id),
          Engine(engine),
          StorageSpeed(storage_speed),
          SorterStorageSpeed(TVolume::TDesc::TStorageSpeed::Slow),
          Priority(priority),
          MaxBlockCacheReadSlotsAllowed(max_block_cache_read_slots_allowed),
          TempFileConsolThresh(temp_file_consol_thresh),
          ExampleKey(example_key),
          ArenaByteOffset(0UL),
          NumArenaNotes(0UL),
          NumArenaBytes(0UL),
          MainArenaRemapIndex(main_arena_remap_index),
          BlockVec(block_vec),
          FileSize(0UL),
          NumCurKeys(0UL),
          NumHistKeys(0UL),
          EndOfHistoryStream(0UL),
          NumCurHistoryElem(0UL),
          ByteOffsetOfCurHistory(0UL),
          ByteOffsetOfIndexMeta(0UL),
          ByteOffsetOfKeyIndex(0UL),
          FirstKey(true),
          GenId(gen_id),
          #ifndef NDEBUG
          WrittenBlockSet(written_block_set),
          #endif
          WasPrepared(false) {
      KeyRemapper = std::bind(&TMergeIndexFile::RemapKey, this, std::placeholders::_1);
      ValRemapper = std::bind(&TMergeIndexFile::RemapVal, this, std::placeholders::_1);
      NumHashTables = *(ExampleKey.GetCore().TryGetElemCount());
    }

    virtual ~TMergeIndexFile() {
      assert(this);
      assert(WasPrepared);
      assert(KeyStream->GetOffset() <= MaxByteOffsetOfKeyStream);
      KeyStream.reset();
      /* write out the index meta-data */ {
        TDataOutStream meta_stream(HERE,
                                   Source::DataFileOther,
                                   Engine->GetVolMan(),
                                   ByteOffsetOfIndexMeta,
                                   *BlockVec,
                                   KeyCollisionMap,
                                   KeyTrigger,
                                   Priority,
                                   true /*do cache */,
                                   #ifndef NDEBUG
                                   WrittenBlockSet,
                                   #endif
                                   [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});

        meta_stream << ArenaByteOffset;  // Offset of Arena
        meta_stream << NumArenaNotes;  // number of arena notes
        meta_stream << NumArenaBytes;  // number of arena bytes
        meta_stream << ArenaTypeBoundaryOffsetVec.size();  // number of arena type boundaries
        meta_stream << NumCurKeys;  // # Current Keys
        meta_stream << NumHistKeys;  // # History Keys
        meta_stream << ByteOffsetOfKeyIndex;  // Current Key Offset
        meta_stream << NumHashTables;  // # of hash indexes (n)


        for (const auto &hash_table : NumHashFieldsByOffset) {
          meta_stream << hash_table.first << hash_table.second;
        }
        for (const auto &offset : ArenaTypeBoundaryOffsetVec) {
          meta_stream << offset;
        }
      }
      /* flush collision blocks */ {
        for (auto iter : KeyCollisionMap) {
          assert(iter.first < BlockVec->Size());
          #ifndef NDEBUG
          WrittenBlockSet.insert((*BlockVec)[iter.first]);
          #endif
          Engine->GetVolMan()->WriteBlock(HERE,
                                          Disk::Util::PageCheckedBlock,
                                          Source::MergeDataFileOther,
                                          iter.second->GetData(),
                                          (*BlockVec)[iter.first],
                                          Priority,
                                          Disk::Util::CacheAll,
                                          KeyTrigger);
        }
      }
      KeyTrigger.Wait();
    }

    void ConstructArena(const std::vector<std::vector<size_t>> &type_boundary_offset_vec,
                        size_t max_total_note_bytes) {
      ArenaByteOffset = MakeArena(Engine,
                                  StorageSpeed,
                                  SorterStorageSpeed,
                                  Priority,
                                  MaxBlockCacheReadSlotsAllowed,
                                  TempFileConsolThresh,
                                  DiskArenaVec,
                                  type_boundary_offset_vec,
                                  ArenaTypeBoundaryOffsetVec,
                                  ArenaRemapSorterVec,
                                  ArenaKeeperVec,
                                  *BlockVec,
                                  #ifndef NDEBUG
                                  WrittenBlockSet,
                                  #endif
                                  max_total_note_bytes,
                                  NumArenaNotes,
                                  NumArenaBytes);
      FileSize = ArenaByteOffset + NumArenaBytes;
      MyArena = std::unique_ptr<TDataDiskArena<true>>(new TDataDiskArena<true>(this, Engine->GetCache<TDataDiskArena<true>::PhysicalCachePageSize>(), Priority));
    }

    void PrepKeyRange(size_t max_keys, TUpdateCollector *update_collector) {
      assert(this);
      UpdateCollector = update_collector;
      for (size_t i = 0; i < NumHashTables; ++i) {
        HashCollectorVec.emplace_back(new THashCollector(HERE, Source::MergeDataFileHashIndex, TempFileConsolThresh, SorterStorageSpeed, Engine, true));
      }
      const size_t bytes_of_metadata = (TData::NumIndexMetaFields * sizeof(size_t)) + (NumHashTables * sizeof(size_t) * 2UL) + (ArenaTypeBoundaryOffsetVec.size() * sizeof(size_t));
      if (bytes_of_metadata >= LogicalBlockSize) {
        throw std::runtime_error("Index metadata >= 1 block");
      }
      size_t max_blocks_required = ceil(static_cast<double>((max_keys * TData::KeyEntrySize) + bytes_of_metadata) / LogicalBlockSize);
      ByteOffsetOfIndexMeta = BlockVec->Size() * LogicalBlockSize;
      ByteOffsetOfKeyIndex = ByteOffsetOfIndexMeta + bytes_of_metadata;
      MaxByteOffsetOfKeyStream = ByteOffsetOfKeyIndex + (max_keys * TData::KeyEntrySize);

      /* collision at beginning of this key range. We're going to leave some room to include this index's metadata */
      auto ret = KeyCollisionMap.insert(std::make_pair(ByteOffsetOfIndexMeta / LogicalBlockSize, nullptr));
      if (ret.second) { // fresh insert
        ret.first->second = std::shared_ptr<const TBufBlock>(new TBufBlock());
      }

      const size_t total_num_blocks = BlockVec->Size() + max_blocks_required;
      Engine->AppendReserveBlocks(StorageSpeed, max_blocks_required, *BlockVec);
      assert(BlockVec->Size() == total_num_blocks);
      assert(((MaxByteOffsetOfKeyStream - 1) / LogicalBlockSize) < BlockVec->Size());
      KeyStream = std::unique_ptr<TDataOutStream>(new TDataOutStream(HERE,
                                                                     Source::DataFileKey,
                                                                     Engine->GetVolMan(),
                                                                     ByteOffsetOfKeyIndex,
                                                                     *BlockVec,
                                                                     KeyCollisionMap,
                                                                     KeyTrigger,
                                                                     Priority,
                                                                     true /* do_cache */,
                                                                     #ifndef NDEBUG
                                                                     WrittenBlockSet,
                                                                     #endif
                                                                     [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;}));
      WasPrepared = true;
    }

    void PushCurKey(TSequenceNumber seq_num, const Atom::TCore &key, const Atom::TCore &val) {
      assert(this);
      ++NumCurKeys;
      TDataOutStream &stream = *KeyStream;
      if (!FirstKey) {
        stream << NumCurHistoryElem << ByteOffsetOfCurHistory;
        ByteOffsetOfCurHistory += NumCurHistoryElem * TData::KeyHistorySize;
        NumCurHistoryElem = 0UL;
      } else {
        FirstKey = false;
      }
      CurKeyOffset = stream.GetOffset();
      stream << seq_num;
      stream.Write(&key, sizeof(key));
      stream.Write(&val, sizeof(val));
      assert(stream.GetOffset() <= MaxByteOffsetOfKeyStream);

      Atom::TCore prefix_core = key;
      Atom::TCore::TArena *const prefix_arena = MyArena.get();

      void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
      #ifndef NDEBUG
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      #endif
      do {
        const TKey prefix_key(prefix_core, prefix_arena);
        const uint64_t prefix_size = Sabot::GetTupleSize(*Sabot::Type::TAny::TWrapper(prefix_core.GetType(prefix_arena, type_alloc)));
        auto ret = SeenPrefix.insert(std::make_pair(prefix_size, prefix_key));
        bool is_new = ret.second;
        if (!is_new) {
          TKey &cur_prefix = ret.first->second;
          if (cur_prefix != prefix_key) {
            assert(Atom::IsLt(cur_prefix.Compare(prefix_key)));
            cur_prefix = prefix_key;
            is_new = true;
          }
        }
        if (is_new) {
          assert(prefix_key.GetHash() == Sabot::GetHash(*Sabot::State::TAny::TWrapper(prefix_core.NewState(prefix_arena, state_alloc))));
          size_t prefix_hash = prefix_key.GetHash();
          prefix_core.TrySetStoredHash(prefix_hash);
          HashCollectorVec[prefix_size - 1]->Emplace(prefix_core, prefix_hash, CurKeyOffset);
        } else {
          break;
        }

      } while (prefix_core.TryTruncateTuple());
      UpdateCollector->Emplace(seq_num, CurKeyOffset, true, IndexId);
    }

    void PushHistKey(TSequenceNumber seq_num, const Atom::TCore &key, const Atom::TCore &val) {
      assert(this);
      ++NumHistKeys;
      HistKeyVec.emplace_back(seq_num, key, val);
      UpdateCollector->Emplace(seq_num, ByteOffsetOfCurHistory + NumCurHistoryElem * TData::KeyHistorySize, false, IndexId);
      ++NumCurHistoryElem;
    }

    void FlushHistory() {
      assert(this);
      TDataOutStream &stream = *KeyStream;
      if (!FirstKey) {
        stream << NumCurHistoryElem << ByteOffsetOfCurHistory;
      }
      ByteOffsetOfHistory = stream.GetOffset();
      for (const auto &hist : HistKeyVec) {
        stream << hist.SeqNum;
        stream.Write(&hist.Key, sizeof(hist.Key));
        stream.Write(&hist.Val, sizeof(hist.Val));
        assert(stream.GetOffset() <= MaxByteOffsetOfKeyStream);
      }
      EndOfHistoryStream = stream.GetOffset();
      MyArena.reset();
    }

    void FlushHashes() {
      assert(this);
      size_t total_bytes_required = 0UL;
      std::unordered_map<size_t, std::shared_ptr<const TBufBlock>> collision_map {};
      size_t hash_index_byte_offset = BlockVec->Size() * LogicalBlockSize;
      for (const auto &collection : HashCollectorVec) {
        /* collision at beginning of this hash index */
        auto ret = collision_map.insert(std::make_pair((hash_index_byte_offset + total_bytes_required) / LogicalBlockSize, nullptr));
        if (ret.second) { // fresh insert
          ret.first->second = std::shared_ptr<const TBufBlock>(new TBufBlock());
        }
        size_t num_hash_fields = Disk::Util::SuggestHashSize(collection->GetSize());
        NumHashFieldsByOffset.emplace_back(hash_index_byte_offset + total_bytes_required, num_hash_fields);
        total_bytes_required += num_hash_fields * TDataFile::HashEntrySize;
        /* collision at end of this hash index (if the block doesn't get filled completely) */
        if ((hash_index_byte_offset + total_bytes_required) % LogicalBlockSize != 0) {
          ret = collision_map.insert(std::make_pair((hash_index_byte_offset + total_bytes_required) / LogicalBlockSize, nullptr));
          if (ret.second) { // fresh insert
            ret.first->second = std::shared_ptr<const TBufBlock>(new TBufBlock());
          }
        }
      }
      assert(NumHashFieldsByOffset.size() == NumHashTables);
      size_t max_blocks_required = ceil(static_cast<double>(total_bytes_required) / LogicalBlockSize);
      const size_t total_num_blocks_required = BlockVec->Size() + max_blocks_required;
      Engine->AppendReserveBlocks(StorageSpeed, max_blocks_required, *BlockVec);
      assert(BlockVec->Size() == total_num_blocks_required);
      #ifndef NDEBUG
      /* check that each collision block offset actually exists in the allocated space */
      for (auto iter : collision_map) {
        assert(iter.first < BlockVec->Size());
      }
      #endif
      TCompletionTrigger completion_trigger;
      for (const auto &collection : HashCollectorVec) {
        size_t num_hash_fields = Disk::Util::SuggestHashSize(collection->GetSize());
        /* make a new index with the hashes modded by the hash field size */
        THashCollector modded_hash_collector(HERE, Source::DataFileHashIndex, TempFileConsolThresh, SorterStorageSpeed, Engine, true);
        for (typename THashCollector::TCursor orig_csr(collection.get(), MaxBlockCacheReadSlotsAllowed); orig_csr; ++orig_csr) {
          const THashObj &obj = *orig_csr;
          modded_hash_collector.Emplace(obj.Core, obj.Hash % num_hash_fields, obj.Offset);
        }
        typename THashCollector::TCursor hash_csr(&modded_hash_collector, MaxBlockCacheReadSlotsAllowed);
        /* do the first pass over the hash map */ {
          assert(hash_index_byte_offset + num_hash_fields * TData::HashEntrySize <= BlockVec->Size() * LogicalBlockSize);
          TDataOutStream stream(HERE,
                                Source::MergeDataFileHashIndex,
                                Engine->GetVolMan(),
                                hash_index_byte_offset,
                                *BlockVec,
                                collision_map,
                                completion_trigger,
                                Priority,
                                true /* do_cache */,
                                #ifndef NDEBUG
                                WrittenBlockSet,
                                #endif
                                [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
          for (size_t i = 0; i < num_hash_fields; ++i) {
            if (hash_csr && (*hash_csr).Hash <= i) {
              const THashObj &obj = *hash_csr;
              assert(obj.Core.IsTuple());
              stream.Write(&obj.Core, sizeof(Atom::TCore));
              stream << obj.Offset;
              ++hash_csr;
            } else {
              stream.Write(&TData::NullCore, sizeof(Atom::TCore));
              stream << 0UL;
            }
          }
          FileSize = stream.GetOffset();
        }
        /* do the second pass if it is required */ {
          if (hash_csr) {
            /* flush the collision blocks surrounding the hash index */ {
              const size_t collision_block_pos = hash_index_byte_offset / Disk::Util::LogicalBlockSize;
              assert(collision_block_pos < BlockVec->Size());
              assert(collision_map.find(collision_block_pos) != collision_map.end());
              #ifndef NDEBUG
              WrittenBlockSet.insert((*BlockVec)[collision_block_pos]);
              #endif
              Engine->GetVolMan()->WriteBlock(HERE,
                                              Disk::Util::PageCheckedBlock,
                                              Source::MergeDataFileHash,
                                              collision_map[collision_block_pos]->GetData(),
                                              (*BlockVec)[collision_block_pos],
                                              Priority,
                                              Disk::Util::CacheAll,
                                              completion_trigger);
              const size_t byte_offset_of_end = hash_index_byte_offset + num_hash_fields * TDataFile::HashEntrySize;
              const size_t end_block_pos = byte_offset_of_end / LogicalBlockSize;
              if ((collision_block_pos != end_block_pos) && (end_block_pos < BlockVec->Size())) {
                assert(end_block_pos < BlockVec->Size());
                assert(collision_map.find(end_block_pos) != collision_map.end());
                #ifndef NDEBUG
                WrittenBlockSet.insert((*BlockVec)[end_block_pos]);
                #endif
                Engine->GetVolMan()->WriteBlock(HERE,
                                                Disk::Util::PageCheckedBlock,
                                                Source::MergeDataFileHash,
                                                collision_map[end_block_pos]->GetData(),
                                                (*BlockVec)[end_block_pos],
                                                Priority,
                                                Disk::Util::CacheAll,
                                                completion_trigger);
              }
            }
            /* wait for the first pass to finish flushing before doing the wrap around pass */ {
              completion_trigger.Wait();
            }
            TDataOutStream out_stream(HERE,
                                      Source::MergeDataFileHashIndex,
                                      Engine->GetVolMan(),
                                      hash_index_byte_offset,
                                      *BlockVec,
                                      collision_map,
                                      completion_trigger,
                                      Priority,
                                      true /* do_cache */,
                                      #ifndef NDEBUG
                                      WrittenBlockSet,
                                      #endif
                                      [/* causing ICE if left to default */](Disk::Util::TVolumeManager *){throw; return 0U;});
            TDataInStream<0UL> in_stream(HERE, Source::DataFileHash, Priority, this, Engine->GetCache<TDataInStream<0UL>::PhysicalCachePageSize>(), hash_index_byte_offset);
            Atom::TCore core;
            size_t key_ptr = 0;
            size_t i = 0;
            for (; i < num_hash_fields; ++i) {
              in_stream.Read(&core, sizeof(Atom::TCore));
              in_stream.Read(key_ptr);
              if (memcmp(&core, &TData::NullCore, sizeof(Atom::TCore)) == 0) {
                assert(hash_csr);
                const THashObj &obj = *hash_csr;
                assert(obj.Core.IsTuple());
                out_stream.Write(&obj.Core, sizeof(Atom::TCore));
                out_stream << obj.Offset;
                ++hash_csr;
              } else {
                out_stream.Write(&core, sizeof(Atom::TCore));
                out_stream << key_ptr;
              }
              if (!hash_csr) {
                break;
              }
            }
            size_t left = std::min(Disk::Util::LogicalBlockSize - in_stream.GetOffset() % LogicalBlockSize, FileSize - in_stream.GetOffset());
            char buf[left];
            in_stream.Read(buf, left);
            out_stream.Write(buf, left);
            assert(!hash_csr);
          }
        }
        hash_index_byte_offset += num_hash_fields * TDataFile::HashEntrySize;
      }
      /* flush collision blocks */ {
        for (auto iter : collision_map) {
          assert(iter.first < BlockVec->Size());
          #ifndef NDEBUG
          WrittenBlockSet.insert((*BlockVec)[iter.first]);
          #endif
          Engine->GetVolMan()->WriteBlock(HERE,
                                          Disk::Util::PageCheckedBlock,
                                          Source::MergeDataFileOther,
                                          iter.second->GetData(),
                                          (*BlockVec)[iter.first],
                                          Priority,
                                          Disk::Util::CacheAll,
                                          completion_trigger);
        }
        completion_trigger.Wait();
      }
      HashCollectorVec.clear();
    }

    std::vector<std::unique_ptr<typename TReader::TIndexFile>> SourceFileVec;
    std::vector<size_t> OrigReadVecPos;
    std::vector<std::unique_ptr<TDataDiskArena<true>>> DiskArenaVec;
    std::vector<std::vector<size_t>> TypeBoundaryOffsetVec;
    size_t MaxArenaBytes;

    private:

    class THistKey {
      public:

      THistKey(TSequenceNumber seq_num, const Atom::TCore &key, const Atom::TCore &val)
          : SeqNum(seq_num),
            Key(key),
            Val(val) {}

      TSequenceNumber SeqNum;
      Atom::TCore Key;
      Atom::TCore Val;

    };

    Base::TUuid IndexId;

    TEngine *Engine;

    TVolume::TDesc::TStorageSpeed StorageSpeed;
    TVolume::TDesc::TStorageSpeed SorterStorageSpeed;

    DiskPriority Priority;

    size_t MaxBlockCacheReadSlotsAllowed;

    size_t TempFileConsolThresh;

    TKey ExampleKey;

    Atom::TCore::TOffset RemapKey(Atom::TCore::TOffset offset) {
      assert(this);
      /* TODO: should we keep the old offset to make sure we've got the right one? */
      const TRemapResolvedObj &obj = **ResolvedKeySorterCursorVec[MyPos];
      assert(offset == obj.OldKey);
      Atom::TCore::TOffset ret = obj.NewKey;
      assert(ResolvedKeySorterCursorVec[MyPos]);
      ++(*ResolvedKeySorterCursorVec[MyPos]);
      return ret;
    }

    Atom::TCore::TOffset RemapVal(Atom::TCore::TOffset offset) {
      assert(this);
      /* TODO: should we keep the old offset to make sure we've got the right one? */
      const TRemapResolvedObj &obj = **ResolvedValSorterCursorVec[MyPos];
      assert(offset == obj.OldKey);
      Atom::TCore::TOffset ret = obj.NewKey;
      assert(ResolvedValSorterCursorVec[MyPos]);
      ++(*ResolvedValSorterCursorVec[MyPos]);
      return ret;
    }

    size_t ArenaByteOffset;
    size_t NumArenaNotes;
    size_t NumArenaBytes;
    std::vector<std::unique_ptr<TArenaKeeperSorter>> ArenaKeeperVec;
    std::vector<std::unique_ptr<TRemapSorter>> ArenaRemapSorterVec;
    TDataFile::TTypeBoundaryOffsetVec ArenaTypeBoundaryOffsetVec;
    std::unique_ptr<TDataDiskArena<true>> MyArena;

    TRemapIndex &MainArenaRemapIndex;
    TBlockVec *BlockVec;
    size_t FileSize;

    size_t MyPos; /* index into ResolvedKeySorterCursorVec, ResolvedValSorterCursorVec */
    std::vector<std::unique_ptr<typename TRemapResolvedSorter::TCursor>> ResolvedKeySorterCursorVec;
    std::vector<std::unique_ptr<typename TRemapResolvedSorter::TCursor>> ResolvedValSorterCursorVec;
    size_t NumCurKeys;
    size_t NumHistKeys;
    size_t EndOfHistoryStream;
    std::vector<THistKey> HistKeyVec;
    size_t NumCurHistoryElem;
    size_t ByteOffsetOfCurHistory;

    size_t ByteOffsetOfIndexMeta;
    size_t ByteOffsetOfKeyIndex;

    size_t NumHashTables;

    bool FirstKey;
    size_t CurKeyOffset;
    std::unique_ptr<TDataOutStream> KeyStream;
    TCompletionTrigger KeyTrigger;
    std::unordered_map<size_t, std::shared_ptr<const TBufBlock>> KeyCollisionMap;
    size_t ByteOffsetOfHistory;
    std::vector<std::vector<bool>> HistoryKeeperFilterVec;

    std::vector<std::unique_ptr<THashCollector>> HashCollectorVec;
    std::unordered_map<uint64_t, TKey> SeenPrefix;

    std::vector<std::pair<size_t, size_t>> NumHashFieldsByOffset;

    TUpdateCollector *UpdateCollector;

    size_t GenId;

    #ifndef NDEBUG
    std::unordered_set<size_t> &WrittenBlockSet;
    #endif

    bool WasPrepared;
    size_t MaxByteOffsetOfKeyStream;

    virtual size_t GetFileLength() const override {
      return FileSize;
      /* we can't do this because we may not have written this far yet. this would cause problems with prefetch or checkbuf()
      return BlockVec->size() * Disk::Util::LogicalBlockSize;
      */
    }

    virtual size_t GetStartingBlock() const override {
      throw std::logic_error("TODO: implement TMergeIndexFile::GetStartingBlock");
    }

    virtual void ReadMeta(size_t /*offset*/, size_t &/*out*/) const override {
      throw std::logic_error("TODO: implement TMergeIndexFile::ReadMeta");
    }

    virtual size_t FindPageIdOfByte(size_t offset) const override {
      assert(offset <= GetFileLength());
      //return (((*BlockVec)[offset / Disk::Util::LogicalBlockSize]) * Disk::Util::PagesPerBlock) + ((offset % Disk::Util::LogicalBlockSize) / Disk::Util::LogicalPageSize);
      return (*BlockVec)[offset / Disk::Util::LogicalBlockSize];
    }

    virtual size_t GetByteOffsetOfArena() const override {
      assert(this);
      return ArenaByteOffset;
    }

    virtual size_t GetNumArenaNotes() const override {
      assert(this);
      return NumArenaNotes;
    }

    virtual Atom::TCore::TOffset GetNumBytesOfArena() const override {
      assert(this);
      return NumArenaBytes;
    }

    public:

    std::function<Atom::TCore::TOffset(Atom::TCore::TOffset)> KeyRemapper;

    std::function<Atom::TCore::TOffset(Atom::TCore::TOffset)> ValRemapper;

    friend class TMergeDataFileImpl<CanTail, CanTailTombstones>;

  };

  /* TODO */
  TEngine *Engine;

  /* TODO */
  Disk::Util::TVolume::TDesc::TStorageSpeed StorageSpeed;
  Disk::Util::TVolume::TDesc::TStorageSpeed UpdateIndexStorageSpeed;
  Disk::Util::TVolume::TDesc::TStorageSpeed SorterStorageSpeed;

  /* TODO */
  DiskPriority Priority;

  /* TODO */
  size_t MaxBlockCacheReadSlotsAllowed;

  /* TODO */
  TBlockVec BlockVec;

  /* TODO */
  size_t MainArenaByteOffset;
  TRemapIndex MainArenaRemapIndex;
  TTypeBoundaryOffsetVec MainArenaTypeBoundaryOffsetVec;

  /* TODO */
  size_t NumUpdates;

  /* TODO */
  size_t NumKeys;
  TSequenceNumber LowestSeq;
  TSequenceNumber HighestSeq;

  /* TODO */
  size_t StartingBlockId;

  /* TODO */
  size_t StartingBlockOffset;

  /* TODO */
  size_t FileLength;

  size_t TempFileConsolThresh;

  /* TODO */
  std::vector<std::unique_ptr<TReader>> ReadFileVec;

  /* TODO */
  TUpdateCollector UpdateCollector;

  /* TODO */
  #ifndef NDEBUG
  std::unordered_set<size_t> WrittenBlockSet;
  #endif

  /* TODO */
  friend class TMergeIndexFile;

};  // TMergeDataFileImpl

TMergeDataFile::TMergeDataFile(Util::TEngine *engine,
                               Disk::Util::TVolume::TDesc::TStorageSpeed storage_speed,
                               const Base::TUuid &file_uuid,
                               const std::vector<size_t> &gen_vec,
                               const Base::TUuid &file_uid,
                               size_t gen_id,
                               TSequenceNumber release_up_to,
                               DiskPriority priority,
                               size_t max_block_cache_read_slots_allowed,
                               size_t temp_file_consol_thresh,
                               bool can_tail,
                               bool can_tail_tombstone) {
  if (can_tail) {
    if (can_tail_tombstone) {
      TMergeDataFileImpl<true, true> merge_file(engine, storage_speed, file_uuid, gen_vec, file_uid, gen_id, release_up_to, priority, max_block_cache_read_slots_allowed, temp_file_consol_thresh);
      NumKeys = merge_file.GetNumKeys();
      LowestSeq = merge_file.GetLowestSequence();
      HighestSeq = merge_file.GetHighestSequence();
    } else {
      TMergeDataFileImpl<true, false> merge_file(engine, storage_speed, file_uuid, gen_vec, file_uid, gen_id, release_up_to, priority, max_block_cache_read_slots_allowed, temp_file_consol_thresh);
      NumKeys = merge_file.GetNumKeys();
      LowestSeq = merge_file.GetLowestSequence();
      HighestSeq = merge_file.GetHighestSequence();
    }
  } else {
    TMergeDataFileImpl<false, false> merge_file(engine, storage_speed, file_uuid, gen_vec, file_uid, gen_id, release_up_to, priority, max_block_cache_read_slots_allowed, temp_file_consol_thresh);
    NumKeys = merge_file.GetNumKeys();
    LowestSeq = merge_file.GetLowestSequence();
    HighestSeq = merge_file.GetHighestSequence();
    assert(!can_tail_tombstone);
  }
}