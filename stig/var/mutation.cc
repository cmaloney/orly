/* <stig/var/mutation.cc>

   Implements <stig/var/mutation.h>

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

#include <stig/var/mutation.h>

#include <stig/rt/mutate.h>
#include <stig/var/new_sabot.h>
#include <stig/var/util.h>
#include <stig/var.h>

using namespace std;
using namespace Stig;
using namespace Stig::Var;

TPtr<TObjChange> TObjChange::New(string key, const TPtr<TChange> &change) {
  return TPtr<TObjChange>(new TObjChange({{key, change}}));
}

/* TODO: The Apply function should be able to be moved into TPartialChange. */
void TObjChange::Apply(TVar &var) const {
   assert(this);
   /*
   std::cout << "TObjChange From [";
   void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
   Sabot::State::TAny::TWrapper(Var::NewSabot(state_alloc, var))->Accept(Sabot::TStateDumper(std::cout));
   std::cout << "]";
   */
  //NOTE: If the assertion fails, there was a type mismatch in the database. Expected an object, didn't get it.
    TObj::TElems elems = var.As<TObj>()->GetVal();
    for(auto &change: GetChanges()) {
      //NOTE: If this throws, then there was a type mismatch in the database. Expected a field, didn't find it.
      change.second->Apply(elems.at(change.first));
    }
    var = TVar::Obj(elems);
    /*
    std::cout << "To [";
    Sabot::State::TAny::TWrapper(Var::NewSabot(state_alloc, var))->Accept(Sabot::TStateDumper(std::cout));
    std::cout << "]" << std::endl;
    */
}

TObjChange::TObjChange(TChanges &&changes) : TPartialChange(move(changes)) {}


TPtr<TDictChange> TDictChange::New(const Var::TVar &key, const TPtr<TChange> &change) {
  return TPtr<TDictChange>(new TDictChange({{key, change}}));
}

void TDictChange::Apply(TVar &var) const {
   assert(this);

  //NOTE: If the assertion fails, there was a type mismatch in the database. Expected an object, didn't get it.
    const TDict *dict = var.As<TDict>();
    TDict::TElems elems = dict->GetVal();
    for(auto &change: GetChanges()) {
      //NOTE: If this throws, then there was a type mismatch in the database. Expected a field, didn't find it.
      change.second->Apply(elems.at(change.first));
    }
    var = TVar::Dict(elems, dict->GetKeyType(), dict->GetValType());
}

TDictChange::TDictChange(TChanges &&changes) : TPartialChange(move(changes)) {}

TPtr<TAddrChange> TAddrChange::New(uint32_t key, const TPtr<TChange> &change) {
  return TPtr<TAddrChange>(new TAddrChange({{key, change}}));
}

void TAddrChange::Apply(TVar &var) const {
   assert(this);

  //NOTE: If the assertion fails, there was a type mismatch in the database. Expected an object, didn't get it.
    TAddr::TElems elems = var.As<TAddr>()->GetVal();
    for(auto &change: GetChanges()) {
      //NOTE: If this throws, then there was a type mismatch in the database. Expected a field, didn't find it.
      change.second->Apply(elems.at(change.first).second);
    }
    var = TVar::Addr(elems);
}

TAddrChange::TAddrChange(TChanges &&changes) : TPartialChange(move(changes)) {}

TPtr<TListChange> TListChange::New(uint64_t key, const TPtr<TChange> &change) {
  return TPtr<TListChange>(new TListChange({{key, change}}));
}

void TListChange::Apply(TVar &var) const {
   assert(this);

 //NOTE: If the assertion fails, there was a type mismatch in the database. Expected an object, didn't get it.
    const TList *list = var.As<TList>();
    TList::TElems elems = var.As<TList>()->GetVal();
    for(auto &change: GetChanges()) {
      //NOTE: If this throws, then there was a type mismatch in the database. Expected a field, didn't find it.
      change.second->Apply(elems.at(change.first));
    }
    var = TVar::List(elems, list->GetElemType());
}

TListChange::TListChange(TChanges &&changes) : TPartialChange(move(changes)) {}

TPtr<TMutation> TMutation::New(TMutator mutator, const Var::TVar &rhs) {
  return TPtr<TMutation>(new TMutation(mutator, rhs));
}

void TMutation::Apply(Var::TVar &var) const {
  assert(this);

  var = Stig::Rt::Mutate(var, Mutator, Rhs);
}

void TMutation::Augment(const TPtr<const TChange> &) {
  throw Rt::TSystemError(HERE, "Conflicting updates to the same key. Applied a partial mutation and a full mutation to the same part of a key.");
}

bool TMutation::IsDelete() const {
  assert(this);
  return false;
}

bool TMutation::IsFinal() const {
  assert(this);
  return Mutator == TMutator::Assign;
}

TMutation::TMutation(TMutator mutator, const Var::TVar &rhs) : Mutator(mutator), Rhs(rhs) {}

TPtr<TDelete> TDelete::New() {
  return TPtr<TDelete>(new TDelete());
}

void TDelete::Apply(Var::TVar &var) const {
  var = TVar();
}

void TDelete::Augment(const TPtr<const TChange> &) {
  throw Rt::TSystemError(HERE, "Conflicting updates to the same key. Deleted the key, and applied some other change.");
}

bool TDelete::IsDelete() const {
  assert(this);
  return true;
}

bool TDelete::IsFinal() const {
  assert(this);
  return true;
}

TDelete::TDelete() {}

TPtr<TNew> TNew::New(const Var::TVar &val) {
  return TPtr<TNew>(new TNew(val));
}

void TNew::Apply(Var::TVar &var) const {
  var = Val;
}

void TNew::Augment(const TPtr<const TChange> &) {
  throw Rt::TSystemError(HERE, "Conflicting updates to the same key. Inserted (with new) the key, and applied some other change.");
}

bool TNew::IsDelete() const {
  assert(this);
  return false;
}

bool TNew::IsFinal() const {
  assert(this);

  return true;
}

TNew::TNew(const Var::TVar &val) : Val(val) {}