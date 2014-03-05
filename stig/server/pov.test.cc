/* <stig/server/pov.test.cc>

   Unit test for <stig/server/pov.h>.

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

#include <stig/server/pov.h>

#include <stig/durable/test_manager.h>
#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Stig::Durable;
using namespace Stig::Server;
using namespace Stig::Indy;
using namespace Stig::Indy::Util;

const Stig::Indy::TMasterContext::TProtocol Stig::Indy::TMasterContext::TProtocol::Protocol;
const Stig::Indy::TSlaveContext::TProtocol Stig::Indy::TSlaveContext::TProtocol::Protocol;

static const size_t BlockSize = 4096UL * 16;

TPool TUpdate::Pool(sizeof(TUpdate), "Update", 1000000UL);
TPool TUpdate::TEntry::Pool(sizeof(TUpdate::TEntry), "Entry", 2000000UL);
Disk::TBufBlock::TPool Disk::TBufBlock::Pool(BlockSize, 20000);

Stig::Indy::Util::TPool L1::TTransaction::TMutation::Pool(max(max(sizeof(L1::TTransaction::TPusher), sizeof(L1::TTransaction::TPopper)), sizeof(L1::TTransaction::TStatusChanger)), "Transaction::TMutation");
Stig::Indy::Util::TPool L1::TTransaction::Pool(sizeof(L1::TTransaction), "Transaction");

Stig::Indy::Util::TLocklessPool Disk::TDurableManager::TMapping::Pool(sizeof(Disk::TDurableManager::TMapping), "Durable Mapping", 100);
Stig::Indy::Util::TLocklessPool Disk::TDurableManager::TMapping::TEntry::Pool(sizeof(Disk::TDurableManager::TMapping::TEntry), "Durable Mapping Entry", 1000);
Stig::Indy::Util::TPool Disk::TDurableManager::TDurableLayer::Pool(std::max(sizeof(Disk::TDurableManager::TMemSlushLayer), sizeof(Disk::TDurableManager::TDiskOrderedLayer)), "Durable Layer", 1000);
Stig::Indy::Util::TPool Disk::TDurableManager::TMemSlushLayer::TDurableEntry::Pool(sizeof(Disk::TDurableManager::TMemSlushLayer::TDurableEntry), "Durable Entry", 1000);

Stig::Indy::Util::TPool TRepo::TMapping::Pool(sizeof(TRepo::TMapping), "Repo Mapping");
Stig::Indy::Util::TPool TRepo::TMapping::TEntry::Pool(sizeof(TRepo::TMapping::TEntry), "Repo Mapping Entry");
Stig::Indy::Util::TPool TRepo::TDataLayer::Pool(max(sizeof(TMemoryLayer), sizeof(TDiskLayer)), "Data Layer");


static const seconds OneMinute(60);

FIXTURE(Typical) {
  const TUuid expected_session_id = TUuid::Best;
  const TPov::TSharedParents expected_parents = { TUuid::Best, TUuid::Best, TUuid::Best };
  auto manager = make_shared<TTestManager>(0);
  auto pov = manager->New<TPov>(TUuid::Twister, OneMinute, expected_session_id, TPov::TAudience::Private, TPov::TPolicy::Fast, expected_parents);
  auto id = pov->GetId();
  pov.Reset();
  pov = manager->Open<TPov>(id);
  EXPECT_TRUE(pov->GetSessionId() == expected_session_id);
  EXPECT_TRUE(pov->GetAudience() == TPov::TAudience::Private);
  EXPECT_TRUE(pov->GetPolicy() == TPov::TPolicy::Fast);
  const auto &actual_parents = pov->GetSharedParents();
  if (EXPECT_EQ(actual_parents.size(), expected_parents.size())) {
    for (size_t i = 0; i < expected_parents.size(); ++i) {
      EXPECT_EQ(actual_parents[i], expected_parents[i]);
    }
  }
}
