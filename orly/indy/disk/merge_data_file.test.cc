/* <orly/indy/disk/merge_data_file.test.cc>

   Unit test for <orly/indy/disk/merge_data_file.h>.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/indy/disk/merge_data_file.h>

#include <valgrind/callgrind.h>

#include <base/scheduler.h>
#include <orly/indy/disk/data_file.h>
#include <orly/indy/disk/disk_test.h>
#include <orly/indy/disk/read_file.h>
#include <orly/indy/disk/sim/mem_engine.h>
#include <orly/indy/fiber/fiber_test_runner.h>

#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Orly;
using namespace Orly::Atom;
using namespace Orly::Indy;
using namespace Orly::Indy::Disk;
using namespace Orly::Indy::Disk::Util;
using namespace Orly::Indy::Fiber;

static const size_t BlockSize = Disk::Util::PhysicalBlockSize;

Orly::Indy::Util::TPool L0::TManager::TRepo::TMapping::Pool(sizeof(TRepo::TMapping), "Repo Mapping");
Orly::Indy::Util::TPool L0::TManager::TRepo::TMapping::TEntry::Pool(sizeof(TRepo::TMapping::TEntry), "Repo Mapping Entry");
Orly::Indy::Util::TPool L0::TManager::TRepo::TDataLayer::Pool(sizeof(TMemoryLayer), "Data Layer");

Orly::Indy::Util::TPool TUpdate::Pool(sizeof(TUpdate), "Update", 1048578UL);
Orly::Indy::Util::TPool TUpdate::TEntry::Pool(sizeof(TUpdate::TEntry), "Entry", 1048578UL);
Disk::TBufBlock::TPool Disk::TBufBlock::Pool(BlockSize, 2000UL);

FIXTURE(BasicTailing) {
  TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    TScheduler scheduler(TScheduler::TPolicy(4, 10, milliseconds(10)));

    Sim::TMemEngine mem_engine(&scheduler,
                               256 /* disk space: 256MB */,
                               256 /* slow disk space: 256MB */,
                               16384 /* page cache slots: 64MB */,
                               1 /* num page lru */,
                               1024 /* block cache slots: 64MB */,
                               1 /* num block lru */);

    Base::TUuid file_id(TUuid::Best);
    TSequenceNumber seq_num = 0U;
    TUuid index_id(TUuid::Twister);
    /* Make data file 1 */ {
      TSuprena arena;
      TMockMem mem_layer;
      /* insert data */ {
        TSuprena suprena;
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        /* insert <[int64_t, string, desc<int64_t>, desc<string>]> */
        Insert(mem_layer, ++seq_num, index_id, TKey(46L, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("short"));
        Insert(mem_layer, ++seq_num, index_id, TKey(49L, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
        Insert(mem_layer, ++seq_num, index_id, TKey(57L, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
      }
      size_t data_gen_id = 1;
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, data_gen_id, 20UL, 0U, Medium);
    }
    /* Make data file 2 (more of the same) */ {
      TSuprena arena;
      TMockMem mem_layer;
      /* insert data */ {
        TSuprena suprena;
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        /* insert <[int64_t, string, desc<int64_t>, desc<string>]> */
        Insert(mem_layer, ++seq_num, index_id, TKey(7L, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("short"));
        Insert(mem_layer, ++seq_num, index_id, TKey(409L, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one"));
        Insert(mem_layer, ++seq_num, index_id, TKey(Native::TTombstone::Tombstone, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
      }
      size_t data_gen_id = 2;
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, data_gen_id, 20UL, 0U, Medium);
    }
    /* merge them. Here seq (4,5,6) should be visible */ {
      TSuprena arena;
      TMergeDataFile merge_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, file_id, vector<size_t>{1, 2}, file_id, 4UL, 0U, Low, 16384, 20UL, true, false);
      TReader reader(HERE, mem_engine.GetEngine(), file_id, 4UL);
      TReader::TArena main_arena(&reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TReader::TIndexFile idx_file(&reader, index_id, RealTime);
      TReader::TArena index_arena(&idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TStream<Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::PhysicalBlockSize, Orly::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
      size_t out_offset;
      /* what came from file 1 but got overriden in 2 */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      /* this has been tombstoned in file 2, but because it had history it's still visible as a tombstone */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      /* this got added in file 2 */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      vector<pair<TKey, TKey>> expected_vec;
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc),
                                TKey(7L, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                TKey(Native::TTombstone::Tombstone, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc),
                                TKey(409L, &arena, state_alloc));
      size_t seen = 0UL;
      for (TReader::TIndexFile::TKeyCursor cur_key_csr(&idx_file); cur_key_csr; ++cur_key_csr, ++seen) {
        const TReader::TIndexFile::TKeyItem &item = *cur_key_csr;
        EXPECT_TRUE(cur_key_csr);
        if (EXPECT_GE(expected_vec.size(), seen + 1)) {
          EXPECT_EQ(TKey(item.Key, &index_arena), expected_vec[seen].first);
          EXPECT_EQ(TKey(item.Value, &main_arena), expected_vec[seen].second);
        }
      }
      EXPECT_EQ(expected_vec.size(), seen);
    }
    /* merge it again to get rid of the remaining history behind the tombstone. Here seq (4,5,6) should be visible */ {
      TSuprena arena;
      TMergeDataFile merge_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, file_id, vector<size_t>{4}, file_id, 5UL, 0U, Low, 16384, 20UL, true, true);
      TReader reader(HERE, mem_engine.GetEngine(), file_id, 5UL);
      TReader::TArena main_arena(&reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TReader::TIndexFile idx_file(&reader, index_id, RealTime);
      TReader::TArena index_arena(&idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TStream<Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::PhysicalBlockSize, Orly::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
      size_t out_offset;
      /* what came from file 1 but got overriden in 2 */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      /* this has been tombstoned in file 2, but because it had history it's still visible as a tombstone */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      /* this got added in file 2 */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      vector<pair<TKey, TKey>> expected_vec;
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc),
                                TKey(7L, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                TKey(Native::TTombstone::Tombstone, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc),
                                TKey(409L, &arena, state_alloc));
      size_t seen = 0UL;
      for (TReader::TIndexFile::TKeyCursor cur_key_csr(&idx_file); cur_key_csr; ++cur_key_csr, ++seen) {
        const TReader::TIndexFile::TKeyItem &item = *cur_key_csr;
        EXPECT_TRUE(cur_key_csr);
        if (EXPECT_GE(expected_vec.size(), seen + 1)) {
          EXPECT_EQ(TKey(item.Key, &index_arena), expected_vec[seen].first);
          EXPECT_EQ(TKey(item.Value, &main_arena), expected_vec[seen].second);
        }
      }
      EXPECT_EQ(expected_vec.size(), seen);
    }
    /* merge it again to get rid of the tombstone. Here seq (4,5) should be visible */ {
      TSuprena arena;
      TMergeDataFile merge_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, file_id, vector<size_t>{5}, file_id, 6UL, 0U, Low, 16384, 20UL, true, true);
      TReader reader(HERE, mem_engine.GetEngine(), file_id, 6UL);
      TReader::TArena main_arena(&reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TReader::TIndexFile idx_file(&reader, index_id, RealTime);
      TReader::TArena index_arena(&idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TStream<Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::PhysicalBlockSize, Orly::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
      size_t out_offset;
      /* what came from file 1 but got overriden in 2 */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      /* this has been tombstoned in file 2. */
      EXPECT_FALSE(idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      /* this got added in file 2 */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      vector<pair<TKey, TKey>> expected_vec;
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc),
                                TKey(7L, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc),
                                TKey(409L, &arena, state_alloc));
      size_t seen = 0UL;
      for (TReader::TIndexFile::TKeyCursor cur_key_csr(&idx_file); cur_key_csr; ++cur_key_csr, ++seen) {
        const TReader::TIndexFile::TKeyItem &item = *cur_key_csr;
        EXPECT_TRUE(cur_key_csr);
        if (EXPECT_GE(expected_vec.size(), seen + 1)) {
          EXPECT_EQ(TKey(item.Key, &index_arena), expected_vec[seen].first);
          EXPECT_EQ(TKey(item.Value, &main_arena), expected_vec[seen].second);
        }
      }
      EXPECT_EQ(expected_vec.size(), seen);
    }
    GracefullShutdown();
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  });
}

FIXTURE(BasicTailingDisabled) {
  TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    TScheduler scheduler(TScheduler::TPolicy(4, 10, milliseconds(10)));

    Sim::TMemEngine mem_engine(&scheduler,
                               256 /* disk space: 256MB */,
                               256 /* slow disk space: 256MB */,
                               16384 /* page cache slots: 64MB */,
                               1 /* num page lru */,
                               1024 /* block cache slots: 64MB */,
                               1 /* num block lru */);

    Base::TUuid file_id(TUuid::Best);
    TSequenceNumber seq_num = 0U;
    TUuid index_id(TUuid::Twister);
    /* Make data file 1 */ {
      TSuprena arena;
      TMockMem mem_layer;
      /* insert data */ {
        TSuprena suprena;
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        /* insert <[int64_t, string, desc<int64_t>, desc<string>]> */
        Insert(mem_layer, ++seq_num, index_id, TKey(46L, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("short"));
        Insert(mem_layer, ++seq_num, index_id, TKey(49L, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
        Insert(mem_layer, ++seq_num, index_id, TKey(57L, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
      }
      size_t data_gen_id = 1;
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, data_gen_id, 20UL, 0U, Medium);
    }
    /* Make data file 2 (more of the same) */ {
      TSuprena arena;
      TMockMem mem_layer;
      /* insert data */ {
        TSuprena suprena;
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        /* insert <[int64_t, string, desc<int64_t>, desc<string>]> */
        Insert(mem_layer, ++seq_num, index_id, TKey(7L, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("short"));
        Insert(mem_layer, ++seq_num, index_id, TKey(409L, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one"));
        Insert(mem_layer, ++seq_num, index_id, TKey(Native::TTombstone::Tombstone, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
      }
      size_t data_gen_id = 2;
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, data_gen_id, 20UL, 0U, Medium);
    }
    /* merge them. Here seq (4,5,6) should be visible */ {
      TSuprena arena;
      TMergeDataFile merge_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, file_id, vector<size_t>{1, 2}, file_id, 4UL, 0U, Low, 16384, 20UL, false, false);
      TReader reader(HERE, mem_engine.GetEngine(), file_id, 4UL);
      TReader::TArena main_arena(&reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TReader::TIndexFile idx_file(&reader, index_id, RealTime);
      TReader::TArena index_arena(&idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TStream<Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::PhysicalBlockSize, Orly::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
      size_t out_offset;
      /* what came from file 1 but got overriden in 2 */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      /* this has been tombstoned in file 2, but because it had history it's still visible as a tombstone */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      /* this got added in file 2 */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      vector<pair<TKey, TKey>> expected_vec;
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc),
                                TKey(7L, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                TKey(Native::TTombstone::Tombstone, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc),
                                TKey(409L, &arena, state_alloc));
      size_t seen = 0UL;
      for (TReader::TIndexFile::TKeyCursor cur_key_csr(&idx_file); cur_key_csr; ++cur_key_csr, ++seen) {
        const TReader::TIndexFile::TKeyItem &item = *cur_key_csr;
        EXPECT_TRUE(cur_key_csr);
        if (EXPECT_GE(expected_vec.size(), seen + 1)) {
          EXPECT_EQ(TKey(item.Key, &index_arena), expected_vec[seen].first);
          EXPECT_EQ(TKey(item.Value, &main_arena), expected_vec[seen].second);
        }
      }
      EXPECT_EQ(expected_vec.size(), seen);
    }
    for (size_t i = 0; i < 3; ++i) {
      /* merge it again to get make sure nothing changed. Here seq (4,5,6) should be visible */ {
        TSuprena arena;
        TMergeDataFile merge_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, file_id, vector<size_t>{(4 + i)}, file_id, 5UL + i, 0U, Low, 16384, 20UL, false, false);
        TReader reader(HERE, mem_engine.GetEngine(), file_id, 5UL);
        TReader::TArena main_arena(&reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
        TReader::TIndexFile idx_file(&reader, index_id, RealTime);
        TReader::TArena index_arena(&idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
        TStream<Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::PhysicalBlockSize, Orly::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
        size_t out_offset;
        /* what came from file 1 but got overriden in 2 */
        EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &index_arena));
        /* this has been tombstoned in file 2, but because it had history it's still visible as a tombstone */
        EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &index_arena));
        /* this got added in file 2 */
        EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc), out_offset, in_stream, &index_arena));
        vector<pair<TKey, TKey>> expected_vec;
        expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc),
                                  TKey(7L, &arena, state_alloc));
        expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                  TKey(Native::TTombstone::Tombstone, &arena, state_alloc));
        expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc),
                                  TKey(409L, &arena, state_alloc));
        size_t seen = 0UL;
        for (TReader::TIndexFile::TKeyCursor cur_key_csr(&idx_file); cur_key_csr; ++cur_key_csr, ++seen) {
          const TReader::TIndexFile::TKeyItem &item = *cur_key_csr;
          EXPECT_TRUE(cur_key_csr);
          if (EXPECT_GE(expected_vec.size(), seen + 1)) {
            EXPECT_EQ(TKey(item.Key, &index_arena), expected_vec[seen].first);
            EXPECT_EQ(TKey(item.Value, &main_arena), expected_vec[seen].second);
          }
        }
        EXPECT_EQ(expected_vec.size(), seen);
      }
    }
    GracefullShutdown();
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  });
}

FIXTURE(Deep) {
  TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    TScheduler scheduler(TScheduler::TPolicy(4, 10, milliseconds(10)));

    Sim::TMemEngine mem_engine(&scheduler,
                               256 /* disk space: 256MB */,
                               256 /* slow disk space: 256MB */,
                               16384 /* page cache slots: 64MB */,
                               1 /* num page lru */,
                               1024 /* block cache slots: 64MB */,
                               1 /* num block lru */);

    Base::TUuid file_id(TUuid::Best);
    TSequenceNumber seq_num = 0U;
    TUuid int_str_decint_decstr_idx(TUuid::Twister);
    TUuid int_str_int_str_idx(TUuid::Twister);
    TUuid int_str_decint_str_idx(TUuid::Twister);
    /* Make data file 1 */ {
      TSuprena arena;
      TMockMem mem_layer;
      /* insert data */ {
        TSuprena suprena;
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        /* insert <[int64_t, string, desc<int64_t>, desc<string>]> */
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hello")}, {TDesc<int64_t>(9L), string("This is also a longer string")}}, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("short"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(20L), string("Some form of a longer string")}}, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(27L), string("This is also a longer string")}}, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
        /* insert <[int64_t, string, int64_t, string]> */
        Insert(mem_layer, ++seq_num, int_str_int_str_idx, TKey(set<int64_t>{1, 3, 9}, &suprena, state_alloc),
               1L, string("Orly"), 1L, string("short"));
      }
      size_t data_gen_id = 1;
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, data_gen_id, 20UL, 0U, Medium);
    }
    /* Make data file 2 (more of the same) */ {
      TSuprena arena;
      TMockMem mem_layer;
      /* insert data */ {
        TSuprena suprena;
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        /* insert <[int64_t, string, desc<int64_t>, desc<string>]> */
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hello")}, {TDesc<int64_t>(9L), string("This is also a longer string")}}, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(2L), TDesc<string>("short"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(20L), string("Some form of a longer string")}}, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(2L), TDesc<string>("This string should be too long to fit in a core"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(27L), string("This is also a longer string")}}, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(4L), TDesc<string>("This string should be too long to fit in a core"));
        /* insert <[int64_t, string, int64_t, string]> */
        Insert(mem_layer, ++seq_num, int_str_int_str_idx, TKey(set<int64_t>{1, 3, 9}, &suprena, state_alloc),
               1L, string("Orly"), 2L, string("short"));
      }
      size_t data_gen_id = 2;
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, data_gen_id, 20UL, 0U, Medium);
    }
    /* Make data file 3 (something different) */ {
      TSuprena arena;
      TMockMem mem_layer;
      /* insert data */ {
        TSuprena suprena;
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        /* insert <[int64_t, string, desc<int64_t>, desc<string>]> */
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hello")}, {TDesc<int64_t>(9L), string("This is also a longer string")}}, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(5L), TDesc<string>("short"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(20L), string("Some form of a longer string")}}, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(5L), TDesc<string>("This string should be too long to fit in a core"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(27L), string("This is also a longer string")}}, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(6L), TDesc<string>("This string should be too long to fit in a core"));
        /* insert <[int64_t, string, desc<int64_t>, string]> */
        Insert(mem_layer, ++seq_num, int_str_decint_str_idx, TKey(set<TDesc<int64_t>>{TDesc<int64_t>(1), TDesc<int64_t>(7), TDesc<int64_t>(20)}, &suprena, state_alloc),
               1L, string("Orly"), TDesc<int64_t>(2L), string("short"));
      }
      size_t data_gen_id = 3;
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, data_gen_id, 20UL, 0U, Medium);
    }
    /* merge them */ {
      TSuprena arena;
      TMergeDataFile merge_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, file_id, vector<size_t>{1, 2, 3}, file_id, 4UL, 0U, Low, 16384, 20UL, true, false);
      TReader reader(HERE, mem_engine.GetEngine(), file_id, 4UL);
      TReader::TArena main_arena(&reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TReader::TIndexFile int_str_decint_decstr_idx_file(&reader, int_str_decint_decstr_idx, RealTime);
      TReader::TArena int_str_decint_decstr_idx_arena(&int_str_decint_decstr_idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TReader::TIndexFile int_str_int_str_idx_file(&reader, int_str_int_str_idx, RealTime);
      TReader::TArena int_str_int_str_idx_arena(&int_str_int_str_idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TReader::TIndexFile int_str_decint_str_idx_file(&reader, int_str_decint_str_idx, RealTime);
      TReader::TArena int_str_decint_str_idx_arena(&int_str_decint_str_idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TStream<Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::PhysicalBlockSize, Orly::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
      size_t out_offset;
      /* what came from file 1 */
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_int_str_idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), 1L, string("short")), &arena, state_alloc), out_offset, in_stream, &int_str_int_str_idx_arena));
      /* what came from file 2 */
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(2L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(2L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(4L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_int_str_idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), 2L, string("short")), &arena, state_alloc), out_offset, in_stream, &int_str_int_str_idx_arena));
      /* what came from file 3 */
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(5L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(5L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(6L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_str_idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(2L), string("short")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_str_idx_arena));
      /* what doesn't exist */
      EXPECT_FALSE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(6L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_FALSE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(7L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_FALSE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(8L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_FALSE(int_str_decint_str_idx_file.FindInHash(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(3L), string("short")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_str_idx_arena));
      vector<pair<TKey, TKey>> expected_vec;
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(6L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(27L), string("This is also a longer string")}}, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(5L), TDesc<string>("short")), &arena, state_alloc),
                                TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hello")}, {TDesc<int64_t>(9L), string("This is also a longer string")}}, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(5L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(20L), string("Some form of a longer string")}}, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(4L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(27L), string("This is also a longer string")}}, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(2L), TDesc<string>("short")), &arena, state_alloc),
                                TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hello")}, {TDesc<int64_t>(9L), string("This is also a longer string")}}, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(2L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(20L), string("Some form of a longer string")}}, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc),
                                TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hello")}, {TDesc<int64_t>(9L), string("This is also a longer string")}}, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Orly"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(27L), string("This is also a longer string")}}, &arena, state_alloc));
      size_t seen = 0UL;
      for (TReader::TIndexFile::TKeyCursor cur_key_csr(&int_str_decint_decstr_idx_file); cur_key_csr; ++cur_key_csr, ++seen) {
        const TReader::TIndexFile::TKeyItem &item = *cur_key_csr;
        EXPECT_TRUE(cur_key_csr);
        if (EXPECT_GE(expected_vec.size(), seen + 1)) {
          EXPECT_EQ(TKey(item.Key, &int_str_decint_decstr_idx_arena), expected_vec[seen].first);
          EXPECT_EQ(TKey(item.Value, &main_arena), expected_vec[seen].second);
        }
      }
      EXPECT_EQ(expected_vec.size(), seen);
    }
    GracefullShutdown();
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  });
}

FIXTURE(SomeHistory) {
  TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    TScheduler scheduler(TScheduler::TPolicy(10, 10, milliseconds(10)));

    Sim::TMemEngine mem_engine(&scheduler,
                               256 /* disk space: 256MB */,
                               256 /* slow disk space: 256MB */,
                               16384 /* page cache slots: 64MB */,
                               1 /* num page lru */,
                               1024 /* block cache slots: 64MB */,
                               1 /* num block lru */);

    Base::TUuid file_id(TUuid::TimeAndMAC);
    TSuprena arena;
    TSequenceNumber seq_num = 0U;
    Base::TUuid int_idx(Base::TUuid::Twister);
    /* data file 1 */ {
      TMockMem mem_layer;
      for (int64_t i = 0; i < 11; i += 2) {
        mem_layer.Insert(TMockUpdate::NewMockUpdate(TUpdate::TOpByKey{ { TIndexKey(int_idx, TKey(make_tuple(i), &arena, state_alloc)), TKey(i * 10, &arena, state_alloc)}, { TIndexKey(int_idx, TKey(make_tuple(i + 1L), &arena, state_alloc)), TKey((i + 1L) * 10, &arena, state_alloc)} }, TKey(&arena), TKey(Base::TUuid(TUuid::Best), &arena, state_alloc), ++seq_num));
        mem_layer.Insert(TMockUpdate::NewMockUpdate(TUpdate::TOpByKey{ { TIndexKey(int_idx, TKey(make_tuple(i), &arena, state_alloc)), TKey(i * 10, &arena, state_alloc)}, { TIndexKey(int_idx, TKey(make_tuple(i + 1L), &arena, state_alloc)), TKey((i + 1L) * 10, &arena, state_alloc)} }, TKey(&arena), TKey(Base::TUuid(TUuid::Best), &arena, state_alloc), ++seq_num));
      }
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, 1UL, 20UL, 0U, RealTime);
    }
    /* data file 2 */ {
      TMockMem mem_layer;
      for (int64_t i = 0; i < 11; i += 2) {
        mem_layer.Insert(TMockUpdate::NewMockUpdate(TUpdate::TOpByKey{ { TIndexKey(int_idx, TKey(make_tuple(i), &arena, state_alloc)), TKey(i * 10, &arena, state_alloc)}, { TIndexKey(int_idx, TKey(make_tuple(i + 1L), &arena, state_alloc)), TKey((i + 1L) * 10, &arena, state_alloc)} }, TKey(&arena), TKey(Base::TUuid(TUuid::Best), &arena, state_alloc), ++seq_num));
        mem_layer.Insert(TMockUpdate::NewMockUpdate(TUpdate::TOpByKey{ { TIndexKey(int_idx, TKey(make_tuple(i), &arena, state_alloc)), TKey(i * 10, &arena, state_alloc)}, { TIndexKey(int_idx, TKey(make_tuple(i + 1L), &arena, state_alloc)), TKey((i + 1L) * 10, &arena, state_alloc)} }, TKey(&arena), TKey(Base::TUuid(TUuid::Best), &arena, state_alloc), ++seq_num));
      }
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, 2UL, 20UL, 0U, RealTime);
    }
    /* merge them */ {
      TMergeDataFile merge_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, file_id, vector<size_t>{1UL, 2UL}, file_id, 3UL, 0U, Low, 16384, 20UL, false, false);
    }
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  });
}

FIXTURE(JumpGrowingMainArena) {
  TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    stringstream my_str;
    const string zello_str("zello World");
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    Base::TUuid file_id(TUuid::TimeAndMAC);
    Base::TUuid int_idx(Base::TUuid::Twister);
    const size_t start_amt = 4096 * 16;
    while (my_str.str().size() < start_amt) {
      my_str << (rand() % 10);
    }
    const std::string add_str = my_str.str();
    for (size_t i = start_amt; i < 4096 * 16 * 32; i += start_amt) {
      my_str << add_str;
      cout << "Trying i = [" << i << "]" << endl;
      TScheduler scheduler(TScheduler::TPolicy(10, 10, milliseconds(10)));

      Sim::TMemEngine mem_engine(&scheduler,
                                 1024 /* disk space: 1GB */,
                                 512 /* slow disk space: 512MB */,
                                 65536 /* page cache slots: 256MB */,
                                 1 /* num page lru */,
                                 2048 /* block cache slots: 128MB */,
                                 1 /* num block lru */);
      TSuprena arena;
      TSequenceNumber seq_num = 0U;
      /* data file 1 */ {
        TMockMem mem_layer;
        Insert(mem_layer, ++seq_num, int_idx, TKey(my_str.str(), &arena, state_alloc),
                 1L);
        Insert(mem_layer, ++seq_num, int_idx, TKey(zello_str, &arena, state_alloc),
                 2L);
        TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, 1UL, 20UL, 0U, RealTime);
      }
      /* push it through the merger */ {
        TMergeDataFile merge_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, file_id, vector<size_t>{1UL}, file_id, 3UL, 0U, Low, 16384, 20UL, false, false);
      }
    }
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  });
}
