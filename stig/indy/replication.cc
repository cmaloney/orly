/* <stig/indy/replication.cc>

   Implements <stig/indy/replication.h>.

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

#include <stig/indy/replication.h>

#include <base/debug_log.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::Atom;
using namespace Stig::Indy;
using namespace Stig::Indy::Util;

TReplicationQueue::TReplicationItem::TReplicationItem()
    : QueueMembership(this) {}

TReplicationQueue::TReplicationItem::~TReplicationItem() {}

TReplicationQueue::TReplicationQueue()
    : ItemCollection(this) {}

TReplicationQueue::~TReplicationQueue() {
  assert(this);
  ItemCollection.DeleteEachMember();
}

void TReplicationQueue::Swap(TReplicationQueue &that) {
  assert(this);
  assert(&that);
  for (auto item = that.ItemCollection.TryGetFirstMember(); item; item = that.ItemCollection.TryGetFirstMember()) {
    item->QueueMembership.Remove();
    ItemCollection.Insert(&item->QueueMembership);
  }
}

TRepoReplication::TRepoReplication(const Base::TUuid &repo_id, bool is_safe, const TTtl &ttl, const Base::TOpt<Base::TUuid> &opt_parent_repo_id)
    : Ttl(ttl),
      RepoId(repo_id),
      IsSafe(is_safe),
      OptParentRepoId(opt_parent_repo_id) {}

TRepoReplication::~TRepoReplication() {}

TDurableReplication::TDurableReplication(const Base::TUuid &durable_id, const TTtl &ttl, const std::string &serialized_obj)
    : DurableTtl(ttl),
      DurableId(durable_id),
      SerializedObj(serialized_obj) {}

TDurableReplication::~TDurableReplication() {}

TIndexIdReplication::TIndexIdReplication(const Base::TUuid &id, const Indy::TKey &key, const Indy::TKey &val)
    : Id(id),
      Key(&Suprena, alloca(Sabot::State::GetMaxStateSize()), key),
      Val(&Suprena, alloca(Sabot::State::GetMaxStateSize()), val) {}

TIndexIdReplication::~TIndexIdReplication() {}

TTransactionReplication::TTransactionReplication() {}

TTransactionReplication::TTransactionReplication(L1::TTransaction::TReplica &&replica)
    : Replica(std::move(replica)) {}

TTransactionReplication::~TTransactionReplication() {}

TReplicationStreamer::TReplicationStreamer() {}

TReplicationStreamer::~TReplicationStreamer() {}

void TReplicationStreamer::Write(Io::TBinaryOutputStream &strm) const {
  assert(this);
  IndexIdBuilder.Write(strm);
  RepoBuilder.Write(strm);
  DurableBuilder.Write(strm);
  TransactionBuilder.Write(strm);
}

void TReplicationStreamer::Read(Io::TBinaryInputStream &strm) {
  assert(this);
  assert(!IndexIdVector);
  assert(!RepoVector);
  assert(!DurableVector);
  assert(!TransactionVector);
  IndexIdVector = std::unique_ptr<TCoreVector>(new TCoreVector(strm));
  RepoVector = std::unique_ptr<TCoreVector>(new TCoreVector(strm));
  DurableVector = std::unique_ptr<TCoreVector>(new TCoreVector(strm));
  TransactionVector = std::unique_ptr<TCoreVector>(new TCoreVector(strm));
}

void TReplicationStreamer::PushIndexId(const TIndexIdReplication &index_id) {
  assert(this);
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  IndexIdBuilder.Push(index_id.GetId());
  IndexIdBuilder.PushState(index_id.GetKey().GetState(state_alloc));
  IndexIdBuilder.PushState(index_id.GetVal().GetState(state_alloc));
}

void TReplicationStreamer::PushTransaction(const L1::TTransaction::TReplica &replica) {
  assert(this);
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  TransactionBuilder.Push(replica.GetMutationList().size()); /* number of mutations in this transaction. */
  for (const auto &mutation : replica.GetMutationList()) {
    assert(mutation.GetSequenceNumber());
    TSequenceNumber mutation_seq_num = *mutation.GetSequenceNumber();
    switch (mutation.GetKind()) {
      case L1::TTransaction::TReplica::TMutation::Pusher : {
        //std::cout << "\t\tPush[" << mutation.GetRepoId() << "]\t[" << mutation_seq_num << "]" << std::endl;
        const auto &cur_update = mutation.GetUpdate();
        TransactionBuilder.Push(static_cast<uint32_t>(Push)); /* kind of operation */
        TransactionBuilder.Push(mutation.GetRepoId()); /* repo id */
        TransactionBuilder.Push(mutation_seq_num); /* sequence number */
        TransactionBuilder.PushState(cur_update.GetMetadata().NewState(cur_update.GetSuprena().get(), state_alloc)); /* meta-data */
        TransactionBuilder.PushState(cur_update.GetId().NewState(cur_update.GetSuprena().get(), state_alloc)); /* id */
        TransactionBuilder.Push(cur_update.GetOpByKey().size()); /* number of index -> key -> val pairs */
        for (const auto &cur_op_by_key : cur_update.GetOpByKey()) {
          TransactionBuilder.Push(cur_op_by_key.first.GetIndexId()); /* index id */
          TransactionBuilder.PushState(cur_op_by_key.first.GetKey().GetCore().NewState(cur_update.GetSuprena().get(), state_alloc)); /* key */
          TransactionBuilder.PushState(cur_op_by_key.second.NewState(cur_update.GetSuprena().get(), state_alloc)); /* val */
        }
        break;
      }
      case L1::TTransaction::TReplica::TMutation::Popper : {
        //std::cout << "\t\tPop[" << mutation.GetRepoId() << "]\t[" << mutation_seq_num << "]" << std::endl;
        TransactionBuilder.Push(static_cast<uint32_t>(Pop)); /* kind of operation */
        TransactionBuilder.Push(mutation.GetRepoId()); /* repo id */
        TransactionBuilder.Push(mutation_seq_num); /* sequence number */
        break;
      }
      case L1::TTransaction::TReplica::TMutation::Failer : {
        TransactionBuilder.Push(static_cast<uint32_t>(Fail)); /* kind of operation */
        TransactionBuilder.Push(mutation.GetRepoId()); /* repo id */
        TransactionBuilder.Push(mutation_seq_num); /* sequence number */
        break;
      }
      case L1::TTransaction::TReplica::TMutation::Pauser : {
        TransactionBuilder.Push(static_cast<uint32_t>(Pause)); /* kind of operation */
        TransactionBuilder.Push(mutation.GetRepoId()); /* repo id */
        TransactionBuilder.Push(mutation_seq_num); /* sequence number */
        break;
      }
      case L1::TTransaction::TReplica::TMutation::UnPauser : {
        TransactionBuilder.Push(static_cast<uint32_t>(UnPause)); /* kind of operation */
        TransactionBuilder.Push(mutation.GetRepoId()); /* repo id */
        TransactionBuilder.Push(mutation_seq_num); /* sequence number */
        break;
      }
    }
  }
}

void TReplicationStreamer::PushDurable(const TDurableReplication &durable) {
  assert(this);
  DurableBuilder.Push(durable.GetId());
  DurableBuilder.Push(durable.GetTtl());
  DurableBuilder.Push(durable.GetSerializedObj());
}

void TReplicationStreamer::PushRepo(const TRepoReplication &repo) {
  assert(this);
  RepoBuilder.Push(repo.GetId());
  RepoBuilder.Push(Sabot::TStdDuration(repo.GetTtl()));
  RepoBuilder.Push(repo.GetIsSafe());
  RepoBuilder.Push(repo.GetOptParentRepoId());
}