/* <stig/indy/memory_layer.cc>

   Implements <stig/indy/memory_layer.h>.

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

#include <stig/indy/memory_layer.h>

using namespace std;
using namespace Base;
using namespace Stig::Indy;

TMemoryLayer::TMemoryLayer(L0::TManager *manager)
    : TDataLayer(manager),
      UpdateCollection(this),
      EntryCollection(this),
      Size(0UL) {}

TMemoryLayer::~TMemoryLayer() {
  assert(this);
  UpdateCollection.DeleteEachMember();
  EntryCollection.DeleteEachMember();
}

void TMemoryLayer::Insert(TUpdate *update) NO_THROW {
  assert(this);
  for (TUpdate::TEntryCollection::TCursor csr(&update->EntryCollection/*, InvCon::TOrient::Rev*/); csr; ++csr) {
    ++Size;
    csr->MemoryLayerMembership.ReverseInsert(&EntryCollection);
  }
  update->MemoryLayerMembership.ReverseInsert(&UpdateCollection);
}

void TMemoryLayer::ReverseInsert(TUpdate *update) NO_THROW {
  assert(this);
  for (TUpdate::TEntryCollection::TCursor csr(&update->EntryCollection); csr; ++csr) {
    ++Size;
    csr->MemoryLayerMembership.ReverseInsert(&EntryCollection);
  }
  update->MemoryLayerMembership.ReverseInsert(&UpdateCollection);
}

std::unique_ptr<TPresentWalker> TMemoryLayer::NewPresentWalker(const TIndexKey &from,
                                                               const TIndexKey &to) const {
  assert(this);
  return unique_ptr<TRangePresentWalker>(new TRangePresentWalker(this, from, to));
}

std::unique_ptr<TPresentWalker> TMemoryLayer::NewPresentWalker(const TIndexKey &key) const {
  assert(this);
  return unique_ptr<TMatchPresentWalker>(new TMatchPresentWalker(this, key));
}

std::unique_ptr<Stig::Indy::TUpdateWalker> TMemoryLayer::NewUpdateWalker(TSequenceNumber from) const {
  assert(this);
  return unique_ptr<TUpdateWalker>(new TUpdateWalker(this, from));
}

TMemoryLayer::TMatchPresentWalker::TMatchPresentWalker(const TMemoryLayer *layer,
                                                       const TIndexKey &key)
    : Stig::Indy::TPresentWalker(Match),
      Layer(layer),
      Key(key),
      Csr(Layer->GetEntryCollection()),
      Valid(true),
      Cached(false),
      PassedMatch(false) {
  Refresh();
}

TMemoryLayer::TMatchPresentWalker::~TMatchPresentWalker() {}

inline TMemoryLayer::TMatchPresentWalker::operator bool() const {
  assert(this);
  Refresh();
  return Valid;
}

const TPresentWalker::TItem &TMemoryLayer::TMatchPresentWalker::operator*() const {
  assert(this);
  Refresh();
  assert(Valid);
  return Item;
}

inline TMemoryLayer::TMatchPresentWalker &TMemoryLayer::TMatchPresentWalker::operator++() {
  assert(this);
  Cached = false;
  ++Csr;
  Refresh();
  return *this;
}

void TMemoryLayer::TMatchPresentWalker::Refresh() const {
  assert(this);
  if (Valid && !Cached) {
    void *key_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
    void *search_state_alloc = reinterpret_cast<uint8_t *>(key_state_alloc) + Sabot::State::GetMaxStateSize();
    Sabot::State::TAny::TWrapper key_state(Key.GetKey().GetCore().NewState(Key.GetKey().GetArena(), key_state_alloc));
    for (;Csr; ++Csr) {
      Cached = true;
      Atom::TComparison index_id_comp = Atom::CompareOrdered(Key.GetIndexId(), Csr->GetIndexKey().GetIndexId());
      switch (index_id_comp) {
        case Atom::TComparison::Lt: {
          Valid = false;
          return;
        }
        case Atom::TComparison::Eq: {
          Sabot::State::TAny::TWrapper cur_state(Csr->GetKey().GetCore().NewState(Csr->GetKey().GetArena(), search_state_alloc));
          Sabot::TMatchResult result = MatchPrefixState(*key_state, *cur_state);
          switch (result) {
            case Sabot::TMatchResult::NoMatch: {
              if (PassedMatch) {
                Valid = false;
                return;
              }
              break;
            }
            case Sabot::TMatchResult::PrefixMatch: {
              break;
            }
            case Sabot::TMatchResult::Unifies: {
              PassedMatch = true;
              Item.KeyArena = Csr->GetKey().GetArena();
              Item.OpArena = Csr->GetKey().GetArena();
              Item.Key = Csr->GetKey().GetCore();
              Item.Op = Csr->GetOp();
              Item.SequenceNumber = Csr->GetSequenceNumber();
              return;
              break;
            }
          }
          break;
        }
        case Atom::TComparison::Gt: {
          break;
        }
        case Atom::TComparison::Ne: {
          throw;
        }
      }
    }
    Valid = false;
  }
}

TMemoryLayer::TRangePresentWalker::TRangePresentWalker(const TMemoryLayer *layer,
                                                       const TIndexKey &from,
                                                       const TIndexKey &to)
    : Stig::Indy::TPresentWalker(Range),
      Layer(layer),
      From(from),
      To(to),
      Csr(Layer->GetEntryCollection()),
      Valid(true), Cached(false), PassedMatch(false) {
  assert(From.GetIndexId() == To.GetIndexId());
  Refresh();
}

TMemoryLayer::TRangePresentWalker::~TRangePresentWalker() {}

TMemoryLayer::TRangePresentWalker::operator bool() const {
  assert(this);
  Refresh();
  return Valid;
}

const TPresentWalker::TItem &TMemoryLayer::TRangePresentWalker::operator*() const {
  assert(this);
  Refresh();
  assert(Valid);
  assert(Cached);
  return Item;
}

TMemoryLayer::TRangePresentWalker &TMemoryLayer::TRangePresentWalker::operator++() {
  assert(this);
  assert(Valid);
  Cached = false;
  ++Csr;
  Refresh();
  return *this;
}

void TMemoryLayer::TRangePresentWalker::Refresh() const {
  assert(this);
  if (Valid && !Cached) {
    void *key_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
    void *search_state_alloc = reinterpret_cast<uint8_t *>(key_state_alloc) + Sabot::State::GetMaxStateSize();
    Sabot::State::TAny::TWrapper key_state(PassedMatch ?
                                           To.GetKey().GetCore().NewState(To.GetKey().GetArena(), key_state_alloc) :
                                           From.GetKey().GetCore().NewState(From.GetKey().GetArena(), key_state_alloc));
    for (;Csr; ++Csr) {
      Cached = true;
      Atom::TComparison index_id_comp = Atom::CompareOrdered(From.GetIndexId(), Csr->GetIndexKey().GetIndexId());
      switch (index_id_comp) {
        case Atom::TComparison::Lt: {
          Valid = false;
          return;
        }
        case Atom::TComparison::Eq: {
          Sabot::State::TAny::TWrapper cur_state(Csr->GetKey().GetCore().NewState(Csr->GetKey().GetArena(), search_state_alloc));
          Atom::TComparison comp = OrderStates(*cur_state, *key_state);
          if (!PassedMatch) {
            if (Atom::IsGe(comp)) {
              PassedMatch = true;
              Sabot::State::TAny::TWrapper to_state(To.GetKey().GetCore().NewState(To.GetKey().GetArena(), key_state_alloc));
              Atom::TComparison comp = OrderStates(*cur_state, *key_state);
              if (Atom::IsGt(comp)) {
                Valid = false;
                return;
              }
            } else {
              continue;
            }
          } else {
            if (Atom::IsGt(comp)) {
              Valid = false;
              return;
            }
          }
          Item.KeyArena = Csr->GetKey().GetArena();
          Item.OpArena = Csr->GetKey().GetArena();
          Item.Key = Csr->GetKey().GetCore();
          Item.Op = Csr->GetOp();
          Item.SequenceNumber = Csr->GetSequenceNumber();
          return;
        }
        case Atom::TComparison::Gt: {
          break;
        }
        case Atom::TComparison::Ne: {
          throw;
        }
      }
    }
    Valid = false;
  }
}


TMemoryLayer::TUpdateWalker::TUpdateWalker(const TMemoryLayer *layer, TSequenceNumber from)
    : Layer(layer), From(from), Csr(Layer->GetUpdateCollection()), Valid(false) {
  Refresh();
}

TMemoryLayer::TUpdateWalker::~TUpdateWalker() {}

TMemoryLayer::TUpdateWalker::operator bool() const {
  assert(this);
  return Valid;
}

const TUpdateWalker::TItem &TMemoryLayer::TUpdateWalker::operator*() const {
  assert(this);
  assert(Valid);
  Item.MainArena = &Csr->GetSuprena();
  Item.SequenceNumber = Csr->GetSequenceNumber();
  Item.Metadata = Csr->GetMetadata();
  Item.Id = Csr->GetId();
  Item.EntryVec.clear();
  for (TUpdate::TEntryCollection::TCursor csr(Csr->GetEntryCollection()); csr; ++csr) {
    Item.EntryVec.push_back(std::make_pair(csr->GetIndexKey(), csr->GetOp()));
  }
  return Item;
}

TMemoryLayer::TUpdateWalker &TMemoryLayer::TUpdateWalker::operator++() {
  assert(this);
  ++Csr;
  Refresh();
  return *this;
}

void TMemoryLayer::TUpdateWalker::Refresh() {
  assert(this);
  Valid = false;
  for (;Csr; ++Csr) {
    if (Csr->GetSequenceNumber() >= From) {
      Valid = true;
      break;
    }
  }
}

void TMemoryLayer::ImporterAppendUpdate(TUpdate *update) {
  assert(this);
  assert(update);
  update->MemoryLayerMembership.ReverseInsert(&UpdateCollection);
}

void TMemoryLayer::ImporterAppendEntry(TUpdate::TEntry *entry) {
  assert(this);
  assert(entry);
  ++Size;
  entry->MemoryLayerMembership.ReverseInsert(&EntryCollection);
}