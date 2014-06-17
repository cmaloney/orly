/* <orly/indy/disk/data_file.test.cc>

   Unit test for <orly/indy/disk/data_file.h>.

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

#include <orly/indy/disk/data_file.h>

#include <valgrind/callgrind.h>

#include <base/scheduler.h>
#include <base/usage_meter.h>
#include <orly/indy/disk/disk_test.h>
#include <orly/indy/disk/read_file.h>
#include <orly/indy/disk/sim/mem_engine.h>
#include <orly/indy/disk/test_file_service.h>
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

static const size_t BlockSize = Disk::Util::PhysicalBlockSize;

Orly::Indy::Util::TPool L0::TManager::TRepo::TMapping::Pool(sizeof(TRepo::TMapping), "Repo Mapping");
Orly::Indy::Util::TPool L0::TManager::TRepo::TMapping::TEntry::Pool(sizeof(TRepo::TMapping::TEntry), "Repo Mapping Entry");
Orly::Indy::Util::TPool L0::TManager::TRepo::TDataLayer::Pool(sizeof(TMemoryLayer), "Data Layer");

Orly::Indy::Util::TPool TUpdate::Pool(sizeof(TUpdate), "Update", 750010UL);
Orly::Indy::Util::TPool TUpdate::TEntry::Pool(sizeof(TUpdate::TEntry), "Entry", 1500020UL);
Disk::TBufBlock::TPool Disk::TBufBlock::Pool(BlockSize, 2000UL);

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
    const string orly_str("Orly");
    const string mofo_str("Mofo");
    typedef tuple<int64_t, string, int64_t, int64_t> TTup;
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    vector<TTup> val_vec;
    /* Make a data file */ {
      TSuprena arena;
      TMockMem mem_layer;
      for (int64_t i = 0; i < num_iter; ++i) {  /* insert data */
        val_vec.emplace_back(i % 7L, (i % 2 == 0 ? orly_str : mofo_str), 1L + i * 2L, 7L * i);
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
      TStream<Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::PhysicalBlockSize, Orly::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
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
