/* <stig/indy/disk/merge_data_file.test.cc>

   Unit test for <stig/indy/disk/merge_data_file.h>.

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

#include <valgrind/callgrind.h>

#include <base/scheduler.h>
#include <stig/indy/disk/data_file.h>
#include <stig/indy/disk/disk_test.h>
#include <stig/indy/disk/read_file.h>
#include <stig/indy/disk/sim/mem_engine.h>
#include <stig/indy/fiber/fiber_test_runner.h>

#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Stig;
using namespace Stig::Atom;
using namespace Stig::Indy;
using namespace Stig::Indy::Disk;
using namespace Stig::Indy::Disk::Util;
using namespace Stig::Indy::Fiber;

static const size_t BlockSize = Disk::Util::PhysicalBlockSize;

Stig::Indy::Util::TPool L0::TManager::TRepo::TMapping::Pool(sizeof(TRepo::TMapping), "Repo Mapping");
Stig::Indy::Util::TPool L0::TManager::TRepo::TMapping::TEntry::Pool(sizeof(TRepo::TMapping::TEntry), "Repo Mapping Entry");
Stig::Indy::Util::TPool L0::TManager::TRepo::TDataLayer::Pool(sizeof(TMemoryLayer), "Data Layer");

Stig::Indy::Util::TPool TUpdate::Pool(sizeof(TUpdate), "Update", 1048578UL);
Stig::Indy::Util::TPool TUpdate::TEntry::Pool(sizeof(TUpdate::TEntry), "Entry", 1048578UL);
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
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short"));
        Insert(mem_layer, ++seq_num, index_id, TKey(49L, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
        Insert(mem_layer, ++seq_num, index_id, TKey(57L, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
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
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short"));
        Insert(mem_layer, ++seq_num, index_id, TKey(409L, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one"));
        Insert(mem_layer, ++seq_num, index_id, TKey(Native::TTombstone::Tombstone, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
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
      TStream<Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::PhysicalBlockSize, Stig::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
      size_t out_offset;
      /* what came from file 1 but got overriden in 2 */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      /* this has been tombstoned in file 2, but because it had history it's still visible as a tombstone */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      /* this got added in file 2 */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      vector<pair<TKey, TKey>> expected_vec;
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc),
                                TKey(7L, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                TKey(Native::TTombstone::Tombstone, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc),
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
      TStream<Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::PhysicalBlockSize, Stig::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
      size_t out_offset;
      /* what came from file 1 but got overriden in 2 */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      /* this has been tombstoned in file 2, but because it had history it's still visible as a tombstone */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      /* this got added in file 2 */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      vector<pair<TKey, TKey>> expected_vec;
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc),
                                TKey(7L, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                TKey(Native::TTombstone::Tombstone, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc),
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
      TStream<Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::PhysicalBlockSize, Stig::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
      size_t out_offset;
      /* what came from file 1 but got overriden in 2 */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      /* this has been tombstoned in file 2. */
      EXPECT_FALSE(idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      /* this got added in file 2 */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      vector<pair<TKey, TKey>> expected_vec;
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc),
                                TKey(7L, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc),
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
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short"));
        Insert(mem_layer, ++seq_num, index_id, TKey(49L, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
        Insert(mem_layer, ++seq_num, index_id, TKey(57L, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
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
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short"));
        Insert(mem_layer, ++seq_num, index_id, TKey(409L, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one"));
        Insert(mem_layer, ++seq_num, index_id, TKey(Native::TTombstone::Tombstone, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
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
      TStream<Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::PhysicalBlockSize, Stig::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
      size_t out_offset;
      /* what came from file 1 but got overriden in 2 */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      /* this has been tombstoned in file 2, but because it had history it's still visible as a tombstone */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      /* this got added in file 2 */
      EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc), out_offset, in_stream, &index_arena));
      vector<pair<TKey, TKey>> expected_vec;
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc),
                                TKey(7L, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                TKey(Native::TTombstone::Tombstone, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc),
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
        TStream<Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::PhysicalBlockSize, Stig::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
        size_t out_offset;
        /* what came from file 1 but got overriden in 2 */
        EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &index_arena));
        /* this has been tombstoned in file 2, but because it had history it's still visible as a tombstone */
        EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &index_arena));
        /* this got added in file 2 */
        EXPECT_TRUE(idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc), out_offset, in_stream, &index_arena));
        vector<pair<TKey, TKey>> expected_vec;
        expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc),
                                  TKey(7L, &arena, state_alloc));
        expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                  TKey(Native::TTombstone::Tombstone, &arena, state_alloc));
        expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("Here's a new one")), &arena, state_alloc),
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
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(20L), string("Some form of a longer string")}}, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(27L), string("This is also a longer string")}}, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
        /* insert <[int64_t, string, int64_t, string]> */
        Insert(mem_layer, ++seq_num, int_str_int_str_idx, TKey(set<int64_t>{1, 3, 9}, &suprena, state_alloc),
               1L, string("Tagged"), 1L, string("short"));
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
               1L, string("Tagged"), TDesc<int64_t>(2L), TDesc<string>("short"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(20L), string("Some form of a longer string")}}, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(2L), TDesc<string>("This string should be too long to fit in a core"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(27L), string("This is also a longer string")}}, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(4L), TDesc<string>("This string should be too long to fit in a core"));
        /* insert <[int64_t, string, int64_t, string]> */
        Insert(mem_layer, ++seq_num, int_str_int_str_idx, TKey(set<int64_t>{1, 3, 9}, &suprena, state_alloc),
               1L, string("Tagged"), 2L, string("short"));
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
               1L, string("Tagged"), TDesc<int64_t>(5L), TDesc<string>("short"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(20L), string("Some form of a longer string")}}, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(5L), TDesc<string>("This string should be too long to fit in a core"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(27L), string("This is also a longer string")}}, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(6L), TDesc<string>("This string should be too long to fit in a core"));
        /* insert <[int64_t, string, desc<int64_t>, string]> */
        Insert(mem_layer, ++seq_num, int_str_decint_str_idx, TKey(set<TDesc<int64_t>>{TDesc<int64_t>(1), TDesc<int64_t>(7), TDesc<int64_t>(20)}, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(2L), string("short"));
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
      TStream<Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::PhysicalBlockSize, Stig::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
      size_t out_offset;
      /* what came from file 1 */
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_int_str_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), 1L, string("short")), &arena, state_alloc), out_offset, in_stream, &int_str_int_str_idx_arena));
      /* what came from file 2 */
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(2L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(2L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(4L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_int_str_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), 2L, string("short")), &arena, state_alloc), out_offset, in_stream, &int_str_int_str_idx_arena));
      /* what came from file 3 */
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(5L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(5L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(6L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_str_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(2L), string("short")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_str_idx_arena));
      /* what doesn't exist */
      EXPECT_FALSE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(6L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_FALSE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(7L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_FALSE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(8L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_FALSE(int_str_decint_str_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(3L), string("short")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_str_idx_arena));
      vector<pair<TKey, TKey>> expected_vec;
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(6L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(27L), string("This is also a longer string")}}, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(5L), TDesc<string>("short")), &arena, state_alloc),
                                TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hello")}, {TDesc<int64_t>(9L), string("This is also a longer string")}}, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(5L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(20L), string("Some form of a longer string")}}, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(4L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(27L), string("This is also a longer string")}}, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(2L), TDesc<string>("short")), &arena, state_alloc),
                                TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hello")}, {TDesc<int64_t>(9L), string("This is also a longer string")}}, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(2L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(20L), string("Some form of a longer string")}}, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc),
                                TKey(map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hello")}, {TDesc<int64_t>(9L), string("This is also a longer string")}}, &arena, state_alloc));
      expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
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

FIXTURE(BigSingleIndex) {
  TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    const int64_t num_iter = 2000000L;
    const int64_t num_merge_files = 4L;
    assert(num_iter % num_merge_files == 0);
    const size_t num_per_file = num_iter / num_merge_files;
    //void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    TScheduler scheduler(TScheduler::TPolicy(4, 10, milliseconds(10)));

    Sim::TMemEngine mem_engine(&scheduler,
                               1024 /* disk space: 1GB */,
                               512 /* slow disk space: 512MB */,
                               65536 /* page cache slots: 256MB */,
                               1 /* num page lru */,
                               2048 /* block cache slots: 128MB */,
                               1 /* num block lru */);

    Base::TUuid file_id(TUuid::Best);
    TSequenceNumber seq_num = 0U;
    TUuid int_str_int_idx(TUuid::Twister);
    const string tagged_str("Tagged");
    const string stig_str("Stig");
    typedef tuple<int64_t, string, int64_t, int64_t> TTup;
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    vector<TTup> val_vec;
    vector<size_t> data_gen_vec;
    /* Make the data file */ {
      for (int64_t i = 0; i < num_iter; ++i) {  /* insert data */
        val_vec.emplace_back(i % 7L, (i % 2 == 0 ? tagged_str : stig_str), 1L + i * 2L, 7L * i);
      }
      sort(val_vec.begin(), val_vec.end(), [](const TTup &lhs, const TTup &rhs) {
        Atom::TComparison comp = Atom::CompareOrdered(get<0>(lhs), get<0>(rhs));
        if (Atom::IsEq(comp)) {
          comp = Atom::CompareOrdered(get<1>(lhs), get<1>(rhs));
          if (Atom::IsEq(comp)) {
            comp = Atom::CompareOrdered(get<2>(lhs), get<2>(rhs));
            return Atom::IsLt(comp);
          } else {
            return Atom::IsLt(comp);
          }
        } else {
          return Atom::IsLt(comp);
        }
      });
      for (int64_t i = 0; i < num_merge_files; ++i) {
        TSuprena arena;
        TMockMem mem_layer;
        for (size_t j = 0; j < num_per_file; ++j) {
          const TTup &tup = val_vec[j * num_merge_files + i];
          Insert(mem_layer, ++seq_num, int_str_int_idx, get<3>(tup),
                 get<0>(tup), get<1>(tup), get<2>(tup));
        }
        Base::TTimer timer;
        timer.Start();
        TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, i + 1, 20UL, 0U, Medium);
        timer.Stop();
        cout << "DataFile [" << timer.Total() << "]\t[" << (timer.Total() / num_per_file) << " / key]" << endl;
        data_gen_vec.push_back(i + 1);
      }
    }
    /* merge them */ {
      TSuprena arena;
      size_t merge_file_id = num_merge_files + 1;
      Base::TTimer merge_timer;
      merge_timer.Start();
      //CALLGRIND_START_INSTRUMENTATION;
      TMergeDataFile merge_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, file_id, data_gen_vec, file_id, merge_file_id, 0U, Low, 16384, 20UL, false, false);
      //CALLGRIND_STOP_INSTRUMENTATION;
      merge_timer.Stop();
      cout << "MergeDataFile [" << merge_timer.Total() << "]\t[" << (merge_timer.Total() / num_iter) << " / key]" << endl;
      TReader reader(HERE, mem_engine.GetEngine(), file_id, merge_file_id);
      TReader::TArena main_arena(&reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TReader::TIndexFile idx_file(&reader, int_str_int_idx, RealTime);
      TReader::TArena index_arena(&idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TStream<Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::PhysicalBlockSize, Stig::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
      size_t out_offset;
      /* ensure they exist in the hash */
      int64_t found_in_hash = 0UL;
      Base::TTimer hash_timer;
      hash_timer.Start();
      //CALLGRIND_START_INSTRUMENTATION;
      for (int64_t i = 0; i < num_iter; ++i) {
        found_in_hash += idx_file.FindInHash(TKey(make_tuple(i % 7L, (i % 2 == 0 ? tagged_str : stig_str), 1L + i * 2L), &arena, state_alloc), out_offset, in_stream, &index_arena) ? 1UL : 0UL;
      }
      //CALLGRIND_STOP_INSTRUMENTATION;
      hash_timer.Stop();
      EXPECT_EQ(found_in_hash, num_iter);
      cout << endl << "Hash check [" << hash_timer.Total() << "]\t[" << (hash_timer.Total() / num_iter) << " / key]" << endl << endl;
      size_t seen = 0UL;
      Base::TTimer walk_timer;
      walk_timer.Start();
      size_t matched_correct = 0UL;
      size_t cur_key_correct = 0UL;
      for (TReader::TIndexFile::TKeyCursor cur_key_csr(&idx_file); cur_key_csr; ++cur_key_csr, ++seen) {
        const TReader::TIndexFile::TKeyItem &item = *cur_key_csr;
        cur_key_correct += static_cast<bool>(cur_key_csr) ? 1UL : 0UL;
        if (val_vec.size() >= seen + 1) {
          const TTup &tup = val_vec[seen];
          TKey expected_key(make_tuple(get<0>(tup), get<1>(tup), get<2>(tup)), &arena, state_alloc);
          TKey expected_val(get<3>(tup), &arena, state_alloc);
          matched_correct += TKey(item.Key, &index_arena) == expected_key ? 1UL : 0UL;
          matched_correct += TKey(item.Value, &main_arena) == expected_val ? 1UL : 0UL;
          /*
          EXPECT_EQ(TKey(item.Key, &index_arena), expected_key);
          EXPECT_EQ(TKey(item.Value, &main_arena), expected_val);
          */
        }
      }
      walk_timer.Stop();
      cout << "Walk check [" << walk_timer.Total() << "]\t[" << (walk_timer.Total() / num_iter) << " / key]" << endl;
      EXPECT_EQ(val_vec.size(), seen);
      EXPECT_EQ(cur_key_correct, seen);
      EXPECT_EQ(matched_correct, 2UL * seen);
    }
    GracefullShutdown();
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  });
}

FIXTURE(StressSingleIndex) {
  TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    const int64_t max_iter = 1048576L;
    for (int64_t num_iter = 4; num_iter <= max_iter; num_iter *= 2) {
      cout << "\t\t~~~ Testing [" << num_iter << "] ~~~" << endl;
      const int64_t num_merge_files = 4L;
      assert(num_iter % num_merge_files == 0);
      const size_t num_per_file = num_iter / num_merge_files;
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      TScheduler scheduler(TScheduler::TPolicy(4, 10, milliseconds(10)));

      Sim::TMemEngine mem_engine(&scheduler,
                               1024 /* disk space: 1GB */,
                               512 /* slow disk space: 512MB */,
                               65536 /* page cache slots: 256MB */,
                               1 /* num page lru */,
                               2048 /* block cache slots: 128MB */,
                               1 /* num block lru */);

      Base::TUuid file_id(TUuid::Best);
      TSequenceNumber seq_num = 0U;
      TUuid int_str_int_idx(TUuid::Twister);
      const string tagged_str("Tagged");
      const string stig_str("Stig");
      typedef tuple<int64_t, string, int64_t, int64_t> TTup;
      vector<TTup> val_vec;
      vector<size_t> data_gen_vec;
      /* Make the data file */ {
        for (int64_t i = 0; i < num_iter; ++i) {  /* insert data */
          val_vec.emplace_back(i % 7L, (i % 2 == 0 ? tagged_str : stig_str), 1L + i * 2L, 7L * i);
        }
        sort(val_vec.begin(), val_vec.end(), [](const TTup &lhs, const TTup &rhs) {
          Atom::TComparison comp = Atom::CompareOrdered(get<0>(lhs), get<0>(rhs));
          if (Atom::IsEq(comp)) {
            comp = Atom::CompareOrdered(get<1>(lhs), get<1>(rhs));
            if (Atom::IsEq(comp)) {
              comp = Atom::CompareOrdered(get<2>(lhs), get<2>(rhs));
              return Atom::IsLt(comp);
            } else {
              return Atom::IsLt(comp);
            }
          } else {
            return Atom::IsLt(comp);
          }
        });
        for (int64_t i = 0; i < num_merge_files; ++i) {
          TSuprena arena;
          TMockMem mem_layer;
          for (size_t j = 0; j < num_per_file; ++j) {
            const TTup &tup = val_vec[j * num_merge_files + i];
            Insert(mem_layer, ++seq_num, int_str_int_idx, get<3>(tup),
                   get<0>(tup), get<1>(tup), get<2>(tup));
          }
          Base::TTimer timer;
          timer.Start();
          TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, i + 1, 20UL, 0U, Medium);
          timer.Stop();
          cout << "DataFile [" << timer.Total() << "]\t[" << (timer.Total() / num_per_file) << " / key]" << endl;
          data_gen_vec.push_back(i + 1);
        }
      }
      /* merge them */ {
        TSuprena arena;
        size_t merge_file_id = num_merge_files + 1;
        Base::TTimer merge_timer;
        merge_timer.Start();
        TMergeDataFile merge_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, file_id, data_gen_vec, file_id, merge_file_id, 0U, Low, 16384, 20UL, false, false);
        merge_timer.Stop();
        cout << "MergeDataFile [" << merge_timer.Total() << "]\t[" << (merge_timer.Total() / num_iter) << " / key]" << endl;
        TReader reader(HERE, mem_engine.GetEngine(), file_id, merge_file_id);
        TReader::TArena main_arena(&reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
        TReader::TIndexFile idx_file(&reader, int_str_int_idx, RealTime);
        TReader::TArena index_arena(&idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
        TStream<Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::PhysicalBlockSize, Stig::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
        size_t out_offset;
        /* ensure they exist in the hash */
        int64_t found_in_hash = 0UL;
        Base::TTimer hash_timer;
        hash_timer.Start();
        for (int64_t i = 0; i < num_iter; ++i) {
          found_in_hash += idx_file.FindInHash(TKey(make_tuple(i % 7L, (i % 2 == 0 ? tagged_str : stig_str), 1L + i * 2L), &arena, state_alloc), out_offset, in_stream, &index_arena) ? 1UL : 0UL;
        }
        hash_timer.Stop();
        EXPECT_EQ(found_in_hash, num_iter);
        cout << "Hash check [" << hash_timer.Total() << "]\t[" << (hash_timer.Total() / num_iter) << " / key]" << endl;
        size_t seen = 0UL;
        Base::TTimer walk_timer;
        size_t matched_correct = 0UL;
        size_t cur_key_correct = 0UL;
        walk_timer.Start();
        for (TReader::TIndexFile::TKeyCursor cur_key_csr(&idx_file); cur_key_csr; ++cur_key_csr, ++seen) {
          const TReader::TIndexFile::TKeyItem &item = *cur_key_csr;
          cur_key_correct += static_cast<bool>(cur_key_csr) ? 1UL : 0UL;
          if (val_vec.size() >= seen + 1) {
            const TTup &tup = val_vec[seen];
            TKey expected_key(make_tuple(get<0>(tup), get<1>(tup), get<2>(tup)), &arena, state_alloc);
            TKey expected_val(get<3>(tup), &arena, state_alloc);
            matched_correct += TKey(item.Key, &index_arena) == expected_key ? 1UL : 0UL;
            matched_correct += TKey(item.Value, &main_arena) == expected_val ? 1UL : 0UL;
          }
        }
        walk_timer.Stop();
        cout << "Walk check [" << walk_timer.Total() << "]\t[" << (walk_timer.Total() / num_iter) << " / key]" << endl;
        EXPECT_EQ(val_vec.size(), seen);
        EXPECT_EQ(cur_key_correct, seen);
        EXPECT_EQ(matched_correct, seen * 2UL);
      }
      GracefullShutdown();
    }
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  });
}

FIXTURE(Many) {
  TFiberTestRunner runner([](std::mutex &test_mut, std::condition_variable &test_cond, bool &test_fin, Fiber::TRunner::TRunnerCons &) {
    int num_cpu = sysconf(_SC_NPROCESSORS_ONLN);
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    int64_t keys_per_file = 100000UL;
    const TScheduler::TPolicy job_policy(min(4, num_cpu), min(4, num_cpu), milliseconds(10));
    TScheduler job_scheduler(job_policy);
    TScheduler scheduler(TScheduler::TPolicy(4, 10, milliseconds(10)));

    Sim::TMemEngine mem_engine(&scheduler,
                               1024 /* disk space: 1GB */,
                               512 /* slow disk space: 512MB */,
                               65536 /* page cache slots: 256MB */,
                               1 /* num page lru */,
                               2048 /* block cache slots: 128MB */,
                               1 /* num block lru */);

    int64_t num_iter = 800000L;
    size_t num_data_file = ceil(static_cast<double>(num_iter) / keys_per_file);
    Base::TUuid file_id(TUuid::Best);

    TUuid int_str_int_idx(TUuid::Twister);

    mutex output_lock;
    condition_variable cond;
    size_t finished = 0UL;
    auto data_gen_func = [&](int64_t offset, TSequenceNumber seq_num) {
      try {
        /* we can get rid of the memory data after we flush */ {
          TMockMem mem_layer;
          for (int64_t i = offset; i < offset + keys_per_file && i < num_iter; ++i) {
            TSuprena arena;
            Insert(mem_layer, ++seq_num, int_str_int_idx, i,
                   i / 200L, string("Tagged"), TDesc<int64_t>(num_iter - i));
          }
          TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, (offset / keys_per_file) + 1, 20UL, 0U, Medium);
        }
        cout << "Finished [" << offset << "] to [" << (offset + keys_per_file) << "]" << endl;
        lock_guard<mutex> lock(output_lock);
        ++finished;
        cond.notify_one();
      } catch (const exception &ex) {
        cerr << "Error in data_gen_func [" << ex.what() << "]" << endl;
        throw;
      }
    };

    for (int64_t it = 0; it < num_iter; it += keys_per_file) {
      job_scheduler.Schedule(bind(data_gen_func, it, it));
    }
    /* wait for them to finish */ {
      unique_lock<mutex> lock(output_lock);
      while (finished < num_data_file) {
        cond.wait(lock);
      }
    }
    vector<size_t> start_vec;
    vector<size_t> end_vec;
    size_t next_id = num_data_file + 1;
    for (size_t i = 0; i < num_data_file; ++i) {
      start_vec.push_back(i + 1);
    }
    auto merge_job = [&](size_t lhs, size_t rhs, size_t new_id) {

      Base::TTimer timer;
      Base::TCPUTimer cpu_timer;
      timer.Start();
      cpu_timer.Start();
      TMergeDataFile merge_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, file_id, vector<size_t>{lhs, rhs}, file_id, new_id, 0U, Low, 16384, 20UL, false, false);
      cpu_timer.Stop();
      timer.Stop();

      cout << "tReal Time=[" << timer.Total() << "],\tCPU Time=[" << cpu_timer.Total() << "]" << endl;

      lock_guard<mutex> lock(output_lock);
      ++finished;
      cond.notify_one();
    };
    while (start_vec.size() >= 2) {
      size_t expected_jobs = start_vec.size() / 2;
      cout << "Starting on [" << expected_jobs << "] merge files..." << endl;
      finished = 0UL;
      while (start_vec.size() >= 2) {
        size_t lhs_id = start_vec[0];
        size_t rhs_id = start_vec[1];
        start_vec.erase(start_vec.begin());
        start_vec.erase(start_vec.begin());
        job_scheduler.Schedule(bind(merge_job, lhs_id, rhs_id, next_id));
        end_vec.push_back(next_id);
        ++next_id;
      }
      /* wait for them to finish */ {
        unique_lock<mutex> lock(output_lock);
        while (finished < expected_jobs) {
          cond.wait(lock);
        }
        if (end_vec.size() == 1) {
          break;
        }
      }
      start_vec = end_vec;
      end_vec.clear();
    }

    /* ensure they exist in the hash */
    assert(end_vec.size() == 1);
    TReader reader(HERE, mem_engine.GetEngine(), file_id, end_vec[0]);
    TReader::TArena main_arena(&reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
    TReader::TIndexFile idx_file(&reader, int_str_int_idx, RealTime);
    TReader::TArena index_arena(&idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
    TStream<Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::PhysicalBlockSize, Stig::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
    size_t out_offset;
    int64_t found_in_hash = 0UL;
    Base::TTimer hash_timer;
    hash_timer.Start();
    for (int64_t i = 0; i < num_iter; ++i) {
      // i / 200L, string("Tagged"), TDesc<int64_t>(num_iter - i)
      TSuprena arena;
      found_in_hash += idx_file.FindInHash(TKey(make_tuple(i / 200L, string("Tagged"), TDesc<int64_t>(num_iter - i)), &arena, state_alloc), out_offset, in_stream, &index_arena) ? 1UL : 0UL;
    }
    hash_timer.Stop();
    EXPECT_EQ(found_in_hash, num_iter);
    cout << "Hash check [" << hash_timer.Total() << "]\t[" << (hash_timer.Total() / num_iter) << " / key]" << endl;
    int64_t seen = 0UL;
    size_t matched_correct = 0UL;
    int64_t cur_key_correct = 0UL;
    Base::TTimer walk_timer;
    walk_timer.Start();
    for (TReader::TIndexFile::TKeyCursor cur_key_csr(&idx_file); cur_key_csr; ++cur_key_csr, ++seen) {
      TSuprena arena;
      const TReader::TIndexFile::TKeyItem &item = *cur_key_csr;
      cur_key_correct += static_cast<bool>(cur_key_csr) ? 1UL : 0UL;
      TKey expected_key(make_tuple(seen / 200L, string("Tagged"), TDesc<int64_t>(num_iter - seen)), &arena, state_alloc);
      TKey expected_val(seen, &arena, state_alloc);
      matched_correct += TKey(item.Key, &index_arena) == expected_key ? 1UL : 0UL;
      matched_correct += TKey(item.Value, &main_arena) == expected_val ? 1UL : 0UL;
    }
    walk_timer.Stop();
    cout << "Walk check [" << walk_timer.Total() << "]\t[" << (walk_timer.Total() / num_iter) << " / key]" << endl;
    EXPECT_EQ(num_iter, seen);
    EXPECT_EQ(cur_key_correct, seen);
    EXPECT_EQ(matched_correct, 2UL * seen);
    std::lock_guard<std::mutex> lock(test_mut);
    test_fin = true;
    test_cond.notify_one();
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
