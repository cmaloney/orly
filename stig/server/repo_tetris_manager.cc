/* <stig/server/repo_tetris_manager.cc>

   Implements <stig/server/repo_tetris_manager.h>

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

#include <vector>

#include <stig/notification/pov_failure.h>
#include <stig/notification/update_progress.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Stig::Indy;
using namespace Stig::Server;

TRepoTetrisManager::TRepoTetrisManager(
    TScheduler *scheduler,
    Fiber::TRunner::TRunnerCons &runner_cons,
    Base::TThreadLocalPoolManager<Indy::Fiber::TFrame, size_t, Indy::Fiber::TRunner *> *frame_pool_manager,
    const std::function<void (Indy::Fiber::TRunner *)> &runner_setup_cb,
    bool is_master,
    Indy::TManager *repo_manager,
    Package::TManager *package_manager,
    Durable::TManager *durable_manager,
    bool log_assertion_failures)
    : TTetrisManager(scheduler, runner_cons, frame_pool_manager, runner_setup_cb, is_master),
      PushCount(0UL),
      PopCount(0UL),
      FailCount(0UL),
      RoundCount(0UL),
      RepoManager(repo_manager),
      PackageManager(package_manager),
      DurableManager(durable_manager),
      LogAssertionFailures(log_assertion_failures) {
  assert(repo_manager);
  assert(package_manager);
  assert(durable_manager);
}

TRepoTetrisManager::~TRepoTetrisManager() {
  assert(this);
  StopAllPlayers();
}

TRepoTetrisManager::TPlayer::TPlayer(TRepoTetrisManager *repo_tetris_manager, const TUuid &parent_pov_id, const TUuid &child_pov_id, bool is_paused, bool is_master)
    : TTetrisManager::TPlayer(repo_tetris_manager), RepoTetrisManager(repo_tetris_manager) {
  Repo = repo_tetris_manager->RepoManager->ForceGetRepo(parent_pov_id);
  OnJoin(child_pov_id);
  Start(is_paused, is_master);
}

TRepoTetrisManager::TPlayer::~TPlayer() {
  assert(this);
  assert(ChildByPovId.size() == 1UL);
  for (const auto &item: ChildByPovId) {
    delete item.second;
  }
}

TRepoTetrisManager::TPlayer::TChild::TChild(TPlayer *player, const TUuid &child_pov_id)
    : Player(player), Age(0), FailureCount(0) {
  Repo = player->RepoTetrisManager->RepoManager->ForceGetRepo(child_pov_id);
}

bool TRepoTetrisManager::TPlayer::TChild::Play(
    const unique_ptr<Indy::L1::TTransaction, function<void (Indy::L1::TTransaction *)>> &transaction, Indy::TContext &context) {
  assert(this);
  assert(transaction);
  bool success = TestAssertions(context);
  if (success) {
    /* swap the metadata with just the session ids if we're pushing to global */
    if (Player->Repo->GetId() == TSession::GlobalPovId) {
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      if (FuncHolderByUpdateId.size() == 1) {
        /* we're storing just a single session id. */
        PeekedUpdate->SetMetadata(Indy::TKey(MetaRecord.GetEntry(FuncHolderByUpdateId.begin()->first).GetSessionId(), &PeekedUpdate->GetSuprena(), state_alloc));
      } else {
        throw std::runtime_error("We don't currently support collapsed updates.");
      }
    }
    transaction->Push(Player->Repo, PeekedUpdate);
    transaction->Pop(Repo);
    ++(Player->RepoTetrisManager->PushCount);
    ++(Player->RepoTetrisManager->PopCount);
    for (const auto &item: FuncHolderByUpdateId) {
      const auto &entry = MetaRecord.GetEntry(item.first);
      auto session = Player->RepoTetrisManager->DurableManager->Open<TSession>(entry.GetSessionId());
      if (session) {
        session->InsertNotification(Notification::TUpdateProgress::New(Player->Repo->GetId(), item.first, Notification::TUpdateProgress::Accepted));
      }
    }
    Flush();
  } else {
    ++FailureCount;
    if (FailureCount >= 10) {
      transaction->Fail(Repo);
      for (const auto &item: FuncHolderByUpdateId) {
        const auto &entry = MetaRecord.GetEntry(item.first);
        auto session = Player->RepoTetrisManager->DurableManager->Open<TSession>(entry.GetSessionId());
        if (session) {
          session->InsertNotification(Notification::TPovFailure::New(Repo->GetId()));
        }
      }
      ++(Player->RepoTetrisManager->FailCount);
      stringstream ss;
      ss << Repo->GetId();
      syslog(LOG_INFO, "Failing Repo [%s]", ss.str().c_str());
      Flush();
    }
  }
  return success;
}

bool TRepoTetrisManager::TPlayer::TChild::Refresh(const unique_ptr<Indy::L1::TTransaction, function<void (Indy::L1::TTransaction *)>> &transaction) {
  assert(this);
  assert(transaction);
  if (Repo->GetStatus() == Stig::Indy::Normal && Repo->GetSequenceNumberStart() && !PeekedUpdate) {
    PeekedUpdate = transaction->Peek(Repo);
    if (PeekedUpdate) {
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      Sabot::ToNative(*Sabot::State::TAny::TWrapper(PeekedUpdate->GetMetadata().NewState(&PeekedUpdate->GetSuprena(), state_alloc)), MetaRecord);
      for (const auto &item: MetaRecord.GetEntryByUpdateId()) {
        const auto &entry = item.second;
        FuncHolderByUpdateId[item.first] =
            Player->RepoTetrisManager->PackageManager->Get(entry.GetPackageFqName())->GetFunctionInfo(AsPiece(entry.GetMethodName()));
      }
      Age = 0;
      FailureCount = 0;
    }
  }
  ++Age;
  return static_cast<bool>(PeekedUpdate);
}

bool TRepoTetrisManager::TPlayer::TChild::SortsBefore(const TChild *lhs, const TChild *rhs) {
  assert(lhs);
  assert(rhs);
  return lhs->Age > rhs->Age;
}

void TRepoTetrisManager::TPlayer::TChild::Flush() {
  assert(this);
  PeekedUpdate.reset();
  MetaRecord = TMetaRecord();
  FuncHolderByUpdateId.clear();
}

namespace Stig {
  namespace Rt {
    template <typename TVal>
    std::ostream &operator<<(std::ostream &out, const TOpt<TVal> &that) {
      assert(&out);
      assert(&that);

      if(that.IsKnown()) {
        out<<that.GetVal();
      }

      return out;
    }

  }
}

bool TRepoTetrisManager::TPlayer::TChild::TestAssertions(Indy::TContext &context) const {
  assert(this);
  assert(&context);
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  for (const auto &item: FuncHolderByUpdateId) {
    const auto &entry = MetaRecord.GetEntry(item.first);
    const auto &expected_predicate_results = entry.GetExpectedPredicateResults();
    if (expected_predicate_results.size()) {
      Atom::TSuprena my_arena;
      Rt::TOpt<TUUID> user_id;
      if (entry.GetUserId()) {
        user_id = TUUID(entry.GetUserId()->GetRaw());
      }
      TUUID session_id(entry.GetSessionId().GetRaw());
      Indy::TIndyContext indy_context(user_id, session_id, context, &my_arena,
          Player->RepoTetrisManager->GetScheduler(), entry.GetRunTimestamp(), entry.GetRandomSeed());
      const auto &arg_by_name = entry.GetArgByName();
      try {
        unordered_map<string, Indy::TKey> arg_map;
        for (const auto &iter : arg_by_name) {
          Atom::TCore core(&my_arena, Sabot::State::TAny::TWrapper(Var::NewSabot(state_alloc, iter.second)).get());
          arg_map.insert(make_pair(iter.first, Indy::TKey(core, &my_arena)));
        }
        item.second->Call(indy_context, arg_map);
        if (vector<bool>(expected_predicate_results.begin(), expected_predicate_results.end()) != indy_context.GetPredicateResults()) {
          if (Player->RepoTetrisManager->LogAssertionFailures) {
            stringstream ss;
            ss << "package=[";
            for (const auto &elem : entry.GetPackageFqName()) {
              ss << "/" << elem;
            }
            ss << "], method=[" << entry.GetMethodName() << "]"
               << ", user_id=[" << user_id << "]"
               << ", session_id=[" << session_id << "]"
               << ", ARGS {";
            for (const auto &iter : arg_by_name) {
              ss << " " << iter.first << "=[" << iter.second << "]";
            }
            ss << "} KEYS {";
            for (const auto &iter : indy_context.GetEffects()) {
              ss << "[" << iter.first.GetKey() << "] ";
            }
            ss << "}";
            syslog(LOG_INFO, "Assertion Failure %s", ss.str().c_str());
          }
          return false;
        }
      } catch (const exception &ex) {
        stringstream strm;
        strm << Repo->GetId();
        syslog(LOG_INFO, "exception while testing assertions in pov %s; %s", strm.str().c_str(), ex.what());
        return false;
      }
    }
  }
  return true;
}

void TRepoTetrisManager::TPlayer::OnJoin(const TUuid &child_pov_id) {
  assert(this);
  lock_guard<mutex> lock(Mutex);
  auto child = new TChild(this, child_pov_id);
  try {
    ChildByPovId.insert(make_pair(child_pov_id, child));
  } catch (...) {
    delete child;
    throw;
  }
}

void TRepoTetrisManager::TPlayer::OnPart(const TUuid &child_pov_id) {
  assert(this);
  lock_guard<mutex> lock(Mutex);
  auto iter = ChildByPovId.find(child_pov_id);
  if (iter != ChildByPovId.end()) {
    delete iter->second;
    ChildByPovId.erase(iter);
  }
}

void TRepoTetrisManager::TPlayer::OnPause() {
  assert(this);
}

void TRepoTetrisManager::TPlayer::OnUnpause() {
  assert(this);
}

void TRepoTetrisManager::TPlayer::Play() {
  assert(this);
  Base::TCPUTimer snapshot_timer, sort_timer, play_timer, commit_timer;
  Atom::TSuprena my_arena;
  try {
    /* Begin a transaction and make a vector of all our children who are ready to participate in it. */
    unique_ptr<Indy::L1::TTransaction, function<void (Indy::L1::TTransaction *)>> transaction = RepoTetrisManager->RepoManager->NewTransaction();
    vector<TChild *> children;
    snapshot_timer.Start();
    /* extra */ {
      lock_guard<mutex> lock(Mutex);
      children.reserve(ChildByPovId.size());
      for (const auto &item: ChildByPovId) {
        TChild *child = item.second;
        if (child->Refresh(transaction)) {
          children.push_back(child);
        }
      }
    }
    snapshot_timer.Stop();
    /* Sort by decreasing promote-ness. */
    sort_timer.Start();
    sort(children.begin(), children.end(), TChild::SortsBefore);
    sort_timer.Stop();
    /* Give each child a chance to play.  At most one will be permitted to promote (for now), but any number might fail
       due to age. */
    play_timer.Start();
    Indy::TContext context(Repo, &my_arena);
    for (TChild *child: children) {
      if (child->Play(transaction, context)) {
        break;
      }
    }
    play_timer.Stop();
    /* Commit. */
    transaction->Prepare();
    transaction->CommitAction();
    commit_timer.Start();
    ++(RepoTetrisManager->RoundCount);
  } catch (const std::exception &ex) {
    syslog(LOG_EMERG, "Tetris::TPlayer::Play error : %s", ex.what());
    throw;
  }
  commit_timer.Stop();

  std::lock_guard<std::mutex> lock(RepoTetrisManager->TetrisTimerLock);
  RepoTetrisManager->TetrisSnapshotCPUTime.Push(snapshot_timer.Total());
  RepoTetrisManager->TetrisSortCPUTime.Push(sort_timer.Total());
  RepoTetrisManager->TetrisPlayCPUTime.Push(play_timer.Total());
  RepoTetrisManager->TetrisCommitCPUTime.Push(commit_timer.Total());
}

TTetrisManager::TPlayer *TRepoTetrisManager::NewPlayer(const TUuid &parent_pov_id, const TUuid &child_pov_id, bool is_paused, bool is_master) {
  assert(this);
  return new TPlayer(this, parent_pov_id, child_pov_id, is_paused, is_master);
}
