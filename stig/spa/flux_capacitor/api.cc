/* <stig/spa/flux_capacitor/api.cc>

   Implements <stig/spa/flux_capacitor/api.h>

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

#include <stig/spa/flux_capacitor/api.h>

#include <stig/spa/error.h>

using namespace std;
using namespace Stig;
using namespace Stig::Spa::FluxCapacitor;

TSessionObj::~TSessionObj() {
  std::lock_guard<std::recursive_mutex> lock(NotifierLock);
  for(auto it: Notifiers) {
    delete it.second;
  }
}

void TSessionObj::Poll(const std::unordered_set<TUUID> &notifiers,
      Base::TOpt<std::chrono::milliseconds> timeout,
      std::unordered_map<TUUID, TNotifierState> &out) {
  assert(&notifiers);
  assert(&out);

  unordered_map<MultiEvent::TEvent::TPtr, TNotifier*> notifier_events;

  {
    //Lock the notifier maps, add some notifiers to the "working" set.
    std::lock_guard<std::recursive_mutex> lock(NotifierLock);
    for(auto it: notifiers) {
      auto notifier = Notifiers.find(it);
      if(notifier == Notifiers.end()) {
        std::ostringstream s;
        s << "Given notifier '" << it << "' does not exist";
        throw TNotifierError(HERE, s.str().c_str());
      }

      if(WaitingNotifiers.count(it) > 0) {
        std::ostringstream s;
        s << "Given notifier '" << it << "' is already being waited upon";
        throw TNotifierError(HERE, s.str().c_str());
      }
      WaitingNotifiers.insert(it);

      notifier_events.insert(make_pair(notifier->second->GetEvent(), Notifiers[it]));
    }
  }

  /* TODO: Not low memory safe, we could actually error while generating the out map, which would cause us to have done
     processing, and possibly change internal state but not report it back to the user. */
  std::vector<const TNotifier*> finished;
  finished.reserve(notifiers.size());

  //Wait for events.
  MultiEvent::TWaiter().Wait<TNotifier*>(notifier_events, timeout, [&out,&finished](const TNotifier *val) {
    out.insert(make_pair(val->GetId(), val->GetState()));
    finished.push_back(val);
  });

  RemoveNotifiers(finished);
}

void TSessionObj::CleanupNotifiers(const std::unordered_map<TUUID, TUUID> &notifiers) {
  std::vector<const TNotifier*> n_ptrs;
  n_ptrs.reserve(notifiers.size());

  std::lock_guard<std::recursive_mutex> lock(NotifierLock);
  for(auto it: notifiers) {
    n_ptrs.push_back(Notifiers[it.second]);
  }
  RemoveNotifiers(n_ptrs);
}

bool TSessionObj::ForEachSession(const std::function<bool (TSessionObj*)> &cb) {
  return Base::AssertTrue(TAnyHoncho::GetAnyHoncho())->GetSessionObjStore().ForEach(cb);
}

void TSessionObj::RemoveNotifiers(const std::vector<const TNotifier*> &notifiers) {
  std::lock_guard<std::recursive_mutex> lock(NotifierLock);
  //remove the notifiers from all the maps
  std::unordered_map<TUUID, TNotifier*> notifiers_map(Notifiers); //notifier id -> notifier (Poll)
  std::unordered_map<TUUID, std::unordered_map<const TPov*, TNotifier*>> notifiers_by_update(NotifiersByUpdate); //update -> pov -> notifier (OnPromote)
  std::unordered_multimap<TPov*, TNotifier*> notifiers_by_pov(NotifiersByPov); //pov -> notifier (TChildPov::OnFail)

  for(auto it: notifiers) {
    notifiers_map.erase(it->GetId());

    auto nbu_it = notifiers_by_update.find(it->GetUpdateId());
    assert(nbu_it != notifiers_by_update.end());
    if(nbu_it->second.size() == 1) {
      notifiers_by_update.erase(nbu_it);
    } else {
      nbu_it->second.erase(it->GetPov());
    }

    auto range = notifiers_by_pov.equal_range(it->GetPov());
    assert(range.first != notifiers_by_pov.end());
    for(; range.first != range.second; ++range.first) {
      if(range.first->second == it) {
        notifiers_by_pov.erase(range.first);
        break;
      }
    }
  }

  //Delete the now removed notifiers
  for(auto it: notifiers) {
    delete it;
  }

  //Swap in the updated maps.
  swap(Notifiers, notifiers_map);
  swap(NotifiersByUpdate, notifiers_by_update);
  swap(NotifiersByPov, notifiers_by_pov);
}



void TSessionObj::OnPromote(TUUID update_id, const TPov *pov) {
  assert(pov);

  std::lock_guard<std::recursive_mutex> lock(NotifierLock);
  auto it = NotifiersByUpdate.find(update_id);
  if(it != NotifiersByUpdate.end()) {
    auto it2 = it->second.find(pov);
    if(it2 != it->second.end()) {
      it2->second->Fire(Succeeded);
    }
  }
}

void TSessionObj::OnPovFail(TPov *pov) {
  assert(this);
  assert(pov);
  std::lock_guard<std::recursive_mutex> lock(NotifierLock);

  auto range = NotifiersByPov.equal_range(pov);
  if(range.first == NotifiersByPov.end() && range.second == NotifiersByPov.end()) {
    return;
  }

  //Mark each update as failed, as well as all their parent updates.
  for(; range.first != range.second; ++range.first) {
    range.first->second->Fire(Failed);
    auto uuid = range.first->second->GetUpdateId();
    const std::unordered_map<const TPov*, TNotifier*> &map = NotifiersByUpdate.find(uuid)->second;
    for(auto it = map.begin(); it != map.end(); ++it) {
      if(it->second->TryGetState().IsUnknown()) {
        it->second->Fire(Failed);
      }
    }
  }

}

TSessionObj::TNotifier::TNotifier(TPov *pov, TUUID update_id) : Event(MultiEvent::TEvent::New()), Pov(pov), UpdateId(update_id) {}

void TSessionObj::TNotifier::Fire(TNotifierState state) {
  assert(this);
  assert(!State);

  State = state;
  Event->Fire();
}



MultiEvent::TEvent::TPtr TSessionObj::TNotifier::GetEvent() const {
  assert(this);
  return Event;
}

const TUUID &TSessionObj::TNotifier::GetId() const {
  assert(this);
  return Id;
}

TPov *TSessionObj::TNotifier::GetPov() const {
  assert(this);
  return Pov;
}

TNotifierState TSessionObj::TNotifier::GetState() const {
  assert(this);
  return *State;
}

const TUUID &TSessionObj::TNotifier::GetUpdateId() const {
  assert(this);
  return UpdateId;
}


const Base::TOpt<TNotifierState> &TSessionObj::TNotifier::TryGetState() const {
  assert(this);
  return State;
}

void TSessionObj::MakeNotifiers(const TPrivatePovObj* ppov, const std::unordered_set<TUUID> &notify_povs, const TUUID &update_id, std::unordered_map<TUUID, TUUID> &out) {
  assert(this);
  std::lock_guard<std::recursive_mutex> lock(NotifierLock);

  std::unordered_map<TUUID, TNotifier*> notifiers_map(Notifiers); //notifier id -> notifier (Poll)
  std::unordered_map<TUUID, std::unordered_map<const TPov*, TNotifier*>> notifiers_by_update(NotifiersByUpdate); //update -> pov -> notifier (OnPromote)
  std::unordered_multimap<TPov*, TNotifier*> notifiers_by_pov(NotifiersByPov); //pov -> notifier (TChildPov::OnFail)

  TNotifier **notifier_list = new TNotifier*[notify_povs.size()];
  try {
    unsigned int num_completed = 0;

    try {
      for(auto it: notify_povs) {
        //Find the pov for the item and make sure that it is either a parent of, or the exact private pov.
        /* TODO: This isn't the most efficient search, esp. since we will likely recompute results. */
        //TODO: The try/catch here is FUGLY. Should have a "Rendezvous with pov".
        TPov *pov_native = 0;
        //TODO: allow the global pov.
        try {
          //NOTE: The TTL could technically expire before we exit all of this... That would be bad... LALALALALALALA
          TPrivatePovObj::TPtr notify_ppov = TPrivatePovObj::TPrivatePovHandle::Rendezvous(it);
          pov_native = notify_ppov->GetPrivatePov();
        } catch (const TPrivatePovError &ex) {
          try {
             //NOTE: The TTL could technically expire before we exit all of this... That would be bad... LALALALALALALA
            TSharedPovObj::TPtr notify_spov = TSharedPovObj::TSharedPovHandle::Rendezvous(it);
            pov_native = notify_spov->GetSharedPov();
          } catch (const TSharedPovError &ex) {
            std::ostringstream s;
            s << "Given UUID '" << it << "' could not be found" << endl;
            throw TGeneralError(HERE, s.str().c_str());
          }
        }
        if(!ppov->GetNative()->HasAncestor(pov_native)) {
          /* TODO: More precise error message */
          throw TNotifierError(HERE, "one (or more) of the given povs isn't an ancestor of the private pov");
        }

        TNotifier *notifier = new TNotifier(pov_native, update_id);
        ++num_completed;

        out.insert(make_pair(it, notifier->GetId()));
        notifiers_map.insert(make_pair(notifier->GetId(), notifier));
        {
          auto n_up_it = notifiers_by_update.find(update_id);
          /* TODO: Should this really liven up the pov? */
          if(n_up_it != notifiers_by_update.end()) {
            //NOTE: Guaranteed not to replace, since the notify_povs come out of a set.
            n_up_it->second.insert(make_pair(pov_native, notifier));
          } else {

            notifiers_by_update.insert(
                make_pair(
                  update_id,
                  std::unordered_map<const TPov*, TSessionObj::TNotifier*>({{pov_native, notifier}})
                )
              );

          }
        }
        notifiers_by_pov.insert(make_pair(pov_native, notifier));
      }
    } catch (...) {
      for(unsigned int i=0; i<num_completed; ++i) {
        delete notifier_list[i];
      }
      out.clear();
      throw;
    }
  } catch (...) {
    delete [] notifier_list;
    throw;
  }

  swap(Notifiers, notifiers_map);
  swap(NotifiersByUpdate, notifiers_by_update);
  swap(NotifiersByPov, notifiers_by_pov);
}