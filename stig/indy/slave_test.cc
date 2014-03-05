/* <stig/indy/slave_test.cc>

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
static const char *SlavePath = "/dev/sdc";
static const char *SlaveDeviceName = "sdc";
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
    //throw std::runtime_error("Comparison failed");
  }
}

void StateChanged(TManager::TState) {}

int main(int argc, char *argv[]) {
  assert(argc == 2);
  signal(SIGPIPE, [](int){});
  const char *host = "localhost";
  const char *port_num = argv[1];
  Base::TFd slave_sock = socket(AF_INET, SOCK_STREAM, 0);
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  Base::Zero(hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;
  Base::IfLt0(getaddrinfo(host, port_num, &hints, &result));
  try {
    for (rp = result; rp != NULL; rp = rp->ai_next) {
      int sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      if (sock == -1) {
        continue;
      }
      slave_sock = sock;
      Base::IfLt0(connect(slave_sock, rp->ai_addr, rp->ai_addrlen));
      break;
    }
  } catch (...) {
    freeaddrinfo(result);
    throw;
  }
  freeaddrinfo(result);

  std::thread join_thread;
  auto wait_for_slave = [port_num, &join_thread](const std::shared_ptr<std::function<void (const TFd &)>> &cb) {
    std::cout << "Wait_for_slave()" << std::endl;
    join_thread = std::thread([port_num, cb]() {
      Base::TFd serv_sock = socket(AF_INET, SOCK_STREAM, 0);
      int flag = true;
      Base::IfLt0(setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)));
      struct sockaddr_in addr;
      Base::Zero(addr);
      addr.sin_family = AF_INET;
      addr.sin_port = htons(atoi(port_num));
      Base::Zero(addr.sin_addr.s_addr);
      Base::IfLt0(::bind(serv_sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)));
      Base::IfLt0(listen(serv_sock, 5));
      struct sockaddr_in client_addr;
      socklen_t client_len = sizeof(client_addr);
      Base::TFd client_fd = accept(serv_sock, reinterpret_cast<struct sockaddr *>(&client_addr), &client_len);
      (*cb)(client_fd);
    });
  };
  unique_ptr<TManager> slave(new TManager(SlavePath ,
                                          SlaveDeviceName,
                                          BlockSize,
                                          CacheSize,
                                          QueueDepth,
                                          MergeMemDelay,
                                          FlushMemDelay,
                                          MergeDiskDelay,
                                          ReplicationDelay,
                                          TManager::SyncSlave,
                                          slave_sock,
                                          wait_for_slave,
                                          StateChanged));
  std::thread queuer([&slave]() {
    slave->RunReplicationQueue();
  });
  std::thread runner([&slave]() {
    slave->RunReplicationWork();
  });
  std::thread replicator([&slave]() {
    slave->RunReplicateTransaction();
  });
  std::thread mem_merger([&slave]() {
    slave->RunMergeMem();
  });
  std::thread mem_flusher([&slave]() {
    slave->RunFlushMem();
  });
  std::thread disk_merger([&slave]() {
    slave->RunMergeDisk();
  });
  //sleep(200);
  pause();
  TUuid repo_id("DDDDDDDD-DDDD-DDDD-DDDD-DDDDDDDDDDDD");
  auto repo = slave->GetRepo(repo_id, false);
  TInt num_iter = 400000L;
  /* check that all the updates are there */ {
    std::cout << "checking" << std::endl;
    for (TInt i = 0; i < num_iter; ++i) {
      TFastArena fast_arena;
      auto walker_ptr = repo->NewPresentWalker(TAtom(std::tuple<TInt>(i), &fast_arena), TAtom(std::tuple<TInt>(i), &fast_arena));
      int64_t count = 0L;
      for (auto &walker = *walker_ptr; walker; ++walker) {
        //Check((*walker).SequenceNumber, static_cast<size_t>(num_iter) - count);
        Check((*walker).Key, TAtom(std::tuple<TInt>(i), &fast_arena));
        Check((*walker).Op, TAtom(i * 10L, &fast_arena));
        ++count;
      }
      Check(count, 1L);
    }
    std::cout << "done checking" << std::endl;
  }
  mem_merger.join();
  mem_flusher.join();
  disk_merger.join();
  replicator.join();
  queuer.join();
  runner.join();
}