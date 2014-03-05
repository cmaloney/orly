/* <stig/spa/flux_capacitor/flux_capacitor.cc>

   Implements <stig/spa/flux_capacitor/flux_capacitor.h>.

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

#include <stig/spa/flux_capacitor/flux_capacitor.h>

#include <unistd.h>
#include <sys/epoll.h>

#include <base/os_error.h>
#include <base/zero.h>

using namespace std;
using namespace placeholders;
using namespace Base;
using namespace Stig::Spa;
using namespace Stig::Spa::FluxCapacitor;

/* These are used in the ForEachTour() functions in TNodeIndex and TEdgeIndex. */
static const string EmptyString;

/*  ----------------------------------------------------------------------
 *  TPov::TTrailingUpdateCursor.
 */

TPov::TTrailingUpdateCursor &TPov::TTrailingUpdateCursor::operator++() {
  assert(this);
  assert(TrailingUpdate);
  TrailingUpdate = TrailingUpdate->NextTrailingUpdate;
  return *this;
}

/*  ----------------------------------------------------------------------
 *  TPov.
 */

void TPov::DeleteAllTrailingUpdate() {
  assert(this);
  while (FirstTrailingUpdate) {
    delete FirstTrailingUpdate;
  }
}

bool TPov::HasAncestor(const TPov *ancestor) const {
  assert(this);
  const TPov *pov = this;
  while (pov && pov != ancestor) {
    pov = pov->TryGetParentPov();
  }
  return pov != 0;
}

const TPov *TPov::TryGetChildPov(const TPov *target) const {
  assert(this);
  assert(target);
  const TPov *result = 0;
  if (this != target) {
    for (;;) {
      TPov *pov = target->TryGetParentPov();
      if (!pov) {
        break;
      }
      if (pov == this) {
        result = target;
        break;
      }
      target = pov;
    }
  }
  return result;
}


TPov::TPov()
    : Root(0), FirstTrailingUpdate(0), LastTrailingUpdate(0), KVIndex(0) {
  try {
    KVIndex = new TKVIndex(this);
    TOsError::IfLt0(HERE, pipe(PipeHandles));
  } catch (...) {
    delete KVIndex;
    throw;
  }
}

TPov::~TPov() {
  assert(this);
  assert(!FirstTrailingUpdate);
  for (size_t i = 0; i < 2; ++i) {
    close(PipeHandles[i]);
  }
  delete KVIndex;
}

void TPov::GetIndex(TIndex<TKV> *&out) const {
  assert(this);
  assert(&out);
  out = KVIndex;
}

/*  ----------------------------------------------------------------------
 *  TParentPov::TChildPovCursor.
 */

TParentPov::TChildPovCursor &TParentPov::TChildPovCursor::operator++() {
  assert(this);
  assert(ChildPov);
  ChildPov = ChildPov->NextChildPov;
  return *this;
}

/*  ----------------------------------------------------------------------
 *  TParentPov.
 */

bool TParentPov::PlayTetris(int timeout) {
  assert(this);
  vector<TSync::TExclusiveLock *> child_locks;
  unordered_set<TUpdate *> updates;
  epoll_event *events = new epoll_event[ChildPovCount];
  try {
    int ready_count;
    TOsError::IfLt0(
        HERE,
        ready_count = epoll_wait(TetrisWaitHandle, events, ChildPovCount, timeout));
    for (int i = 0; i < ready_count; ++i) {
      TChildPov *child_pov = static_cast<TChildPov *>(events[i].data.ptr);
      assert(child_pov);
      child_locks.push_back(new TSync::TExclusiveLock(child_pov->GetSync()));
      for (TTrailingUpdateCursor csr(child_pov); csr; ++csr) {
        updates.insert(*csr);
      }
    }
  } catch (...) {
    delete [] events;
    for (auto lock: child_locks) {
      delete lock;
    }
    throw;
  }
  delete [] events;
  bool success = !updates.empty();
  if (success) {
    TSync::TExclusiveLock lock(Sync);
    Atom::TSuprena arena;
    TContext ctxt(this, &arena);
    TUpdate::PlayTetris(ctxt, [&updates](const function<void (TTetrisPiece *)> &cb) {
      for (auto update: updates) {
        cb(update);
      }
    });
  }
  for (auto lock: child_locks) {
    delete lock;
  }
  return success;
}

TParentPov::TParentPov()
    : FirstChildPov(0), LastChildPov(0), ChildPovCount(0) {
  TOsError::IfLt0(HERE, TetrisWaitHandle = epoll_create1(0));
}

TParentPov::~TParentPov() {
  assert(this);
  while (LastChildPov) {
    delete LastChildPov;
  }
  close(TetrisWaitHandle);
}

/*  ----------------------------------------------------------------------
 *  TChildPov.
 */

void TChildPov::Pause() {
  assert(this);
  assert(FlowState == Running);
  FlowState = Paused;
  PartTetris();
}

void TChildPov::Unpause() {
  assert(this);
  assert(FlowState == Paused);
  FlowState = Running;
  JoinTetris();
}

TChildPov::TChildPov(TParentPov *parent_pov, const TOnFail& on_fail, bool paused)
    : FlowState(paused ? Paused : Running), IsJoinedToTetris(false), OnFail(on_fail), ParentPov(parent_pov) {
  assert(&on_fail);
  assert(parent_pov);
  lock_guard<recursive_mutex> lock(parent_pov->ChildListMutex);
  if (!paused) {
    JoinTetris();
  }
  Root = parent_pov->GetRoot();
  GlobalPov = parent_pov->GlobalPov;
  NextChildPov = 0;
  PrevChildPov = parent_pov->LastChildPov;
  (PrevChildPov ? PrevChildPov->NextChildPov : parent_pov->FirstChildPov) = this;
  parent_pov->LastChildPov = this;
  ++(parent_pov->ChildPovCount);
}

TChildPov::~TChildPov() {
  assert(this);
  lock_guard<recursive_mutex> lock(ParentPov->ChildListMutex);
  --(ParentPov->ChildPovCount);
  (PrevChildPov ? PrevChildPov->NextChildPov : ParentPov->FirstChildPov) = NextChildPov;
  (NextChildPov ? NextChildPov->PrevChildPov : ParentPov->LastChildPov) = PrevChildPov;
  PartTetris();
}

void TChildPov::Fail() {
  assert(this);
  assert(FlowState == Running);
  FlowState = Failed;
  PartTetris();
  if(OnFail) {
    try {
      OnFail(this);
    } catch (...) {
      Base::TError::Abort(HERE); //OnFail functions must not throw.
    }
  }
}

void TChildPov::JoinTetris() {
  assert(this);
  if (!IsJoinedToTetris) {
    lock_guard<recursive_mutex> lock(ParentPov->ChildListMutex);
    epoll_event event;
    Zero(event);
    event.events = EPOLLIN;
    event.data.ptr = this;
    TOsError::IfLt0(
        HERE,
        epoll_ctl(ParentPov->TetrisWaitHandle, EPOLL_CTL_ADD, GetTrailingWaitHandle(), &event));
    IsJoinedToTetris = true;
  }
}

void TChildPov::PartTetris() {
  assert(this);
  if (IsJoinedToTetris) {
    lock_guard<recursive_mutex> lock(ParentPov->ChildListMutex);
    epoll_ctl(ParentPov->TetrisWaitHandle, EPOLL_CTL_DEL, GetTrailingWaitHandle(), 0);
    IsJoinedToTetris = false;
  }
}

/*  ----------------------------------------------------------------------
 *  TGlobalPov.
 */

TGlobalPov::TGlobalPov(const string &/*path*/, bool /*is_create*/) {
  Root = new TRoot(this);
  GlobalPov = this;
}

TGlobalPov::~TGlobalPov() {
  assert(this);
  delete Root;
  DeleteAllTrailingUpdate();
}

void TGlobalPov::Fail() {
  assert(this);
  TError::Abort(HERE);
}

/*  ----------------------------------------------------------------------
 *  TPrivatePov.
 */

TPrivatePov::TPrivatePov(TParentPov *parent_pov, const TOnFail &on_fail, bool paused)
    : TChildPov(parent_pov, on_fail, paused) {
  Leaf = new TLeaf(this);
}

TPrivatePov::~TPrivatePov() {
  assert(this);
  delete Leaf;
  DeleteAllTrailingUpdate();
}

/*  ----------------------------------------------------------------------
 *  TSharedPov.
 */

TSharedPov::~TSharedPov() {
  assert(this);
  DeleteAllTrailingUpdate();
}

/*  ----------------------------------------------------------------------
 *  TEvent.
 */

TEvent::~TEvent() {}

/*  ----------------------------------------------------------------------
 *  TCause::TEffectCursor.
 */

TEffect *TCause::TEffectCursor::operator*() const {
  assert(this);
  assert(LinkToEffect);
  return LinkToEffect->Effect;
}

TEffect *TCause::TEffectCursor::operator->() const {
  assert(this);
  assert(LinkToEffect);
  return LinkToEffect->Effect;
}

TCause::TEffectCursor &TCause::TEffectCursor::operator++() {
  assert(this);
  assert(LinkToEffect);
  LinkToEffect = LinkToEffect->NextLinkToEffect;
  return *this;
}

/*  ----------------------------------------------------------------------
 *  TCause.
 */

TLink *TCause::TryGetLinkToEffect(const TEffect *effect) const {
  assert(this);
  TEffectCursor csr(this);
  for (; csr; ++csr) {
    if (*csr == effect) {
      break;
    }
  }
  return csr.TryGetLinkToEffect();
}

TCause::~TCause() {
  assert(this);
  assert(!FirstLinkToEffect);
}

void TCause::DeleteEachLinkToEffect() {
  assert(this);
  while (FirstLinkToEffect) {
    delete FirstLinkToEffect;
  }
}

/*  ----------------------------------------------------------------------
 *  TEffect::TCauseCursor.
 */

TCause *TEffect::TCauseCursor::operator*() const {
  assert(this);
  assert(LinkToCause);
  return LinkToCause->Cause;
}

TCause *TEffect::TCauseCursor::operator->() const {
  assert(this);
  assert(LinkToCause);
  return LinkToCause->Cause;
}

TEffect::TCauseCursor &TEffect::TCauseCursor::operator++() {
  assert(this);
  assert(LinkToCause);
  LinkToCause = LinkToCause->NextLinkToCause;
  return *this;
}

/*  ----------------------------------------------------------------------
 *  TEffect.
 */

bool TEffect::GetIsTrailing() const {
  assert(this);
  TCauseCursor csr(this);
  for (; csr; ++csr) {
    if (csr->IsLocal(this)) {
      break;
    }
  }
  return !csr;
}

TLink *TEffect::TryGetLinkToCause(const TCause *cause) const {
  assert(this);
  TCauseCursor csr(this);
  for (; csr; ++csr) {
    if (*csr == cause) {
      break;
    }
  }
  return csr.TryGetLinkToCause();
}

TEffect::~TEffect() {
  assert(this);
  assert(!FirstLinkToCause);
}

void TEffect::DeleteEachLinkToCause() {
  assert(this);
  while (FirstLinkToCause) {
    delete FirstLinkToCause;
  }
}

/*  ----------------------------------------------------------------------
 *  TRoot.
 */

TRoot::~TRoot() {
  assert(this);
  DeleteEachLinkToEffect();
}

/*  ----------------------------------------------------------------------
 *  TLeaf.
 */

void TLeaf::AugmentEffectsRecursively(
    unordered_set<const TUpdate *> &/*effects*/,
    const TPov */*pov*/) const {}

TLeaf::~TLeaf() {
  assert(this);
  DeleteEachLinkToCause();
}

void TLeaf::OnLocalCauseJoin() {}

void TLeaf::OnLocalCausePart() {}

/*  ----------------------------------------------------------------------
 *  TUpdate.
 */

TUpdate::TUpdate(
      TPrivatePov *private_pov,
      const unordered_set<TUpdate *> &causes,
      unordered_map<TKV, Var::TPtr<Var::TChange>> &&op_by_kv,
      const TAssertChecker &assert_checker,
      size_t max_age,
      TOnPromote &&on_promote
    )
    : TTetrisPiece(max_age),
      LocalCauseCount(0), IsLinkedToPov(false), NextTrailingUpdate(0), PrevTrailingUpdate(0),
      KVCluster(0), AssertChecker(assert_checker), OnPromote(std::move(on_promote)) {
  assert(private_pov);
  assert(&causes);
  assert(&op_by_kv);
  Pov = private_pov;
  std::vector<TSync::TSharedLock *> shared_lock_vec;
  try {
    unordered_set<TUpdate *> augmented_causes = causes;
    if (!op_by_kv.empty()) {
      KVCluster = new TCluster<TKV>(this, op_by_kv);
      KVCluster->AugmentCauses(augmented_causes, shared_lock_vec);
    }
    TLeaf *leaf = private_pov->GetLeaf();
    new TLink(this, leaf);
    if (!augmented_causes.empty()) {
      for (auto iter = augmented_causes.begin(); iter != augmented_causes.end(); ++iter) {
        TUpdate *cause = *iter;
        TLink *link = cause->TryGetLinkToEffect(leaf);
        if (link) {
          link->SetEffect(this);
        } else {
          try {
            new TLink(cause, this);
          } catch (...) {
            for (auto rescue_iter = augmented_causes.begin(); rescue_iter != iter; ++rescue_iter) {
              TUpdate *cause = *rescue_iter;
              TLink *link = cause->TryGetLinkToEffect(this);
              if (link && link->CauseHasOnlyOneEffect()) {
                link->SetEffect(leaf);
              }
            }
            throw;
          }
        }
      }
    } else {
      new TLink(private_pov->GetRoot(), this);
    }
  } catch (...) {
    for (auto iter = shared_lock_vec.rbegin(); iter != shared_lock_vec.rend(); ++iter) {
      delete *iter;
    }
    delete KVCluster;
    throw;
  }
  for (auto iter = shared_lock_vec.rbegin(); iter != shared_lock_vec.rend(); ++iter) {
    delete *iter;
  }
  SyncWithPov();
}

bool TUpdate::Assert(TContext &ctxt) const {
  assert(this);
  if (AssertChecker) {
    return AssertChecker(ctxt);
  } else {
    return true;
  }
}

void TUpdate::AugmentEffectsRecursively(
    unordered_set<const TUpdate *> &effects,
    const TPov *pov) const {
  assert(this);
  assert(&effects);
  if (pov->HasAncestor(Pov)) {
    auto result = effects.insert(this);
    if (result.second) {
      for (TEffectCursor csr(this); csr; ++csr) {
        csr->AugmentEffectsRecursively(effects, pov);
      }
    }
  }
}

bool TUpdate::ForEachKey(const function<bool (const Var::TVar &)> &cb) const {
  assert(this);
  assert(&cb);
  const unordered_map<TKV, TVersion<TKV> *> &version_by_key = KVCluster->GetVersionByKey();
  for (const pair<TKV, TVersion<TKV> *> &item: version_by_key) {
    if (!cb(item.first.GetKey())) {
      return false;
    }
  }
  return true;
}

size_t TUpdate::GetKeyCount() const {
  assert(this);
  return KVCluster->GetVersionByKey().size();
}

bool TUpdate::Promote() {
  assert(this);
  assert(!LocalCauseCount);
  assert(IsLinkedToPov);
  /* Unlink from our current pov, cache it, then move up to our pov's parent, if any. */
  UnlinkFromPov();
  TPov *old_pov = Pov;
  Pov = Pov->TryGetParentPov();
  /* Let our effects know we've changed pov. */
  for (TEffectCursor csr(this); csr; ++csr) {
    TEffect *effect = *csr;
    TPov *pov_effect = effect->GetPov();
    /* If we used to be a local cause, let the effect know we are no longer local to it. */
    if (pov_effect == old_pov) {
      effect->OnLocalCausePart();
    }
    /* It should be impossible for us to become a local cause of any existing effect via promotion. */
    assert(pov_effect != Pov);
  }
  /* TODO: When collecting updates in global, we need to reset the assert checker, or packages will never fully go away.
  if we're now part of global then we can get rid of our Method reference pointer

  if (Pov && Pov == Pov->GlobalPov) {
    MethodPtr.Reset();
  }*/

  //We've been promoted to a new, pov, call OnPromote!
  try {
    //TODO: Check that OnPromote actually points to something good.
    assert(OnPromote);
    if(OnPromote) {
      OnPromote(Pov);
    }
  } catch(const std::exception &ex) {
    /* TODO: Do something nicer here, eventually. */
    //cout << ex.what() << endl;
    Base::TError::Abort(HERE); //OnPromote functions must not throw.
  } catch (...) {
    /* TODO: Do something nicer here, eventually. */
    Base::TError::Abort(HERE); //OnPromote functions must not throw.
  }

  /* If we have no new pov, it's time to die. */
  if (!Pov) {
    delete this;
    return false;
  }
  /* We have a new pov.  Look through our causes to see which ones might now be local to us and
     link to our new pov as appropriate. */
  for (TCauseCursor csr(this); csr; ++csr) {
    if (csr->IsLocal(this)) {
      ++LocalCauseCount;
    }
  }
  if (KVCluster) {
    KVCluster->OnPromote();
  }
  SyncWithPov();
  /* When we next play Tetris, we should seem new again. */
  ResetAge();
  return true;
}

TUpdate::~TUpdate() {
  assert(this);
  assert(!LocalCauseCount);
  UnlinkFromPov();
  DeleteEachLinkToEffect();
  DeleteEachLinkToCause();
  delete KVCluster;
}

void TUpdate::LinkToPov() {
  assert(this);
  if (!IsLinkedToPov) {
    assert(!LocalCauseCount);
    assert(!NextTrailingUpdate);
    PrevTrailingUpdate = Pov->LastTrailingUpdate;
    (PrevTrailingUpdate ? PrevTrailingUpdate->NextTrailingUpdate : Pov->FirstTrailingUpdate) = this;
    Pov->LastTrailingUpdate = this;
    IsLinkedToPov = true;
    if (!PrevTrailingUpdate) {
      assert(Pov->FirstTrailingUpdate == this);
      assert(Pov->LastTrailingUpdate == this);
      char temp = '\0';
      ssize_t result = write(Pov->PipeHandles[1], &temp, 1);
      TOsError::IfLt0(HERE, result);
      assert(result == 1);
    }
  }
}

void TUpdate::OnLocalCauseJoin() {
  assert(this);
  ++LocalCauseCount;
  assert(LocalCauseCount);
  UnlinkFromPov();
}

void TUpdate::OnLocalCausePart() {
  assert(this);
  assert(LocalCauseCount);
  --LocalCauseCount;
  if (!LocalCauseCount) {
    LinkToPov();
  }
}

void TUpdate::SyncWithPov() {
  assert(this);
  if (LocalCauseCount) {
    UnlinkFromPov();
  } else {
    LinkToPov();
  }
}

void TUpdate::UnlinkFromPov() {
  assert(this);
  if (IsLinkedToPov) {
    assert(!LocalCauseCount);
    (PrevTrailingUpdate ? PrevTrailingUpdate->NextTrailingUpdate : Pov->FirstTrailingUpdate) = NextTrailingUpdate;
    (NextTrailingUpdate ? NextTrailingUpdate->PrevTrailingUpdate : Pov->LastTrailingUpdate) = PrevTrailingUpdate;
    IsLinkedToPov = false;
    NextTrailingUpdate = 0;
    PrevTrailingUpdate = 0;
    if (!Pov->FirstTrailingUpdate) {
      assert(!Pov->LastTrailingUpdate);
      char temp;
      ssize_t result = read(Pov->PipeHandles[0], &temp, 1);
      TOsError::IfLt0(HERE, result);
      assert(result == 1);
    }
  }
}

/*  ----------------------------------------------------------------------
 *  TKVIndex.
 */

TKVIndex::TKeyIndexCursor::TKeyIndexCursor(const TKVIndex *index, const Var::TVar &pattern, const Base::TUuid &index_id)
    : Valid(false), KV(0), Pov(Base::AssertTrue(index)->GetPov()), Pattern(pattern), IndexId(index_id) {
  TPov *pov = Pov;
  try {
    for (; pov; pov = pov->TryGetParentPov()) {
      SharedLockVec.push_back(new TSync::TSharedLock(pov->GetSync()));
    }
  } catch (...) {
    for (auto iter = SharedLockVec.rbegin(); iter != SharedLockVec.rend(); ++iter) {
      delete *iter;
    }
    throw;
  }
  /* init iterators */
  size_t num_povs = 0U;
  pov = Pov;
  for (; pov; pov = pov->TryGetParentPov(), ++num_povs) {
    if (Pattern) {
      kIterVec.insert(kIterVec.begin(), make_pair(pov->GetKVIndex()->K.lower_bound(TKV(Pattern, index_id)), pov->GetKVIndex()->K.upper_bound(TKV(Pattern, index_id))));
    } else {
      TourByKeyIterVec.insert(TourByKeyIterVec.begin(), make_pair(pov->GetKVIndex()->TourByKey.begin(), pov->GetKVIndex()->TourByKey.end()));
    }
  }
  if (Pattern) {
    kIterStateSet.reserve(num_povs);
  } else {
    TourByKeyIterStateSet.reserve(num_povs);
  }
  GetValue();
}

TKVIndex::TKeyIndexCursor::~TKeyIndexCursor() {
  assert(this);
  for (auto iter = SharedLockVec.rbegin(); iter != SharedLockVec.rend(); ++iter) {
    delete *iter;
  }
}

TKVIndex::TKeyIndexCursor &TKVIndex::TKeyIndexCursor::operator++() {
  assert(this);
  KV = 0;
  Valid = false;
  if (Pattern) {
    for (auto iter : kIterStateSet) {
      ++(kIterVec[iter].first);
    }
    kIterStateSet.clear();
  } else {
    for (auto iter : TourByKeyIterStateSet) {
      ++(TourByKeyIterVec[iter].first);
    }
    TourByKeyIterStateSet.clear();
  }
  return *this;
}

void TKVIndex::TKeyIndexCursor::GetValue() const {
  assert(this);

  const TKV *min_kv = 0;
  if (Pattern) {
    /* bring all the iterators upto the point where they match our pattern */
    for (auto vec_iter : kIterVec) {
      while (vec_iter.first != vec_iter.second) {
        if (vec_iter.first->second->GetKey().GetIndexId() == IndexId && vec_iter.first->second->GetKey().GetKey() == Pattern) {
          break;
        }
        ++vec_iter.first;
      }
    }
    /* done bringing all the iterators to the point where they match our pattern */
    kIterStateSet.clear();
    size_t pos = 0;
    for (auto vec_iter : kIterVec) {
      if (vec_iter.first != vec_iter.second) {
        const TKV &cur_kv = vec_iter.first->second->GetKey();
        if (min_kv == 0 || cur_kv < *min_kv) {
          min_kv = &cur_kv;
          kIterStateSet.clear();
          kIterStateSet.insert(pos);
        } else if (cur_kv == *min_kv) {
          min_kv = &cur_kv;
          kIterStateSet.insert(pos);
        }
      }
      ++pos;
    }
    KV = min_kv;
  } else {
    TourByKeyIterStateSet.clear();
    size_t pos = 0;
    for (auto vec_iter : TourByKeyIterVec) {
      if (vec_iter.first != vec_iter.second) {
        const TKV &cur_kv = vec_iter.first->second->GetKey();
        if (min_kv == 0 || cur_kv < *min_kv) {
          min_kv = &cur_kv;
          TourByKeyIterStateSet.clear();
          TourByKeyIterStateSet.insert(pos);
        } else if (cur_kv == *min_kv) {
          min_kv = &cur_kv;
          TourByKeyIterStateSet.insert(pos);
        }
      }
      ++pos;
    }
    KV = min_kv;
  }
  Valid = true;
}

#if 0
bool TNodeIndex::ForEachTour(
    const function<bool (TTour<TNode> *)> &callback,
    const string *location, const string *klass) const {
  assert(this);
  assert(&callback);
  if (location && klass) {
    TTour<TNode> *tour = TryGetTour(TNode(*location, *klass));
    if (tour && !callback(tour)) {
      return false;
    }
  } else if (location) {
    for (auto iter = Lk.lower_bound(TNode(*location, EmptyString));
         iter != Lk.end() && iter->first.GetLocation() == *location; ++iter) {
      if (!callback(iter->second)) {
        return false;
      }
    }
  } else if (klass) {
    for (auto iter = Kl.lower_bound(TNode(EmptyString, *klass));
         iter != Kl.end() && iter->first.GetKlass() == *klass; ++iter) {
      if (!callback(iter->second)) {
        return false;
      }
    }
  } else if (!TIndex<TNode>::ForEachTour(callback)) {
    return false;
  }
  return true;
}
#endif

TKVIndex::~TKVIndex() {}

void TKVIndex::OnTourJoin(const pair<TKV, TTour<TKV> *> &kt_pair) {
  assert(this);
  try {
    InsertOrFail(K, kt_pair);
  } catch (...) {
    OnTourPart(kt_pair.first);
    throw;
  }
}

void TKVIndex::OnTourPart(const TKV &key) {
  assert(this);
  K.erase(key);
}
