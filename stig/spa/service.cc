/* <stig/spa/service.cc>

   Implements <stig/spa/service.h>.

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

#include <stig/spa/service.h>

#include <chrono>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <base/assert_true.h>
#include <base/dir_iter.h>
#include <base/os_error.h>
#include <stig/any_honcho.h>
#include <stig/package/rt.h>
#include <stig/spa/flux_capacitor/api.h>
#include <stig/spa/checkpoint.h>
#include <stig/type/stigify.h>
#include <stig/var/stigify.h>

using namespace Base;
using namespace std;
using namespace std::placeholders;
using namespace Stig;
using namespace Stig::Spa;
using namespace Stig::Spa::FluxCapacitor;
using namespace Tools::Nycr;

const char *TService::GetNotifierStateStr(TNotifierState state) {
  switch(state) {
    case Failed: return "failed";
    case Succeeded: return "succeeded";
    case Indeterminate: return "indeterminate";
  };
  throw Base::TImpossibleError(HERE);
}

TService::TService() : PackageManager(Jhm::TAbsBase("packages/")), GlobalPov("", true) {
  Base::AssertTrue(TAnyHoncho::GetAnyHoncho())->GetTetrisHandler().AddPov(&GlobalPov);
  const TScheduler::TPolicy scheduler_policy(4, 20, chrono::milliseconds(10));
  Scheduler.SetPolicy(scheduler_policy);
}

TService::~TService() {
  TAnyHoncho *any_honcho = Base::AssertTrue(TAnyHoncho::GetAnyHoncho());
  any_honcho->GetTetrisHandler().Stop();
  any_honcho->GetPrivatePovObjStore().Drain();
  any_honcho->GetSharedPovObjStore().Drain();
  any_honcho->GetSessionObjStore().Drain();
  any_honcho->GetTetrisHandler().RemovePov(&GlobalPov);
  assert(this);
}

void TService::CreateSession(const Base::TOpt<TUUID> &acct, int ttl, TUUID &out) {
  assert(this);
  out = TSessionObj::TSessionHandle::New(acct, ttl)->GetUUID();
}

void TService::CreatePrivatePov(const TUUID &session_uuid, const TOpt<TUUID> &parent, int ttl, bool paused, TUUID &out) {
  assert(this);
  assert(&session_uuid);
  assert(&parent);
  assert(&out);

  if(parent) {
    out = TPrivatePovObj::TPrivatePovHandle::New(session_uuid, *parent, ttl, bind(&TService::OnPovFail, this, _1), paused)->GetUUID();
  } else {
    out = TPrivatePovObj::TPrivatePovHandle::New(session_uuid, ttl, &GlobalPov, bind(&TService::OnPovFail, this, _1), paused)->GetUUID();
  }
}

void TService::CreateSharedPov(const TOpt<TUUID> &parent, int ttl, bool paused, TUUID &out) {
  assert(this);
  assert(&parent);
  assert(&out);

  if(parent) {
    out = TSharedPovObj::TSharedPovHandle::New(*parent, ttl, bind(&TService::OnPovFail, this, _1), paused)->GetUUID();
  } else {
    out = TSharedPovObj::TSharedPovHandle::New(ttl, &GlobalPov, bind(&TService::OnPovFail, this, _1), paused)->GetUUID();
  }
}

void TService::SaveCheckpoint(const string &filename) {
  assert(this);
  assert(&filename);

  TSync::TExclusiveLock lock(GlobalPov.GetSync());

  //Write out a checkpoint file
  Atom::TSuprena suprena;
  TContext ctx(&GlobalPov, &suprena);
  ofstream out(filename);

  PackageManager.YieldInstalled([&out](const Package::TVersionedName &package) {
    out << "imports <" << package.Name << "> #" << package.Version << ";" << endl;
    return true;
  });

  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  for(TContext::TKeyCursor it(&ctx, &suprena, Var::TVar(), Base::TUuid(Base::TUuid::Twister) /* this id won't get used because we're leaving TVar default constructed */); it; ++it) {
    //TKV kv = *it;
    Var::TVar var = Var::ToVar(*Sabot::State::TAny::TWrapper(it->GetState(state_alloc)));
    Var::Stigify(out, var);
    out << " <- ";
    Var::Stigify(out, Var::ToVar(*Sabot::State::TAny::TWrapper(ctx[Indy::TIndexKey(SpaIndexId, *it)].GetState(state_alloc))));
    out << ';' << endl;
  }
}

/* TODO: Push Jhm::TAbsBase out to the api. */
void TService::SetPackageDir(const string &dir) {
  assert(this);
  assert(&dir);

  PackageManager.SetPackageDir(Jhm::TAbsBase(dir));
}


void TService::Finalize() {
  assert(this);
  //TODO: A global service lock would be nice...
  Base::AssertTrue(TAnyHoncho::GetAnyHoncho())->GetTetrisHandler().Drain();
}

Package::TManager &TService::GetPackageManager()  {
  assert(this);

  return PackageManager;
}
const Package::TManager &TService::GetPackageManager() const {
  assert(this);

  return PackageManager;
}

/* TODO */
void TService::Poll(
      const TUUID &session_uuid,
      const unordered_set<TUUID> &notifiers,
      TOpt<chrono::milliseconds> timeout,
      unordered_map<TUUID, TNotifierState> &out) {
  assert(this);
  assert(&notifiers);
  assert(&out);
  assert(&session_uuid);

  auto session = TSessionObj::TSessionHandle::Rendezvous(session_uuid);

  session->GetSessionObj()->Poll(notifiers, timeout, out);

}

bool CheckAsserts(Package::TFuncHolder::TPtr func, const TService::TStigArg args, const Base::TOpt<TUUID> acct,
      const TUUID session_id,  vector<bool> original_predicte_results, TContext &flux_ctx, Base::TScheduler *scheduler,
      const Rt::TOpt<Base::Chrono::TTimePnt> now, const Rt::TOpt<uint32_t> random_seed) {

  Atom::TSuprena arena;
  Package::TSpaContext ctx(acct, session_id, flux_ctx, &arena, scheduler, now, random_seed);
  try {
    func->Call(ctx, args);
  } catch (const std::exception &ex) {
    //This indicates an assert failed.
    return false;
  }
  return original_predicte_results == ctx.GetPredicateResults();
}

void TService::Try(
      const Package::TFuncHolder::TPtr &func,
      const TUUID &private_pov,
      const unordered_set<TUUID> &notify_povs,
      const TStigArg &args,
      Atom::TCore &result_core,
      Atom::TSuprena &result_arena,
      unordered_map<TUUID, TUUID> &notifiers) {

  assert(this);
  assert(&func);
  assert(func);
  assert(&private_pov);
  assert(&notify_povs);
  assert(&result_core);
  assert(&result_arena);
  assert(&notifiers);
  assert(notifiers.empty()); // Currently if this is not true, the cleanup notifiers logic will die in bad ways.

  //Make the call
  TPrivatePovObj::TPrivatePovHandle::TPtr pov_obj = TPrivatePovObj::TPrivatePovHandle::Rendezvous(private_pov);
  TSessionObj::TSessionHandle::TPtr session = pov_obj->GetSession();

  Package::TContext::TEffects effects;
  Package::TContext::TPredicateResults predicate_results;
  Rt::TOpt<Base::Chrono::TTimePnt> now;
  Rt::TOpt<uint32_t> random_seed;

  /* flux context */ {
    //Note: It is important the flux context go away before we make the TUpdate as otherwise the update will neve
    // promote as the context has a read lock on the private pov. This is also true of any key cursors created on the
    // context (as they hold a pointer to the flux ctx).
    TContext flux_context(pov_obj->GetPrivatePov(), &result_arena);
    Package::TSpaContext ctx(session->GetAcct(), session->GetUUID(), flux_context, &Arena, &Scheduler,
        *Rt::TOpt<Base::Chrono::TTimePnt>::Unknown, *Rt::TOpt<uint32_t>::Unknown);
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    result_core = Atom::TCore(&result_arena, state_alloc, ctx.GetArena(), func->Call(ctx, args));
    effects = ctx.MoveEffects();
    predicate_results = ctx.GetPredicateResults();
    now = ctx.GetOptNow();
    random_seed = ctx.GetOptRandomSeed();
  }

  //If we have no effects, we have no update or notifiers, so we're done!
  if (effects.empty()) {
    return;
  }

  TUpdate::TOnPromote on_promote = [](const TPov *) {};

  /* If we have to send out notifiers, set them up and build an OnPromote function. */
  if (!notify_povs.empty()) {
    TUUID update_id;
    session->GetSessionObj()->MakeNotifiers(pov_obj->GetObj(), notify_povs, update_id, notifiers);
    on_promote = bind(&FluxCapacitor::TSessionObj::OnPromote, session->GetSessionObj(), update_id, _1);
  }

  try {
    std::unordered_map<TKV, Var::TPtr<Var::TChange>> op_by_kv;
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    for (const auto &iter : effects) {
      op_by_kv.insert(make_pair(TKV(Var::ToVar(*Sabot::State::TAny::TWrapper(iter.first.GetKey().GetState(state_alloc))), iter.first.GetIndexId()), iter.second));
    }
    new TUpdate(pov_obj->GetPrivatePov(), unordered_set<TUpdate *>{}, move(op_by_kv), bind(CheckAsserts, func, args, session->GetAcct(),
        session->GetUUID(), predicate_results, _1, &Scheduler, now, random_seed), 10, move(on_promote));
  } catch (...) {
    if(!notifiers.empty()) {
      // TODO: CleanupNotifiers could throw a new exception...
      session->GetSessionObj()->CleanupNotifiers(notifiers);
      notifiers.clear();
    }
    throw;
  }
}

void PromiseWhenReached(const TPov *target, promise<void> &promise, const TPov *reached_pov) {
  if(target == reached_pov) {
    try {
      promise.set_value();
    } catch (const exception &ex) {
      //NOTE: This really shouldn't happen as the promise __should__ always be good.
      cerr << HERE << "std::promise::set_value threw. Additional details: " << ex.what() << endl;
    }
  }
}

void NewUpdateAndWait(const TPov *target, TPrivatePov *start, std::unordered_map<TKV, Var::TPtr<Var::TChange>> &&effects) {
    //NOTE: This is a minified update because we know if it makes it into the pov it will either never be promoted
    // or be the only thing that can be promoted, and will be promoted once and only once. We also know there is no
    // possibility of a conflicting update, so the assertions __must__ pass (and hence are ommitted).

    promise<void> promoted;
    auto has_promoted = promoted.get_future();
    new TUpdate(start, unordered_set<TUpdate *>{}, move(effects), nullptr, 10, bind(PromiseWhenReached, target, ref(promoted), _1));

    //TODO: Timeout should be user configurable.
    if(has_promoted.wait_for(chrono::seconds(2)) != future_status::ready) {
      throw Rt::TSystemError(HERE, "Timeout waiting for update to be promoted. Promotion took longer than 2 seconds, even though it was completely uncontested. That is odd.");
    } else {
      assert(has_promoted.valid());
    }
}

void TService::LoadCheckpoint(const string &name) {
  assert(this);
  TUUID session_id;
  TUUID ppov_id;

  //NOTE: We would just use the flux private pov API directly here, but that creates a pretty much always-hit race between the update promotion and the KVIndex being detached.
  CreateSession(*TOpt<TUUID>::Unknown, 10000, session_id);
  CreatePrivatePov(session_id, *TOpt<TUUID>::Unknown, 1000, false, ppov_id);
  TPrivatePovObj::TPrivatePovHandle::TPtr pov_obj = TPrivatePovObj::TPrivatePovHandle::Rendezvous(ppov_id);

  //Read the checkpoint
  TCheckpointStmts stmts;
  TCheckpointPackages packages;
  Atom::TSuprena arena;
  ReadCheckpoint(name.c_str(), stmts, packages, &arena);

  //TODO: the database changes and package install should be one atomic operation.
  NewUpdateAndWait(&GlobalPov, pov_obj->GetPrivatePov(), std::move(stmts));
  PackageManager.Install(packages);
}

void TService::OnPovFail(FluxCapacitor::TPov *pov) {
  assert(pov);

  FluxCapacitor::TSessionObj::ForEachSession([pov](TSessionObj *session){
    assert(session);

    session->OnPovFail(pov);

    return true;
  });
}

void RunTestAndPromoteOnceIfEffects(TService &service, const function<void(Package::TSpaContext &ctx)> &func,
      const TUUID &session, const TUUID &spov) {
  TUUID pov;
  service.CreatePrivatePov(session, spov, 10000, false, pov);

  TPrivatePovObj::TPrivatePovHandle::TPtr pov_obj = TPrivatePovObj::TPrivatePovHandle::Rendezvous(pov);
  TSharedPovObj::TSharedPovHandle::TPtr spov_obj = TSharedPovObj::TSharedPovHandle::Rendezvous(spov);

  Atom::TSuprena arena;
  Package::TContext::TEffects effects;
  /* ctx destruction*/ {
    //NOTE: The flux context must be destroyed __BEFORE__ we make the update because it holds a lock on the shared pov
    //      which tetris needs
    TContext flux_context(pov_obj->GetPrivatePov(), &arena);
    Package::TSpaContext ctx(*Rt::TOpt<TUUID>::Unknown, session, flux_context, service.GetArena(), service.GetScheduler(),
      Base::Chrono::CreateTimePnt(2013, 10, 23, 17, 47, 14, 0, 0), 0);

    func(ctx);
    effects = ctx.MoveEffects();
  }

  //If we have no effects, we have no update, so we're done.
  if (!effects.empty()) {
    std::unordered_map<TKV, Var::TPtr<Var::TChange>> op_by_kv;
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    for (const auto &iter : effects) {
      op_by_kv.insert(make_pair(TKV(Var::ToVar(*Sabot::State::TAny::TWrapper(iter.first.GetKey().GetState(state_alloc))), iter.first.GetIndexId()), iter.second));
    }
    NewUpdateAndWait(spov_obj->GetSharedPov(), pov_obj->GetPrivatePov(), move(op_by_kv));
  }
}

bool TService::RunTest(const TUUID &session, const TUUID &spov_outer, const Package::TTestCase &test, bool verbose) {
  assert(&session);
  assert(&spov_outer);
  assert(&test);

  TUUID spov;
  CreateSharedPov(spov_outer, 1000, true, spov);

  if (verbose) {
    //TODO: Dedup printing logic.
    cout << test.Loc;
    if(test.Name.size() > 0) {
      cout << ' ' << test.Name;
    }
    cout << " executing...";
  }

  bool result = false;

  try {
    RunTestAndPromoteOnceIfEffects(*this,
        [&test, &result](Package::TSpaContext &ctx) {
          assert(test.Func);
          assert(test.Func->Runner);

          Atom::TCore::TExtensibleArena *arena = ctx.GetArena();
          Atom::TCore ret = test.Func->Runner(ctx, Package::TArgMap());
          void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
          ToNative(*Sabot::State::TAny::TWrapper(ret.NewState(arena, state_alloc)), result);
        },
        session, spov);

    if (result) {
      if (verbose) {
        cout << " PASSED" << endl;
      }
      return RunTestBlock(session, spov, test.SubCases, verbose);
    } else {
      if(!verbose) {
        //TODO: Dedup printing logic.
        cout << test.Loc;
        if(test.Name.size()) {
          cout << ' ' << test.Name;
        }
      }
      cout << " FAILED";
    }
  } catch (const exception &ex) {
    result  = false;
    if(!verbose) {
      //TODO: Dedup printing logic.
      cout << test.Loc;
      if(test.Name.size()) {
        cout << ' ' << test.Name;
      }
    }
    cout << " FAILED: " << ex.what();
  }

  if (!result) {
    cout << " (child tests will not be executed)" << endl;
  }
  return result;
}


bool TService::RunTestBlock(const TUUID &session, const TUUID &spov, const Package::TTestBlock &test_block, bool verbose) {
  assert(&session);
  assert(&test_block);

  bool result = true;

  for(auto &test: test_block) {
    assert(test);
    result &= RunTest(session, spov, *test, verbose);
  }
  return result;
}

bool TService::RunTestSuite(const Package::TName &name, bool verbose) {
  assert(&name);
  assert(this);

  /* TODO: We really want an API for destroying sessions explicitly */
  TUUID session;
  CreateSession(*Base::TOpt<TUUID>::Unknown, 1000, session);
  bool succeeded = true;

  PackageManager.Get(name)->ForEachTest(
      [this, session, &succeeded, verbose](const Package::TTest *test) -> bool {
        assert(test);

        TUUID spov;
        CreateSharedPov(*TOpt<TUUID>::Unknown, 1000, true, spov);

        if (test->WithBlock) {
          RunTestAndPromoteOnceIfEffects(*this,
              [test](Package::TSpaContext &ctx) {
                assert(&ctx);
                assert(test->WithBlock->Runner);
                test->WithBlock->Runner(ctx, Package::TArgMap());
              },
              session, spov);
        }
        succeeded &= RunTestBlock(session, spov, test->SubCases, verbose);
        return true;
      });
  return succeeded;
}