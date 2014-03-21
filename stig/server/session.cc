/* <stig/server/session.cc>

   Implements <stig/server/session.h>.

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

#include <stig/atom/suprena.h>
#include <stig/indy/context.h>
#include <stig/notification/all.h>
#include <stig/server/meta_record.h>
#include <stig/spa/stig_args.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Stig;
using namespace Stig::Atom;
using namespace Stig::Notification;
using namespace Stig::Server;

TMethodResult TSession::DoInPast(
    TServer */*server*/, const TUuid &/*pov_id*/, const vector<string> &/*fq_name*/, const TClosure &/*closure*/, const TUuid &/*tracking_id*/) {
  assert(this);
  THROW_ERROR(TStubbed) << "DoInPast";
}

bool TSession::ForEachNotification(const function<bool (uint32_t, const TNotification *)> &cb) const {
  assert(this);
  assert(&cb);
  lock_guard<mutex> lock(NotificationMutex);
  for (const auto &item: NotificationBySeqNumber) {
    if (!cb(item.first, item.second)) {
      return false;
    }
  }
  return true;
}

const TNotification *TSession::GetFirstNotification(uint32_t &seq_number) {
  assert(this);
  assert(&seq_number);
  lock_guard<mutex> lock(NotificationMutex);
  assert(!NotificationBySeqNumber.empty());
  auto iter = NotificationBySeqNumber.begin();
  seq_number = iter->first;
  return iter->second;
}

TUuid TSession::NewFastPrivatePov(TServer *server, const TOpt<TUuid> &parent_pov_id, const seconds &time_to_live) {
  assert(this);
  assert(server);
  return NewPov(server, parent_pov_id, TPov::TAudience::Private, TPov::TPolicy::Fast, time_to_live);
}

TUuid TSession::NewFastSharedPov(TServer *server, const TOpt<TUuid> &parent_pov_id, const seconds &time_to_live) {
  assert(this);
  return NewPov(server, parent_pov_id, TPov::TAudience::Shared, TPov::TPolicy::Fast, time_to_live);
}

TUuid TSession::NewSafePrivatePov(TServer *server, const TOpt<TUuid> &parent_pov_id, const seconds &time_to_live) {
  assert(this);
  return NewPov(server, parent_pov_id, TPov::TAudience::Private, TPov::TPolicy::Safe, time_to_live);
}

TUuid TSession::NewSafeSharedPov(TServer *server, const TOpt<TUuid> &parent_pov_id, const seconds &time_to_live) {
  assert(this);
  return NewPov(server, parent_pov_id, TPov::TAudience::Shared, TPov::TPolicy::Safe, time_to_live);
}

void TSession::PausePov(TServer *server, const TUuid &pov_id) {
  assert(this);
  assert(server);
  auto pov = server->GetDurableManager()->Open<TPov>(pov_id);
  auto repo = pov->GetRepo(server);
  std::unique_ptr<Indy::L1::TTransaction, std::function<void (Indy::L1::TTransaction *)>> transaction = server->GetRepoManager()->NewTransaction();
  transaction->Pause(repo);
  transaction->Prepare();
  transaction->CommitAction();
  AddPov(pov);
}

uint32_t TSession::InsertNotification(TNotification *notification) {
  assert(this);
  lock_guard<mutex> lock(NotificationMutex);
  uint32_t result = NextSeqNumber++;
  try {
    NotificationBySeqNumber.insert(make_pair(result, notification));
    NotificationSem.Push();
  } catch (...) {
    --NextSeqNumber;
    delete notification;
    throw;
  }
  return result;
}

void TSession::RemoveNotification(uint32_t seq_number) {
  assert(this);
  lock_guard<mutex> lock(NotificationMutex);
  auto iter = NotificationBySeqNumber.find(seq_number);
  assert(iter != NotificationBySeqNumber.end());
  delete iter->second;
  NotificationBySeqNumber.erase(iter);
  NotificationSem.Pop();
}

void TSession::SetTimeToLive(TServer *server, const TUuid &durable_id, const seconds &time_to_live) {
  assert(this);
  assert(server);
  throw std::runtime_error("TSession::SetTimeToLive is currently not enabled.");
  server->GetDurableManager()->Open<TObj>(durable_id)->SetTtl(time_to_live);
}

void TSession::SetUserId(TServer */*server*/, const TUuid &user_id) {
  assert(this);
  assert(&user_id);
  if (UserId) {
    DEFINE_ERROR(error_t, runtime_error, "user_id already set");
    THROW_ERROR(error_t) << "existing uid = " << user_id;
  }
  UserId = user_id;
}

TMethodResult TSession::Try(TServer *server, const TUuid &pov_id, const vector<string> &fq_name, const TClosure &closure) {
  assert(this);
  assert(Indy::Fiber::TRunner::LocalRunner);
  size_t prev_assignment_count = std::atomic_fetch_add(&server->FastAssignmentCounter, 1UL);
  Indy::Fiber::TSwitchToRunner RunnerSwitcher(server->FastRunnerVec[prev_assignment_count % server->FastRunnerVec.size()].get());
  TCore result_core;
  Base::TTimer timer;
  Base::TTimer call_timer;
  timer.Start();
  bool had_effects = false;
  TOpt<TTracker> tracker = TOpt<TTracker>();
  size_t walker_count = 0UL;
  TSuprena my_arena;
  try {
    // Convert the args to vars.
    Spa::TArgs::TStigArg prog_args;
    void *state_alloc_1 = alloca(Sabot::State::GetMaxStateSize() * 2);
    void *state_alloc_2 = reinterpret_cast<uint8_t *>(state_alloc_1) + Sabot::State::GetMaxStateSize();
    auto arena = closure.GetArena().get();
    for (const auto &item: closure.GetCoreByName()) {
      prog_args.insert(make_pair(item.first, Indy::TKey(item.second, arena)));
    }
    // Open the pov and its repo and prepare the data and package contexts.
    auto pov = server->GetDurableManager()->Open<TPov>(pov_id);
    if (!pov) {
      DEFINE_ERROR(error_t, runtime_error, "unknown pov_id");
      THROW_ERROR(error_t) << pov_id;
    }
    AddPov(pov);
    auto repo = pov->GetRepo(server);
    Indy::TContext context(repo, &my_arena);
    Rt::TOpt<TUUID> user_id;
    if (UserId) {
      user_id = UserId->GetRaw();
    }
    TUUID session_id = GetId().GetRaw();
    Indy::TIndyContext indy_context(user_id, session_id, context, &my_arena, server->GetScheduler(),
      Rt::TOpt<Base::Chrono::TTimePnt>(), Rt::TOpt<uint32_t>());
    // Func it.
    auto func = server->GetPackageManager().Get(fq_name)->GetFunctionInfo(AsPiece(closure.GetMethodName()));
    Package::TContext::TEffects effects;
    call_timer.Start();
    result_core = func->Call(indy_context, prog_args);
    call_timer.Stop();
    effects = indy_context.MoveEffects();
    if (!effects.empty()) {
      had_effects = true;
      auto transaction = server->GetRepoManager()->NewTransaction();
      Indy::TUpdate::TOpByKey op_by_key;
      for (const auto &item: effects) {
        auto key = item.first;
        Var::TVar val;
        if (!item.second->IsDelete()) {
          if (!item.second->IsFinal()) {
            val = Var::ToVar(*Sabot::State::TAny::TWrapper(context[key].GetState(state_alloc_1)));
          }
          item.second->Apply(val);
          op_by_key[key] =
              Indy::TKey(&my_arena, Sabot::State::TAny::TWrapper(Var::NewSabot(state_alloc_2, val)).get());
        }
        else {
          op_by_key[key] =
              Indy::TKey(Native::TTombstone::Tombstone, &my_arena, state_alloc_2);
        }
      }
      TUuid update_id(TUuid::Twister);
      tracker = TTracker(update_id, seconds(0));
      const auto &predicate_results = indy_context.GetPredicateResults();
      TMetaRecord::TEntry::TArgByName meta_args_by_name;
      auto closure_arena = closure.GetArena().get();
      for (const auto &item: closure.GetCoreByName()) {
        auto arg = Var::ToVar(*Sabot::State::TAny::TWrapper(item.second.NewState(closure_arena, state_alloc_1)));
        meta_args_by_name.insert(make_pair(item.first, arg));
      }

      //NOTE: Could do these inline, but this is less fugly to write out because they are so long.
      uint32_t random_seed = 0;
      if(indy_context.GetOptRandomSeed().IsKnown()) {
        random_seed = indy_context.GetOptRandomSeed().GetVal();
      }
      Base::Chrono::TTimePnt run_time = Base::Chrono::CreateTimePnt(2013, 10, 23, 17, 47, 14, 0, 0);
      if(indy_context.GetOptNow().IsKnown()) {
        run_time = indy_context.GetOptNow().GetVal();
      }

      TMetaRecord meta_record(
          update_id,
          TMetaRecord::TEntry(
              GetId(), GetUserId(), fq_name, closure.GetMethodName(),
              TMetaRecord::TEntry::TArgByName(meta_args_by_name.begin(), meta_args_by_name.end()),
              TMetaRecord::TEntry::TExpectedPredicateResults(predicate_results.begin(), predicate_results.end()),
              run_time, random_seed)
      );
      auto update = Indy::TUpdate::NewUpdate(op_by_key, Indy::TKey(meta_record, &my_arena, state_alloc_1), Indy::TKey(update_id, &my_arena, state_alloc_2));
      transaction->Push(repo, update);
      transaction->Prepare();
      transaction->CommitAction();
    }
    walker_count = context.GetWalkerCount();
    timer.Stop();
    /* Acquire TryTime lock */ {
      std::lock_guard<std::mutex> lock(TServer::TryTimeLock);
      if (had_effects) {
        TServer::TryWriteTimeCalc.Push(timer.Total());
        TServer::TryWriteCallTimerCalc.Push(call_timer.Total());
      } else {
        TServer::TryReadTimeCalc.Push(timer.Total());
        TServer::TryReadCallTimerCalc.Push(call_timer.Total());
      }
      TServer::TryWalkerCountCalc.Push(walker_count);
      TServer::TryWalkerConsTimerCalc.Push(context.GetPresentWalkConsTimer().Total());
    }
    return TMethodResult(indy_context.GetArena(), result_core, tracker);
  } catch (const exception &ex) {
    syslog(LOG_ERR, "Error in Session::Try : [%s]", ex.what());
    throw;
  }
}

bool TSession::RunTestSuite(TServer * /*server*/,
    const std::vector<std::string> & /*package_name*/,
    uint64_t /*package_version*/, bool /*verbose*/) {
#if 0
  assert(this);
  assert(server);
  assert(&package_name);

  server->InstallPackage(package_name, package_version);
  bool succeeded = true;
  server->GetPackageManager().Get(package_name)->ForEachTest(
      [this, server, &package_name, verbose](const Package::TTest *test) {
        assert(test);
        Base::TUuid spov = NewFastSharedPov(server, *Base::TOpt<Base::TUuid>::Unknown, std::chrono::seconds(1000));
        PausePov(server, spov);
        if (test->WithBlock) {
          RunInPrivateChildPov(server, package_name, test->withBlock, spov);
        }
        succeeded = RunTestBlock(spov, test->SubCases, verbose) && succeeded;
        return true;
      });
  return succeeded;
#endif

  // TODO: finish implementation

  return true;
}

TNotification *TSession::TryGetNotification(uint32_t seq_number) const {
  assert(this);
  lock_guard<mutex> lock(NotificationMutex);
  auto iter = NotificationBySeqNumber.find(seq_number);
  return (iter != NotificationBySeqNumber.end()) ? iter->second : nullptr;
}

TMethodResult TSession::TryTracked(TServer */*server*/, const TUuid &/*pov_id*/, const vector<string> &/*fq_name*/, const TClosure &/*closure*/) {
  assert(this);
  THROW_ERROR(TStubbed) << "TryTracked";
}

void TSession::UnpausePov(TServer *server, const TUuid &pov_id) {
  assert(this);
  assert(server);
  auto pov = server->GetDurableManager()->Open<TPov>(pov_id);
  auto repo = pov->GetRepo(server);
  auto transaction = server->GetRepoManager()->NewTransaction();
  transaction->UnPause(repo);
  transaction->Prepare();
  transaction->CommitAction();
  AddPov(pov);
}

const TUuid TSession::GlobalPovId = Stig::Indy::GlobalPovId;

TSession::TSession(Durable::TManager *manager, const Base::TUuid &id, const Durable::TTtl &ttl)
    : TObj(manager, id, ttl), NextSeqNumber(1) {}

TSession::TSession(Durable::TManager *manager, const Base::TUuid &id, Io::TBinaryInputStream &strm)
    : TObj(manager, id, strm) {
  assert(&strm);
  try {
    size_t size;
    strm >> UserId >> NextSeqNumber >> size;
    for (size_t i = 0; i < size; ++i) {
      pair<uint32_t, TNotification *> item;
      strm >> item.first;
      if (item.first >= NextSeqNumber) {
        syslog(LOG_ERR, "SyntaxError item.first >= NextSeqNumber [%d >= %d]", item.first, NextSeqNumber);
        throw Io::TInputConsumer::TSyntaxError();
      }
      try {
        item.second = Notification::New(strm);
      } catch (...) {
        syslog(LOG_ERR, "Notification::New() error");
      }
      try {
        if (!NotificationBySeqNumber.insert(item).second) {
          syslog(LOG_ERR, "SyntaxError !NotificationBySeqNumber.insert(item).second");
          throw Io::TInputConsumer::TSyntaxError();
        }
      } catch (...) {
        delete item.second;
        throw;
      }
    }
    NotificationSem.Push(size);
  } catch (...) {
    Cleanup();
    throw;
  }
}

TSession::~TSession() {
  assert(this);
  Cleanup();
}

void TSession::RunInPrivateChildPov(TServer *server,
    const std::vector<std::string> & /*package_name*/,
    const function<void(Package::TContext &ctx)> &func,
    const Base::TUuid &parent_pov_id) {
  assert(server);
  assert(func);
  assert(&parent_pov_id);

  Base::TUuid child_pov_id = NewFastPrivatePov(server, parent_pov_id, std::chrono::seconds(1000));
  Durable::TPtr<TPov> child_pov = server->GetDurableManager()->Open<TPov>(child_pov_id);
  if (!child_pov) {
    DEFINE_ERROR(error_t, runtime_error, "unknown child_pov_id");
    THROW_ERROR(error_t) << child_pov_id;
  }
  AddPov(child_pov);
  const Indy::L0::TManager::TPtr<Indy::TRepo> &repo = child_pov->GetRepo(server);
  TSuprena child_arena;
  Indy::TContext context(repo, &child_arena);
  Rt::TOpt<TUUID> user_id;
  if (UserId) {
    user_id = UserId->GetRaw();
  }
  TUUID session_id = GetId().GetRaw();
  Indy::TIndyContext indy_context(user_id, session_id, context, &child_arena, server->GetScheduler(),
      Base::Chrono::CreateTimePnt(2013, 10, 23, 17, 47, 14, 0, 0), 0);
  func(indy_context);
  Package::TContext::TEffects effects(indy_context.MoveEffects());

  if (!effects.empty()) {
    auto transaction = server->GetRepoManager()->NewTransaction();
    Indy::TUpdate::TOpByKey op_by_key;
    void *state_alloc_1 = alloca(Sabot::State::GetMaxStateSize() * 2);
    void *state_alloc_2 = reinterpret_cast<uint8_t *>(state_alloc_1) + Sabot::State::GetMaxStateSize();
    for (const auto &item: effects) {
      Indy::TIndexKey key = item.first;
      Var::TVar val;
      if (item.second->IsDelete()) {
        op_by_key[key] =
            Indy::TKey(Native::TTombstone::Tombstone, &child_arena, state_alloc_2);
      } else {
        if (!item.second->IsFinal()) {
          val = Var::ToVar(*Sabot::State::TAny::TWrapper(context[key].GetState(state_alloc_1)));
        }
        item.second->Apply(val);
        op_by_key[key] =
            Indy::TKey(&child_arena, Sabot::State::TAny::TWrapper(Var::NewSabot(state_alloc_2, val)).get());
      }
    }
    TUuid update_id(TUuid::Twister);
    TOpt<TTracker> tracker = TTracker(update_id, std::chrono::seconds(0));
#if 0
    const Package::TContext::TPredicateResults &predicate_results = indy_context.GetPredicateResults();
    TMetaRecord::TEntry::TArgByName meta_args_by_name;



    // TODO: finish implementation



    TMetaRecord meta_record(
        update_id,
        TMetaRecord::TEntry(
            GetId(), GetUserId(), package_name, closure.GetMethodName(),
            TMetaRecord::TEntry::TArgByName(meta_args_by_name.begin(), meta_args_by_name.end()),
            TMetaRecord::TEntry::TExpectedPredicateResults(predicate_results.begin(), predicate_results.end())
        )
    );
    auto update = Indy::TUpdate::NewUpdate(op_by_key, Indy::TKey(meta_record, &my_arena, state_alloc_1), Indy::TKey(update_id, &my_arena, state_alloc_2));
    transaction->Push(repo, update);
    transaction->Prepare();
    transaction->CommitAction();
#endif
  }
}

bool TSession::RunTestBlock(const TUUID &/*parent_pov_id*/,
    const Package::TTestBlock &/*test_block*/, bool /*verbose*/) {
  // TODO: finish implementation
  return true;
}

void TSession::AddPov(const Durable::TPtr<TPov> &pov) {
  assert(this);
  std::lock_guard<std::mutex> lock(PovMutex);
  if (find(Povs.begin(), Povs.end(), pov) == Povs.end()) {
    Povs.push_back(pov);
  }
}

void TSession::Write(Io::TBinaryOutputStream &strm) const {
  assert(this);
  assert(&strm);
  lock_guard<mutex> lock(NotificationMutex);
  TObj::Write(strm);
  strm << UserId << NextSeqNumber << NotificationBySeqNumber.size();
  for (const auto &item: NotificationBySeqNumber) {
    strm << item.first;
    Notification::Write(strm, item.second);
  }
}

void TSession::Cleanup() {
  assert(this);
  for (const auto &item: NotificationBySeqNumber) {
    delete item.second;
  }
}

TUuid TSession::NewPov(
    TServer *server, const Base::TOpt<Base::TUuid> &parent_pov_id, TPov::TAudience audience, TPov::TPolicy policy, const seconds &time_to_live) {
  assert(this);
  assert(server);
  assert(&parent_pov_id);
  printf("TSession::NewPov()\n");
  auto durable_manager = server->GetDurableManager();
  TPov::TSharedParents shared_parents;
  if (parent_pov_id) {
    // TODO: add the ability to open a durable w/o changing ttl
    shared_parents = durable_manager->Open<TPov>(*parent_pov_id)->GetSharedParents();
    shared_parents.push_back(*parent_pov_id);
  }
  auto pov = durable_manager->New<TPov>(TUuid::Twister, time_to_live, GetId(), audience, policy, shared_parents);
  pov->GetRepo(server);
  Base::TUuid pov_id = pov->GetId();
  AddPov(std::move(pov));
  printf("TSession::NewPov() FINISH\n");
  return pov_id;
}

bool TSession::ForEachDependentPtr(const function<bool (Durable::TAnyPtr &)> &cb) noexcept {
  assert(this);
  assert(&cb);
  std::lock_guard<std::mutex> lock(PovMutex);
  for (auto &pov: Povs) {
    if (!cb(pov)) {
      return false;
    }
  }
  Povs.clear();
  return true;
}
