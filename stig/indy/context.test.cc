/* <stig/indy/context.test.broken.cc>

   Unit test for <stig/indy/context.h>.

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

#include <stig/indy/context.h>

#include <fstream>

#include <base/scheduler.h>
#include <stig/compiler.h>
#include <stig/honcho.h>
#include <stig/indy/disk/sim/mem_engine.h>
#include <stig/indy/fiber/fiber_test_runner.h>
#include <stig/server/repo_tetris_manager.h>
#include <stig/type/type_czar.h>

#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Stig;
using namespace Stig::Atom;
using namespace Stig::Indy;

const Stig::Indy::TMasterContext::TProtocol Stig::Indy::TMasterContext::TProtocol::Protocol;
const Stig::Indy::TSlaveContext::TProtocol Stig::Indy::TSlaveContext::TProtocol::Protocol;

Stig::Indy::Util::TPool Indy::L0::TManager::TRepo::TMapping::Pool(sizeof(TRepo::TMapping), "Repo Mapping", 100UL);
Stig::Indy::Util::TPool Indy::L0::TManager::TRepo::TMapping::TEntry::Pool(sizeof(TRepo::TMapping::TEntry), "Repo Mapping Entry", 200UL);
Stig::Indy::Util::TPool Indy::L0::TManager::TRepo::TDataLayer::Pool(sizeof(TMemoryLayer), "Data Layer", 200UL);

Stig::Indy::Util::TLocklessPool Disk::TDurableManager::TMapping::Pool(sizeof(Disk::TDurableManager::TMapping), "Durable Mapping", 10UL);
Stig::Indy::Util::TLocklessPool Disk::TDurableManager::TMapping::TEntry::Pool(sizeof(Disk::TDurableManager::TMapping::TEntry), "Durable Mapping Entry", 10UL);
Stig::Indy::Util::TPool Disk::TDurableManager::TDurableLayer::Pool(std::max(sizeof(Disk::TDurableManager::TMemSlushLayer), sizeof(Disk::TDurableManager::TDiskOrderedLayer)), "Durable Layer", 10UL);
Stig::Indy::Util::TPool Disk::TDurableManager::TMemSlushLayer::TDurableEntry::Pool(sizeof(Disk::TDurableManager::TMemSlushLayer::TDurableEntry), "Durable Entry", 10UL);

Stig::Indy::Util::TPool L1::TTransaction::TMutation::Pool(max(max(sizeof(L1::TTransaction::TPusher), sizeof(L1::TTransaction::TPopper)), sizeof(L1::TTransaction::TStatusChanger)), "Transaction::TMutation", 100UL);
Stig::Indy::Util::TPool L1::TTransaction::Pool(sizeof(L1::TTransaction), "Transaction", 100UL);

Stig::Indy::Util::TPool TUpdate::Pool(sizeof(TUpdate), "Update", 20000UL);
Stig::Indy::Util::TPool TUpdate::TEntry::Pool(sizeof(TUpdate::TEntry), "Entry", 40000UL);
Disk::TBufBlock::TPool Disk::TBufBlock::Pool(Disk::Util::PhysicalBlockSize, 20000UL);

Base::TSigmaCalc Stig::Server::TSession::TServer::TryReadTimeCalc;
Base::TSigmaCalc Stig::Server::TSession::TServer::TryReadCPUTimeCalc;
Base::TSigmaCalc Stig::Server::TSession::TServer::TryWriteTimeCalc;
Base::TSigmaCalc Stig::Server::TSession::TServer::TryWriteCPUTimeCalc;
Base::TSigmaCalc Stig::Server::TSession::TServer::TryWalkerCountCalc;
Base::TSigmaCalc Stig::Server::TSession::TServer::TryCallCPUTimerCalc;
Base::TSigmaCalc Stig::Server::TSession::TServer::TryReadCallTimerCalc;
Base::TSigmaCalc Stig::Server::TSession::TServer::TryWriteCallTimerCalc;
Base::TSigmaCalc Stig::Server::TSession::TServer::TryWalkerConsTimerCalc;
Base::TSigmaCalc Stig::Server::TSession::TServer::TryFetchCountCalc;
Base::TSigmaCalc Stig::Server::TSession::TServer::TryHashHitCountCalc;
Base::TSigmaCalc Stig::Server::TSession::TServer::TryWriteSyncHitCalc;
Base::TSigmaCalc Stig::Server::TSession::TServer::TryWriteSyncTimeCalc;
Base::TSigmaCalc Stig::Server::TSession::TServer::TryReadSyncHitCalc;
Base::TSigmaCalc Stig::Server::TSession::TServer::TryReadSyncTimeCalc;
std::mutex       Stig::Server::TSession::TServer::TryTimeLock;

void StateChanged(TManager::TState) {}

void UpdateReplicationNotificationCb(const Base::TUuid &, const Base::TUuid &, const Base::TUuid &) {}
void OnReplicateIndexIdCb(const Base::TUuid &/*idx_id*/, const Indy::TKey &/*key*/, const Indy::TKey &/*val*/) {}
void ForEachIndexCb(const std::function<void (const Base::TUuid &/*idx_id*/, const Indy::TKey &/*key*/, const Indy::TKey &/*val*/)> &) {}
void ForEachSchedulerCb(const std::function<bool (Fiber::TRunner *)> &) {}

const size_t ReplicationSyncBufMB = 16UL;
const size_t MergeMemInterval = 10UL;
const size_t MergeDiskInterval = 100UL;
const size_t LayerCleaningInterval = 1000UL;
const size_t ReplicationInterval = 100UL;
const bool AllowTailing = true;
const bool AllowFileSync = true;
const bool NoRealtime = true;
const size_t BlockSlotsAvailablePerMerger = 100UL;
const size_t MaxRepoCacheSize = 100UL;
const size_t WalkerLocalCacheSize = 100UL;
const size_t TempFileConsolidationThreshold = 20UL;

static const char *sample_package = "package #1;"
                                    "test_func = (true);";

bool PrintCmds = false; /* For starsha runner */

void RunTestLogic(const std::function<void (const Base::TUuid &session_id,
                                            const std::unique_ptr<TManager> &manager,
                                            const TManager::TPtr<Indy::TRepo> &global_repo,
                                            const TManager::TPtr<Indy::TRepo> &repo,
                                            Base::TScheduler &scheduler)> &cb) {
  Fiber::TFiberTestRunner runner([&](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &runner_cons) {
    using TLocalReadFileCache = Stig::Indy::Disk::TLocalReadFileCache<Stig::Indy::Disk::Util::LogicalPageSize,
      Stig::Indy::Disk::Util::LogicalBlockSize,
      Stig::Indy::Disk::Util::PhysicalBlockSize,
      Stig::Indy::Disk::Util::CheckedPage, true>;
    if (!TLocalReadFileCache::Cache) {
      TLocalReadFileCache::Cache = new TLocalReadFileCache();
    }
    if (!Disk::TLocalWalkerCache::Cache) {
      Disk::TLocalWalkerCache::Cache = new Disk::TLocalWalkerCache();
    }
    /* all our RAII scopes */ {
      Stig::THoncho Honcho;
      /* try to compile the binary */
      ofstream package_file;
      package_file.open("/tmp/context.stig");
      package_file << sample_package;
      package_file.close();
      Stig::Compiler::Compile(Jhm::TAbsPath(Jhm::TAbsBase("/tmp/"), Jhm::TRelPath("context.stig")), string("/tmp/"), false, false, false);

      Base::TThreadLocalPoolManager<Stig::Indy::Fiber::TFrame, size_t, Stig::Indy::Fiber::TRunner *> *frame_pool_manager = Stig::Indy::Fiber::TFrame::LocalFramePool->GetPoolManager();
      Base::TFd solo_sock;
      auto wait_for_slave = [](const shared_ptr<function<void (const TFd &)>> &/*cb*/) {};
      const TScheduler::TPolicy scheduler_policy(20, 20, milliseconds(10));
      TScheduler scheduler;
      scheduler.SetPolicy(scheduler_policy);
      auto util_reporter = make_shared<Disk::TIndyUtilReporter>();

      Stig::Indy::Disk::Sim::TMemEngine mem_engine(&scheduler,
                                                   256 /* fast disk space: 256MB */,
                                                   64 /* slow disk space: 64MB */,
                                                   128 /* page cache slots: 8MB */,
                                                   1 /* num page lru */,
                                                   64 /* block cache slots: 4MB */,
                                                   1 /* num block lru */);

      Fiber::TRunner bg_runner(runner_cons);
      std::vector<size_t> merge_mem_cores{0};
      std::vector<size_t> merge_disk_cores{0};
      unique_ptr<TManager> manager(new TManager(mem_engine.GetEngine(),
                                                ReplicationSyncBufMB,
                                                MergeMemInterval,
                                                MergeDiskInterval,
                                                LayerCleaningInterval,
                                                ReplicationInterval,
                                                TManager::Solo,
                                                AllowTailing,
                                                AllowFileSync,
                                                NoRealtime,
                                                std::move(solo_sock),
                                                wait_for_slave,
                                                StateChanged,
                                                UpdateReplicationNotificationCb,
                                                OnReplicateIndexIdCb,
                                                ForEachIndexCb,
                                                ForEachSchedulerCb,
                                                &scheduler,
                                                &bg_runner,
                                                BlockSlotsAvailablePerMerger,
                                                MaxRepoCacheSize,
                                                WalkerLocalCacheSize,
                                                TempFileConsolidationThreshold,
                                                merge_mem_cores,
                                                merge_disk_cores,
                                                true));
      auto durable_manager = make_shared<Stig::Indy::Disk::TDurableManager>(&scheduler,
                                                                            runner_cons,
                                                                            frame_pool_manager,
                                                                            manager.get(),
                                                                            manager->GetEngine(),
                                                                            10UL,
                                                                            100UL,
                                                                            1000UL,
                                                                            1000UL,
                                                                            20,
                                                                            true);
      manager->SetDurableManager(durable_manager);
      auto tetris_runner_setup_cb = [](Indy::Fiber::TRunner */*runner*/) {};
      Stig::Package::TManager package_manager(std::string("/tmp"));
      Stig::Server::TRepoTetrisManager *tetris_manager= new Stig::Server::TRepoTetrisManager(&scheduler, runner_cons, frame_pool_manager, tetris_runner_setup_cb, true /*is master*/, manager.get(), &package_manager, durable_manager.get(), true);
      manager->SetTetrisManager(tetris_manager);

      package_manager.Install(unordered_set<Package::TVersionedName>{ Package::TVersionedName { string("context"), 1 } });

      /* Merge memory layers in a repo. */
      Fiber::TRunner mem_merge_runner(runner_cons);
      scheduler.Schedule(std::bind(Fiber::LaunchSlowFiberSched, &mem_merge_runner, frame_pool_manager));
      Fiber::TFrame *mem_merge_frame = Fiber::TFrame::LocalFramePool->Alloc();
      try {
        mem_merge_frame->Latch(&mem_merge_runner, static_cast<Stig::Indy::L0::TManager *>(manager.get()), static_cast<Fiber::TRunnable::TFunc>(&Stig::Indy::L0::TManager::RunMergeMem));
      } catch (...) {
        Fiber::TFrame::LocalFramePool->Free(mem_merge_frame);
        throw;
      }
      /* Merge multiple disk files of a specific size category, in the same safe repo. */
      Fiber::TRunner disk_merge_runner(runner_cons);
      scheduler.Schedule(std::bind(Fiber::LaunchSlowFiberSched, &disk_merge_runner, frame_pool_manager));
      Fiber::TFrame *disk_merge_frame = Fiber::TFrame::LocalFramePool->Alloc();
      try {
        disk_merge_frame->Latch(&disk_merge_runner, static_cast<Stig::Indy::L0::TManager *>(manager.get()), static_cast<Fiber::TRunnable::TFunc>(&Stig::Indy::L0::TManager::RunMergeDisk));
      } catch (...) {
        Fiber::TFrame::LocalFramePool->Free(disk_merge_frame);
        throw;
      }

      TUuid global_repo_id(TUuid::Best);
      TUuid repo_id(TUuid::Best);

      auto global_repo = manager->NewSafeRepo(global_repo_id, TTtl(0UL));
      auto repo = manager->NewFastRepo(repo_id, TTtl(0UL), global_repo);

      Base::TUuid session_id(Base::TUuid::Twister);
      auto session = durable_manager->New<Stig::Server::TSession>(session_id, TTtl(0UL));

      cb(session_id, manager, global_repo, repo, scheduler);

      sleep(2);
      delete tetris_manager;
      session.Reset();
      durable_manager->Clear();
      durable_manager.reset();
      repo.Reset();
      sleep(2);
      global_repo.Reset();
      mem_merge_runner.ShutDown();
      disk_merge_runner.ShutDown();
    }  // close all of our RAII scopes
    if (TLocalReadFileCache::Cache) {
      delete TLocalReadFileCache::Cache;
    }
    if (Disk::TLocalWalkerCache::Cache) {
      delete Disk::TLocalWalkerCache::Cache;
    }
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  }, 6);
}

FIXTURE(Typical) {
  RunTestLogic([](const Base::TUuid &session_id,
                  const std::unique_ptr<TManager> &manager,
                  const TManager::TPtr<Indy::TRepo> &global_repo,
                  const TManager::TPtr<Indy::TRepo> &repo,
                  Base::TScheduler &/*scheduler*/) {
    int64_t num_iter = 10L;
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());

    TUuid idx_id(TUuid::Best);
    TSuprena arena;
    std::vector<Base::TUuid> transaction_id_vec;
    for(int64_t i = 0; i < num_iter; ++i) {
      transaction_id_vec.push_back(Base::TUuid(TUuid::Best));
    }


    const std::vector<std::string> fq_name {"context"};
    const std::string method_name = "test_func";
    uint32_t random_seed = 0;
    Base::Chrono::TTimePnt run_time = Base::Chrono::CreateTimePnt(2013, 10, 23, 17, 47, 14, 0, 0);

    /* write an update with the same uuid to the global repo */ {
      Base::TUuid update_id(Base::TUuid::Twister);
      Stig::Server::TMetaRecord meta_record(
          update_id,
          Stig::Server::TMetaRecord::TEntry(
              session_id, session_id, fq_name, method_name,
              Stig::Server::TMetaRecord::TEntry::TArgByName{},
              Stig::Server::TMetaRecord::TEntry::TExpectedPredicateResults{},
              run_time, random_seed)
      );
      auto transaction = manager->NewTransaction();
      auto update = TUpdate::NewUpdate(TUpdate::TOpByKey{ { TIndexKey(idx_id, TKey(make_tuple(num_iter - 1L), &arena, state_alloc)), TKey(666L, &arena, state_alloc)} }, TKey(meta_record, &arena, state_alloc), TKey(transaction_id_vec[num_iter - 1], &arena, state_alloc));
      transaction->Push(global_repo, update);
      transaction->Prepare();
      transaction->CommitAction();
    }
    for (int64_t i = 0; i < num_iter; ++i) {
      /* write num_iter updates to the repo */
      Base::TUuid update_id(Base::TUuid::Twister);
      Stig::Server::TMetaRecord meta_record(
          update_id,
          Stig::Server::TMetaRecord::TEntry(
              session_id, session_id, fq_name, method_name,
              Stig::Server::TMetaRecord::TEntry::TArgByName{},
              Stig::Server::TMetaRecord::TEntry::TExpectedPredicateResults{},
              run_time, random_seed)
      );
      auto transaction = manager->NewTransaction();
      auto update = TUpdate::NewUpdate(TUpdate::TOpByKey{ { TIndexKey(idx_id, TKey(make_tuple(i), &arena, state_alloc)), TKey(i * 10L, &arena, state_alloc)} }, TKey(meta_record, &arena, state_alloc), TKey(transaction_id_vec[i], &arena, state_alloc));
      transaction->Push(repo, update);
      transaction->Prepare();
      transaction->CommitAction();
    }
    /* context scope */ {
      TSuprena context_arena;
      TContext context(repo, &context_arena);
      /* check that we are not reading the global update */ {
        TIndexKey search_key(idx_id, TKey(make_tuple(num_iter - 1L), &arena, state_alloc));
        EXPECT_EQ(context[search_key], TKey((num_iter - 1L) * 10L, &arena, state_alloc));
        EXPECT_TRUE(context.Exists(search_key));
      }
      for (int64_t i = 0; i < num_iter; ++i) {
        TIndexKey search_key(idx_id, TKey(make_tuple(i), &arena, state_alloc));
        EXPECT_EQ(context[search_key], TKey(i * 10L, &arena, state_alloc));
        EXPECT_TRUE(context.Exists(search_key));
      }
      for (int64_t i = num_iter; i < num_iter * 2; ++i) {
        TIndexKey search_key(idx_id, TKey(make_tuple(i), &arena, state_alloc));
        EXPECT_FALSE(context.Exists(search_key));
      }
    }  // context scope
  });
}

FIXTURE(Keys) {
  RunTestLogic([](const Base::TUuid &session_id,
                  const std::unique_ptr<TManager> &manager,
                  const TManager::TPtr<Indy::TRepo> &global_repo,
                  const TManager::TPtr<Indy::TRepo> &repo,
                  Base::TScheduler &scheduler) {
    int64_t num_iter = 10L;
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());

    TUuid idx_id(TUuid::Best);
    TSuprena arena;
    std::vector<Base::TUuid> transaction_id_vec;
    for(int64_t i = 0; i < num_iter; ++i) {
      transaction_id_vec.push_back(Base::TUuid(TUuid::Best));
    }


    const std::vector<std::string> fq_name {"context"};
    const std::string method_name = "test_func";
    uint32_t random_seed = 0;
    Base::Chrono::TTimePnt run_time = Base::Chrono::CreateTimePnt(2013, 10, 23, 17, 47, 14, 0, 0);

    /* write an update with the same uuid to the global repo */ {
      Base::TUuid update_id(Base::TUuid::Twister);
      Stig::Server::TMetaRecord meta_record(
          update_id,
          Stig::Server::TMetaRecord::TEntry(
              session_id, session_id, fq_name, method_name,
              Stig::Server::TMetaRecord::TEntry::TArgByName{},
              Stig::Server::TMetaRecord::TEntry::TExpectedPredicateResults{},
              run_time, random_seed)
      );
      auto transaction = manager->NewTransaction();
      auto update = TUpdate::NewUpdate(TUpdate::TOpByKey{ { TIndexKey(idx_id, TKey(make_tuple(num_iter - 1L), &arena, state_alloc)), TKey(666L, &arena, state_alloc)} }, TKey(meta_record, &arena, state_alloc), TKey(transaction_id_vec[num_iter - 1], &arena, state_alloc));
      transaction->Push(global_repo, update);
      transaction->Prepare();
      transaction->CommitAction();
    }
    for (int64_t i = 0; i < num_iter; ++i) {
      /* write num_iter updates to the repo */
      Base::TUuid update_id(Base::TUuid::Twister);
      Stig::Server::TMetaRecord meta_record(
          update_id,
          Stig::Server::TMetaRecord::TEntry(
              session_id, session_id, fq_name, method_name,
              Stig::Server::TMetaRecord::TEntry::TArgByName{},
              Stig::Server::TMetaRecord::TEntry::TExpectedPredicateResults{},
              run_time, random_seed)
      );
      auto transaction = manager->NewTransaction();
      auto update = TUpdate::NewUpdate(TUpdate::TOpByKey{ { TIndexKey(idx_id, TKey(make_tuple(i), &arena, state_alloc)), TKey(i * 10L, &arena, state_alloc)} }, TKey(meta_record, &arena, state_alloc), TKey(transaction_id_vec[i], &arena, state_alloc));
      transaction->Push(repo, update);
      transaction->Prepare();
      transaction->CommitAction();
    }
    /* context scope */ {
      TSuprena context_arena;
      TContext context(repo, &context_arena);
      TIndyContext package_context(Rt::TOpt<TUUID>(), TUUID(), context, &context_arena, &scheduler, run_time, random_seed);

      std::unique_ptr<TKeyCursor> key_cursor_ptr(package_context.NewKeyCursor(&context, TIndexKey(idx_id, TKey(make_tuple(Native::TFree<int64_t>()), &arena, state_alloc))));
      TKeyCursor &cursor = *key_cursor_ptr;
      int64_t expected = 0;
      for (; cursor; ++cursor) {
        EXPECT_EQ(*cursor, TKey(make_tuple(expected), &arena, state_alloc));
        EXPECT_EQ(context[TIndexKey(idx_id, *cursor)], TKey(expected * 10L, &arena, state_alloc));
        ++expected;
      }
      EXPECT_EQ(expected, num_iter);
    }  // context scope
  });
}

FIXTURE(Tombstone) {
  RunTestLogic([](const Base::TUuid &session_id,
                  const std::unique_ptr<TManager> &manager,
                  const TManager::TPtr<Indy::TRepo> &/*global_repo*/,
                  const TManager::TPtr<Indy::TRepo> &repo,
                  Base::TScheduler &scheduler) {
    int64_t num_iter = 10L;
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());

    TUuid idx_id(TUuid::Best);
    TSuprena arena;
    std::vector<Base::TUuid> transaction_id_vec;
    for(int64_t i = 0; i < num_iter; ++i) {
      transaction_id_vec.push_back(Base::TUuid(TUuid::Best));
    }


    const std::vector<std::string> fq_name {"context"};
    const std::string method_name = "test_func";
    uint32_t random_seed = 0;
    Base::Chrono::TTimePnt run_time = Base::Chrono::CreateTimePnt(2013, 10, 23, 17, 47, 14, 0, 0);

    for (int64_t i = 0; i < num_iter; ++i) {
      /* write num_iter updates to the repo */
      Base::TUuid update_id(Base::TUuid::Twister);
      Stig::Server::TMetaRecord meta_record(
          update_id,
          Stig::Server::TMetaRecord::TEntry(
              session_id, session_id, fq_name, method_name,
              Stig::Server::TMetaRecord::TEntry::TArgByName{},
              Stig::Server::TMetaRecord::TEntry::TExpectedPredicateResults{},
              run_time, random_seed)
      );
      auto transaction = manager->NewTransaction();
      auto update = TUpdate::NewUpdate(TUpdate::TOpByKey{ { TIndexKey(idx_id, TKey(make_tuple(i), &arena, state_alloc)), TKey(i * 10L, &arena, state_alloc)} }, TKey(meta_record, &arena, state_alloc), TKey(transaction_id_vec[i], &arena, state_alloc));
      transaction->Push(repo, update);
      transaction->Prepare();
      transaction->CommitAction();
    }
    /* Tombstone a key */ {
      Base::TUuid update_id(Base::TUuid::Twister);
      Stig::Server::TMetaRecord meta_record(
          update_id,
          Stig::Server::TMetaRecord::TEntry(
              session_id, session_id, fq_name, method_name,
              Stig::Server::TMetaRecord::TEntry::TArgByName{},
              Stig::Server::TMetaRecord::TEntry::TExpectedPredicateResults{},
              run_time, random_seed)
      );
      auto transaction = manager->NewTransaction();
      auto update = TUpdate::NewUpdate(TUpdate::TOpByKey{ { TIndexKey(idx_id, TKey(make_tuple(0L), &arena, state_alloc)), TKey(Native::TTombstone::Tombstone, &arena, state_alloc)} }, TKey(meta_record, &arena, state_alloc), TKey(TUuid(TUuid::Best), &arena, state_alloc));
      transaction->Push(repo, update);
      transaction->Prepare();
      transaction->CommitAction();
    }
    /* context scope */ {
      TSuprena context_arena;
      TContext context(repo, &context_arena);
      TIndyContext package_context(Rt::TOpt<TUUID>(), TUUID(), context, &context_arena, &scheduler, run_time, random_seed);

      std::unique_ptr<TKeyCursor> key_cursor_ptr(package_context.NewKeyCursor(&context, TIndexKey(idx_id, TKey(make_tuple(Native::TFree<int64_t>()), &arena, state_alloc))));
      TKeyCursor &cursor = *key_cursor_ptr;
      int64_t expected = 1L;
      for (; cursor; ++cursor) {
        EXPECT_EQ(*cursor, TKey(make_tuple(expected), &arena, state_alloc));
        EXPECT_EQ(context[TIndexKey(idx_id, *cursor)], TKey(expected * 10L, &arena, state_alloc));
        ++expected;
      }
      EXPECT_EQ(expected, num_iter);
    }  // context scope
  });
}