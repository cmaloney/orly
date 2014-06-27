/* <orly/indy/context.test.broken.cc>

   Unit test for <orly/indy/context.h>.

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

#include <orly/indy/context.h>

#include <fstream>

#include <base/scheduler.h>
#include <orly/compiler.h>
#include <orly/spa/honcho.h>
#include <orly/indy/disk/sim/mem_engine.h>
#include <orly/indy/fiber/fiber_test_runner.h>
#include <orly/server/repo_tetris_manager.h>
#include <orly/type/type_czar.h>

#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Orly;
using namespace Orly::Atom;
using namespace Orly::Indy;

const Orly::Indy::TMasterContext::TProtocol Orly::Indy::TMasterContext::TProtocol::Protocol;
const Orly::Indy::TSlaveContext::TProtocol Orly::Indy::TSlaveContext::TProtocol::Protocol;

Orly::Indy::Util::TPool Indy::L0::TManager::TRepo::TMapping::Pool(sizeof(TRepo::TMapping), "Repo Mapping", 100UL);
Orly::Indy::Util::TPool Indy::L0::TManager::TRepo::TMapping::TEntry::Pool(sizeof(TRepo::TMapping::TEntry), "Repo Mapping Entry", 200UL);
Orly::Indy::Util::TPool Indy::L0::TManager::TRepo::TDataLayer::Pool(sizeof(TMemoryLayer), "Data Layer", 200UL);

Orly::Indy::Util::TLocklessPool Disk::TDurableManager::TMapping::Pool(sizeof(Disk::TDurableManager::TMapping), "Durable Mapping", 10UL);
Orly::Indy::Util::TLocklessPool Disk::TDurableManager::TMapping::TEntry::Pool(sizeof(Disk::TDurableManager::TMapping::TEntry), "Durable Mapping Entry", 10UL);
Orly::Indy::Util::TPool Disk::TDurableManager::TDurableLayer::Pool(std::max(sizeof(Disk::TDurableManager::TMemSlushLayer), sizeof(Disk::TDurableManager::TDiskOrderedLayer)), "Durable Layer", 10UL);
Orly::Indy::Util::TPool Disk::TDurableManager::TMemSlushLayer::TDurableEntry::Pool(sizeof(Disk::TDurableManager::TMemSlushLayer::TDurableEntry), "Durable Entry", 10UL);

Orly::Indy::Util::TPool L1::TTransaction::TMutation::Pool(max(max(sizeof(L1::TTransaction::TPusher), sizeof(L1::TTransaction::TPopper)), sizeof(L1::TTransaction::TStatusChanger)), "Transaction::TMutation", 100UL);
Orly::Indy::Util::TPool L1::TTransaction::Pool(sizeof(L1::TTransaction), "Transaction", 100UL);

Orly::Indy::Util::TPool TUpdate::Pool(sizeof(TUpdate), "Update", 20000UL);
Orly::Indy::Util::TPool TUpdate::TEntry::Pool(sizeof(TUpdate::TEntry), "Entry", 40000UL);
Disk::TBufBlock::TPool Disk::TBufBlock::Pool(Disk::Util::PhysicalBlockSize, 20000UL);

Base::TSigmaCalc Orly::Server::TSession::TServer::TryReadTimeCalc;
Base::TSigmaCalc Orly::Server::TSession::TServer::TryReadCPUTimeCalc;
Base::TSigmaCalc Orly::Server::TSession::TServer::TryWriteTimeCalc;
Base::TSigmaCalc Orly::Server::TSession::TServer::TryWriteCPUTimeCalc;
Base::TSigmaCalc Orly::Server::TSession::TServer::TryWalkerCountCalc;
Base::TSigmaCalc Orly::Server::TSession::TServer::TryCallCPUTimerCalc;
Base::TSigmaCalc Orly::Server::TSession::TServer::TryReadCallTimerCalc;
Base::TSigmaCalc Orly::Server::TSession::TServer::TryWriteCallTimerCalc;
Base::TSigmaCalc Orly::Server::TSession::TServer::TryWalkerConsTimerCalc;
Base::TSigmaCalc Orly::Server::TSession::TServer::TryFetchCountCalc;
Base::TSigmaCalc Orly::Server::TSession::TServer::TryHashHitCountCalc;
Base::TSigmaCalc Orly::Server::TSession::TServer::TryWriteSyncHitCalc;
Base::TSigmaCalc Orly::Server::TSession::TServer::TryWriteSyncTimeCalc;
Base::TSigmaCalc Orly::Server::TSession::TServer::TryReadSyncHitCalc;
Base::TSigmaCalc Orly::Server::TSession::TServer::TryReadSyncTimeCalc;
std::mutex       Orly::Server::TSession::TServer::TryTimeLock;

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
const size_t TempFileConsolidationThreshold = 20UL;

static const char *sample_package = "package #1;"
                                    "test_func = (true);";

void RunTestLogic(const std::function<void (const Base::TUuid &session_id,
                                            const std::unique_ptr<TManager> &manager,
                                            const TManager::TPtr<Indy::TRepo> &global_repo,
                                            const TManager::TPtr<Indy::TRepo> &repo,
                                            Base::TScheduler &scheduler)> &cb) {
  Fiber::TFiberTestRunner runner([&](std::mutex &mut,
                                     std::condition_variable &cond,
                                     bool &fin,
                                     Fiber::TRunner::TRunnerCons &runner_cons) {
    using TLocalReadFileCache = Orly::Indy::Disk::TLocalReadFileCache<Orly::Indy::Disk::Util::LogicalPageSize,
      Orly::Indy::Disk::Util::LogicalBlockSize,
      Orly::Indy::Disk::Util::PhysicalBlockSize,
      Orly::Indy::Disk::Util::CheckedPage, true>;
    if (!TLocalReadFileCache::Cache) {
      TLocalReadFileCache::Cache = new TLocalReadFileCache();
    }
    if (!Disk::TLocalWalkerCache::Cache) {
      Disk::TLocalWalkerCache::Cache = new Disk::TLocalWalkerCache();
    }
    /* all our RAII scopes */ {
      Orly::Spa::THoncho Honcho;
      /* try to compile the binary */
      ofstream package_file;
      package_file.open("/tmp/context.orly");
      package_file << sample_package;
      package_file.close();
      Orly::Compiler::Compile(Jhm::TAbsPath(Jhm::TAbsBase("/tmp/"), Jhm::TRelPath("context.orly")), string("/tmp/"), false, false, false);

      Base::TThreadLocalGlobalPoolManager<Orly::Indy::Fiber::TFrame, size_t, Orly::Indy::Fiber::TRunner *> *frame_pool_manager = Orly::Indy::Fiber::TFrame::LocalFramePool->GetPoolManager();
      Base::TFd solo_sock;
      auto wait_for_slave = [](const shared_ptr<function<void (const TFd &)>> &/*cb*/) {};
      const TScheduler::TPolicy scheduler_policy(20, 20, milliseconds(10));
      TScheduler scheduler;
      scheduler.SetPolicy(scheduler_policy);
      auto util_reporter = make_shared<Disk::TIndyUtilReporter>();

      Orly::Indy::Disk::Sim::TMemEngine mem_engine(&scheduler,
                                                   256 /* fast disk space: 256MB */,
                                                   64 /* slow disk space: 64MB */,
                                                   128 /* page cache slots: 8MB */,
                                                   1 /* num page lru */,
                                                   64 /* block cache slots: 4MB */,
                                                   1 /* num block lru */);

      Fiber::TRunner bg_runner(runner_cons);
      std::vector<size_t> merge_mem_cores{0};
      std::vector<size_t> merge_disk_cores{0};
      auto manager = make_unique<TManager>(mem_engine.GetEngine(),
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
                                           TempFileConsolidationThreshold,
                                           merge_mem_cores,
                                           merge_disk_cores,
                                           true);
      auto durable_manager = make_shared<Orly::Indy::Disk::TDurableManager>(&scheduler,
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
      Orly::Package::TManager package_manager(std::string("/tmp"));
      Orly::Server::TRepoTetrisManager *tetris_manager= new Orly::Server::TRepoTetrisManager(&scheduler, runner_cons, frame_pool_manager, tetris_runner_setup_cb, true /*is master*/, manager.get(), &package_manager, durable_manager.get(), true);
      manager->SetTetrisManager(tetris_manager);

      package_manager.Install(unordered_set<Package::TVersionedName>{ Package::TVersionedName { string("context"), 1 } });

      /* Merge memory layers in a repo. */
      Fiber::TRunner mem_merge_runner(runner_cons);
      scheduler.Schedule(std::bind(Fiber::LaunchSlowFiberSched, &mem_merge_runner, frame_pool_manager));
      Fiber::TFrame *mem_merge_frame = Fiber::TFrame::LocalFramePool->Alloc();
      try {
        mem_merge_frame->Latch(&mem_merge_runner, static_cast<Orly::Indy::L0::TManager *>(manager.get()), static_cast<Fiber::TRunnable::TFunc>(&Orly::Indy::L0::TManager::RunMergeMem));
      } catch (...) {
        Fiber::TFrame::LocalFramePool->Free(mem_merge_frame);
        throw;
      }
      /* Merge multiple disk files of a specific size category, in the same safe repo. */
      Fiber::TRunner disk_merge_runner(runner_cons);
      scheduler.Schedule(std::bind(Fiber::LaunchSlowFiberSched, &disk_merge_runner, frame_pool_manager));
      Fiber::TFrame *disk_merge_frame = Fiber::TFrame::LocalFramePool->Alloc();
      try {
        disk_merge_frame->Latch(&disk_merge_runner, static_cast<Orly::Indy::L0::TManager *>(manager.get()), static_cast<Fiber::TRunnable::TFunc>(&Orly::Indy::L0::TManager::RunMergeDisk));
      } catch (...) {
        Fiber::TFrame::LocalFramePool->Free(disk_merge_frame);
        throw;
      }

      TUuid global_repo_id(TUuid::Best);
      TUuid repo_id(TUuid::Best);

      auto global_repo = manager->NewSafeRepo(global_repo_id, TTtl(0UL));
      auto repo = manager->NewFastRepo(repo_id, TTtl(0UL), global_repo);

      Base::TUuid session_id(Base::TUuid::Twister);
      auto session = durable_manager->New<Orly::Server::TSession>(session_id, TTtl(0UL));

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
    delete TLocalReadFileCache::Cache;
    delete Disk::TLocalWalkerCache::Cache;
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
                                 },
                                 6);
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
      Orly::Server::TMetaRecord meta_record(
          update_id,
          Orly::Server::TMetaRecord::TEntry(
              session_id, session_id, fq_name, method_name,
              Orly::Server::TMetaRecord::TEntry::TArgByName{},
              Orly::Server::TMetaRecord::TEntry::TExpectedPredicateResults{},
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
      Orly::Server::TMetaRecord meta_record(
          update_id,
          Orly::Server::TMetaRecord::TEntry(
              session_id, session_id, fq_name, method_name,
              Orly::Server::TMetaRecord::TEntry::TArgByName{},
              Orly::Server::TMetaRecord::TEntry::TExpectedPredicateResults{},
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
      Orly::Server::TMetaRecord meta_record(
          update_id,
          Orly::Server::TMetaRecord::TEntry(
              session_id, session_id, fq_name, method_name,
              Orly::Server::TMetaRecord::TEntry::TArgByName{},
              Orly::Server::TMetaRecord::TEntry::TExpectedPredicateResults{},
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
      Orly::Server::TMetaRecord meta_record(
          update_id,
          Orly::Server::TMetaRecord::TEntry(
              session_id, session_id, fq_name, method_name,
              Orly::Server::TMetaRecord::TEntry::TArgByName{},
              Orly::Server::TMetaRecord::TEntry::TExpectedPredicateResults{},
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
      Orly::Server::TMetaRecord meta_record(
          update_id,
          Orly::Server::TMetaRecord::TEntry(
              session_id, session_id, fq_name, method_name,
              Orly::Server::TMetaRecord::TEntry::TArgByName{},
              Orly::Server::TMetaRecord::TEntry::TExpectedPredicateResults{},
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
      Orly::Server::TMetaRecord meta_record(
          update_id,
          Orly::Server::TMetaRecord::TEntry(
              session_id, session_id, fq_name, method_name,
              Orly::Server::TMetaRecord::TEntry::TArgByName{},
              Orly::Server::TMetaRecord::TEntry::TExpectedPredicateResults{},
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
