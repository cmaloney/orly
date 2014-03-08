/* <stig/indy/transaction_base.test.cc>

   Unit test for <stig/indy/transaction_base.h>.

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

#include <stig/indy/transaction_base.h>

#include <base/scheduler.h>
#include <stig/indy/disk/sim/mem_engine.h>
#include <stig/indy/fiber/fiber_test_runner.h>
#include <stig/indy/repo.h>

#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Stig;
using namespace Stig::Atom;
using namespace Stig::Indy;

Stig::Indy::Util::TPool L0::TManager::TRepo::TMapping::Pool(sizeof(TRepo::TMapping), "Repo Mapping", 100UL);
Stig::Indy::Util::TPool L0::TManager::TRepo::TMapping::TEntry::Pool(sizeof(TRepo::TMapping::TEntry), "Repo Mapping Entry", 100UL);
Stig::Indy::Util::TPool L0::TManager::TRepo::TDataLayer::Pool(sizeof(TMemoryLayer), "Data Layer", 100UL);

Stig::Indy::Util::TPool L1::TTransaction::TMutation::Pool(max(max(sizeof(L1::TTransaction::TPusher), sizeof(L1::TTransaction::TPopper)), sizeof(L1::TTransaction::TStatusChanger)), "Transaction::TMutation", 100UL);
Stig::Indy::Util::TPool L1::TTransaction::Pool(sizeof(L1::TTransaction), "Transaction", 100UL);

Disk::TBufBlock::TPool Disk::TBufBlock::Pool(Disk::Util::PhysicalBlockSize);

Stig::Indy::Util::TPool TUpdate::Pool(sizeof(TUpdate), "Update", 100UL);
Stig::Indy::Util::TPool TUpdate::TEntry::Pool(sizeof(TUpdate::TEntry), "Entry", 200UL);

const std::vector<size_t> MemMergeCoreVec{0};
const std::vector<size_t> DiskMergeCoreVec{0};

class TMyManager
    : public L1::TManager {
  NO_COPY_SEMANTICS(TMyManager);
  public:

  TMyManager(Disk::Util::TEngine *engine,
             Base::TScheduler *scheduler,
             const std::vector<size_t> &mem_merge_cores,
             const std::vector<size_t> &disk_merge_cores)
      : TManager(engine,
                 10UL,
                 100UL,
                 true,
                 true,
                 1000UL,
                 scheduler,
                 100UL,
                 100UL,
                 20UL,
                 mem_merge_cores,
                 disk_merge_cores,
                 true) {}

  virtual ~TMyManager() {}

  virtual TRepo *ConstructRepo(const Base::TUuid &repo_id,
                                     const Base::TOpt<TTtl> &ttl,
                                     const Base::TOpt<TManager::TPtr<TRepo>> &parent_repo,
                                     bool is_safe,
                                     bool /*create*/) override {
    return is_safe ?
      static_cast<TRepo *>(new TSafeRepo(this, repo_id, *ttl, parent_repo))
    : static_cast<TRepo *>(new TFastRepo(this, repo_id, *ttl, parent_repo));
  }

  virtual void SaveRepo(Stig::Indy::L0::TManager::TRepo *) override {}

  virtual void Enqueue(Stig::Indy::TTransactionReplication *, Stig::Indy::L1::TTransaction::TReplica &&) NO_THROW override {}

  virtual Stig::Indy::TTransactionReplication* NewTransactionReplication() override {
    return nullptr;
  }

  virtual void DeleteTransactionReplication(Stig::Indy::TTransactionReplication*) NO_THROW override {}

  virtual void ForEachScheduler(const std::function<bool (Fiber::TRunner *)> &/*cb*/) const override {}

  virtual bool CanLoad(const L0::TId &/*id*/) override {
    return true;
  }

  virtual void Delete(const L0::TId &/*id*/, L0::TSem */*sem*/) override {}

  virtual void Save(const L0::TId &/*id*/, const L0::TDeadline &/*deadline*/, const std::string &/*blob*/, L0::TSem */*sem*/) override {}

  virtual bool TryLoad(const L0::TId &/*id*/, std::string &/*blob*/) override {
    return true;
  }

  virtual TRepo *ReconstructRepo(const Base::TUuid &/*repo_id*/) override {
    return nullptr;
  }

  virtual void RunReplicationQueue() override {}

  virtual void RunReplicationWork() override {}

  virtual void RunReplicateTransaction() override {}

  virtual std::mutex &GetReplicationQueueLock() NO_THROW override {
    return ReplicationQueueLock;
  }

  inline TManager::TPtr<Indy::TRepo> GetRepo(const Base::TUuid &repo_id,
                                                 const Base::TOpt<TTtl> &ttl,
                                                 const Base::TOpt<TManager::TPtr<L0::TManager::TRepo>> &parent_repo,
                                                 bool is_safe,
                                                 bool create) {
    assert(this);
    return create ? OpenOrCreate(repo_id, ttl, parent_repo, is_safe) : ForceOpenRepo(repo_id);
  }

  using TManager::OpenOrCreate;

  private:

  std::mutex ReplicationQueueLock;

};

FIXTURE(Typical) {
  Fiber::TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    TSuprena arena;
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    const TScheduler::TPolicy scheduler_policy(10, 10, milliseconds(10));
    TScheduler scheduler;
    scheduler.SetPolicy(scheduler_policy);
    Stig::Indy::Disk::Sim::TMemEngine mem_engine(&scheduler,
                                                 256 /* fast disk space: 256MB */,
                                                 64 /* slow disk space: 64MB */,
                                                 128 /* page cache slots: 8MB */,
                                                 1 /* num page lru */,
                                                 64 /* block cache slots: 4MB */,
                                                 1 /* num block lru */);
    unique_ptr<TMyManager> manager(new TMyManager(mem_engine.GetEngine(), &scheduler, MemMergeCoreVec, DiskMergeCoreVec));
    Base::TUuid repo_1_id(TUuid::Twister);
    Base::TUuid idx_id(TUuid::Twister);
    auto repo_1 = manager->GetRepo(repo_1_id, TTtl::max(), *TOpt<Indy::L0::TManager::TPtr<Indy::L0::TManager::TRepo>>::Unknown, false, true);
    EXPECT_EQ(repo_1->GetStatus(), Normal);
    /* don't commit Push */ {
      auto transaction = manager->NewTransaction();
      EXPECT_TRUE(transaction);
      auto update = TUpdate::NewUpdate(TUpdate::TOpByKey{ { TIndexKey(idx_id, TKey(make_tuple(1L), &arena, state_alloc)), TKey(10L, &arena, state_alloc)} }, TKey(&arena), TKey(Base::TUuid(TUuid::Best), &arena, state_alloc));
      transaction->Push(repo_1, update);
    }
    EXPECT_EQ(repo_1->GetStatus(), Normal);
    /* check that nothing is there */ {
      unique_ptr<TRepo::TView> view(new TRepo::TView(repo_1));
      auto walker_ptr = repo_1->NewPresentWalker(view, TIndexKey(idx_id, TKey(make_tuple(1L), &arena, state_alloc)), TIndexKey(idx_id, TKey(make_tuple(10L), &arena, state_alloc)));
      auto &walker = *walker_ptr;
      EXPECT_FALSE(static_cast<bool>(walker));
    }
    EXPECT_EQ(repo_1->GetStatus(), Normal);
    /* commit Push */ {
      auto transaction = manager->NewTransaction();
      EXPECT_TRUE(transaction);
      auto update = TUpdate::NewUpdate(TUpdate::TOpByKey{ { TIndexKey(idx_id, TKey(make_tuple(1L), &arena, state_alloc)), TKey(10L, &arena, state_alloc)} }, TKey(&arena), TKey(Base::TUuid(TUuid::Best), &arena, state_alloc));
      transaction->Push(repo_1, update);
      transaction->Prepare();
      transaction->CommitAction();
    }
    EXPECT_EQ(repo_1->GetStatus(), Normal);
    /* check that our update is there */ {
      unique_ptr<TRepo::TView> view(new TRepo::TView(repo_1));
      auto walker_ptr = repo_1->NewPresentWalker(view, TIndexKey(idx_id, TKey(make_tuple(1L), &arena, state_alloc)), TIndexKey(idx_id, TKey(make_tuple(10L), &arena, state_alloc)));
      auto &walker = *walker_ptr;
      if (EXPECT_TRUE(static_cast<bool>(walker))) {
        EXPECT_EQ((*walker).SequenceNumber, 1UL);
        ++walker;
      }
      EXPECT_FALSE(static_cast<bool>(walker));
    }
    EXPECT_EQ(repo_1->GetStatus(), Normal);
    /* don't commit Pop */ {
      auto transaction = manager->NewTransaction();
      EXPECT_TRUE(transaction);
      transaction->Pop(repo_1);
    }
    EXPECT_EQ(repo_1->GetStatus(), Normal);
    /* check that our update is there */ {
      unique_ptr<TRepo::TView> view(new TRepo::TView(repo_1));
      auto walker_ptr = repo_1->NewPresentWalker(view, TIndexKey(idx_id, TKey(make_tuple(1L), &arena, state_alloc)), TIndexKey(idx_id, TKey(make_tuple(10L), &arena, state_alloc)));
      auto &walker = *walker_ptr;
      if (EXPECT_TRUE(static_cast<bool>(walker))) {
        EXPECT_EQ((*walker).SequenceNumber, 1UL);
        ++walker;
      }
      EXPECT_FALSE(static_cast<bool>(walker));
    }
    EXPECT_EQ(repo_1->GetStatus(), Normal);
    /* commit Pop */ {
      auto transaction = manager->NewTransaction();
      EXPECT_TRUE(transaction);
      transaction->Pop(repo_1);
      transaction->Prepare();
      transaction->CommitAction();
    }
    EXPECT_EQ(repo_1->GetStatus(), Normal);
    /* check that nothing is there */ {
      unique_ptr<TRepo::TView> view(new TRepo::TView(repo_1));
      auto walker_ptr = repo_1->NewPresentWalker(view, TIndexKey(idx_id, TKey(make_tuple(1L), &arena, state_alloc)), TIndexKey(idx_id, TKey(make_tuple(10L), &arena, state_alloc)));
      auto &walker = *walker_ptr;
      EXPECT_FALSE(static_cast<bool>(walker));
    }
    EXPECT_EQ(repo_1->GetStatus(), Normal);
    /* don't commit Pause */ {
      auto transaction = manager->NewTransaction();
      EXPECT_TRUE(transaction);
      transaction->Pause(repo_1);
    }
    EXPECT_EQ(repo_1->GetStatus(), Normal);
    /* commit Pause */ {
      auto transaction = manager->NewTransaction();
      EXPECT_TRUE(transaction);
      transaction->Pause(repo_1);
      transaction->Prepare();
      transaction->CommitAction();
    }
    EXPECT_EQ(repo_1->GetStatus(), Paused);
    /* don't commit UnPause */ {
      auto transaction = manager->NewTransaction();
      EXPECT_TRUE(transaction);
      transaction->UnPause(repo_1);
    }
    EXPECT_EQ(repo_1->GetStatus(), Paused);
    /* commit UnPause */ {
      auto transaction = manager->NewTransaction();
      EXPECT_TRUE(transaction);
      transaction->UnPause(repo_1);
      transaction->Prepare();
      transaction->CommitAction();
    }
    EXPECT_EQ(repo_1->GetStatus(), Normal);
    /* don't commit Fail */ {
      auto transaction = manager->NewTransaction();
      EXPECT_TRUE(transaction);
      transaction->Fail(repo_1);
    }
    EXPECT_EQ(repo_1->GetStatus(), Normal);
    /* commit Fail */ {
      auto transaction = manager->NewTransaction();
      EXPECT_TRUE(transaction);
      transaction->Fail(repo_1);
      transaction->Prepare();
      transaction->CommitAction();
    }
    EXPECT_EQ(repo_1->GetStatus(), Failed);
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  });
}

FIXTURE(Promoter) {
  Fiber::TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    TSuprena arena;
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    const TScheduler::TPolicy scheduler_policy(10, 10, milliseconds(10));
    TScheduler scheduler;
    scheduler.SetPolicy(scheduler_policy);
    Stig::Indy::Disk::Sim::TMemEngine mem_engine(&scheduler,
                                                 256 /* fast disk space: 256MB */,
                                                 64 /* slow disk space: 64MB */,
                                                 128 /* page cache slots: 8MB */,
                                                 1 /* num page lru */,
                                                 64 /* block cache slots: 4MB */,
                                                 1 /* num block lru */);
    unique_ptr<TMyManager> manager(new TMyManager(mem_engine.GetEngine(), &scheduler, MemMergeCoreVec, DiskMergeCoreVec));
    Base::TUuid repo_1_id(TUuid::Twister);
    Base::TUuid repo_2_id(TUuid::Twister);
    Base::TUuid idx_id(TUuid::Twister);
    auto repo_1 = manager->GetRepo(repo_1_id, TTtl::max(), *TOpt<Indy::L0::TManager::TPtr<Indy::L0::TManager::TRepo>>::Unknown, false, true);
    auto repo_2 = manager->GetRepo(repo_2_id, TTtl::max(), *TOpt<Indy::L0::TManager::TPtr<Indy::L0::TManager::TRepo>>::Unknown, false, true);
    /* Push to 1*/ {
      auto transaction = manager->NewTransaction();
      EXPECT_TRUE(transaction);
      auto update = TUpdate::NewUpdate(TUpdate::TOpByKey{ { TIndexKey(idx_id, TKey(make_tuple(1L), &arena, state_alloc)), TKey(10L, &arena, state_alloc)} }, TKey(&arena), TKey(Base::TUuid(TUuid::Best), &arena, state_alloc));
      transaction->Push(repo_1, update);
      transaction->Prepare();
      transaction->CommitAction();
    }
    /* check that repo 1 has the update */ {
      unique_ptr<TRepo::TView> view(new TRepo::TView(repo_1));
      auto walker_ptr = repo_1->NewPresentWalker(view, TIndexKey(idx_id, TKey(make_tuple(1L), &arena, state_alloc)), TIndexKey(idx_id, TKey(make_tuple(10L), &arena, state_alloc)));
      auto &walker = *walker_ptr;
      if (EXPECT_TRUE(static_cast<bool>(walker))) {
        EXPECT_EQ((*walker).SequenceNumber, 1UL);
        ++walker;
      }
      EXPECT_FALSE(static_cast<bool>(walker));
    }
    /* pop from 1, push to 2 */ {
      auto transaction = manager->NewTransaction();
      EXPECT_TRUE(transaction);
      transaction->Pop(repo_1);
      transaction->Push(repo_2, transaction->Peek(repo_1));
      transaction->Prepare();
      transaction->CommitAction();
    }
    /* check that repo 2 has the update */ {
      unique_ptr<TRepo::TView> view(new TRepo::TView(repo_2));
      auto walker_ptr = repo_2->NewPresentWalker(view, TIndexKey(idx_id, TKey(make_tuple(1L), &arena, state_alloc)), TIndexKey(idx_id, TKey(make_tuple(10L), &arena, state_alloc)));
      auto &walker = *walker_ptr;
      if (EXPECT_TRUE(static_cast<bool>(walker))) {
        EXPECT_EQ((*walker).SequenceNumber, 1UL);
        ++walker;
      }
      EXPECT_FALSE(static_cast<bool>(walker));
    }
    /* check that nothing is in repo 1*/ {
      unique_ptr<TRepo::TView> view(new TRepo::TView(repo_1));
      auto walker_ptr = repo_1->NewPresentWalker(view, TIndexKey(idx_id, TKey(make_tuple(1L), &arena, state_alloc)), TIndexKey(idx_id, TKey(make_tuple(10L), &arena, state_alloc)));
      auto &walker = *walker_ptr;
      EXPECT_FALSE(static_cast<bool>(walker));
    }
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  });
}

FIXTURE(DiskPromoter) {
  Fiber::TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Fiber::TRunner::TRunnerCons &) {
    TSuprena arena;
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    const TScheduler::TPolicy scheduler_policy(4, 4, milliseconds(10));
    TScheduler scheduler;
    scheduler.SetPolicy(scheduler_policy);


    Stig::Indy::Disk::Sim::TMemEngine mem_engine(&scheduler,
                                                 256 /* fast disk space: 256MB */,
                                                 64 /* slow disk space: 64MB */,
                                                 128 /* page cache slots: 8MB */,
                                                 1 /* num page lru */,
                                                 64 /* block cache slots: 4MB */,
                                                 1 /* num block lru */);
    unique_ptr<TMyManager> manager(new TMyManager(mem_engine.GetEngine(), &scheduler, MemMergeCoreVec, DiskMergeCoreVec));
    Base::TUuid repo_1_id(TUuid::Twister);
    Base::TUuid repo_2_id(TUuid::Twister);
    Base::TUuid idx_id(TUuid::Twister);
    auto repo_1 = manager->GetRepo(repo_1_id, TTtl::max(), *TOpt<Indy::L0::TManager::TPtr<Indy::L0::TManager::TRepo>>::Unknown, true, true);
    auto repo_2 = manager->GetRepo(repo_2_id, TTtl::max(), *TOpt<Indy::L0::TManager::TPtr<Indy::L0::TManager::TRepo>>::Unknown, true, true);
    /* Push to 1*/ {
      auto transaction = manager->NewTransaction();
      EXPECT_TRUE(transaction);
      auto update = TUpdate::NewUpdate(TUpdate::TOpByKey{ { TIndexKey(idx_id, TKey(make_tuple(1L), &arena, state_alloc)), TKey(10L, &arena, state_alloc)} }, TKey(&arena), TKey(Base::TUuid(TUuid::Best), &arena, state_alloc));
      transaction->Push(repo_1, update);
      transaction->Prepare();
      transaction->CommitAction();
    }
    /* check that repo 1 has the update */ {
      unique_ptr<TRepo::TView> view(new TRepo::TView(repo_1));
      auto walker_ptr = repo_1->NewPresentWalker(view, TIndexKey(idx_id, TKey(make_tuple(1L), &arena, state_alloc)), TIndexKey(idx_id, TKey(make_tuple(10L), &arena, state_alloc)));
      auto &walker = *walker_ptr;
      if (EXPECT_TRUE(static_cast<bool>(walker))) {
        EXPECT_EQ((*walker).SequenceNumber, 1UL);
        ++walker;
      }
      EXPECT_FALSE(static_cast<bool>(walker));
    }
    /* pop from 1, push to 2 */ {
      auto transaction = manager->NewTransaction();
      EXPECT_TRUE(transaction);
      transaction->Pop(repo_1);
      transaction->Push(repo_2, transaction->Peek(repo_1));
      transaction->Prepare();
      transaction->CommitAction();
    }
    /* check that repo 2 has the update */ {
      unique_ptr<TRepo::TView> view(new TRepo::TView(repo_2));
      auto walker_ptr = repo_2->NewPresentWalker(view, TIndexKey(idx_id, TKey(make_tuple(1L), &arena, state_alloc)), TIndexKey(idx_id, TKey(make_tuple(10L), &arena, state_alloc)));
      auto &walker = *walker_ptr;
      if (EXPECT_TRUE(static_cast<bool>(walker))) {
        EXPECT_EQ((*walker).SequenceNumber, 1UL);
        ++walker;
      }
      EXPECT_FALSE(static_cast<bool>(walker));
    }
    /* check that nothing is in repo 1*/ {
      unique_ptr<TRepo::TView> view(new TRepo::TView(repo_1));
      auto walker_ptr = repo_1->NewPresentWalker(view, TIndexKey(idx_id, TKey(make_tuple(1L), &arena, state_alloc)), TIndexKey(idx_id, TKey(make_tuple(10L), &arena, state_alloc)));
      auto &walker = *walker_ptr;
      EXPECT_FALSE(static_cast<bool>(walker));
    }
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  });
}