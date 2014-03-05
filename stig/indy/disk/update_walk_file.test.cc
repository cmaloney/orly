/* <stig/indy/disk/update_walk_file.test.cc>

   Unit test for <stig/indy/disk/update_walk_file.h>.

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

#include <stig/indy/disk/update_walk_file.h>

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
using namespace Stig::Indy::Util;

static const size_t BlockSize = Disk::Util::PhysicalBlockSize;
static const size_t CacheSize = 50000UL;
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
  }

  ~TRAIITest() {
    assert(TLocalReadFileCache::Cache);
    delete TLocalReadFileCache::Cache;
    TLocalReadFileCache::Cache = nullptr;
  }
};

FIXTURE(Typical) {
  Fiber::TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    TRAIITest required_thread_locals;
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    TScheduler scheduler(TScheduler::TPolicy(10, 10, milliseconds(10)));

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
    /* data file 1 */ {
      TMockMem mem_layer;
      for (int64_t i = 0; i < 11; i += 2) {
        mem_layer.Insert(TMockUpdate::NewMockUpdate(TUpdate::TOpByKey{ { TIndexKey(int_idx, TKey(make_tuple(i), &arena, state_alloc)), TKey(i * 10, &arena, state_alloc)}, { TIndexKey(int_idx, TKey(make_tuple(i + 1L), &arena, state_alloc)), TKey((i + 1L) * 10, &arena, state_alloc)} }, TKey(&arena), TKey(Base::TUuid(TUuid::Best), &arena, state_alloc), ++seq_num));
        mem_layer.Insert(TMockUpdate::NewMockUpdate(TUpdate::TOpByKey{ { TIndexKey(int_idx, TKey(make_tuple(i), &arena, state_alloc)), TKey(i * 10, &arena, state_alloc)}, { TIndexKey(int_idx, TKey(make_tuple(i + 1L), &arena, state_alloc)), TKey((i + 1L) * 10, &arena, state_alloc)} }, TKey(&arena), TKey(Base::TUuid(TUuid::Best), &arena, state_alloc), ++seq_num));
      }
      TDataFile data_file(mem_engine.GetEngine(), Disk::Util::TVolume::TDesc::Fast, &mem_layer, data_file_id, data_gen_id, 20UL, 0U, RealTime);
    }
    /* simple walker */ {
      TUpdateWalkFile walker(mem_engine.GetEngine(), data_file_id, data_gen_id, 0U);
      size_t found = 0U;
      for (int64_t i = 0L; walker; ++walker, ++i) {
        int64_t expected = (i / 2L * 2L);
        std::map<TIndexKey, TKey> entry_map;
        for (auto iter : (*walker).EntryVec) {
          entry_map.insert(make_pair(iter.first, TKey(iter.second, (*walker).MainArena)));
        }
        EXPECT_EQ((*walker).SequenceNumber, i + 1UL);
        EXPECT_EQ(entry_map.size(), 2UL);
        if (EXPECT_TRUE(entry_map.find(TIndexKey(int_idx, TKey(make_tuple(expected), &arena, state_alloc))) != entry_map.end())) {
          EXPECT_EQ(entry_map.find(TIndexKey(int_idx, TKey(make_tuple(expected), &arena, state_alloc)))->second, TKey(expected * 10, &arena, state_alloc));
        }
        if (EXPECT_TRUE(entry_map.find(TIndexKey(int_idx, TKey(make_tuple(expected + 1L), &arena, state_alloc))) != entry_map.end())) {
          EXPECT_EQ(entry_map.find(TIndexKey(int_idx, TKey(make_tuple(expected + 1L), &arena, state_alloc)))->second, TKey((expected + 1L) * 10L, &arena, state_alloc));
        }
        ++found;
      }
      EXPECT_EQ(found, 12U);
    }
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  });
}

FIXTURE(WithMergeFile) {
  Fiber::TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    TRAIITest required_thread_locals;
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    TScheduler scheduler(TScheduler::TPolicy(10, 10, milliseconds(10)));

    Sim::TMemEngine mem_engine(&scheduler,
                               4 * 1024 /* disk space: 4 GB */,
                               256,
                               262144 /* page cache slots: 1GB */,
                               1 /* num page lru */,
                               16384 /* block cache slots: 1GB */,
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
      TDataFile data_file(mem_engine.GetEngine(), Disk::Util::TVolume::TDesc::Fast, &mem_layer, file_id, 1UL, 20UL, 0U, RealTime);
    }
    /* data file 2 */ {
      TMockMem mem_layer;
      for (int64_t i = 0; i < 11; i += 2) {
        mem_layer.Insert(TMockUpdate::NewMockUpdate(TUpdate::TOpByKey{ { TIndexKey(int_idx, TKey(make_tuple(i), &arena, state_alloc)), TKey(i * 10, &arena, state_alloc)}, { TIndexKey(int_idx, TKey(make_tuple(i + 1L), &arena, state_alloc)), TKey((i + 1L) * 10, &arena, state_alloc)} }, TKey(&arena), TKey(Base::TUuid(TUuid::Best), &arena, state_alloc), ++seq_num));
        mem_layer.Insert(TMockUpdate::NewMockUpdate(TUpdate::TOpByKey{ { TIndexKey(int_idx, TKey(make_tuple(i), &arena, state_alloc)), TKey(i * 10, &arena, state_alloc)}, { TIndexKey(int_idx, TKey(make_tuple(i + 1L), &arena, state_alloc)), TKey((i + 1L) * 10, &arena, state_alloc)} }, TKey(&arena), TKey(Base::TUuid(TUuid::Best), &arena, state_alloc), ++seq_num));
      }
      TDataFile data_file(mem_engine.GetEngine(), Disk::Util::TVolume::TDesc::Fast, &mem_layer, file_id, 2UL, 20UL, 0U, RealTime);
    }
    /* merge them */ {
      TMergeDataFile merge_file(mem_engine.GetEngine(), Disk::Util::TVolume::TDesc::Fast, file_id, std::vector<size_t>{1UL, 2UL}, file_id, 3UL, 0U, Low, 100UL, 20UL, false, false);
    }
    /* simple walker */ {
      TUpdateWalkFile walker(mem_engine.GetEngine(), file_id, 3UL, 0U);
      size_t found = 0U;
      for (int64_t i = 0L; walker; ++walker, ++i) {
        int64_t expected = ((i % 12) / 2L * 2L);
        std::map<TIndexKey, TKey> entry_map;
        for (auto iter : (*walker).EntryVec) {
          entry_map.insert(make_pair(iter.first, TKey(iter.second, (*walker).MainArena)));
        }
        EXPECT_EQ((*walker).SequenceNumber, i + 1UL);
        EXPECT_EQ(entry_map.size(), 2UL);
        if (EXPECT_TRUE(entry_map.find(TIndexKey(int_idx, TKey(make_tuple(expected), &arena, state_alloc))) != entry_map.end())) {
          EXPECT_EQ(entry_map.find(TIndexKey(int_idx, TKey(make_tuple(expected), &arena, state_alloc)))->second, TKey(expected * 10, &arena, state_alloc));
        }
        if (EXPECT_TRUE(entry_map.find(TIndexKey(int_idx, TKey(make_tuple(expected + 1L), &arena, state_alloc))) != entry_map.end())) {
          EXPECT_EQ(entry_map.find(TIndexKey(int_idx, TKey(make_tuple(expected + 1L), &arena, state_alloc)))->second, TKey((expected + 1L) * 10L, &arena, state_alloc));
        }
        ++found;
      }
      EXPECT_EQ(found, 24U);
    }
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  });
}
