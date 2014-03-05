/* <stig/server/session.test.cc>

   Unit test for <stig/server/session.h>.

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

#include <stig/server/session.h>

#include <base/scheduler.h>
#include <stig/durable/test_manager.h>
#include <stig/notification/all.h>
#include <stig/notification/system_shutdown.h>
#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace chrono;
using namespace Stig::Durable;
using namespace Stig::Notification;
using namespace Stig::Server;
using namespace Stig::Indy;
using namespace Stig::Indy::Util;

const Stig::Indy::TMasterContext::TProtocol Stig::Indy::TMasterContext::TProtocol::Protocol;
const Stig::Indy::TSlaveContext::TProtocol Stig::Indy::TSlaveContext::TProtocol::Protocol;

static const size_t BlockSize = 4096UL * 16;

TPool TUpdate::Pool(sizeof(TUpdate), "Update", 1000000UL);
TPool TUpdate::TEntry::Pool(sizeof(TUpdate::TEntry), "Entry", 2000000UL);
Disk::TBufBlock::TPool Disk::TBufBlock::Pool(BlockSize, 20000);

Stig::Indy::Util::TLocklessPool Disk::TDurableManager::TMapping::Pool(sizeof(Disk::TDurableManager::TMapping), "Durable Mapping", 100);
Stig::Indy::Util::TLocklessPool Disk::TDurableManager::TMapping::TEntry::Pool(sizeof(Disk::TDurableManager::TMapping::TEntry), "Durable Mapping Entry", 1000);
Stig::Indy::Util::TPool Disk::TDurableManager::TDurableLayer::Pool(std::max(sizeof(Disk::TDurableManager::TMemSlushLayer), sizeof(Disk::TDurableManager::TDiskOrderedLayer)), "Durable Layer", 1000);
Stig::Indy::Util::TPool Disk::TDurableManager::TMemSlushLayer::TDurableEntry::Pool(sizeof(Disk::TDurableManager::TMemSlushLayer::TDurableEntry), "Durable Entry", 1000);

Stig::Indy::Util::TPool TRepo::TMapping::Pool(sizeof(TRepo::TMapping), "Repo Mapping");
Stig::Indy::Util::TPool TRepo::TMapping::TEntry::Pool(sizeof(TRepo::TMapping::TEntry), "Repo Mapping Entry");
Stig::Indy::Util::TPool TRepo::TDataLayer::Pool(max(sizeof(TMemoryLayer), sizeof(TDiskLayer)), "Data Layer");

Stig::Indy::Util::TPool L1::TTransaction::TMutation::Pool(max(max(sizeof(L1::TTransaction::TPusher), sizeof(L1::TTransaction::TPopper)), sizeof(L1::TTransaction::TStatusChanger)), "Transaction::TMutation");
Stig::Indy::Util::TPool L1::TTransaction::Pool(sizeof(L1::TTransaction), "Transaction");

Base::TSigmaCalc TSession::TServer::TryReadTimeCalc;
Base::TSigmaCalc TSession::TServer::TryReadCPUTimeCalc;
Base::TSigmaCalc TSession::TServer::TryWriteTimeCalc;
Base::TSigmaCalc TSession::TServer::TryWriteCPUTimeCalc;
Base::TSigmaCalc TSession::TServer::TryWalkerCountCalc;
Base::TSigmaCalc TSession::TServer::TryCallCPUTimerCalc;
Base::TSigmaCalc TSession::TServer::TryReadCallTimerCalc;
Base::TSigmaCalc TSession::TServer::TryWriteCallTimerCalc;
Base::TSigmaCalc TSession::TServer::TryWalkerConsTimerCalc;
Base::TSigmaCalc TSession::TServer::TryFetchCountCalc;
Base::TSigmaCalc TSession::TServer::TryHashHitCountCalc;
Base::TSigmaCalc TSession::TServer::TryWriteSyncHitCalc;
Base::TSigmaCalc TSession::TServer::TryWriteSyncTimeCalc;
Base::TSigmaCalc TSession::TServer::TryReadSyncHitCalc;
Base::TSigmaCalc TSession::TServer::TryReadSyncTimeCalc;
std::mutex       TSession::TServer::TryTimeLock;

static const size_t WarningCount = 3;

static const seconds WarningDurations[WarningCount] = { seconds(600), seconds(300), seconds(60) };

static void ValidateSession(const TPtr<TSession> &session) {
  uint32_t i = 0;
  session->ForEachNotification(
      [&i](uint32_t seq_number, const TNotification *actual_notification) {
        assert(actual_notification);
        if (EXPECT_TRUE(i < WarningCount)) {
          unique_ptr<TSystemShutdown> expected_notification(TSystemShutdown::New(WarningDurations[i]));
          EXPECT_EQ(seq_number, i + 1);
          EXPECT_TRUE(Matches(*actual_notification, *expected_notification));
          ++i;
        }
        return true;
      }
  );
}

FIXTURE(Caching) {
  auto manager = make_shared<TTestManager>(1000);
  auto session = manager->New<TSession>(TUuid::Twister, seconds(60));
  for (size_t i = 0; i < WarningCount; ++i) {
    uint32_t seq_number = session->InsertNotification(TSystemShutdown::New(WarningDurations[i]));
    EXPECT_EQ(seq_number, i + 1);
  }
  ValidateSession(session);
  auto id = session->GetId();
  session.Reset();
  session = manager->Open<TSession>(id);
  ValidateSession(session);
}

FIXTURE(Streaming) {
  auto manager = make_shared<TTestManager>(0);
  auto session = manager->New<TSession>(TUuid::Twister, seconds(60));
  for (size_t i = 0; i < WarningCount; ++i) {
    uint32_t seq_number = session->InsertNotification(TSystemShutdown::New(WarningDurations[i]));
    EXPECT_EQ(seq_number, i + 1);
  }
  ValidateSession(session);
  auto id = session->GetId();
  session.Reset();
  session = manager->Open<TSession>(id);
  ValidateSession(session);
}

#if 0
class TTestServer final
    : public TSession::TServer {
  public:

  TTestServer(size_t max_cache_size)
      : DurableManager(make_shared<TTestDurableManager>(max_cache_size)) {}

  shared_ptr<TDurable::TManager> GetDurableManager() const override {
    return DurableManager;
  }

  private:

  shared_ptr<TDurable::TManager> DurableManager;

};

FIXTURE(SetTimeToLive) {
  TTestServer server(1000);
  auto session = server.GetDurableManager()->New<TSession>(seconds(60));
  EXPECT_EQ(session->GetTimeToLive().count(), 60);
  session->SetTimeToLive(&server, session->GetId(), seconds(3600));
  EXPECT_EQ(session->GetTimeToLive().count(), 3600);
}

FIXTURE(NewPov) {
  TTestServer server(1000);
  auto session = server.GetDurableManager()->New<TSession>(seconds(60));
  auto pov_id = session->NewFastPrivatePov(&server, *TOpt<TUuid>::Unknown, seconds(60));
  auto pov = server.GetDurableManager()->Open<TPov>(pov_id, seconds(60));
  EXPECT_TRUE(pov);
}
#endif

#if 0
#include <stig/indy/durable_manager.h>

using namespace Stig::Indy;
using namespace Stig::Indy::Util;

const Stig::Indy::TMasterContext::TProtocol Stig::Indy::TMasterContext::TProtocol::Protocol;
const Stig::Indy::TSlaveContext::TProtocol Stig::Indy::TSlaveContext::TProtocol::Protocol;
static const string MasterDeviceName = "sdb";
static const size_t BlockSize = 4096UL * 16;
static const size_t CacheSize = 10000UL;
static const size_t QueueDepth = 128UL;
static const size_t MergeMemDelay = 10UL;
static const size_t MergeDiskDelay = 10UL;
static const size_t ReplicationDelay = 100UL;
static const size_t DefaultRepoCacheSize = 1000UL;

TPool TUpdate::Pool(sizeof(TUpdate), 1000000UL, "Update");
TPool TUpdate::TEntry::Pool(sizeof(TUpdate::TEntry), 2000000UL, "Entry");
TPool Disk::TController::TEvent::Pool(max(sizeof(Disk::TController::TWriteV), sizeof(Disk::TController::TRead)), 10000, "Controller Event");
Disk::TBufBlock::TPool Disk::TBufBlock::Pool(BlockSize, 20000);
TPool Disk::TCache::TObj::Pool(sizeof(Disk::TCache::TObj), 11000, "Cache Block");
timespec Disk::TCache::ReadWait;

void ScheduleDiskController(TScheduler *scheduler, Disk::TController *controller) {
  assert(scheduler);
  assert(controller);
  scheduler->Schedule(bind(&Disk::TController::RunQueue, controller));
  scheduler->Schedule(bind(&Disk::TController::RunWork, controller));
}

FIXTURE(RepoManager) {
  Base::TFd solo_sock;
  auto wait_for_slave = [](const std::shared_ptr<std::function<void (const TFd &)>> &) {};
  const TScheduler::TPolicy scheduler_policy(2, 2, milliseconds(10));
  TScheduler scheduler;
  scheduler.SetPolicy(scheduler_policy);
  unique_ptr<TManager> repo_manager(new TManager(MasterDeviceName,
                                           BlockSize,
                                           CacheSize,
                                           QueueDepth,
                                           MergeMemDelay,
                                           MergeDiskDelay,
                                           ReplicationDelay,
                                           TManager::Solo,
                                           solo_sock,
                                           wait_for_slave,
                                           [](TManager::TState) {},
                                           std::bind(ScheduleDiskController, &scheduler, std::placeholders::_1),
                                           DefaultRepoCacheSize,
                                           true));
  auto durable_manager= make_shared<TDurableManager>(0, repo_manager.get(), repo_manager->GetSystemRepo());
  auto session = durable_manager->New<TSession>(seconds(60));
  for (size_t i = 0; i < WarningCount; ++i) {
    uint32_t seq_number = session->InsertNotification(TSystemShutdown::New(WarningDurations[i]));
    EXPECT_EQ(seq_number, i + 1);
  }
  ValidateSession(session);
  auto id = session->GetId();
  session.reset();
  session = durable_manager->Open<TSession>(id, seconds(60));
  ValidateSession(session);
}
#endif