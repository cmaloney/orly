/* <stig/indy/disk/present_walk_file.test.cc>

   Unit test for <stig/indy/disk/present_walk_file.h>.

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

#include <stig/indy/disk/present_walk_file.h>

#include <base/scheduler.h>
#include <stig/indy/disk/data_file.h>
#include <stig/indy/disk/disk_test.h>
#include <stig/indy/disk/merge_data_file.h>
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
using namespace Stig::Indy::Util;

static const size_t BlockSize = Disk::Util::PhysicalBlockSize;
static const size_t WalkerCacheSize = 128;

Stig::Indy::Util::TPool L0::TManager::TRepo::TMapping::Pool(sizeof(TRepo::TMapping), "Repo Mapping");
Stig::Indy::Util::TPool L0::TManager::TRepo::TMapping::TEntry::Pool(sizeof(TRepo::TMapping::TEntry), "Repo Mapping Entry");
Stig::Indy::Util::TPool L0::TManager::TRepo::TDataLayer::Pool(sizeof(TMemoryLayer), "Data Layer");

Stig::Indy::Util::TPool TUpdate::Pool(sizeof(TUpdate), "Update", 4000004UL);
Stig::Indy::Util::TPool TUpdate::TEntry::Pool(sizeof(TUpdate::TEntry), "Entry", 4000004UL);
Disk::TBufBlock::TPool Disk::TBufBlock::Pool(BlockSize, 60000UL);

class TRAIITest {
  NO_COPY_SEMANTICS(TRAIITest);
  public:

  using TLocalReadFileCache = Stig::Indy::Disk::TLocalReadFileCache<Stig::Indy::Disk::Util::LogicalPageSize,
    Stig::Indy::Disk::Util::LogicalBlockSize,
    Stig::Indy::Disk::Util::PhysicalBlockSize,
    Stig::Indy::Disk::Util::CheckedPage, true>;

  TRAIITest() {
    assert(!TLocalReadFileCache::Cache);
    TLocalReadFileCache::Cache = new TLocalReadFileCache();
    assert(!Disk::TLocalWalkerCache::Cache);
    Disk::TLocalWalkerCache::Cache = new Disk::TLocalWalkerCache();
  }

  ~TRAIITest() {
    assert(TLocalReadFileCache::Cache);
    delete TLocalReadFileCache::Cache;
    TLocalReadFileCache::Cache = nullptr;
    assert(Disk::TLocalWalkerCache::Cache);
    delete Disk::TLocalWalkerCache::Cache;
    Disk::TLocalWalkerCache::Cache = nullptr;
  }
};

FIXTURE(Typical) {
  Fiber::TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    TRAIITest required_thread_locals;
    const TScheduler::TPolicy scheduler_policy(10, 10, milliseconds(10));
    TScheduler scheduler;
    scheduler.SetPolicy(scheduler_policy);

    Sim::TMemEngine mem_engine(&scheduler,
                               4 * 1024 /* disk space: 4 GB */,
                               256,
                               262144 /* page cache slots: 1GB */,
                               1 /* num page lru */,
                               16384 /* block cache slots: 1GB */,
                               1 /* num block lru */);

    Base::TUuid data_file_id(TUuid::TimeAndMAC);
    TSuprena arena;
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    size_t data_gen_id = 1;
    TSequenceNumber seq_num = 0U;
    Base::TUuid int_idx(Base::TUuid::Twister);
    /* data file 1 */ {
      TMockMem mem_layer;
      for (int64_t i = 0; i < 11; i += 2) {
        Insert(mem_layer, ++seq_num, int_idx, static_cast<int64_t>(rand()),
                 i);
      }
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, data_file_id, data_gen_id, 20UL, 0U, RealTime);
    }
    /* exists single */ {
      TPresentWalkFileWrapper walker(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, TKey(make_tuple(0L), &arena, state_alloc));
      size_t found = 0U;
      for (int64_t i = 0L; walker; ++walker, i += 2) {
        EXPECT_EQ(TKey((*walker).Key, (*walker).KeyArena), TKey(make_tuple(i), &arena, state_alloc));
        ++found;
      }
      EXPECT_EQ(found, 1U);
    }
    /* no exist single */ {
      TPresentWalkFileWrapper walker(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, TKey(make_tuple(5L), &arena, state_alloc));
      size_t found = 0U;
      for (int64_t i = 5L; walker; ++walker, ++i) {
        ++found;
      }
      EXPECT_EQ(found, 0U);
    }
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
    GracefullShutdown();
  });
}

template <typename ...TArgs>
void CheckExists(TEngine *engine, const Base::TUuid &file_id, size_t gen_id, const Base::TUuid &index_id, const TArgs &...args) {
  TSuprena arena;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  TPresentWalkFileWrapper walker(engine, file_id, gen_id, index_id, TKey(make_tuple(args...), &arena, state_alloc));
  if (EXPECT_TRUE(walker)) {
    EXPECT_EQ(TKey((*walker).Key, (*walker).KeyArena), TKey(make_tuple(args...), &arena, state_alloc));
    ++walker;
    EXPECT_FALSE(walker);
  }
}

template <typename ...TArgs>
size_t CheckExistsFree(TEngine *engine, const Base::TUuid &file_id, size_t gen_id, const Base::TUuid &index_id, const std::function<void (const TKey &)> &cb, const TArgs &...args) {
  TSuprena arena;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  size_t matched = 0UL;
  for (TPresentWalkFileWrapper walker(engine, file_id, gen_id, index_id, TKey(make_tuple(args...), &arena, state_alloc)); walker; ++walker, ++matched) {
    cb(TKey((*walker).Key, (*walker).KeyArena));
  }
  return matched;
}

template <typename ...TArgs>
void CheckNotExists(TEngine *engine, const Base::TUuid &file_id, size_t gen_id, const Base::TUuid &index_id, const TArgs &...args) {
  TSuprena arena;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  TPresentWalkFileWrapper walker(engine, file_id, gen_id, index_id, TKey(make_tuple(args...), &arena, state_alloc));
  EXPECT_FALSE(walker);
}

FIXTURE(MultiIndex) {
  Fiber::TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    TRAIITest required_thread_locals;
    const TScheduler::TPolicy scheduler_policy(10, 10, milliseconds(10));
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    TScheduler scheduler;
    scheduler.SetPolicy(scheduler_policy);

    Sim::TMemEngine mem_engine(&scheduler,
                               4 * 1024 /* disk space: 4 GB */,
                               256,
                               262144 /* page cache slots: 1GB */,
                               1 /* num page lru */,
                               16384 /* block cache slots: 1GB */,
                               1 /* num block lru */);

    Base::TUuid data_file_id(TUuid::TimeAndMAC);
    TSuprena arena;
    size_t data_gen_id = 1;
    TSequenceNumber seq_num = 0U;
    Base::TUuid int_idx(Base::TUuid::Twister);
    Base::TUuid int_int_idx(Base::TUuid::Twister);
    /* data file 1 */ {
      TMockMem mem_layer;
      /* insert <[int64_t]> <- int64_t */ {
        Insert(mem_layer, ++seq_num, int_idx, 2L,
               1L);
        Insert(mem_layer, ++seq_num, int_idx, 4L,
               2L);
        Insert(mem_layer, ++seq_num, int_idx, 6L,
               7L);
        Insert(mem_layer, ++seq_num, int_idx, 8L,
               91L);
        Insert(mem_layer, ++seq_num, int_idx, 2L,
               92L);
      }
      /* insert <[int64_t, int64_t]> <- int64_t */ {
        Insert(mem_layer, ++seq_num, int_int_idx, 2L,
               1L, 2L);
        Insert(mem_layer, ++seq_num, int_int_idx, 4L,
               2L, 4L);
        Insert(mem_layer, ++seq_num, int_int_idx, 6L,
               7L, 14L);
        Insert(mem_layer, ++seq_num, int_int_idx, 8L,
               91L, 182L);
        Insert(mem_layer, ++seq_num, int_int_idx, 2L,
               92L, 184L);
      }
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, data_file_id, data_gen_id, 20UL, 0U, RealTime);
    }
    /* exists <[int64_t]> */ {
      CheckExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, 1L);
      CheckExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, 2L);
      CheckExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, 7L);
      CheckExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, 91L);
      CheckExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, 92L);
      std::vector<TKey> expected;
      TSuprena arena;
      expected.push_back(TKey(make_tuple(1L), &arena, state_alloc));
      expected.push_back(TKey(make_tuple(2L), &arena, state_alloc));
      expected.push_back(TKey(make_tuple(7L), &arena, state_alloc));
      expected.push_back(TKey(make_tuple(91L), &arena, state_alloc));
      expected.push_back(TKey(make_tuple(92L), &arena, state_alloc));
      size_t pos = 0UL;
      auto cb = [&expected, &pos](const TKey &key) {
        EXPECT_EQ(key, expected[pos]);
        ++pos;
      };
      EXPECT_EQ(CheckExistsFree(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, cb, Native::TFree<int64_t>()), 5UL);
    }
    /* exists <[int64_t, int64_t]> */ {
      CheckExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_int_idx, 1L, 2L);
      CheckExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_int_idx, 2L, 4L);
      CheckExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_int_idx, 7L, 14L);
      CheckExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_int_idx, 91L, 182L);
      CheckExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_int_idx, 92L, 184L);
      /* check range <[1L, Free<int64_t>]> */ {
        TSuprena arena;
        TKey expected(make_tuple(1L, 2L), &arena, state_alloc);
        auto cb = [&expected](const TKey &key) {
          EXPECT_EQ(key, expected);
        };
        EXPECT_EQ(CheckExistsFree(mem_engine.GetEngine(), data_file_id, data_gen_id, int_int_idx, cb, 1L, Native::TFree<int64_t>()), 1UL);
      }
    }
    /* no exist <[int64_t]> */ {
      CheckNotExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, 3L);
      CheckNotExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, 4L);
      CheckNotExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, 8L);
      CheckNotExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, 90L);
      CheckNotExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, 93L);
    }
    /* no exist <[int64_t]> */ {
      CheckNotExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_int_idx, 1L, 184L);
      CheckNotExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_int_idx, 2L, 182L);
      CheckNotExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_int_idx, 7L, 2L);
      CheckNotExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_int_idx, 91L, 4L);
      CheckNotExists(mem_engine.GetEngine(), data_file_id, data_gen_id, int_int_idx, 92L, 14L);
    }
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
    GracefullShutdown();
  });
}

FIXTURE(Range) {
  Fiber::TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    TRAIITest required_thread_locals;
    const TScheduler::TPolicy scheduler_policy(10, 10, milliseconds(10));
    TScheduler scheduler;
    scheduler.SetPolicy(scheduler_policy);

    Sim::TMemEngine mem_engine(&scheduler,
                               4 * 1024 /* disk space: 4 GB */,
                               256,
                               262144 /* page cache slots: 1GB */,
                               1 /* num page lru */,
                               16384 /* block cache slots: 1GB */,
                               1 /* num block lru */);

    Base::TUuid data_file_id(TUuid::TimeAndMAC);
    TSuprena arena;
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    size_t data_gen_id = 1;
    TSequenceNumber seq_num = 0U;
    Base::TUuid int_idx(Base::TUuid::Twister);
    /* data file 1 */ {
      TMockMem mem_layer;
      for (int64_t i = 0; i < 11; i += 2) {
        Insert(mem_layer, ++seq_num, int_idx, static_cast<int64_t>(rand()),
                 i);
      }
      TDataFile data_file(mem_engine.GetEngine(), TVolume::TDesc::Fast, &mem_layer, data_file_id, data_gen_id, 20UL, 0U, RealTime);
    }
    /* full range using existing start, limit */ {
      TPresentWalkFileWrapper walker(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, TKey(make_tuple(0L), &arena, state_alloc), TKey(make_tuple(10L), &arena, state_alloc));
      size_t found = 0U;
      for (int64_t i = 0L; walker; ++walker, i += 2) {
        EXPECT_EQ(TKey((*walker).Key, (*walker).KeyArena), TKey(make_tuple(i), &arena, state_alloc));
        ++found;
      }
      EXPECT_EQ(found, 6U);
    }
    /* full range using existing start, non-existing limit */ {
      TPresentWalkFileWrapper walker(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, TKey(make_tuple(0L), &arena, state_alloc), TKey(make_tuple(11L), &arena, state_alloc));
      size_t found = 0U;
      for (int64_t i = 0L; walker; ++walker, i += 2) {
        EXPECT_EQ(TKey((*walker).Key, (*walker).KeyArena), TKey(make_tuple(i), &arena, state_alloc));
        ++found;
      }
      EXPECT_EQ(found, 6U);
    }
    /* partial range using existing start, limit */ {
      TPresentWalkFileWrapper walker(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, TKey(make_tuple(2L), &arena, state_alloc), TKey(make_tuple(8L), &arena, state_alloc));
      size_t found = 0U;
      for (int64_t i = 2L; walker; ++walker, i += 2) {
        EXPECT_EQ(TKey((*walker).Key, (*walker).KeyArena), TKey(make_tuple(i), &arena, state_alloc));
        ++found;
      }
      EXPECT_EQ(found, 4U);
    }
    /* partial range using existing start, non-existing limit */ {
      TPresentWalkFileWrapper walker(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, TKey(make_tuple(2L), &arena, state_alloc), TKey(make_tuple(9L), &arena, state_alloc));
      size_t found = 0U;
      for (int64_t i = 2L; walker; ++walker, i += 2) {
        EXPECT_EQ(TKey((*walker).Key, (*walker).KeyArena), TKey(make_tuple(i), &arena, state_alloc));
        ++found;
      }
      EXPECT_EQ(found, 4U);
    }
    /* full range using non-existing start, non-existing limit */ {
      TPresentWalkFileWrapper walker(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, TKey(make_tuple(-1L), &arena, state_alloc), TKey(make_tuple(11L), &arena, state_alloc));
      size_t found = 0U;
      for (int64_t i = 0L; walker; ++walker, i += 2) {
        EXPECT_EQ(TKey((*walker).Key, (*walker).KeyArena), TKey(make_tuple(i), &arena, state_alloc));
        ++found;
      }
      EXPECT_EQ(found, 6U);
    }
    /* partial range using non-existing start, non-existing limit */ {
      TPresentWalkFileWrapper walker(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, TKey(make_tuple(1L), &arena, state_alloc), TKey(make_tuple(9L), &arena, state_alloc));
      size_t found = 0U;
      for (int64_t i = 2L; walker; ++walker, i += 2) {
        EXPECT_EQ(TKey((*walker).Key, (*walker).KeyArena), TKey(make_tuple(i), &arena, state_alloc));
        ++found;
      }
      EXPECT_EQ(found, 4U);
    }
    /* empty range using non-existing start, non-existing limit */ {
      TPresentWalkFileWrapper walker(mem_engine.GetEngine(), data_file_id, data_gen_id, int_idx, TKey(make_tuple(11L), &arena, state_alloc), TKey(make_tuple(13L), &arena, state_alloc));
      EXPECT_FALSE(walker);
    }
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
    GracefullShutdown();
  });
}
