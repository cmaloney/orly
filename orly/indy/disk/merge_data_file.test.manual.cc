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
    const string mofo_str("Mofo");
    const string orly_str("Orly");
    typedef tuple<int64_t, string, int64_t, int64_t> TTup;
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    vector<TTup> val_vec;
    vector<size_t> data_gen_vec;
    /* Make the data file */ {
      for (int64_t i = 0; i < num_iter; ++i) {  /* insert data */
        val_vec.emplace_back(i % 7L, (i % 2 == 0 ? mofo_str : orly_str), 1L + i * 2L, 7L * i);
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
      TStream<Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::PhysicalBlockSize, Orly::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
      size_t out_offset;
      /* ensure they exist in the hash */
      int64_t found_in_hash = 0UL;
      Base::TTimer hash_timer;
      hash_timer.Start();
      //CALLGRIND_START_INSTRUMENTATION;
      for (int64_t i = 0; i < num_iter; ++i) {
        found_in_hash += idx_file.FindInHash(TKey(make_tuple(i % 7L, (i % 2 == 0 ? mofo_str : orly_str), 1L + i * 2L), &arena, state_alloc), out_offset, in_stream, &index_arena) ? 1UL : 0UL;
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
      const string mofo_str("Mofo");
      const string orly_str("Orly");
      typedef tuple<int64_t, string, int64_t, int64_t> TTup;
      vector<TTup> val_vec;
      vector<size_t> data_gen_vec;
      /* Make the data file */ {
        for (int64_t i = 0; i < num_iter; ++i) {  /* insert data */
          val_vec.emplace_back(i % 7L, (i % 2 == 0 ? mofo_str : orly_str), 1L + i * 2L, 7L * i);
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
        TStream<Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::LogicalBlockSize, Orly::Indy::Disk::Util::PhysicalBlockSize, Orly::Indy::Disk::Util::PageCheckedBlock, 0UL> in_stream(HERE, Source::PresentWalk, RealTime, &reader, mem_engine.GetEngine()->GetCache<TReader::PhysicalCachePageSize>(), 0);
        size_t out_offset;
        /* ensure they exist in the hash */
        int64_t found_in_hash = 0UL;
        Base::TTimer hash_timer;
        hash_timer.Start();
        for (int64_t i = 0; i < num_iter; ++i) {
          found_in_hash += idx_file.FindInHash(TKey(make_tuple(i % 7L, (i % 2 == 0 ? mofo_str : orly_str), 1L + i * 2L), &arena, state_alloc), out_offset, in_stream, &index_arena) ? 1UL : 0UL;
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
