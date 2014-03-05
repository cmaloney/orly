/* <stig/server/repo_tetris_manager.test.broken.cc>

   Unit test for <stig/server/repo_tetris_manager.h>

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

#include <stig/server/repo_tetris_manager.h>

#include <base/scheduler.h>
#include <stig/type/type_czar.h>
#include <stig/indy/disk/sim/mem_engine.h>
#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace placeholders;
using namespace Base;
using namespace Stig;
using namespace Stig::Atom;
using namespace Stig::Indy;
using namespace Stig::Server;

const Stig::Indy::TMasterContext::TProtocol Stig::Indy::TMasterContext::TProtocol::Protocol;
const Stig::Indy::TSlaveContext::TProtocol Stig::Indy::TSlaveContext::TProtocol::Protocol;

static Type::TTypeCzar TypeCzar;

static const size_t BlockSize = 4096UL * 16;
static const size_t CacheSize = 10000UL;
static const size_t MergeMemDelay = 10UL;
static const size_t MergeDiskDelay = 10UL;
static const size_t LayerCleaningInterval = 100UL;
static const size_t ReplicationDelay = 100UL;
static const size_t DefaultRepoCacheSize = 1000UL;

Stig::Indy::Util::TLocklessPool Disk::TDurableManager::TMapping::Pool(sizeof(Disk::TDurableManager::TMapping), "Durable Mapping", 100);
Stig::Indy::Util::TLocklessPool Disk::TDurableManager::TMapping::TEntry::Pool(sizeof(Disk::TDurableManager::TMapping::TEntry), "Durable Mapping Entry", 1000);
Stig::Indy::Util::TPool Disk::TDurableManager::TDurableLayer::Pool(std::max(sizeof(Disk::TDurableManager::TMemSlushLayer), sizeof(Disk::TDurableManager::TDiskOrderedLayer)), "Durable Layer", 1000);
Stig::Indy::Util::TPool Disk::TDurableManager::TMemSlushLayer::TDurableEntry::Pool(sizeof(Disk::TDurableManager::TMemSlushLayer::TDurableEntry), "Durable Entry", 1000);

Stig::Indy::Util::TPool TRepo::TMapping::Pool(sizeof(TRepo::TMapping), "Repo Mapping", 10000);
Stig::Indy::Util::TPool TRepo::TMapping::TEntry::Pool(sizeof(TRepo::TMapping::TEntry), "Repo Mapping Entry", 10000);
Stig::Indy::Util::TPool TRepo::TDataLayer::Pool(max(sizeof(TMemoryLayer), sizeof(TDiskLayer)), "Data Layer", 10000);

Stig::Indy::Util::TPool L1::TTransaction::TMutation::Pool(max(max(sizeof(L1::TTransaction::TPusher), sizeof(L1::TTransaction::TPopper)), sizeof(L1::TTransaction::TStatusChanger)), "Transaction::TMutation", 10000);
Stig::Indy::Util::TPool L1::TTransaction::Pool(sizeof(L1::TTransaction), "Transaction", 10000);

Stig::Indy::Util::TPool TUpdate::Pool(sizeof(TUpdate), "Update", 10000);
Stig::Indy::Util::TPool TUpdate::TEntry::Pool(sizeof(TUpdate::TEntry), "Entry", 10000);
Disk::TBufBlock::TPool Disk::TBufBlock::Pool(BlockSize, 20000);

Base::TSigmaCalc TSession::TServer::TryReadTimeCalc;
Base::TSigmaCalc TSession::TServer::TryReadCPUTimeCalc;
Base::TSigmaCalc TSession::TServer::TryWriteTimeCalc;
Base::TSigmaCalc TSession::TServer::TryWriteCPUTimeCalc;
Base::TSigmaCalc TSession::TServer::TryWalkerCountCalc;
Base::TSigmaCalc TSession::TServer::TryWalkerTimeCalc;
Base::TSigmaCalc TSession::TServer::TryCallCPUTimerCalc;
Base::TSigmaCalc TSession::TServer::TryFetchCountCalc;
Base::TSigmaCalc TSession::TServer::TryHashHitCountCalc;
Base::TSigmaCalc TSession::TServer::TryWriteSyncHitCalc;
Base::TSigmaCalc TSession::TServer::TryWriteSyncTimeCalc;
Base::TSigmaCalc TSession::TServer::TryReadSyncHitCalc;
Base::TSigmaCalc TSession::TServer::TryReadSyncTimeCalc;
std::mutex       TSession::TServer::TryTimeLock;

static void StateChanged(TManager::TState) {}

FIXTURE(Typical) {
  /* Start the system. */
  TScheduler scheduler(TScheduler::TPolicy(1, 100, milliseconds(30000)));
  TFd solo_sock;
  auto wait_for_slave = [](const shared_ptr<function<void (const TFd &)>> &) {};

  Disk::Sim::TMemEngine mem_engine(&scheduler,
                                   256 /* disk space: 256MB */,
                                   32,
                                   16384 /* page cache slots: 64MB */,
                                   1 /* num page lru */,
                                   1024 /* block cache slots: 64MB */,
                                   1 /* num block lru */);

  auto update_replication_notification_cb = [](const Base::TUuid &, const Base::TUuid &, const Base::TUuid &) {};
  TManager repo_manager(
      mem_engine.GetEngine(), 64/* replication sync buf MB */, MergeMemDelay, MergeDiskDelay, LayerCleaningInterval, ReplicationDelay,
      TManager::Solo, false /*allow tailing */, true /* allow file sync */, true /* no real time */, std::move(solo_sock), wait_for_slave, StateChanged,
      update_replication_notification_cb,
      &scheduler,
      DefaultRepoCacheSize, 128 /*walker local cache size */ , true /* create */
  );
  /* Make some keys to play with. */
  TSuprena arena;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Base::TUuid index_id(Base::TUuid::Twister);
  const TKey
      k_empty,
      k_101   (make_tuple(101L),   &arena, state_alloc),
      k_102   (make_tuple(102L),   &arena, state_alloc),
      v_hello (string("hello"),    &arena, state_alloc),
      k_id    (TUuid(TUuid::Best), &arena, state_alloc);
  TUuid repo_id(TUuid::Best);
  auto repo = repo_manager.NewFastRepo(repo_id, Stig::Durable::TTtl(100));
  /* extra */ {
    auto transaction = repo_manager.NewTransaction();
    auto update = TUpdate::NewUpdate(
        TUpdate::TOpByKey({ { TIndexKey(index_id, k_101), v_hello } }),
        k_empty, k_id
    );
    transaction->Push(repo, update);
    transaction->Prepare();
    transaction->CommitAction();
  }
  Atom::TSuprena ctx_arena;
  TContext context(repo, &ctx_arena);
  /* extra */ {
    #if 0
    Spa::FluxCapacitor::TKV search_key = Spa::FluxCapacitor::TKV(
        Var::TVar(
            Rt::TAddr<Rt::TAddrElem<TAddrDir::Asc, int64_t>>(101L)
        ),
        index_id
    );
    #endif
    TIndexKey search_key(index_id, k_101);
    EXPECT_TRUE(context.Exists(search_key));
  }
  /* extra */ {
    #if 0
    Spa::FluxCapacitor::TKV search_key = Spa::FluxCapacitor::TKV(
        Var::TVar(
            Rt::TAddr<Rt::TAddrElem<TAddrDir::Asc, int64_t>>(102L)
        ),
        index_id
    );
    #endif
    TIndexKey search_key(index_id, k_102);
    EXPECT_FALSE(context.Exists(search_key));
  }
}
