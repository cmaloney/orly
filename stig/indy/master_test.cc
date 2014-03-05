/* <stig/indy/master_test.cc>

   Test for <stig/indy/manager.h>.

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

#include <stig/indy/manager.h>

#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

using namespace std;
using namespace Base;
using namespace Stig::Atom;
using namespace Stig::Indy;
using namespace Stig::Indy::Util;

const Stig::Indy::TMasterContext::TProtocol Stig::Indy::TMasterContext::TProtocol::Protocol;
const Stig::Indy::TSlaveContext::TProtocol Stig::Indy::TSlaveContext::TProtocol::Protocol;
static const char *MasterPath = "/dev/sdb";
static const char *MasterDeviceName = "sdb";
static const size_t BlockSize = 4096UL * 16;
static const size_t CacheSize = 10000UL;
static const size_t QueueDepth = 128UL;
static const size_t MergeMemDelay = 10UL;
static const size_t FlushMemDelay = 10UL;
static const size_t MergeDiskDelay = 10UL;
static const size_t ReplicationDelay = 100UL;

TPool TUpdate::Pool(sizeof(TUpdate), 1000000UL);
TPool TUpdate::TEntry::Pool(sizeof(TUpdate::TEntry), 2000000UL);
TPool Disk::TController::TEvent::Pool(max(sizeof(Disk::TController::TWriteV), sizeof(Disk::TController::TRead)), 10000);
Disk::TBufBlock::TPool Disk::TBufBlock::Pool(BlockSize, 20000);
TPool Disk::TCache::TObj::Pool(sizeof(Disk::TCache::TObj), 11000);
timespec Disk::TCache::ReadWait;

template <typename TVal>
void Check(const TVal &lhs, const TVal &rhs) {
  if (lhs != rhs) {
    cerr << "Comparison failed: [" << lhs << ", " << rhs << "]" << endl;
    throw std::runtime_error("Comparison failed");
  }
}

void StateChanged(TManager::TState) {}

int main(int argc, char *argv[]) {
  assert(argc == 2);
  int port_num = atoi(argv[1]);

  signal(SIGPIPE, [](int){});

  int64_t num_iter = 400000L;
  Base::TFd solo_sock;
  std::thread join_thread;
  auto wait_for_slave = [port_num, &join_thread](const std::shared_ptr<std::function<void (const TFd &)>> &cb) {
    std::cout << "Wait_for_slave()" << std::endl;
    if (join_thread.joinable()) {
      join_thread.join();
    }
    join_thread = std::thread([port_num, cb]() {
      Base::TFd serv_sock = socket(AF_INET, SOCK_STREAM, 0);
      int flag = true;
      Base::IfLt0(setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)));
      struct sockaddr_in addr;
      Base::Zero(addr);
      addr.sin_family = AF_INET;
      addr.sin_port = htons(port_num);
      Base::Zero(addr.sin_addr.s_addr);
      Base::IfLt0(::bind(serv_sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)));
      Base::IfLt0(listen(serv_sock, 5));
      struct sockaddr_in client_addr;
      socklen_t client_len = sizeof(client_addr);
      Base::TFd client_fd = accept(serv_sock, reinterpret_cast<struct sockaddr *>(&client_addr), &client_len);
      (*cb)(client_fd);
    });
  };
  unique_ptr<TManager> master(new TManager(MasterPath,
                                           MasterDeviceName,
                                           BlockSize,
                                           CacheSize,
                                           QueueDepth,
                                           MergeMemDelay,
                                           FlushMemDelay,
                                           MergeDiskDelay,
                                           ReplicationDelay,
                                           TManager::Solo,
                                           solo_sock,
                                           wait_for_slave,
                                           StateChanged));
  std::thread queuer([&master]() {
    master->RunReplicationQueue();
  });
  std::thread runner([&master]() {
    master->RunReplicationWork();
  });
  std::thread replicator([&master]() {
    master->RunReplicateTransaction();
  });
  std::thread mem_merger([&master]() {
    master->RunMergeMem();
  });
  std::thread mem_flusher([&master]() {
    master->RunFlushMem();
  });
  std::thread disk_merger([&master]() {
    master->RunMergeDisk();
  });
  TUuid repo_id("DDDDDDDD-DDDD-DDDD-DDDD-DDDDDDDDDDDD");
  auto repo = master->GetRepo(repo_id, true);
  for (int64_t i = 0; i < num_iter; ++i) {
    timespec wait {0, 100000};
    nanosleep(&wait, NULL);
    if (i % 1000 == 0) {
      std::cout << "[" << i << "]" << std::endl;
    }
    /* write num_iter updates to the repo */
    auto transaction = master->NewTransaction();
    TFastArena fast_arena;
    auto update = TUpdate::NewUpdate(TUpdate::TOpByKey{ { TAtom(std::tuple<TInt>(num_iter - 1L - i), &fast_arena), TAtom((num_iter - 1L - i) * 10L)} }, TAtom(), TAtom(Base::TUuid(TUuid::Best)));
    transaction->Push(repo, update);
    transaction->Prepare();
    transaction->CommitAction();
  }
  /* check that all the updates are there */ {
    TFastArena fast_arena;
    auto walker_ptr = repo->NewPresentWalker(TAtom(std::tuple<TInt>(0L), &fast_arena), TAtom(std::tuple<TInt>(num_iter), &fast_arena));
    int64_t count = 0L;
    for (auto &walker = *walker_ptr; walker; ++walker) {
      Check((*walker).SequenceNumber, static_cast<size_t>(num_iter) - count);
      Check((*walker).Key, TAtom(std::tuple<TInt>(count), &fast_arena));
      Check((*walker).Op, TAtom(count * 10L, &fast_arena));
      ++count;
    }
    Check(count, num_iter);
  }
  join_thread.join();
  sleep(100);
  mem_merger.join();
  mem_flusher.join();
  disk_merger.join();
  replicator.join();
  queuer.join();
  runner.join();
}