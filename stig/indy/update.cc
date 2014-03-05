/* <stig/indy/update.cc>

   Implements <stig/indy/update.h>.

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

#include <stig/indy/update.h>

using namespace std;
using namespace Base;
using namespace Stig::Atom;
using namespace Stig::Indy;

TUpdate::TPersistenceNotification::TPersistenceNotification(const std::function<void (TResult)> &cb)
    : Cb(cb) {}

TUpdate::TPersistenceNotification::~TPersistenceNotification() {}

shared_ptr<TUpdate> TUpdate::NewUpdate(const TOpByKey &op_by_key, const TKey &metadata, const TKey &id) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  return std::shared_ptr<TUpdate>(new TUpdate(op_by_key, metadata, id, state_alloc));
}

shared_ptr<TUpdate> TUpdate::ReconstructUpdate(TSequenceNumber seq_num) {
  return std::shared_ptr<TUpdate>(new TUpdate(seq_num));
}

TUpdate *TUpdate::CopyUpdate(TUpdate *that, void *state_alloc) {
  //void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  return new TUpdate(that, state_alloc);
}

TUpdate *TUpdate::ShallowCopy(TUpdate *that, void *state_alloc) {
  return new TUpdate(state_alloc, that);
}

bool TUpdate::TEntry::TEntryKey::operator==(const TUpdate::TEntry::TEntryKey &that) const {
  assert(this);
  return Entry->GetSequenceNumber() == that.Entry->GetSequenceNumber() && Entry->IndexKey == that.Entry->IndexKey;
}

bool TUpdate::TEntry::TEntryKey::operator!=(const TUpdate::TEntry::TEntryKey &that) const {
  assert(this);
  return Entry->GetSequenceNumber() != that.Entry->GetSequenceNumber() || Entry->IndexKey != that.Entry->IndexKey;
}

bool TUpdate::TEntry::TEntryKey::operator<=(const TUpdate::TEntry::TEntryKey &that) const {
  assert(this);
  TComparison comp = Atom::CompareOrdered(Entry->IndexKey.GetIndexId(), that.Entry->IndexKey.GetIndexId());
  switch (comp) {
    case Atom::TComparison::Lt: {
      return true;
    }
    case Atom::TComparison::Eq: {
      comp = Entry->GetKey().Compare(that.Entry->GetKey());
      return Atom::IsLt(comp) || (IsEq(comp) && (Entry->GetSequenceNumber() >= that.Entry->GetSequenceNumber()));
    }
    case Atom::TComparison::Gt: {
      return false;
    }
    case Atom::TComparison::Ne: {
      throw std::logic_error("CompareOrdered should not return Ne");
    }
  }
  throw;
}

bool TUpdate::TEntry::TEntryKey::operator>(const TUpdate::TEntry::TEntryKey &that) const {
  assert(this);
  TComparison comp = Atom::CompareOrdered(Entry->IndexKey.GetIndexId(), that.Entry->IndexKey.GetIndexId());
  switch (comp) {
    case Atom::TComparison::Lt: {
      return false;
    }
    case Atom::TComparison::Eq: {
      comp = Entry->GetKey().Compare(that.Entry->GetKey());
      return Atom::IsGt(comp) || (IsEq(comp) && (Entry->GetSequenceNumber() < that.Entry->GetSequenceNumber()));
    }
    case Atom::TComparison::Gt: {
      return true;
    }
    case Atom::TComparison::Ne: {
      throw std::logic_error("CompareOrdered should not return Ne");
    }
  }
  throw;
}

TUpdate::TEntry::TEntryKey::TEntryKey(const TEntry *entry)
    : Entry(entry) {}

TUpdate::TEntry::TEntry(TUpdate *update, const TIndexKey &index_key, const TKey &op, void *state_alloc)
    : IndexKey(index_key.GetIndexId(), TKey(&update->Suprena, state_alloc, index_key.GetKey())),
      UpdateMembership(this, IndexKey.GetKey(), InvCon::TOrient::Rev, &update->EntryCollection),
      MemoryLayerMembership(this, TEntryKey(this)),
      Op(&update->Suprena, Sabot::State::TAny::TWrapper(op.GetCore().NewState(op.GetArena(), state_alloc))) {
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  #ifndef NDEBUG
  Sabot::AssertTuple(*Sabot::Type::TAny::TWrapper(GetKey().GetCore().GetType(&update->Suprena, type_alloc)));
  #endif
  IndexKey.GetKey().GetCore().TrySetStoredHash(GetKey().GetHash());
}

TUpdate::TUpdate(const TOpByKey &op_by_key, const TKey &metadata, const TKey &id, void *state_alloc)
    : EntryCollection(this),
      MemoryLayerMembership(this, 0UL),
      Metadata(&Suprena, Sabot::State::TAny::TWrapper(metadata.GetCore().NewState(metadata.GetArena(), state_alloc))),
      Id(&Suprena, Sabot::State::TAny::TWrapper(id.GetCore().NewState(id.GetArena(), state_alloc))) {
  try {
    for (auto iter : op_by_key) {
      new TEntry(this, iter.first, iter.second, state_alloc);
    }
  } catch (...) {
    EntryCollection.DeleteEachMember();
    throw;
  }
}

TUpdate::TUpdate(const TUpdate *that, void *state_alloc)
    : EntryCollection(this),
      MemoryLayerMembership(this, that->GetSequenceNumber()),
      Metadata(&Suprena, Sabot::State::TAny::TWrapper(that->Metadata.NewState(&that->Suprena, state_alloc))),
      Id(&Suprena, Sabot::State::TAny::TWrapper(that->Id.NewState(&that->Suprena, state_alloc))),
      PersistenceNotification(that->PersistenceNotification) {
  try {
    for (TEntryCollection::TCursor csr(&that->EntryCollection, InvCon::TOrient::Fwd /*Rev*/); csr; ++csr) {
      new TEntry(this, csr->GetIndexKey(), TKey(csr->GetOp(), static_cast<TCore::TArena *>(&that->Suprena)), state_alloc);
    }
  } catch (...) {
    EntryCollection.DeleteEachMember();
  }
}

/* Shallow Copy! */
TUpdate::TUpdate(void *state_alloc, const TUpdate *that)
    : EntryCollection(this),
      MemoryLayerMembership(this, that->GetSequenceNumber()),
      Metadata(&Suprena, Sabot::State::TAny::TWrapper(that->Metadata.NewState(&that->Suprena, state_alloc))),
      Id(&Suprena, Sabot::State::TAny::TWrapper(that->Id.NewState(&that->Suprena, state_alloc))),
      PersistenceNotification(that->PersistenceNotification) {}

TUpdate::TUpdate(TSequenceNumber seq_num)
    : EntryCollection(this),
      MemoryLayerMembership(this, seq_num) {}

TUpdate::~TUpdate() {
  assert(this);
  EntryCollection.DeleteEachMember();
}