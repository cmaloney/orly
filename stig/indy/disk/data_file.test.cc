/* <stig/indy/disk/data_file.test.cc>

   Unit test for <stig/indy/disk/data_file.h>.

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

#include <stig/indy/disk/data_file.h>

#include <valgrind/callgrind.h>

#include <base/scheduler.h>
#include <base/usage_meter.h>
#include <stig/indy/disk/disk_test.h>
#include <stig/indy/disk/read_file.h>
#include <stig/indy/disk/sim/mem_engine.h>
#include <stig/indy/disk/test_file_service.h>
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

static const size_t BlockSize = Disk::Util::PhysicalBlockSize;

Stig::Indy::Util::TPool L0::TManager::TRepo::TMapping::Pool(sizeof(TRepo::TMapping), "Repo Mapping");
Stig::Indy::Util::TPool L0::TManager::TRepo::TMapping::TEntry::Pool(sizeof(TRepo::TMapping::TEntry), "Repo Mapping Entry");
Stig::Indy::Util::TPool L0::TManager::TRepo::TDataLayer::Pool(sizeof(TMemoryLayer), "Data Layer");

Stig::Indy::Util::TPool TUpdate::Pool(sizeof(TUpdate), "Update", 750010UL);
Stig::Indy::Util::TPool TUpdate::TEntry::Pool(sizeof(TUpdate::TEntry), "Entry", 1500020UL);
Disk::TBufBlock::TPool Disk::TBufBlock::Pool(BlockSize, 2000UL);

FIXTURE(Typical) {
  Fiber::TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    const TScheduler::TPolicy scheduler_policy(4, 10, milliseconds(10));
    TScheduler scheduler;
    scheduler.SetPolicy(scheduler_policy);

    Sim::TMemEngine mem_engine(&scheduler,
                               256 /* disk space: 256MB */,
                               256 /* slow disk space: 256MB */,
                               16384 /* page cache slots: 64MB */,
                               1 /* num page lru */,
                               1024 /* block cache slots: 64MB */,
                               1 /* num block lru */);

    Base::TUuid file_id(TUuid::Best);
    TSequenceNumber seq_num = 0U;
    TUuid int_str_int_idx(TUuid::Twister);
    TUuid int_str_str_idx(TUuid::Twister);
    TUuid int_str_idx(TUuid::Twister);
    TUuid int_idx(TUuid::Twister);
    /* Make a data file */ {
      TSuprena arena;
      TMockMem mem_layer;
      /* insert data */ {
        /* insert <[int64_t, string, int64_t]> */
        Insert(mem_layer, ++seq_num, int_str_int_idx, 7L,
               1L, string("Tagged"), 1L);
        Insert(mem_layer, ++seq_num, int_str_int_idx, 8L,
               1L, string("Tagged"), 2L);
        /* insert <[int64_t, string, string]> */
        Insert(mem_layer, ++seq_num, int_str_str_idx, 14L,
               1L, string("Tagged"), string("Stig"));
        Insert(mem_layer, ++seq_num, int_str_str_idx, 15L,
               1L, string("Tagged"), string("Stih"));
        /* insert <[int64_t, string]> */
        Insert(mem_layer, ++seq_num, int_str_idx, 21L,
               1L, string("Tagged"));
        Insert(mem_layer, ++seq_num, int_str_idx, 22L,
               1L, string("Taggef"));
        /* insert <[int64_t]> */
        Insert(mem_layer, ++seq_num, int_idx, 28L,
               1L);
        Insert(mem_layer, ++seq_num, int_idx, 29L,
               2L);
      }
      size_t data_gen_id = 1;
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, data_gen_id, 20UL, 0U, Medium);
    }
    GracefullShutdown();
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  });
}

FIXTURE(Deep) {
  Fiber::TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    const TScheduler::TPolicy scheduler_policy(4, 10, milliseconds(10));
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    TScheduler scheduler;
    scheduler.SetPolicy(scheduler_policy);

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
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(std::map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hello")}, {TDesc<int64_t>(9L), string("This is also a longer string")}}, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(std::map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(20L), string("Some form of a longer string")}}, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(std::map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(27L), string("This is also a longer string")}}, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core"));
        /* insert <[int64_t, string, int64_t, string]> */
        Insert(mem_layer, ++seq_num, int_str_int_str_idx, TKey(std::set<int64_t>{1, 3, 9}, &suprena, state_alloc),
               1L, string("Tagged"), 1L, string("short"));
      }
      size_t data_gen_id = 1;
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, data_gen_id, 20UL, 0U, Medium);
      TReader reader(HERE, mem_engine.GetEngine(), file_id, data_gen_id);
      TReader::TArena main_arena(&reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TReader::TIndexFile int_str_decint_decstr_idx_file(&reader, int_str_decint_decstr_idx, RealTime);
      TReader::TArena int_str_decint_decstr_idx_arena(&int_str_decint_decstr_idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TReader::TIndexFile int_str_int_str_idx_file(&reader, int_str_int_str_idx, RealTime);
      TReader::TArena int_str_int_str_idx_arena(&int_str_int_str_idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TStream<Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::PhysicalBlockSize, Stig::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
      size_t out_offset;
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_int_str_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), 1L, string("short")), &arena, state_alloc), out_offset, in_stream, &int_str_int_str_idx_arena));
      /* check the <[int64_t, string, desc<int64_t>, desc<string>]> index */ {
        //TReader::TIndexFile idx_file(&reader, int_str_decint_decstr_idx, RealTime);
        std::vector<std::pair<TKey, TKey>> expected_vec;
        expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("short")), &arena, state_alloc),
                                  TKey(std::map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hello")}, {TDesc<int64_t>(9L), string("This is also a longer string")}}, &arena, state_alloc));
        expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(1L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                  TKey(std::map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(27L), string("This is also a longer string")}}, &arena, state_alloc));
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
      }  // done checking the <[int64_t, string, desc<int64_t>, desc<string>]> index
    }
    /* Make data file 2 (more of the same) */ {
      TSuprena arena;
      TMockMem mem_layer;
      /* insert data */ {
        TSuprena suprena;
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        /* insert <[int64_t, string, desc<int64_t>, desc<string>]> */
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(std::map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hello")}, {TDesc<int64_t>(9L), string("This is also a longer string")}}, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(2L), TDesc<string>("short"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(std::map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(20L), string("Some form of a longer string")}}, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(2L), TDesc<string>("This string should be too long to fit in a core"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(std::map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(27L), string("This is also a longer string")}}, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(4L), TDesc<string>("This string should be too long to fit in a core"));
        /* insert <[int64_t, string, int64_t, string]> */
        Insert(mem_layer, ++seq_num, int_str_int_str_idx, TKey(std::set<int64_t>{1, 3, 9}, &suprena, state_alloc),
               1L, string("Tagged"), 2L, string("short"));
      }
      size_t data_gen_id = 2;
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, data_gen_id, 20UL, 0U, Medium);
      TReader reader(HERE, mem_engine.GetEngine(), file_id, data_gen_id);
      TReader::TArena main_arena(&reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TReader::TIndexFile int_str_decint_decstr_idx_file(&reader, int_str_decint_decstr_idx, RealTime);
      TReader::TArena int_str_decint_decstr_idx_arena(&int_str_decint_decstr_idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TReader::TIndexFile int_str_int_str_idx_file(&reader, int_str_int_str_idx, RealTime);
      TReader::TArena int_str_int_str_idx_arena(&int_str_int_str_idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TStream<Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::PhysicalBlockSize, Stig::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
      size_t out_offset;
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(2L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(2L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(4L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_int_str_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), 2L, string("short")), &arena, state_alloc), out_offset, in_stream, &int_str_int_str_idx_arena));
      /* check the <[int64_t, string, desc<int64_t>, desc<string>]> index */ {
        std::vector<std::pair<TKey, TKey>> expected_vec;
        expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(4L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                  TKey(std::map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(27L), string("This is also a longer string")}}, &arena, state_alloc));
        expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(2L), TDesc<string>("short")), &arena, state_alloc),
                                  TKey(std::map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hello")}, {TDesc<int64_t>(9L), string("This is also a longer string")}}, &arena, state_alloc));
        expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(2L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                  TKey(std::map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(20L), string("Some form of a longer string")}}, &arena, state_alloc));
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
      }  // done checking the <[int64_t, string, desc<int64_t>, desc<string>]> index
    }
    /* Make data file 3 (something different) */ {
      TSuprena arena;
      TMockMem mem_layer;
      /* insert data */ {
        TSuprena suprena;
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        /* insert <[int64_t, string, desc<int64_t>, desc<string>]> */
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(std::map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hello")}, {TDesc<int64_t>(9L), string("This is also a longer string")}}, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(5L), TDesc<string>("short"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(std::map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(20L), string("Some form of a longer string")}}, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(5L), TDesc<string>("This string should be too long to fit in a core"));
        Insert(mem_layer, ++seq_num, int_str_decint_decstr_idx, TKey(std::map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(27L), string("This is also a longer string")}}, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(6L), TDesc<string>("This string should be too long to fit in a core"));
        /* insert <[int64_t, string, desc<int64_t>, string]> */
        Insert(mem_layer, ++seq_num, int_str_decint_str_idx, TKey(std::set<TDesc<int64_t>>{TDesc<int64_t>(1), TDesc<int64_t>(7), TDesc<int64_t>(20)}, &suprena, state_alloc),
               1L, string("Tagged"), TDesc<int64_t>(2L), string("short"));
      }
      size_t data_gen_id = 3;
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, data_gen_id, 20UL, 0U, Medium);
      TReader reader(HERE, mem_engine.GetEngine(), file_id, data_gen_id);
      TReader::TArena main_arena(&reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TReader::TIndexFile int_str_decint_decstr_idx_file(&reader, int_str_decint_decstr_idx, RealTime);
      TReader::TArena int_str_decint_decstr_idx_arena(&int_str_decint_decstr_idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TReader::TIndexFile int_str_decint_str_idx_file(&reader, int_str_decint_str_idx, RealTime);
      TReader::TArena int_str_decint_str_idx_arena(&int_str_decint_str_idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TStream<Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::PhysicalBlockSize, Stig::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
      size_t out_offset;
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(5L), TDesc<string>("short")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(5L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_decstr_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(6L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_decstr_idx_arena));
      EXPECT_TRUE(int_str_decint_str_idx_file.FindInHash(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(2L), string("short")), &arena, state_alloc), out_offset, in_stream, &int_str_decint_str_idx_arena));
      /* check the <[int64_t, string, desc<int64_t>, desc<string>]> index */ {
        //TReader::TIndexFile idx_file(&reader, int_str_decint_decstr_idx, RealTime);
        std::vector<std::pair<TKey, TKey>> expected_vec;
        expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(6L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                  TKey(std::map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(27L), string("This is also a longer string")}}, &arena, state_alloc));
        expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(5L), TDesc<string>("short")), &arena, state_alloc),
                                  TKey(std::map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hello")}, {TDesc<int64_t>(9L), string("This is also a longer string")}}, &arena, state_alloc));
        expected_vec.emplace_back(TKey(make_tuple(1L, string("Tagged"), TDesc<int64_t>(5L), TDesc<string>("This string should be too long to fit in a core")), &arena, state_alloc),
                                  TKey(std::map<TDesc<int64_t>, string>{{TDesc<int64_t>(30L), string("Hey yo")}, {TDesc<int64_t>(20L), string("Some form of a longer string")}}, &arena, state_alloc));
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
      }  // done checking the <[int64_t, string, desc<int64_t>, desc<string>]> index
    }
    GracefullShutdown();
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  });
}

FIXTURE(History) {
  Fiber::TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    const TScheduler::TPolicy scheduler_policy(4, 10, milliseconds(10));
    TScheduler scheduler;
    scheduler.SetPolicy(scheduler_policy);
    Base::TUuid file_id(TUuid::Best);
    TSequenceNumber seq_num = 0U;

    Sim::TMemEngine mem_engine(&scheduler,
                               256 /* disk space: 256MB */,
                               256 /* slow disk space: 256MB */,
                               16384 /* page cache slots: 64MB */,
                               1 /* num page lru */,
                               1024 /* block cache slots: 64MB */,
                               1 /* num block lru */);

    TUuid int_str_int_idx(TUuid::Twister);
    /* Make a data file */ {
      TSuprena arena;
      TMockMem mem_layer;
      /* insert data */ {
        /* insert <[int64_t, string, int64_t]> */
        Insert(mem_layer, ++seq_num, int_str_int_idx, 7L,
               1L, string("Tagged"), 1L);
        Insert(mem_layer, ++seq_num, int_str_int_idx, 8L,
               1L, string("Tagged"), 2L);
        Insert(mem_layer, ++seq_num, int_str_int_idx, 9L,
               1L, string("Tagged"), 3L);
        Insert(mem_layer, ++seq_num, int_str_int_idx, 10L,
               1L, string("Tagged"), 4L);
        Insert(mem_layer, ++seq_num, int_str_int_idx, 11L,
               1L, string("Tagged"), 2L);
        Insert(mem_layer, ++seq_num, int_str_int_idx, 12L,
               1L, string("Tagged"), 3L);
      }
      size_t data_gen_id = 1;
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, data_gen_id, 20UL, 0U, Medium);
    }
    GracefullShutdown();
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  });
}

FIXTURE(BigSingleIndex) {
  Fiber::TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    const int64_t num_iter = 750000L;
    const TScheduler::TPolicy scheduler_policy(4, 10, milliseconds(10));
    TScheduler scheduler;
    scheduler.SetPolicy(scheduler_policy);

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
    /* Make a data file */ {
      TSuprena arena;
      TMockMem mem_layer;
      for (int64_t i = 0; i < num_iter; ++i) {  /* insert data */
        val_vec.emplace_back(i % 7L, (i % 2 == 0 ? tagged_str : stig_str), 1L + i * 2L, 7L * i);
      }
      std::sort(val_vec.begin(), val_vec.end(), [](const TTup &lhs, const TTup &rhs){
        Atom::TComparison comp = Atom::CompareOrdered(std::get<0>(lhs), std::get<0>(rhs));
        if (Atom::IsEq(comp)) {
          comp = Atom::CompareOrdered(std::get<1>(lhs), std::get<1>(rhs));
          if (Atom::IsEq(comp)) {
            comp = Atom::CompareOrdered(std::get<2>(lhs), std::get<2>(rhs));
            return Atom::IsLt(comp);
          } else {
            return Atom::IsLt(comp);
          }
        } else {
          return Atom::IsLt(comp);
        }
      });
      for (const auto &tup : val_vec) {
        Insert(mem_layer, ++seq_num, int_str_int_idx, std::get<3>(tup),
               std::get<0>(tup), std::get<1>(tup), std::get<2>(tup));
      }
      size_t data_gen_id = 1;
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, file_id, data_gen_id, 20UL, 0U, Medium);
      TReader reader(HERE, mem_engine.GetEngine(), file_id, data_gen_id);
      TReader::TArena main_arena(&reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TReader::TIndexFile idx_file(&reader, int_str_int_idx, RealTime);
      TReader::TArena idx_arena(&idx_file, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), RealTime);
      TStream<Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::LogicalBlockSize, Stig::Indy::Disk::Util::PhysicalBlockSize, Stig::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
      size_t matched = 0UL;
      for (const auto &tup : val_vec) {
        TSuprena arena;
        size_t out_offset;
        TKey key(make_tuple(std::get<0>(tup), std::get<1>(tup), std::get<2>(tup)), &arena, state_alloc);
        bool ret = idx_file.FindInHash(key, out_offset, in_stream, &idx_arena);
        if (!ret) {
          std::cout << "failed on key [" << key << "]" << std::endl;
        }
        matched += ret ? 1UL : 0UL;
      }
      EXPECT_EQ(matched, val_vec.size());
    }
    GracefullShutdown();
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  });
}
