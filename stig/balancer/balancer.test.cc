/* <stig/balancer/balancer.test.cc>

   Unit test for <stig/balancer/balancer.h>.

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

#include <stig/balancer/balancer.h>

#include <base/io_utils.h>
#include <base/epoll.h>
#include <base/event_counter.h>
#include <base/timer_fd.h>
#include <base/uuid.h>
#include <io/device.h>
#include <rpc/rpc.h>

#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Io;
using namespace Socket;
using namespace Stig::Balancer;

class TRouter : public TBalancer {
  NO_COPY_SEMANTICS(TRouter);
  public:

  TRouter(TScheduler *scheduler, const TBalancer::TCmd &cmd, size_t milli_interval) : TBalancer(scheduler, cmd), Running(true) {
    scheduler->Schedule(bind(&TRouter::CheckHosts, this, milli_interval));
  }

  virtual ~TRouter() {
    Running = false;
    std::unique_lock<std::mutex> lock(HostMutex);
    HostCond.wait(lock);
  }

  virtual const Socket::TAddress &ChooseHost() {
    assert(this);
    std::lock_guard<std::mutex> lock(HostMutex);
    if (MasterHost) {
    } else {
      std::cerr << "No master host available to connect to." << endl;
      throw std::runtime_error("No master host available to connect to.");
    }
    return *MasterHost;
  }

  void AddHost(const Socket::TAddress &address) {
    std::lock_guard<std::mutex> lock(HostMutex);
    auto ret = HostSet.insert(address);
    if (ret.second) {
    } else {
      throw std::runtime_error("Host already exists in HostSet.");
    }
  }

  void RemoveHost(const Socket::TAddress &address) {
    std::lock_guard<std::mutex> lock(HostMutex);
    auto ret = HostSet.erase(address);
    assert(ret == 1);
  }

  void Wait() {
    std::unique_lock<std::mutex> lock(HostMutex);
    while (!MasterHost) {
      HostCond.wait(lock);
    }
  }

  virtual void OnError(const exception &) {}

  private:

  class TConnection
      : public Rpc::TContext {
    NO_COPY_SEMANTICS(TConnection);
    public:

    TConnection(Base::TFd &&fd)
        : TContext(TProtocol::Protocol), Device(make_shared<TDevice>(move(fd))) {
      BinaryIoStream = make_shared<TBinaryIoStream>(Device);
    }

    private:

    class TProtocol
        : public Rpc::TProtocol {
      NO_COPY_SEMANTICS(TProtocol);
      public:

      static const TProtocol Protocol;

      private:

      TProtocol() {}

    };  // TProtocol

    shared_ptr<TDevice> Device;

  };

  void CheckHosts(size_t milli_interval) {
    Base::TTimerFd check_hosts(milli_interval);
    for (;Running;) {
      check_hosts.Pop();
      std::lock_guard<std::mutex> lock(HostMutex);
      MasterHost.Reset();
      for (const auto &addr : HostSet) {
        bool is_master = CheckHost(addr);
        if (MasterHost && is_master) {
          std::cerr << "There is more than 1 master" << std::endl;
          MasterHost.Reset();
          throw std::runtime_error("There is more than 1 master");
        } else if (is_master) {
          MasterHost = addr;
        }
      }
      if (MasterHost) {
        HostCond.notify_all();
      }
    }
    std::lock_guard<std::mutex> lock(HostMutex);
    HostCond.notify_all();
  }

  bool CheckHost(const Socket::TAddress &addr) const;

  std::mutex HostMutex;
  std::condition_variable HostCond;

  std::unordered_set<Socket::TAddress> HostSet;

  Base::TOpt<Socket::TAddress> MasterHost;

  bool Running;

};

class TTestServer {
  NO_COPY_SEMANTICS(TTestServer);
  public:

  static const Rpc::TEntryId
    AddId         = 1001,
    GetPortId     = 1002,
    HealthCheckId = 1003;

  TTestServer(TScheduler *scheduler, in_port_t port_num, char status, int connection_backlog = 5000)
      : Running(true), Scheduler(scheduler), PortNum(port_num), Status(status) {
    /* open the main socket */ {
      TAddress address(TAddress::IPv4Any, port_num);
      MainSocket = TFd(socket(address.GetFamily(), SOCK_STREAM, 0));
      int flag = true;
      IfLt0(setsockopt(MainSocket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)));
      Bind(MainSocket, address);
      IfLt0(listen(MainSocket, connection_backlog));
    }
    scheduler->Schedule(bind(&TTestServer::AcceptClientConnections, this));
  }

  ~TTestServer() {
    Running = false;
    TermFd.Push();
    std::unique_lock<std::mutex> lock(StatusLock);
    Finish.wait(lock);
  }

  void ChangeStatus(char status) {
    std::lock_guard<std::mutex> lock(StatusLock);
    Status = status;
  }

  private:

  class TConnection
      : public Rpc::TContext {
    NO_COPY_SEMANTICS(TConnection);
    public:

    TConnection(TTestServer *server, Base::TFd &&fd)
        : TContext(TProtocol::Protocol), Server(server), Device(make_shared<TDevice>(move(fd))) {
      BinaryIoStream = make_shared<TBinaryIoStream>(Device);
    }

    private:

    int Add(int a, int b) {
      return a + b;
    }

    int GetPort() {
      std::lock_guard<std::mutex> lock(Server->StatusLock);
      if (Server->Status == 'M') {
        return Server->PortNum;
      } else {
        throw std::runtime_error("Slave should not respond");
      }
    }

    char HealthCheck() {
      std::lock_guard<std::mutex> lock(Server->StatusLock);
      return Server->Status;
    }

    class TProtocol
        : public Rpc::TProtocol {
      NO_COPY_SEMANTICS(TProtocol);
      public:

      static const TProtocol Protocol;

      private:

      TProtocol() {
        Register<TConnection, int, int, int>(AddId, &TConnection::Add);
        Register<TConnection, int>(GetPortId, &TConnection::GetPort);
        Register<TConnection, char>(HealthCheckId, &TConnection::HealthCheck);
      }

    };  // TProtocol

    TTestServer *const Server;

    shared_ptr<TDevice> Device;

  };

  void AcceptClientConnections() {
    assert(this);
    try {
      TEpoll poll;
      poll.Add(MainSocket);
      poll.Add(TermFd.GetFd());
      for (;Running;) {
        TAddress client_address;
        int ready_fd = poll.WaitForOne();
        if (ready_fd == MainSocket) {
          TFd client_socket(Accept(MainSocket, client_address));
          Scheduler->Schedule(bind(&TTestServer::ServeClient, this, move(client_socket), client_address));
        } else {
          /* we're exiting now */
        }
      }
    } catch (const std::exception &/*ex*/) {
      /* do nothing */
    }
    std::lock_guard<std::mutex> lock(StatusLock);
    Finish.notify_all();
  }

  void ServeClient(TFd &fd, const TAddress &client_address) {
    assert(this);
    assert(&fd);
    assert(&client_address);
    std::shared_ptr<TConnection> connection = make_shared<TConnection>(this, std::move(fd));
    try {
      /* Run the server thru one cycle. */ {
        /* Read the request. */
        auto request = connection->Read();
        /* Service the request. */
        (*request)();
      }
    } catch (const std::exception &ex) {
    }
  }

  bool Running;

  std::mutex StatusLock;
  std::condition_variable Finish;

  Base::TScheduler *const Scheduler;

  Base::TFd MainSocket;

  Base::TEventCounter TermFd;

  in_port_t PortNum;

  char Status;

};

std::mutex ExpectedHostLock;
int ExpectedHost = 0;

class TTestClient {
  NO_COPY_SEMANTICS(TTestClient);
  public:

  TTestClient(TScheduler *scheduler, const TAddress &server_addr)
      : ServerAddr(server_addr), Running(true) {
    scheduler->Schedule(bind(&TTestClient::Runner, this));
  }

  ~TTestClient() {
    Running = false;
  }

  void Runner() {
    for (;Running;) {
      try {
        TFd new_server_socket(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
        Connect(new_server_socket, ServerAddr);
        std::shared_ptr<TConnection> connection = make_shared<TConnection>(std::move(new_server_socket));
        /* get port number */ {
          std::lock_guard<std::mutex> lock(ExpectedHostLock);
          auto future = connection->Write<int>(TTestServer::GetPortId);
          connection->Read();
          int from_host = **future;
          EXPECT_EQ(from_host, ExpectedHost);
        }
      } catch (const Io::TInputConsumer::TPastEndError &ex) {
        /* do nothing */
      } catch (const std::exception &ex) {
      }
    }
  }

  private:

  class TConnection
      : public Rpc::TContext {
    NO_COPY_SEMANTICS(TConnection);
    public:

    TConnection(Base::TFd &&fd)
        : TContext(TProtocol::Protocol), Device(make_shared<TDevice>(move(fd))) {
      BinaryIoStream = make_shared<TBinaryIoStream>(Device);
    }

    private:

    class TProtocol
        : public Rpc::TProtocol {
      NO_COPY_SEMANTICS(TProtocol);
      public:

      static const TProtocol Protocol;

      private:

      TProtocol() {
      }

    };  // TProtocol

    shared_ptr<TDevice> Device;

  };

  const TAddress ServerAddr;

  bool Running;

};

bool TRouter::CheckHost(const Socket::TAddress &addr) const {
  TFd new_server_socket(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  try {
    Connect(new_server_socket, addr);
    std::shared_ptr<TConnection> connection = make_shared<TConnection>(std::move(new_server_socket));
    auto future = connection->Write<char>(TTestServer::HealthCheckId);
    connection->Read();
    return **future == 'M';
  } catch (const std::exception &ex) {
    return false;
  }
  return false;
}

const TRouter::TConnection::TProtocol TRouter::TConnection::TProtocol::Protocol;
const TTestServer::TConnection::TProtocol TTestServer::TConnection::TProtocol::Protocol;
const TTestClient::TConnection::TProtocol TTestClient::TConnection::TProtocol::Protocol;

FIXTURE(Typical) {
  const size_t check_interval_milli = 500;
  TBalancer::TCmd cmd;
  const TScheduler::TPolicy scheduler_policy(4, 1000, milliseconds(1000));
  TScheduler scheduler;
  scheduler.SetPolicy(scheduler_policy);
  TAddress router_address(TAddress::IPv4Loopback, 19380);
  TAddress server_1_address(TAddress::IPv4Loopback, 19381);
  TAddress server_2_address(TAddress::IPv4Loopback, 19382);
  TRouter router(&scheduler, cmd, check_interval_milli);
  std::unique_ptr<TTestServer> test_server_1(new TTestServer(&scheduler, 19381, 'M'));
  router.AddHost(server_1_address);
  std::unique_ptr<TTestServer> test_server_2(new TTestServer(&scheduler, 19382, 'S'));
  router.AddHost(server_2_address);
  router.Wait();
  /* set master */ {
    std::lock_guard<std::mutex> lock(ExpectedHostLock);
    ExpectedHost = 19381;
  }
  TTestClient client_1(&scheduler, router_address);
  sleep(3);
  router.RemoveHost(TAddress(TAddress::IPv4Loopback, 19381));
  /* switch hosts */ {
    std::lock_guard<std::mutex> lock(ExpectedHostLock);
    test_server_1->ChangeStatus('S');
    ExpectedHost = 19382;
    test_server_2->ChangeStatus('M');
    test_server_1.reset();
  }
  sleep(3);
  test_server_2.reset();
}