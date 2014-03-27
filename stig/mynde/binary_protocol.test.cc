/* <stig/mynde/binary_protocol.test.cc>

   Unit test for <stig/mynde/binary_protocol.h>.

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

#include <memory>

#include <signal.h>
#include <sys/socket.h>

#include <base/error_utils.h>
#include <base/subprocess.h>
#include <socket/address.h>
#include <stig/server/server.h>
#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Socket;
using namespace Stig;
using namespace Stig::Server;

#if 0
static bool Running = false;

static void ServerMain() {
  const struct cmd_t final : public TServer::TCmd {
    cmd_t() {
      MemorySim = true;
    }
  } cmd;
  unique_ptr<TServer> svr;
  TScheduler::TPolicy(100, 100, milliseconds(0), false).RunUntilCtrlC(
      [&cmd, &svr](TScheduler *) {
        //svr.reset(new TServer(scheduler, cmd));
        Running = true;
      }
  );
  exit(0);
}
#endif

FIXTURE(Typical) {
  #if 0
  TPump pump;
  auto subprocess = TSubprocess::New(pump);
  if (!subprocess) {
    ServerMain();
  }
  while (!Running);
  #endif
  const struct cmd_t final : public TServer::TCmd {
    cmd_t() {
      StartingState = "SOLO";
      MemorySim = true;
      InstanceName = "flapjack";
    }
  } cmd;
  TScheduler scheduler(TScheduler::TPolicy(100, 100, milliseconds(0), false));
  TServer server(&scheduler, cmd);
  bool connected;
  try {
    TFd fd(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    Connect(fd, TAddress(TAddress::IPv4Any, 11211));
    connected = true;
  } catch (...) {
    connected = false;
  }
  EXPECT_TRUE(connected);
  #if 0
  IfLt0(kill(subprocess->GetChildId(), SIGINT));
  subprocess->Wait();
  #endif
}

#if 0
#include <fstream>

#include <starsha/runner.h>
#include <stig/client/client.h>
#include <stig/compiler.h>

#include <stig/server/server.h>

using namespace std;
using namespace chrono;

using namespace Base;
using namespace Socket;

using namespace Stig;
using namespace Stig::Client;
using namespace Stig::Server;

const char *ServerBlockDevice = "sdb";

bool PrintCmds = false;

FIXTURE(Typical) {
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

