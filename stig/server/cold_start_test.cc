/* <stig/server/cold_start_test.cc>

   Unit test for cold start of an Indy server.

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
#include <stig/client/client.h>
#include <stig/compiler.h>
#include <stig/honcho.h>

#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Socket;
using namespace Stig;
using namespace Stig::Client;
using namespace Stig::Server;

const char *ServerBlockDevice = "sdb";

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

static const char *sample_package = "package #1;"
                                    "insert = ((true) effecting {"
                                    "    new <[a, b, c]> <- val;"
                                    "  }) where {"
                                    "  a = given::(int);"
                                    "  b = given::(int);"
                                    "  c = given::(int);"
                                    "  val = given::(int);"
                                    "};"
                                    "has = (exists (int) @ <[a, b, c]>) where {"
                                    "  a = given::(int);"
                                    "  b = given::(int);"
                                    "  c = given::(int);"
                                    "};"
                                    "get = (*<[a, b, c]>::(int)) where {"
                                    "  a = given::(int);"
                                    "  b = given::(int);"
                                    "  c = given::(int);"
                                    "};";

#if 0
FIXTURE(SmallTypical) {
  Stig::THoncho Honcho;
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
  ini_path << "--args=" << SRC_ROOT << "/stig/server/cold_start_test.ini";

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

  TAddress server_addr(TAddress::IPv4Any, 19281);

  /* launch the server which will start as solo and create a new disk image. */ {
    auto server_pid = fork();
    if (!server_pid) {
      string block_device = string("--device_name=") + ServerBlockDevice;
      string block_sys_path = string("--device_sys_path=") + ServerBlockDevice;
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
    /* wait for the server to start up. */
    sleep(10);
    /* install the package on the server */ {
      auto client = make_shared<TExerciseClient>(*TOpt<TUuid>::Unknown, server_addr);
      auto ack = client->InstallPackage({ "sample" }, 1);
      ack->Sync();
    }
    /* connect a client to the server and push a key */ {
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
      auto client = make_shared<TExerciseClient>(session_id, server_addr);
      auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(10));
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
      client->WaitForTetris(tracker->Id, Stig::Indy::GlobalPovId);
      std::cout << "Done insert" << std::endl;
    }
    /* connect a client to the server and get the key to see if it's there. */
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
      auto client = make_shared<TExerciseClient>(session_id, server_addr);
      auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(10));
      Base::TUuid id_to_use = **pov_id;
      auto read_result = client->Try(id_to_use, { "sample" }, TClosure(string("get"),
                                                                       string("a"), 1L,
                                                                       string("b"), 2L,
                                                                       string("c"), 3L));
      int64_t out;
      Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_result)->GetValue().NewState((*read_result)->GetArena().get(), state_alloc)), out);
      EXPECT_EQ(out, 4L);
      std::cout << "Done Read" << std::endl;
    /* Stop the server */
    std::cout << "Stopping server" << std::endl;
    sleep(1);
    kill(server_pid, SIGINT);
    sleep(3);
    kill(server_pid, SIGKILL);
  }
  sleep(2);
  /* start a new server from the existing disk image */ {
    auto server_pid = fork();
    if (!server_pid) {
      string block_device = string("--device_name=") + ServerBlockDevice;
      string block_sys_path = string("--device_sys_path=") + ServerBlockDevice;
      const char *args[] = {
        path.str().c_str(),
        "--create=false",
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
    /* wait for the server to start up. */
    sleep(10);
    /* we shouldn't have to install the package if it were stored in the system repo... */
    /* install the package on the server */ {
      auto client = make_shared<TExerciseClient>(*TOpt<TUuid>::Unknown, server_addr);
      auto ack = client->InstallPackage({ "sample" }, 1);
      ack->Sync();
    }
    /* connect a client to the server and get the key to see if it's there. */
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
      auto client = make_shared<TExerciseClient>(session_id, server_addr);
      auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(10));
      Base::TUuid id_to_use = **pov_id;
      auto read_result = client->Try(id_to_use, { "sample" }, TClosure(string("get"),
                                                                       string("a"), 1L,
                                                                       string("b"), 2L,
                                                                       string("c"), 3L));
      int64_t out;
      Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_result)->GetValue().NewState((*read_result)->GetArena().get(), state_alloc)), out);
      EXPECT_EQ(out, 4L);
      std::cout << "Done Read" << std::endl;
    /* Stop the server */
    std::cout << "Stopping server" << std::endl;
    sleep(1);
    kill(server_pid, SIGINT);
  }
}
#endif

FIXTURE(MediumTypical) {
  const int64_t num_iter = 5000L;
  const size_t max_outstanding = 500L;
  Stig::THoncho Honcho;
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
  ini_path << "--args=" << SRC_ROOT << "/stig/server/cold_start_test.ini";

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

  TAddress server_addr(TAddress::IPv4Any, 19281);

  /* launch the server which will start as solo and create a new disk image. */ {
    auto server_pid = fork();
    if (!server_pid) {
      string block_device = string("--device_name=") + ServerBlockDevice;
      string block_sys_path = string("--device_sys_path=") + ServerBlockDevice;
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
    /* wait for the server to start up. */
    sleep(10);
    /* install the package on the server */ {
      auto client = make_shared<TExerciseClient>(*TOpt<TUuid>::Unknown, server_addr);
      auto ack = client->InstallPackage({ "sample" }, 1);
      ack->Sync();
    }
    /* connect a client to the server and push a key */ {
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
      auto client = make_shared<TExerciseClient>(session_id, server_addr);
      auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(10));
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
          client->WaitForTetris(id, Stig::Indy::GlobalPovId);
          trackers.erase(trackers.begin());
        }
      }
      for (const auto &id : trackers) {
        client->WaitForTetris(id, Stig::Indy::GlobalPovId);
      }
      std::cout << "Done insert" << std::endl;
    }
    /* connect a client to the server and get the key to see if it's there. */
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
      auto client = make_shared<TExerciseClient>(session_id, server_addr);
      auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(0));
      Base::TUuid id_to_use = **pov_id;
      for (int64_t i = 0L; i < num_iter; ++i) {
        auto read_result = client->Try(id_to_use, { "sample" }, TClosure(string("get"),
                                                                         string("a"), i,
                                                                         string("b"), (i + 1L),
                                                                         string("c"), (i + 2L)));
        int64_t out;
        Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_result)->GetValue().NewState((*read_result)->GetArena().get(), state_alloc)), out);
        EXPECT_EQ(out, (i + 3L));
      }
      std::cout << "Done Read" << std::endl;
    /* Stop the server */
    std::cout << "Stopping server" << std::endl;
    sleep(1);
    kill(server_pid, SIGINT);
    sleep(3);
    kill(server_pid, SIGKILL);
  }
  sleep(2);
  /* start a new server from the existing disk image */ {
    auto server_pid = fork();
    if (!server_pid) {
      string block_device = string("--device_name=") + ServerBlockDevice;
      string block_sys_path = string("--device_sys_path=") + ServerBlockDevice;
      const char *args[] = {
        path.str().c_str(),
        "--create=false",
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
    /* wait for the server to start up. */
    sleep(10);
    /* we shouldn't have to install the package if it were stored in the system repo... */
    /* install the package on the server */ {
      auto client = make_shared<TExerciseClient>(*TOpt<TUuid>::Unknown, server_addr);
      auto ack = client->InstallPackage({ "sample" }, 1);
      ack->Sync();
    }
    /* connect a client to the server and get the key to see if it's there. */
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      Base::TOpt<TUuid> session_id = *TOpt<TUuid>::Unknown;
      auto client = make_shared<TExerciseClient>(session_id, server_addr);
      auto pov_id = client->NewFastPrivatePov(*TOpt<TUuid>::Unknown, seconds(10));
      Base::TUuid id_to_use = **pov_id;
      for (int64_t i = 0L; i < num_iter; ++i) {
        auto read_result = client->Try(id_to_use, { "sample" }, TClosure(string("get"),
                                                                         string("a"), i,
                                                                         string("b"), (i + 1L),
                                                                         string("c"), (i + 2L)));
        int64_t out;
        Sabot::ToNative(*Sabot::State::TAny::TWrapper((*read_result)->GetValue().NewState((*read_result)->GetArena().get(), state_alloc)), out);
        EXPECT_EQ(out, (i + 3L));
      }
      std::cout << "Done Read" << std::endl;
    /* Stop the server */
    std::cout << "Stopping server" << std::endl;
    sleep(1);
    kill(server_pid, SIGINT);
  }
}