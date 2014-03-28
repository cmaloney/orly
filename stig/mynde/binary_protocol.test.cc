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

#include <stig/mynde/binary_protocol.h>

#include <cstdio>
#include <memory>

#include <signal.h>
#include <sys/socket.h>

#include <base/error_utils.h>
#include <base/subprocess.h>
#include <base/zero.h>
#include <socket/address.h>
#include <stig/server/server.h>
#include <strm/fd.h>
#include <strm/bin/in.h>
#include <strm/bin/out.h>
#include <test/kit.h>

using namespace std;
using namespace std::chrono;
using namespace Base;
using namespace Socket;
using namespace Stig;
using namespace Stig::Mynde;
using namespace Stig::Server;

/* Runs a Stig server in a child process. */
class TSubprocServer final {
  public:

  /* Start the server and wait for it to be ready. */
  TSubprocServer() {
    Subprocess = TSubprocess::New(Pump);
    if (!Subprocess) {
      ServerMain();
    }
    auto *f = fdopen(Subprocess->GetStdErrFromChild(), "r");
    char line[2048];
    do {
      fgets(line, sizeof(line), f);
      printf(line);  // TODO: Remove this when we're confident.
    } while (!strstr(line, "TServer::Init end"));
    // NOTE: We're leaking f.  Big deal.
  }

  /* Shutdown the server and wait for the child proc to exit. */
  ~TSubprocServer() {
    assert(this);
    /* TODO: We should be sending SIGINT, not SIGKILL, but the server is
       currently hanging during shutdown.  Once the server gets back to
       exiting gracefully, change this call to trigger it to do so. */
    kill(Subprocess->GetChildId(), SIGKILL);
    Subprocess->Wait();
  }

  private:

  /* Child proc enters here. */
  void ServerMain() __attribute__((noreturn)) {
    const struct cmd_t final : public TServer::TCmd {
      cmd_t() {
        Echo = true;
        All = true;
        MemorySim = true;
        StartingState = "SOLO";
        InstanceName = "flapjack";
        MemorySimMB = 512;
        MemorySimSlowMB = 256;
      }
    } cmd;
    TLog log(cmd);
    unique_ptr<TServer> svr;
    TScheduler::TPolicy(100, 100, milliseconds(0), false).RunUntilCtrlC(
        [&cmd, &svr](TScheduler *scheduler) {
          svr.reset(new TServer(scheduler, cmd));
        }
    );
    exit(0);
  }

  /* I/O pump between parent and child procs. */
  TPump Pump;

  /* The server process. */
  unique_ptr<TSubprocess> Subprocess;

};  // TSubprocServer

static TFd ConnectToServer() {
  TFd fd(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  Connect(fd, TAddress(TAddress::IPv4Any, 11211));
  return move(fd);
}

FIXTURE(Typical) {
  /* Launch the server in a child process and wait for it to be ready
     to serve, then connect. */
  TSubprocServer server;
  Strm::TFd<> fd(ConnectToServer());
  /* extra */ {
    uint8_t
        extras[4] = { 0, 0, 0, 0 },
        key[4]    = { 'M', 'O', 'F', 'O' };
    TRequestHeader hdr;
    Zero(hdr);
    hdr.Magic = BinaryMagicRequest;
    hdr.Opcode = static_cast<uint8_t>(TRawOpcode::Get);
    hdr.ExtrasLength = sizeof(extras);
    hdr.KeyLength = sizeof(key);
    hdr.TotalBodyLength = hdr.KeyLength + hdr.ExtrasLength;
    Strm::Bin::TOut strm(&fd);
    strm << hdr << extras << key;
  }
  /* extra */ {
    TResponseHeader hdr;
    Zero(hdr);
    Strm::Bin::TIn strm(&fd);
    strm >> hdr;
    if (!EXPECT_EQ(hdr.Magic, BinaryMagicResponse)) {
      auto *msg = reinterpret_cast<char *>(&hdr);
      msg[sizeof(hdr) - 1] = '\0';
      printf("got a msg: \"%s\"", msg);
    }
  }
}
