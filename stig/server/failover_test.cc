/* <stig/server/failover_test.cc>

   Unit test for Failing over a master Indy server to a slave Indy server.

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

#include <stig/server/server.h>

#include <fstream>

#include <starsha/runner.h>
#include <stig/balancer/failover_test_balancer.h>
#include <stig/client/client.h>
#include <stig/compiler.h>
#include <stig/spa/honcho.h>

#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Socket;
using namespace Stig;
using namespace Stig::Client;
using namespace Stig::Balancer;
using namespace Stig::Server;

//#if 0
const char *Server1BlockDevice = "sdb";
const char *Server2BlockDevice = "sdc";
//#endif

#if 0
/* amazon high-io*/
const char *Server1BlockDevice = "xvdf";
const char *Server2BlockDevice = "xvdg";
#endif

bool PrintCmds = false;

class TExerciseClient final
    : public TClient {
  public:

  TExerciseClient(const Base::TOpt<TUuid> &session_id, const TAddress &addr)
      : TClient(addr, session_id, seconds(600)) {}

  void WaitForReplication(const Base::TUuid &tracker_id, const Base::TUuid &repo_id) {
    std::unique_lock<std::mutex> lock(ReplicationMutex);
    auto search_pair = make_pair(tracker_id, repo_id);
    auto ret = ReplicatedUpdateToRepo.find(search_pair);
    while (ret == ReplicatedUpdateToRepo.end()) {
      ReplicationCond.wait(lock);
      ret = ReplicatedUpdateToRepo.find(search_pair);
    }
    ReplicatedUpdateToRepo.erase(ret);
  }

  void WaitForTetris(const Base::TUuid &tracker_id, const Base::TUuid &repo_id) {
    std::unique_lock<std::mutex> lock(TetrisMutex);
    auto search_pair = make_pair(tracker_id, repo_id);
    auto ret = TetrisedUpdateToRepo.find(search_pair);
    while (ret == TetrisedUpdateToRepo.end()) {
      TetrisCond.wait(lock);
      ret = TetrisedUpdateToRepo.find(search_pair);
    }
    TetrisedUpdateToRepo.erase(ret);
  }

  private:

  virtual void OnPovFailed(const Base::TUuid &/*repo_id*/) override {}

  virtual void OnUpdateAccepted(const Base::TUuid &repo_id, const Base::TUuid &tracking_id) override {
    std::lock_guard<std::mutex> lock(TetrisMutex);
    auto ret = TetrisedUpdateToRepo.insert(make_pair(tracking_id, repo_id));
    assert(ret.second);
    TetrisCond.notify_all();
  }

  virtual void OnUpdateReplicated(const Base::TUuid &repo_id, const Base::TUuid &tracking_id) override {
    std::lock_guard<std::mutex> lock(ReplicationMutex);
    auto ret = ReplicatedUpdateToRepo.insert(make_pair(tracking_id, repo_id));
    assert(ret.second);
    ReplicationCond.notify_all();
  }

  virtual void OnUpdateDurable(const Base::TUuid &/*repo_id*/, const Base::TUuid &/*tracking_id*/) override {}

  virtual void OnUpdateSemiDurable(const Base::TUuid &/*repo_id*/, const Base::TUuid &/*tracking_id*/) override {}

  std::unordered_set<std::pair<Base::TUuid, Base::TUuid>> ReplicatedUpdateToRepo;
  std::mutex ReplicationMutex;
  std::condition_variable ReplicationCond;
  std::unordered_set<std::pair<Base::TUuid, Base::TUuid>> TetrisedUpdateToRepo;
  std::mutex TetrisMutex;
  std::condition_variable TetrisCond;

};

#if 0
static const char *sample_package = "package #1;"
                                    "insert = ((true) effecting {"
                                    "    new <[a, b, c]> <- val;"
                                    "  }) where {"
                                    "  a = given::(int);"
                                    "  b = given::(int);"
                                    "  c = given::(int);"
                                    "  val = given::(int);"
                                    "};"
                                    "get = (*<[a, b, c]>::(int)) where {"
                                    "  a = given::(int);"
                                    "  b = given::(int);"
                                    "  c = given::(int);"
                                    "};";

FIXTURE(SmallTypical) {
  Stig::Spa::THoncho Honcho;
  /* make the path to the server binary */
  stringstream out_path;
  out_path << SRC_ROOT;
  out_path << "/../out/";
  #ifndef NDEBUG
  out_path << "debug";
  #else
  out_path << "release";
  #endif
  out_path << "/stig/server/";
  /* make the path to the service argument ini file */
  /* make the path to the server binary */
  stringstream ini_path;
  ini_path << "--args=" << SRC_ROOT << "/stig/server/failover_test.ini";

  stringstream path;
  path << out_path.str() << "stig";

  //#if 0
  /* try to compile the binary */
  ofstream package_file;
  package_file.open("/tmp/sample.stig");
  package_file << sample_package;
  package_file.close();
  Stig::Compiler::Compile(Jhm::TAbsPath(Jhm::TAbsBase("/tmp/"), Jhm::TRelPath("sample.stig")), string("/tmp/"), false, false, false);
  //#endif

  TAddress server_1_addr(TAddress::IPv4Any, 19281);
  TAddress server_2_addr(TAddress::IPv4Any, 19282);

  /* launch server_1 which will start as solo */
  auto server_1_pid = fork();
  if (!server_1_pid) {
    string block_device = string("--device_name=") + Server1BlockDevice;
    string block_sys_path = string("--device_sys_path=") + Server1BlockDevice;
    const char *args[] = {
      path.str().c_str(),
      "--create=true",
      block_device.c_str(),
      block_sys_path.c_str(),
      "--starting_state=SOLO",
      "--le",
      "--log_debug",
      "--log_info",
      "--log_notice",
      "--log_warning",
      "--port_number=19281",
      "--reporting_port_number=19288",
      ini_path.str().c_str(),
      nullptr
    };
    try {
      IfLt0(execv(args[0], const_cast<char **>(args)));
    } catch (...) {
      std::cerr << "Error during exec:" << std::endl;
      throw;
    }
  }
  /* wait for the master to start up. */
  sleep(10);
  /* launch server_2 which will start as slave and connect to server_1 */
  auto server_2_pid = fork();
  if (!server_2_pid) {
    string block_device = string("--device_name=") + Server2BlockDevice;
    string block_sys_path = string("--device_sys_path=") + Server2BlockDevice;
    const char *args[] = {
      path.str().c_str(),
      "--create=true",
      block_device.c_str(),
      block_sys_path.c_str(),
      "--starting_state=SLAVE",
      "--le",
      "--log_debug",
      "--log_info",
      "--log_notice",
      "--log_warning",
      "--port_number=19282",
      "--address_of_master=127.0.0.1:19381",
      "--reporting_port_number=19289",
      ini_path.str().c_str(),
      nullptr
    };
    IfLt0(execv(args[0], const_cast<char **>(args)));
  }
  sleep(10);
  /* Start a load balancer */
  const size_t check_interval_milli = 500;
  TBalancer::TCmd cmd;
  const TScheduler::TPolicy scheduler_policy(4, 1000, milliseconds(1000));
  TScheduler scheduler;
  scheduler.SetPolicy(scheduler_policy);
  TAddress balancer_address(TAddress::IPv4Loopback, 19380);
  TFailoverTestBalancer balancer(&scheduler, cmd, check_interval_milli);
  balancer.RegisterHost(server_1_addr);
  balancer.RegisterHost(server_2_addr);
  sleep(2);

  /* install the package on server_1 */ {
    auto client = make_shared<TExerciseClient>(*TOpt<TUuid>::Unknown, server_1_addr);
    auto ack = client->InstallPackage({ "sample" }, 1);
    ack->Sync();
  }
  /* install the package on server_2 */ {
    auto client = make_shared<TExerciseClient>(*TOpt<TUuid>::Unknown, server_2_addr);
    auto ack = client->InstallPackage({ "sample" }, 1);
    ack->Sync();
  }

  /* connect a client to the balancer and push a key */ {
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
    auto client = make_shared<TExerciseClient>(session_id, balancer_address);
    auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(0));
    Base::TUuid id_to_use = **pov_id;
    auto push_result = client->Try(id_to_use, { "sample" }, TClosure(string("insert"),
                                                                     string("a"), 1L,
                                                                     string("b"), 2L,
                                                                     string("c"), 3L,
                                                                     string("val"), 4L));
    bool out;
    Sabot::ToNative(*Sabot::State::TAny::TWrapper((*push_result)->GetValue().NewState((*push_result)->GetArena().get(), state_alloc)), out);
    const Base::TOpt<TTracker> tracker = (*push_result)->GetTracker();
    assert(tracker);
    client->WaitForReplication(tracker->Id, id_to_use);
    client->WaitForTetris(tracker->Id, Stig::Indy::GlobalPovId);
    client->WaitForReplication(tracker->Id, Stig::Indy::GlobalPovId);
    std::cout << "Done insert" << std::endl;
  }

  /* connect a client to the balancer and get the key to see if it's there. we're connecting to server 1 in this case. */ {
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
    auto client = make_shared<TExerciseClient>(session_id, balancer_address);
    auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(0));
    Base::TUuid id_to_use = **pov_id;
    auto read_result = client->Try(id_to_use, { "sample" }, TClosure(string("get"),
                                                                     string("a"), 1L,
                                                                     string("b"), 2L,
                                                                     string("c"), 3L));
    int64_t out;
    Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_result)->GetValue().NewState((*read_result)->GetArena().get(), state_alloc)), out);
    EXPECT_EQ(out, 4L);
    std::cout << "Done Read" << std::endl;
  }

  /* Kill the master */
  std::cout << "Stopping server 1" << std::endl;
  kill(server_1_pid, SIGINT);

  /* connect a client to the balancer and get the key to see if it's there. we're connecting to server 2 in this case and checking to see if the slave also has the key replicated. */ {
    for (;;) {
      try {
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
        auto client = make_shared<TExerciseClient>(session_id, balancer_address);
        auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(0));
        Base::TUuid id_to_use = **pov_id;
        auto read_result = client->Try(id_to_use, { "sample" }, TClosure(string("get"),
                                                                         string("a"), 1L,
                                                                         string("b"), 2L,
                                                                         string("c"), 3L));
        int64_t out;
        Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_result)->GetValue().NewState((*read_result)->GetArena().get(), state_alloc)), out);
        EXPECT_EQ(out, 4L);
        break;
      } catch (const exception &ex) {
        cout << "Read 2 error [" << ex.what() << "]" << endl;
        usleep(10);
      }
    }
    std::cout << "Done Read" << std::endl;
  }

  /* Kill the slave */
  std::cout << "Stopping server 2" << std::endl;
  sleep(1);
  kill(server_2_pid, SIGINT);
}
#endif

#if 0
FIXTURE(MediumTypical) {
  const int64_t num_iter = 50000L;
  Stig::Spa::THoncho Honcho;
  /* make the path to the server binary */
  stringstream out_path;
  out_path << SRC_ROOT;
  out_path << "/../out/";
  #ifndef NDEBUG
  out_path << "debug";
  #else
  out_path << "release";
  #endif
  out_path << "/stig/server/";
  /* make the path to the service argument ini file */
  /* make the path to the server binary */
  stringstream ini_path;
  ini_path << "--args=" << SRC_ROOT << "/stig/server/failover_test.ini";

  stringstream path;
  path << out_path.str() << "stig";

  //#if 0
  /* try to compile the binary */
  ofstream package_file;
  package_file.open("/tmp/sample.stig");
  package_file << sample_package;
  package_file.close();
  Stig::Compiler::Compile(Jhm::TAbsPath(Jhm::TAbsBase("/tmp/"), Jhm::TRelPath("sample.stig")), string("/tmp/"), false, false, false);
  //#endif

  TAddress server_1_addr(TAddress::IPv4Any, 19281);
  TAddress server_2_addr(TAddress::IPv4Any, 19282);

  /* launch server_1 which will start as solo */
  auto server_1_pid = fork();
  if (!server_1_pid) {
    string block_device = string("--device_name=") + Server1BlockDevice;
    string block_sys_path = string("--device_sys_path=") + Server1BlockDevice;
    const char *args[] = {
      path.str().c_str(),
      "--create=true",
      block_device.c_str(),
      block_sys_path.c_str(),
      "--starting_state=SOLO",
      "--le",
      "--log_debug",
      "--log_info",
      "--log_notice",
      "--log_warning",
      "--port_number=19281",
      "--reporting_port_number=19288",
      //ini_path.str().c_str(),
      "--args=failover_test.ini",
      nullptr
    };
    try {
      IfLt0(execv(args[0], const_cast<char **>(args)));
    } catch (const std::exception &ex) {
      std::cerr << "Error during exec:" << ex.what() << std::endl;
      throw;
    }
  }
  /* wait for the master to start up. */
  sleep(10);
  /* launch server_2 which will start as slave and connect to server_1 */
  auto server_2_pid = fork();
  if (!server_2_pid) {
    string block_device = string("--device_name=") + Server2BlockDevice;
    string block_sys_path = string("--device_sys_path=") + Server2BlockDevice;
    const char *args[] = {
      path.str().c_str(),
      "--create=true",
      block_device.c_str(),
      block_sys_path.c_str(),
      "--starting_state=SLAVE",
      "--le",
      "--log_debug",
      "--log_info",
      "--log_notice",
      "--log_warning",
      "--port_number=19282",
      "--address_of_master=127.0.0.1:19381",
      "--reporting_port_number=19289",
      //ini_path.str().c_str(),
      "--args=failover_test.ini",
      nullptr
    };
    try {
      IfLt0(execv(args[0], const_cast<char **>(args)));
    } catch (const std::exception &ex) {
      std::cerr << "Error during exec:" << ex.what() << std::endl;
      throw;
    }
  }
  sleep(10);
  /* Start a load balancer */
  const size_t check_interval_milli = 100;
  TBalancer::TCmd cmd;
  const TScheduler::TPolicy scheduler_policy(4, 1000, milliseconds(1000));
  TScheduler scheduler;
  scheduler.SetPolicy(scheduler_policy);
  TAddress balancer_address(TAddress::IPv4Loopback, 19380);
  TFailoverTestBalancer balancer(&scheduler, cmd, check_interval_milli);
  balancer.RegisterHost(server_1_addr);
  balancer.RegisterHost(server_2_addr);
  sleep(2);

  /* install the package on server_1 */ {
    auto client = make_shared<TExerciseClient>(*TOpt<TUuid>::Unknown, server_1_addr);
    auto ack = client->InstallPackage({ "sample" }, 1);
    ack->Sync();
  }
  /* install the package on server_2 */ {
    auto client = make_shared<TExerciseClient>(*TOpt<TUuid>::Unknown, server_2_addr);
    auto ack = client->InstallPackage({ "sample" }, 1);
    ack->Sync();
  }

  /* connect a client to the balancer and push num_iter keys */ {
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
    auto client = make_shared<TExerciseClient>(session_id, balancer_address);
    auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(0));
    Base::TUuid id_to_use = **pov_id;
    std::vector<Base::TUuid> trackers;
    for (int64_t i = 0; i < num_iter; ++i) {
      auto push_result = client->Try(id_to_use, { "sample" }, TClosure(string("insert"),
                                                                       string("a"), i,
                                                                       string("b"), (i + 1L),
                                                                       string("c"), (i + 2L),
                                                                       string("val"), (i + 3L)));
      bool out;
      Sabot::ToNative(*Sabot::State::TAny::TWrapper((*push_result)->GetValue().NewState((*push_result)->GetArena().get(), state_alloc)), out);
      const Base::TOpt<TTracker> tracker = (*push_result)->GetTracker();
      assert(tracker);
      trackers.push_back(tracker->Id);
    }
    for (const auto &id : trackers) {
      client->WaitForReplication(id, id_to_use);
      client->WaitForTetris(id, Stig::Indy::GlobalPovId);
      client->WaitForReplication(id, Stig::Indy::GlobalPovId);
    }
    std::cout << "Done insert" << std::endl;
  }

  /* connect a client to the balancer and get the keys to see if they're there. we're connecting to server 1 in this case. */ {
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
    auto client = make_shared<TExerciseClient>(session_id, balancer_address);
    auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(0));
    Base::TUuid id_to_use = **pov_id;
    for (int64_t i = 0; i < num_iter; ++i) {
      auto read_result = client->Try(id_to_use, { "sample" }, TClosure(string("get"),
                                                                       string("a"), i,
                                                                       string("b"), (i + 1L),
                                                                       string("c"), (i + 2L)));
      int64_t out;
      Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_result)->GetValue().NewState((*read_result)->GetArena().get(), state_alloc)), out);
      EXPECT_EQ(out, (i + 3L));
    }
    std::cout << "Done Read" << std::endl;
  }

  /* Kill the master */
  std::cout << "Stopping server 1" << std::endl;
  kill(server_1_pid, SIGINT);

  /* connect a client to the balancer and get the keys to see if they're there. we're connecting to server 2 in this case and checking to see if the slave also has the keys replicated. */ {
    int64_t i = 0;
    for (;;) {
      try {
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
        auto client = make_shared<TExerciseClient>(session_id, balancer_address);
        auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(0));
        Base::TUuid id_to_use = **pov_id;
        for (; i < num_iter; ++i) {
          auto read_result = client->Try(id_to_use, { "sample" }, TClosure(string("get"),
                                                                           string("a"), i,
                                                                           string("b"), (i + 1L),
                                                                           string("c"), (i + 2L)));
          int64_t out;
          Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_result)->GetValue().NewState((*read_result)->GetArena().get(), state_alloc)), out);
          EXPECT_EQ(out, (i + 3L));
        }
        break;
      } catch (const exception &ex) {
        cout << "Read 2 error [" << ex.what() << "]" << endl;
        usleep(100000);
      }
    }
    std::cout << "Done Read" << std::endl;
    EXPECT_EQ(i, num_iter);
  }

  /* Kill the slave */
  std::cout << "Stopping server 2" << std::endl;
  sleep(1);
  kill(server_2_pid, SIGINT);
}
#endif

FIXTURE(ResyncTypical) {
  #if 0
  const int64_t num_iter = 500L;
  const size_t max_outstanding = 5000UL;
  #endif
  Stig::Spa::THoncho Honcho;
  /* make the path to the server binary */
  stringstream out_path;
  //#if 0
  out_path << SRC_ROOT;
  out_path << "/../out/";
  #ifndef NDEBUG
  out_path << "debug";
  #else
  out_path << "release";
  #endif
  out_path << "/stig/server/";
  /* make the path to the service argument ini file */
  /* make the path to the server binary */
  stringstream ini_path;
  ini_path << "--args=" << SRC_ROOT << "/stig/server/failover_test.ini";
  //#endif
  out_path << "";

  stringstream path;
  path << out_path.str() << "stig";

  #if 0
  /* try to compile the binary */
  ofstream package_file;
  package_file.open("/tmp/sample.stig");
  package_file << sample_package;
  package_file.close();
  Stig::Compiler::Compile(Jhm::TAbsPath(Jhm::TAbsBase("/tmp/"), Jhm::TRelPath("sample.stig")), string("/tmp/"), false, false, false);
  #endif

  TAddress server_1_addr(TAddress::IPv4Any, 19281);
  TAddress server_2_addr(TAddress::IPv4Any, 19282);
  TAddress server_3_addr(TAddress::IPv4Any, 19281);

#if 0
  /* launch server_1 which will start as solo */
  auto server_1_pid = fork();
  if (!server_1_pid) {
    string block_device = string("--device_name=") + Server1BlockDevice;
    string block_sys_path = string("--device_sys_path=") + Server1BlockDevice;
    const char *args[] = {
      path.str().c_str(),
      "--create=true",
      block_device.c_str(),
      block_sys_path.c_str(),
      "--starting_state=SOLO",
      "--le",
      "--log_debug",
      "--log_info",
      "--log_notice",
      "--log_warning",
      "--port_number=19281",
      "--reporting_port_number=19288",
      //ini_path.str().c_str(),
      "--args=failover_test.ini",
      nullptr
    };
    try {
      IfLt0(execv(args[0], const_cast<char **>(args)));
    } catch (const std::exception &ex) {
      std::cerr << "Error during exec:" << ex.what() << std::endl;
      throw;
    }
  }
  /* wait for the master to start up. */
  sleep(3);
  /* launch server_2 which will start as slave and connect to server_1 */
  auto server_2_pid = fork();
  if (!server_2_pid) {
    string block_device = string("--device_name=") + Server2BlockDevice;
    string block_sys_path = string("--device_sys_path=") + Server2BlockDevice;
    const char *args[] = {
      path.str().c_str(),
      "--create=true",
      block_device.c_str(),
      block_sys_path.c_str(),
      "--starting_state=SLAVE",
      "--le",
      "--log_debug",
      "--log_info",
      "--log_notice",
      "--log_warning",
      "--port_number=19282",
      "--address_of_master=127.0.0.1:19381",
      "--reporting_port_number=19289",
      //ini_path.str().c_str(),
      "--args=failover_test.ini",
      nullptr
    };
    try {
      IfLt0(execv(args[0], const_cast<char **>(args)));
    } catch (const std::exception &ex) {
      std::cerr << "Error during exec:" << ex.what() << std::endl;
      throw;
    }
  }
#endif
  sleep(3);
  /* Start a load balancer */
  const size_t check_interval_milli = 100;
  TBalancer::TCmd cmd;
  const TScheduler::TPolicy scheduler_policy(4, 1000, milliseconds(1000));
  TScheduler scheduler;
  scheduler.SetPolicy(scheduler_policy);
  TAddress balancer_address(TAddress::IPv4Loopback, 19380);
  TFailoverTestBalancer balancer(&scheduler, cmd, check_interval_milli);
  balancer.RegisterHost(server_1_addr);
  balancer.RegisterHost(server_2_addr);
  sleep(2);

  #if 0
  /* install the package on server_1 */ {
    auto client = make_shared<TExerciseClient>(*TOpt<TUuid>::Unknown, server_1_addr);
    auto ack = client->InstallPackage({ "sample" }, 1);
    ack->Sync();
  }
  /* install the package on server_2 */ {
    auto client = make_shared<TExerciseClient>(*TOpt<TUuid>::Unknown, server_2_addr);
    auto ack = client->InstallPackage({ "sample" }, 1);
    ack->Sync();
  }

  /* connect a client to the balancer and push num_iter keys */ {
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
    auto client = make_shared<TExerciseClient>(session_id, balancer_address);
    auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(0));
    Base::TUuid id_to_use = **pov_id;
    std::vector<Base::TUuid> trackers;
    for (int64_t i = 0; i < num_iter; ++i) {
      if (i % 10000 == 0) {
        std::cout << "\t@" << i << std::endl;
      }
      auto push_result = client->Try(id_to_use, { "sample" }, TClosure(string("insert"),
                                                                       string("a"), i,
                                                                       string("b"), (i + 1L),
                                                                       string("c"), (i + 2L),
                                                                       string("val"), (i + 3L)));
      bool out;
      Sabot::ToNative(*Sabot::State::TAny::TWrapper((*push_result)->GetValue().NewState((*push_result)->GetArena().get(), state_alloc)), out);
      const Base::TOpt<TTracker> tracker = (*push_result)->GetTracker();
      assert(tracker);
      trackers.push_back(tracker->Id);
      if (trackers.size() > max_outstanding) {
        Base::TUuid id = *trackers.begin();
        client->WaitForReplication(id, id_to_use);
        client->WaitForTetris(id, Stig::Indy::GlobalPovId);
        client->WaitForReplication(id, Stig::Indy::GlobalPovId);
        trackers.erase(trackers.begin());
      }
    }
    for (const auto &id : trackers) {
      client->WaitForReplication(id, id_to_use);
      client->WaitForTetris(id, Stig::Indy::GlobalPovId);
      client->WaitForReplication(id, Stig::Indy::GlobalPovId);
    }
    std::cout << "Done insert" << std::endl;
  }

  /* connect a client to the balancer and get the keys to see if they're there. we're connecting to server 1 in this case. */ {
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
    auto client = make_shared<TExerciseClient>(session_id, balancer_address);
    auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(0));
    Base::TUuid id_to_use = **pov_id;
    for (int64_t i = 0; i < num_iter; ++i) {
      auto read_result = client->Try(id_to_use, { "sample" }, TClosure(string("get"),
                                                                       string("a"), i,
                                                                       string("b"), (i + 1L),
                                                                       string("c"), (i + 2L)));
      int64_t out;
      Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_result)->GetValue().NewState((*read_result)->GetArena().get(), state_alloc)), out);
      EXPECT_EQ(out, (i + 3L));
    }
    std::cout << "Done Read" << std::endl;
  }

  //#if 0
  /* Kill the master */
  std::cout << "Stopping server 1" << std::endl;
  kill(server_1_pid, SIGINT);
  //#endif

  /* connect a client to the balancer and get the keys to see if they're there. we're connecting to server 2 in this case and checking to see if the slave also has the keys replicated. */ {
    int64_t i = 0;
    for (;;) {
      try {
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
        auto client = make_shared<TExerciseClient>(session_id, balancer_address);
        auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(0));
        Base::TUuid id_to_use = **pov_id;
        for (; i < num_iter; ++i) {
          auto read_result = client->Try(id_to_use, { "sample" }, TClosure(string("get"),
                                                                           string("a"), i,
                                                                           string("b"), (i + 1L),
                                                                           string("c"), (i + 2L)));
          int64_t out;
          Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_result)->GetValue().NewState((*read_result)->GetArena().get(), state_alloc)), out);
          EXPECT_EQ(out, (i + 3L));
        }
        break;
      } catch (const exception &ex) {
        cout << "Read 2 error [" << ex.what() << "]" << endl;
        usleep(100000);
      }
    }
    std::cout << "Done Reading from server 2" << std::endl;
    EXPECT_EQ(i, num_iter);
  }

  /* connect a client to the balancer and push num_iter keys again */ {
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
    auto client = make_shared<TExerciseClient>(session_id, balancer_address);
    auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(0));
    Base::TUuid id_to_use = **pov_id;
    std::vector<Base::TUuid> trackers;
    for (int64_t i = 0; i < num_iter; ++i) {
      if (i % 10000 == 0) {
        std::cout << "\t@" << i << std::endl;
      }
      auto push_result = client->Try(id_to_use, { "sample" }, TClosure(string("insert"),
                                                                       string("a"), i + num_iter,
                                                                       string("b"), (i + num_iter + 1L),
                                                                       string("c"), (i + num_iter + 2L),
                                                                       string("val"), (i + num_iter + 3L)));
      bool out;
      Sabot::ToNative(*Sabot::State::TAny::TWrapper((*push_result)->GetValue().NewState((*push_result)->GetArena().get(), state_alloc)), out);
      const Base::TOpt<TTracker> tracker = (*push_result)->GetTracker();
      assert(tracker);
      trackers.push_back(tracker->Id);
      if (trackers.size() > max_outstanding) {
        Base::TUuid id = *trackers.begin();
        client->WaitForTetris(id, Stig::Indy::GlobalPovId);
        trackers.erase(trackers.begin());
      }
    }
    for (const auto &id : trackers) {
      client->WaitForTetris(id, Stig::Indy::GlobalPovId);
    }
    std::cout << "Done insert" << std::endl;
  }

  /* connect a client to the balancer and get the keys to see if they're all there. we're connecting to server 2 in this case and checking to see if the slave now has the new updates as well (checking tetris). */ {
    int64_t i = 0;
    for (;;) {
      try {
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
        auto client = make_shared<TExerciseClient>(session_id, balancer_address);
        auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(0));
        Base::TUuid id_to_use = **pov_id;
        for (; i < num_iter * 2; ++i) {
          auto read_result = client->Try(id_to_use, { "sample" }, TClosure(string("get"),
                                                                           string("a"), i,
                                                                           string("b"), (i + 1L),
                                                                           string("c"), (i + 2L)));
          int64_t out;
          Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_result)->GetValue().NewState((*read_result)->GetArena().get(), state_alloc)), out);
          EXPECT_EQ(out, (i + 3L));
        }
        break;
      } catch (const exception &ex) {
        cout << "Read 2 error [" << ex.what() << "]" << endl;
        usleep(100000);
      }
    }
    std::cout << "Done Reading from server 2 second time" << std::endl;
    EXPECT_EQ(i, num_iter * 2);
  }

  std::cout << "\n\n\n\n~~~ Starting server 3 (slave to resync with current solo)~~~ \n\n\n\n" << endl;

  auto server_3_pid = fork();
  /* now start another slave to resynchronize with the current solo (current solo was previous slave) */ {
    if (!server_3_pid) {
      string block_device = string("--device_name=") + Server1BlockDevice;
      string block_sys_path = string("--device_sys_path=") + Server1BlockDevice;
      const char *args[] = {
        path.str().c_str(),
        "--create=true",
        block_device.c_str(),
        block_sys_path.c_str(),
        "--starting_state=SLAVE",
        "--le",
        "--log_debug",
        "--log_info",
        "--log_notice",
        "--log_warning",
        "--port_number=19281",
        "--reporting_port_number=19288",
        "--address_of_master=127.0.0.1:19381",
        //ini_path.str().c_str(),
        "--args=failover_test.ini",
        nullptr
      };
      try {
        IfLt0(execv(args[0], const_cast<char **>(args)));
      } catch (const std::exception &ex) {
        std::cerr << "Error during exec:" << ex.what() << std::endl;
        throw;
      }
    }
  }
  sleep(20);

  /* install the package on server_3 */ {
    auto client = make_shared<TExerciseClient>(*TOpt<TUuid>::Unknown, server_3_addr);
    auto ack = client->InstallPackage({ "sample" }, 1);
    ack->Sync();
  }

  //#if 0
  /* Kill the new master */
  std::cout << "Stopping server 2" << std::endl;
  sleep(1);
  kill(server_2_pid, SIGINT);
  //#endif

  /* connect a client to the balancer and get the keys to see if they're there. we're connecting to server 3 in this case and checking to see if the new slave also has the keys replicated. */ {
    int64_t i = 0;
    for (;;) {
      try {
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
        auto client = make_shared<TExerciseClient>(session_id, balancer_address);
        auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(0));
        Base::TUuid id_to_use = **pov_id;
        for (; i < num_iter; ++i) {
          auto read_result = client->Try(id_to_use, { "sample" }, TClosure(string("get"),
                                                                           string("a"), i,
                                                                           string("b"), (i + 1L),
                                                                           string("c"), (i + 2L)));
          int64_t out;
          Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_result)->GetValue().NewState((*read_result)->GetArena().get(), state_alloc)), out);
          EXPECT_EQ(out, (i + 3L));
        }
        break;
      } catch (const exception &ex) {
        cout << "Read 3 error [" << ex.what() << "]" << endl;
        usleep(100000);
      }
    }
    std::cout << "Done Reading from server 3" << std::endl;
    EXPECT_EQ(i, num_iter);
  }

  /* Kill the new slave */
  std::cout << "Stopping server 3" << std::endl;
  sleep(1);
  kill(server_3_pid, SIGINT);
#endif
  sleep(5000000);
}