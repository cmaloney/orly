/* <stig/indy/transaction_base.cc> 

   Implements <stig/indy/transaction_base.h>.

   Copyright 2010-2014 Tagged
   
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

#include <iostream> /* TODO GET RID OF */

#include <base/debug_log.h>

using namespace std;
using namespace Base;
using namespace Stig::Atom;
using namespace Stig::Indy::L1;

bool TTransaction::Push(const L0::TManager::TPtr<TRepo> &repo, const shared_ptr<TUpdate> &update, const Base::TOpt<TSequenceNumber> &ensure_or_discard) {
  assert(this);
  Prepared = false;
  EnsureOrDiscard = EnsureOrDiscard || ensure_or_discard;
  TMutation *mutation = MutationCollection.TryGetFirstMember(repo->GetId());
  if (mutation) {
    assert(false);  // Cannot attach a Pusher to a repo with an existing mutation
    throw std::runtime_error("Cannot attach a Pusher to a repo with an existing mutation.");
  }
  
  assert (!ensure_or_discard || (repo->GetNextSequenceNumber() >= *ensure_or_discard));
  if (!ensure_or_discard || (repo->GetNextSequenceNumber() == *ensure_or_discard)) {
    new TPusher(this, repo, update);
    return true;
  } else if (ensure_or_discard && repo->GetNextSequenceNumber() < *ensure_or_discard) {
    syslog(LOG_ERR, "MAJOR ERROR: missing data! ensure_or_discard =[%ld] vs. GetNextSequenceNumber =[%ld]", *ensure_or_discard, repo->GetNextSequenceNumber());
    throw std::logic_error("Let's check what we're doing here in transaction::push()");
  } else if (ensure_or_discard) {
    std::cout << "Discarding Push [" << *ensure_or_discard << "]" << std::endl;
  }
  return false;
}

bool TTransaction::Pop(const L0::TManager::TPtr<TRepo> &repo, const Base::TOpt<TSequenceNumber> &ensure_or_discard) {
  assert(this);
  Prepared = false;
  EnsureOrDiscard = EnsureOrDiscard || ensure_or_discard;
  TMutation *mutation = MutationCollection.TryGetFirstMember(repo->GetId());
  if (!mutation) {
    assert (!ensure_or_discard || (!repo->GetSequenceNumberStart() || *(repo->GetSequenceNumberStart()) >= *ensure_or_discard));
    if (!ensure_or_discard || (repo->GetSequenceNumberStart() && *(repo->GetSequenceNumberStart()) == *ensure_or_discard)) {
      new TPopper(this, repo, TPopper::Pop);
      return true;
    } else if (ensure_or_discard) {
      std::cout << "Discarding POP [" << *ensure_or_discard << "]" << std::endl;
    }
  } else {
    switch (mutation->GetKind()) {
      case TMutation::Pusher : {
        assert(false);  // Cannot attach a Popper and Pusher to the same repo
        throw std::runtime_error("Cannot attach a Popper and Pusher to the same repo.");
        break;
      };
      case TMutation::Popper : {
        /* We've already got a popper, make sure it's in the right state. */
        TPopper &popper = *dynamic_cast<TPopper *>(mutation);
        switch (popper.GetState()) {
          case TPopper::Peek : {
            assert (!ensure_or_discard || (!repo->GetSequenceNumberStart() || *(repo->GetSequenceNumberStart()) >= *ensure_or_discard));
            if (!ensure_or_discard || (repo->GetSequenceNumberStart() && *(repo->GetSequenceNumberStart()) == *ensure_or_discard)) {
              popper.SetState(TPopper::Pop);
              return true;
            }
            break;
          }
          case TPopper::Pop : {
            assert (!ensure_or_discard || (!repo->GetSequenceNumberStart() || *(repo->GetSequenceNumberStart()) >= *ensure_or_discard));
            if (!ensure_or_discard || (repo->GetSequenceNumberStart() && *(repo->GetSequenceNumberStart()) == *ensure_or_discard)) {
              return true;
            } else {
              throw std::runtime_error("TODO: check behavior");
              delete &popper;
            }
            break;
          }
          case TPopper::Fail : {
            assert (!ensure_or_discard || (!repo->GetSequenceNumberStart() || *(repo->GetSequenceNumberStart()) >= *ensure_or_discard));
            if (!ensure_or_discard || (repo->GetSequenceNumberStart() && *(repo->GetSequenceNumberStart()) == *ensure_or_discard)) {
              popper.SetState(TPopper::Pop);
              return true;
            } else {
              throw std::runtime_error("TODO: check behavior");
              delete &popper;
            }
            break;
          }
        }
        break;
      };
      case TMutation::StatusChanger : {
        assert(false);  // Cannot attach a Popper and StatusChanger to the same repo
        throw std::runtime_error("Cannot attach a Popper and StatusChanger to the same repo.");
        break;
      };
    }
  }
  return false;
}

bool TTransaction::Fail(const L0::TManager::TPtr<TRepo> &repo, const Base::TOpt<TSequenceNumber> &follow_or_discard) {
  assert(this);
  Prepared = false;
  EnsureOrDiscard = EnsureOrDiscard || follow_or_discard;
  TMutation *mutation = MutationCollection.TryGetFirstMember(repo->GetId());
  if (!mutation) {
    assert (!follow_or_discard || (*(repo->GetSequenceNumberStart()) >= *follow_or_discard));
    if (!follow_or_discard || (*(repo->GetSequenceNumberStart()) == *follow_or_discard)) {
      new TPopper(this, repo, TPopper::Fail);
      return true;
    }
  } else {
    switch (mutation->GetKind()) {
      case TMutation::Pusher : {
        assert(false);  // Cannot attach a Popper and Pusher to the same repo
        throw std::runtime_error("Cannot attach a Popper and Pusher to the same repo.");
        break;
      };
      case TMutation::Popper : {
        /* We've already got a popper, make sure it's in the right state. */
        TPopper &popper = *dynamic_cast<TPopper *>(mutation);
        switch (popper.GetState()) {
          case TPopper::Peek : {
            assert (!follow_or_discard || (*(repo->GetSequenceNumberStart()) >= *follow_or_discard));
            if (!follow_or_discard || (*(repo->GetSequenceNumberStart()) == *follow_or_discard)) {
              popper.SetState(TPopper::Fail);
              return true;
            } else {
              throw std::runtime_error("TODO: check behavior");
              delete &popper;
            }
            break;
          }
          case TPopper::Pop : {
            assert (!follow_or_discard || (*(repo->GetSequenceNumberStart()) >= *follow_or_discard));
            if ((!follow_or_discard || (*(repo->GetSequenceNumberStart()) == *follow_or_discard))) {
              popper.SetState(TPopper::Fail);
              return true;
            } else {
              throw std::runtime_error("TODO: check behavior");
              delete &popper;
            }
            break;
          }
          case TPopper::Fail : {
            break;
          }
        }
        break;
      };
      case TMutation::StatusChanger : {
        assert(false);  // Cannot attach a Popper and StatusChanger to the same repo
        throw std::runtime_error("Cannot attach a Popper and StatusChanger to the same repo.");
        break;
      };
    }
  }
  return false;
}

bool TTransaction::Pause(const L0::TManager::TPtr<TRepo> &repo, const Base::TOpt<TSequenceNumber> &follow_or_discard) {
  assert(this);
  Prepared = false;
  EnsureOrDiscard = EnsureOrDiscard || follow_or_discard;
  TMutation *mutation = MutationCollection.TryGetFirstMember(repo->GetId());
  if (mutation) {
    assert(false);  // Cannot attach a StatusChanger to a repo with an existing mutation
    throw std::runtime_error("Cannot attach a StatusChanger to a repo with an existing mutation.");
  }
  assert (!follow_or_discard || (*(repo->GetSequenceNumberStart()) >= *follow_or_discard));
  if ((!follow_or_discard || (*(repo->GetSequenceNumberStart()) == *follow_or_discard))) {
    new TStatusChanger(this, repo, Paused);
    return true;
  }
  return false;
}

bool TTransaction::UnPause(const L0::TManager::TPtr<TRepo> &repo, const Base::TOpt<TSequenceNumber> &follow_or_discard) {
  assert(this);
  Prepared = false;
  EnsureOrDiscard = EnsureOrDiscard || follow_or_discard;
  TMutation *mutation = MutationCollection.TryGetFirstMember(repo->GetId());
  if (mutation) {
    assert(false);  // Cannot attach a StatusChanger to a repo with an existing mutation
    throw std::runtime_error("Cannot attach a StatusChanger to a repo with an existing mutation.");
  }
  assert (!follow_or_discard || (*(repo->GetSequenceNumberStart()) >= *follow_or_discard));
  if ((!follow_or_discard || (*(repo->GetSequenceNumberStart()) == *follow_or_discard))) {
    new TStatusChanger(this, repo, Normal);
    return false;
  }
  return false;
}

const std::shared_ptr<Stig::Indy::TUpdate> &TTransaction::Peek(const L0::TManager::TPtr<TRepo> &repo) {
  assert(this);
  TMutation *mutation = MutationCollection.TryGetFirstMember(repo->GetId());
  TPopper *popper = nullptr;
  if (mutation) {
    switch (mutation->GetKind()) {
      case TMutation::Pusher : {
        assert(false);  // Cannot attach a Popper and Pusher to the same repo
        throw std::runtime_error("Cannot attach a Popper and Pusher to the same repo.");
        break;
      };
      case TMutation::Popper : {
        /* We've already got a popper, and peek is not an end state. */
        popper = dynamic_cast<TPopper *>(mutation);
        break;
      };
      case TMutation::StatusChanger : {
        assert(false);  // Cannot attach a Popper and StatusChanger to the same repo
        throw std::runtime_error("Cannot attach a Popper and StatusChanger to the same repo.");
        break;
      };
    }
  } else {
    popper = new TPopper(this, repo, TPopper::Peek);
  }
  popper->DoPeek();
  const auto &ret = popper->GetUpdate();
  return ret;
}

void TTransaction::Prepare() {
  assert(this);
  if (!TransactionCompletion) {
    /* acquire TransactionCompletion lock */ {
      std::lock_guard<std::mutex> lock(Manager->TransactionCompletionLock);
      TransactionCompletion = new TTransactionCompletion(Manager);
    }
  }
  Replica.Reset();
  size_t num_push = 0UL;
  std::vector<std::pair<Base::TUuid, TSequenceNumber>> release_set;
  for (TMutationCollection::TCursor csr(&MutationCollection); csr; ++csr) {
    switch (csr->GetKind()) {
      case TMutation::Pusher : {
        const TPusher *pusher = dynamic_cast<TPusher *>(&*csr);
        pusher->MyMutation = Replica.Push(pusher->GetRepoId(), pusher->Update);

        if (pusher->Repo->IsSafeRepo()) {
          ++num_push;
          TTransactionCompletion *transaction_completion = TransactionCompletion;
          auto func = [transaction_completion](TUpdate::TPersistenceNotification::TResult result) {
            switch (result) {
              case TUpdate::TPersistenceNotification::Completed: {
                transaction_completion->RegisterCompletion();
                break;
              }
              case TUpdate::TPersistenceNotification::Failed: {
                transaction_completion->RegisterFailure();
                break;
              }
            }
          };
          pusher->Update->SetPersistenceNotification(func);
        }


        break;
      }
      case TMutation::Popper : {
        const TPopper *popper = dynamic_cast<TPopper *>(&*csr);
        switch (popper->State) {
          case TPopper::Peek : {
            /* Do nothing */
            break;
          }
          case TPopper::Pop : {
            assert(popper->Repo->GetSequenceNumberStart());
            popper->MyMutation = Replica.Pop(popper->GetRepoId(), popper->Repo->GetSequenceNumberStart());
            assert(popper->MyMutation->GetSequenceNumber());
            release_set.push_back(make_pair(popper->GetRepoId(), *(popper->MyMutation->GetSequenceNumber())));
            break;
          }
          case TPopper::Fail : {
            popper->MyMutation = Replica.Fail(popper->GetRepoId(), popper->Repo->GetSequenceNumberStart());
            break;
          }
        }
        break;
      }
      case TMutation::StatusChanger : {
        const TStatusChanger *status_changer = dynamic_cast<TStatusChanger *>(&*csr);
        switch (status_changer->Status) {
          case Normal : {
            status_changer->MyMutation = Replica.UnPause(status_changer->GetRepoId(), status_changer->Repo->GetSequenceNumberStart());
            break;
          }
          case Paused : {
            status_changer->MyMutation = Replica.Pause(status_changer->GetRepoId(), status_changer->Repo->GetSequenceNumberStart());
            break;
          }
          case Failed : {
            assert(false); /* a status changer should not be failing a repo */
            throw;
            break;
          }
        }
        break;
      }
    }
  }
  TManager *my_manager = Manager;
  bool ensure_or_discard = EnsureOrDiscard;
  std::shared_ptr<std::function<void (TTransactionCompletion::TTransactionResult)>> func = make_shared<std::function<void (TTransactionCompletion::TTransactionResult)>>([release_set, my_manager, ensure_or_discard](TTransactionCompletion::TTransactionResult result) {
    switch (result) {
      case TTransactionCompletion::Completed: {
        for (const auto &entry : release_set) {
          auto base_repo = my_manager->ForceOpenRepo(entry.first);
          TRepo *repo = dynamic_cast<TRepo *>(base_repo.Get());
          assert(repo);
          repo->ReleaseUpdate(entry.second, ensure_or_discard);
        }
        break;
      }
      case TTransactionCompletion::Failed: {
        syslog(LOG_ERR, "Transaction failed");
        break;
      }
    }
  });
  TransactionCompletion->SetCb(func);
  TransactionCompletion->SetWaitFor(num_push);
  Prepared = true;
}

TTransaction::TTransactionCompletion::TTransactionCompletion(TManager *manager)
    : WaitFor(0U), NumCompleted(0U), ManagerMembership(this, &manager->TransactionCompletionCollection) {}

TTransaction::TTransactionCompletion::~TTransactionCompletion() {
  assert(this);
  assert(ManagerMembership.TryGetCollector());
  /* acquire TransactionCompletion lock */ {
    std::lock_guard<std::mutex> lock(ManagerMembership.TryGetCollector()->TransactionCompletionLock);
    ManagerMembership.Remove();
  }  // release TransactionCompletion lock
}

/* TODO */
void TTransaction::TTransactionCompletion::SetWaitFor(size_t wait_for) {
  assert(this);
  WaitFor = wait_for;
  if (WaitFor == 0) {
    assert(Cb);
    (*Cb)(Completed);
    delete this;
  }
}

void TTransaction::TTransactionCompletion::RegisterCompletion() {
  assert(this);
  bool del = false;
  /* lock life-span */ {
    std::lock_guard<std::mutex> lock(Mutex);
    ++NumCompleted;
    if (NumCompleted == WaitFor) {
      (*Cb)(Completed);
      del = true;
    }
  }
  if (del) {
    delete this;
  }
}

void TTransaction::TTransactionCompletion::RegisterFailure() {
  assert(this);
  /* lock life-span */ {
    std::lock_guard<std::mutex> lock(Mutex);
    (*Cb)(Failed);
  }
  delete this;
}

TTransaction::TTransaction(TManager *manager, bool should_replicate)
    : Manager(manager),
      ManagerMembership(this),
      MutationCollection(this),
      CommitFlag(false),
      ShouldReplicate(should_replicate),
      Prepared(false),
      EnsureOrDiscard(false),
      TransactionReplication(Manager->NewTransactionReplication()),
      TransactionCompletion(nullptr) {}

TTransaction::~TTransaction() NO_THROW {
  assert(this);
  assert(!CommitFlag || Prepared);

  /* hold replication queue lock */ {
    std::lock_guard<std::mutex> lock(Manager->GetReplicationQueueLock());

    /* first delete the pushers */
    for(TMutation *mutation = MutationCollection.TryGetFirstMember(); mutation;) {
      TMutation *my_mutation = mutation;
      mutation = mutation->TryGetNextMember();
      if (my_mutation->GetKind() == TMutation::Pusher) {
        delete my_mutation;
      }
    }

    /* now delete the rest */
    for(TMutation *mutation = MutationCollection.TryGetFirstMember(); mutation; mutation = MutationCollection.TryGetFirstMember()) {
      delete mutation;
    }

    if (CommitFlag) {
      if (ShouldReplicate) {
        Manager->Enqueue(TransactionReplication, std::move(Replica));
      } else {
        Manager->DeleteTransactionReplication(TransactionReplication);
      }
    } else {
      delete TransactionCompletion;
      Manager->DeleteTransactionReplication(TransactionReplication);
    }
  }
}

TTransaction::TReplica::TReplica(TReplica &&that)
    : MutationList(std::move(that.MutationList)) {}

TTransaction::TReplica::TReplica(const TReplica &that)
    : MutationList(that.MutationList) {}

TTransaction::TReplica::TReplica() {}

TTransaction::TReplica::~TReplica() {}

TTransaction::TReplica &TTransaction::TReplica::operator=(TReplica &&that) {
  assert(this);
  assert(&that);
  std::swap(MutationList, that.MutationList);
  return *this;
}

TTransaction::TReplica &TTransaction::TReplica::operator=(const TReplica &that) {
  assert(this);
  return *this = TReplica(that);
}

TTransaction::TReplica::TMutation::TUpdate::TUpdate()
    : Suprena(nullptr) {}

TTransaction::TReplica::TMutation::TUpdate::TUpdate(TUpdate &&that)
    : Suprena(std::move(that.Suprena)),
      Metadata(that.Metadata),
      Id(that.Id),
      OpByKey(std::move(that.OpByKey)) {}

TTransaction::TReplica::TMutation::TUpdate::TUpdate(const TUpdate &that)
    : Suprena(make_shared<TSuprena>()) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Metadata = TCore(Suprena.get(), state_alloc, that.Suprena.get(), that.Metadata);
  Id = TCore(Suprena.get(), state_alloc, that.Suprena.get(), that.Id);
  for (const auto &op_by_key : that.OpByKey) {
    OpByKey.push_back(make_pair(TIndexKey(op_by_key.first.GetIndexId(), TKey(TCore(Suprena.get(), state_alloc, that.Suprena.get(), op_by_key.first.GetKey().GetCore()), Suprena.get())),
                                TCore(Suprena.get(), state_alloc, that.Suprena.get(), op_by_key.second)));
  }
}

TTransaction::TReplica::TMutation::TUpdate::TUpdate(const Stig::Indy::TUpdate *that)
    : Suprena(make_shared<TSuprena>()) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Metadata = TCore(Suprena.get(), state_alloc, &that->GetSuprena(), that->GetMetadata());
  Id = TCore(Suprena.get(), state_alloc, &that->GetSuprena(), that->GetId());
  for (Stig::Indy::TUpdate::TEntryCollection::TCursor csr(that->GetEntryCollection()); csr; ++csr) {
    OpByKey.push_back(make_pair(TIndexKey(csr->GetIndexKey().GetIndexId(), TKey(TCore(Suprena.get(), state_alloc, &csr->GetSuprena(), csr->GetKey().GetCore()), Suprena.get())),
                                TCore(Suprena.get(), state_alloc, &csr->GetSuprena(), csr->GetOp())));
  }
}

TTransaction::TReplica::TMutation::TUpdate::~TUpdate() {}

TTransaction::TReplica::TMutation::TUpdate &TTransaction::TReplica::TMutation::TUpdate::operator=(TUpdate &&that) {
  assert(this);
  assert(&that);
  std::swap(Suprena, that.Suprena);
  Metadata = that.Metadata;
  Id = that.Id;
  std::swap(OpByKey, that.OpByKey);
  return *this;
}

TTransaction::TReplica::TMutation::TUpdate &TTransaction::TReplica::TMutation::TUpdate::operator=(const TUpdate &that) {
  assert(this);
  return *this = TUpdate(that);
}

void TTransaction::TReplica::TMutation::TUpdate::Write(Io::TBinaryOutputStream &strm, const Atom::TCore::TRemap &remap) const {
  assert(this);
  strm << TCore(Metadata, remap)  // (Atom::TCore) Metadata
      << TCore(Id, remap)  // (Atom::TCore) Id
      << OpByKey.size();  // (size_t) num pairs of key = op
  //cout << "Streaming [" << TKey(Metadata, Suprena.get()) << "][" << TKey(Id, Suprena.get()) << "][" << OpByKey.size() << "]" << endl;
  for (const auto &op_by_key : OpByKey) {
    const auto &key = op_by_key.first;
    const auto &op = op_by_key.second;
    strm << op_by_key.first.GetIndexId()   // (Base::TUuid) index_id
        << TCore(key.GetKey().GetCore(), remap)  // (Atom::TCore) key
        << TCore(op, remap);  // (Atom::TCore) op
  }
}

void TTransaction::TReplica::TMutation::TUpdate::Read(Io::TBinaryInputStream &strm, const Atom::TCore::TRemap &remap) {
  assert(this);
  size_t num_op_by_key;
  Base::TUuid temp_index_id;
  Atom::TCore temp_key;
  Atom::TCore temp_op;
  strm >> Metadata  // (Atom::TCore) Metadata
      >> Id  // (Atom::TCore) Id
      >> num_op_by_key;  // (size_t) num pairs of key = op
  Metadata.Remap(remap);
  Id.Remap(remap);
  //cout << "Reading [" << TKey(Metadata, Suprena.get()) << "][" << TKey(Id, Suprena.get()) << "][" << num_op_by_key << "]" << endl;
  try {
    OpByKey.reserve(num_op_by_key);
  } catch (const exception &ex) {
    DEBUG_LOG("Exception in TTransaction::TReplica::TMutation::TUpdate::Read [%s] trying to reserve [%ld]", ex.what(), num_op_by_key);
    throw;
  }
  for (size_t i = 0; i < num_op_by_key; ++i) {
    strm >> temp_index_id  // (Base::TUuid) index_id
        >> temp_key  // (Atom::TCore) key
        >> temp_op;  // (Atom::TCore) op
    temp_key.Remap(remap);
    temp_op.Remap(remap);
    OpByKey.push_back(make_pair(TIndexKey(temp_index_id, TKey(temp_key, Suprena.get())), temp_op));
  }
}

TTransaction::TReplica::TMutation::TMutation(TMutation &&that)
    : Kind(that.Kind),
      RepoId(std::move(that.RepoId)),
      Update(std::move(that.Update)),
      SequenceNumber(that.SequenceNumber) {}

TTransaction::TReplica::TMutation::TMutation(const TMutation &that)
    : Kind(that.Kind),
      RepoId(that.RepoId),
      Update(that.Update),
      SequenceNumber(that.SequenceNumber) {}

TTransaction::TReplica::TMutation::TMutation(TKind kind, const Base::TUuid &repo_id, const Base::TOpt<TSequenceNumber> &seq_num)
    : Kind(kind),
      RepoId(repo_id),
      SequenceNumber(seq_num) {
  assert(Kind != Pusher);
}

TTransaction::TReplica::TMutation::TMutation(TKind kind, const Base::TUuid &repo_id, const Stig::Indy::TUpdate *update, const Base::TOpt<TSequenceNumber> &seq_num)
    : Kind(kind),
      RepoId(repo_id),
      Update(update),
      SequenceNumber(seq_num) {
  assert(Kind == Pusher);
}

TTransaction::TReplica::TMutation::TMutation(TKind kind, const Base::TUuid &repo_id, TUpdate &&update, const Base::TOpt<TSequenceNumber> &seq_num)
    : Kind(kind),
      RepoId(repo_id),
      Update(std::move(update)),
      SequenceNumber(seq_num) {
  assert(Kind == Pusher);
}

TTransaction::TReplica::TMutation::~TMutation() {}

TTransaction::TReplica::TMutation &TTransaction::TReplica::TMutation::operator=(TMutation &&that) {
  assert(this);
  assert(&that);
  std::swap(Kind, that.Kind);
  std::swap(RepoId, that.RepoId);
  std::swap(Update, that.Update);
  std::swap(SequenceNumber, that.SequenceNumber);
  return *this;
}

TTransaction::TReplica::TMutation &TTransaction::TReplica::TMutation::operator=(const TMutation &that) {
  assert(this);
  return *this = TMutation(that);
}

TTransaction::TReplica::TMutation *TTransaction::TReplica::Push(const Base::TUuid &repo_id, const TUpdate *update) {
  assert(this);
  MutationList.emplace_front(TMutation::Pusher, repo_id, update, Base::TOpt<TSequenceNumber>());
  return &MutationList.front();
}

TTransaction::TReplica::TMutation *TTransaction::TReplica::Pop(const Base::TUuid &repo_id, const Base::TOpt<TSequenceNumber> &seq_num) {
  assert(this);
  MutationList.emplace_front(TMutation::Popper, repo_id, seq_num);
  return &MutationList.front();
}

TTransaction::TReplica::TMutation *TTransaction::TReplica::Fail(const Base::TUuid &repo_id, const Base::TOpt<TSequenceNumber> &seq_num) {
  assert(this);
  MutationList.emplace_front(TMutation::Failer, repo_id, seq_num);
  return &MutationList.front();
}

TTransaction::TReplica::TMutation *TTransaction::TReplica::Pause(const Base::TUuid &repo_id, const Base::TOpt<TSequenceNumber> &seq_num) {
  assert(this);
  MutationList.emplace_front(TMutation::Pauser, repo_id, seq_num);
  return &MutationList.front();
}

TTransaction::TReplica::TMutation *TTransaction::TReplica::UnPause(const Base::TUuid &repo_id, const Base::TOpt<TSequenceNumber> &seq_num) {
  assert(this);
  MutationList.emplace_front(TMutation::UnPauser, repo_id, seq_num);
  return &MutationList.front();
}

void TTransaction::TReplica::Reset() {
  assert(this);
  MutationList.clear();
}

TTransaction::TMutation::TMutation(TTransaction *transaction, const L0::TManager::TPtr<TRepo> &repo)
    : TransactionMembership(this, repo->GetId(), &transaction->MutationCollection),
      Repo(repo),
      MyMutation(nullptr) {}

TTransaction::TMutation::~TMutation() NO_THROW {}

TTransaction::TPusher::TPusher(TTransaction *transaction, const L0::TManager::TPtr<TRepo> &repo, const std::shared_ptr<TUpdate> &update)
    : TMutation(transaction, repo) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Update = new TUpdate(update.get(), state_alloc);
}

TTransaction::TPusher::~TPusher() NO_THROW {
  assert(this);
  assert(TransactionMembership.TryGetCollector());
  if (TransactionMembership.TryGetCollector()->GetCommitFlag()) {
    assert(MyMutation);
    MyMutation->SetSequenceNumber(Repo->AppendUpdate(Update, MyMutation->GetNextUpdate()));
  } else {
    delete Update;
  }
}

TTransaction::TPopper::TPopper(TTransaction *transaction, const L0::TManager::TPtr<TRepo> &repo, TState state)
    : TMutation(transaction, repo),
      State(state) {}

TTransaction::TPopper::~TPopper() NO_THROW {
  assert(this);
  assert(TransactionMembership.TryGetCollector());
  if (TransactionMembership.TryGetCollector()->GetCommitFlag()) {
    switch(State) {
      case Peek : {
        break;
      };
      case Pop : {
        assert(MyMutation);
        MyMutation->SetSequenceNumber(Repo->PopLowest(MyMutation->GetNextUpdate()));
        break;
      };
      case Fail : {
        assert(MyMutation);
        MyMutation->SetSequenceNumber(Repo->ChangeStatus(Failed, MyMutation->GetNextUpdate()));
        break;
      };
    }
  }
}

void TTransaction::TPopper::DoPeek() const {
  assert(this);
  if (!View) {
    View = make_shared<TRepo::TView>(Repo);
    Update = Repo->GetLowestUpdate();
  }
}

TTransaction::TStatusChanger::TStatusChanger(TTransaction *transaction, const L0::TManager::TPtr<TRepo> &repo, TStatus status)
    : TMutation(transaction, repo), Status(status) {}

TTransaction::TStatusChanger::~TStatusChanger() NO_THROW {
  assert(this);
  assert(TransactionMembership.TryGetCollector());
  if (TransactionMembership.TryGetCollector()->GetCommitFlag()) {
    switch (Status) {
      case Normal : {
        assert(MyMutation);
        MyMutation->SetSequenceNumber(Repo->ChangeStatus(Status, MyMutation->GetNextUpdate()));
        break;
      }
      case Paused : {
        assert(MyMutation);
        MyMutation->SetSequenceNumber(Repo->ChangeStatus(Status, MyMutation->GetNextUpdate()));
        break;
      }
      case Failed : {
        assert(false);  // A status changer should never be failing a repo. Only tetris should fail repo's through a popper
        break;
      }
    }
  }
}

unique_ptr<TTransaction, std::function<void (TTransaction *)>> TManager::NewTransaction(bool should_replicate) {
  TTransaction *transaction = new TTransaction(this, should_replicate);
  /* no-throws */
  /* acquire transaction lock */ {
    std::lock_guard<std::mutex> lock(TransactionLock);
    TransactionCollection.Insert(&transaction->ManagerMembership);
  }  // release transaction lock
  return std::unique_ptr<TTransaction, std::function<void (TTransaction *)>>(transaction, OnCloseTransactionCb);
}

TManager::TManager(Disk::Util::TEngine *engine,
                   size_t merge_mem_delay,
                   size_t merge_disk_delay,
                   bool allow_tailing,
                   bool no_realtime,
                   size_t layer_cleaning_interval_milliseconds,
                   Base::TScheduler *scheduler,
                   size_t block_slots_available_per_merger,
                   size_t max_repo_cache_size,
                   size_t walker_local_cache_size,
                   size_t temp_file_consol_thresh,
                   const std::vector<size_t> &merge_mem_cores,
                   const std::vector<size_t> &merge_disk_cores,
                   bool create_new)
    : L0::TManager(engine,
                   merge_mem_delay,
                   merge_disk_delay,
                   allow_tailing,
                   no_realtime,
                   layer_cleaning_interval_milliseconds,
                   scheduler,
                   block_slots_available_per_merger,
                   max_repo_cache_size,
                   walker_local_cache_size,
                   temp_file_consol_thresh,
                   merge_mem_cores,
                   merge_disk_cores,
                   create_new),
      TransactionCollection(this),
      TransactionCompletionCollection(this),
      OnCloseTransactionCb(std::bind(&TManager::OnCloseTransaction, this, std::placeholders::_1)) {}

TManager::~TManager() {}

void TManager::OnCloseTransaction(TTransaction *transaction) {
  assert(this);
  /* acquire transaction lock */ {
    std::lock_guard<std::mutex> lock(TransactionLock);
    transaction->ManagerMembership.Remove();
  }  // release transaction lock
  delete transaction;
}
