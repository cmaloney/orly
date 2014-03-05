/* <stig/server/stig.cc>

   The 'main' of the Stig server.

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

#include <functional>

#include <cstdlib>
#include <iostream>

#include <base/debug_log.h>
#include <base/scheduler.h>
#include <server/daemonize.h>
#include <stig/server/server.h>

using namespace std;
using namespace chrono;
using namespace placeholders;
using namespace Base;
using namespace Stig::Server;

/* Command-line arguments. */
class TCmd final
    : public TServer::TCmd {
  public:

  /* Construct with defaults. */
  TCmd()
      : MinWorkerCount(200), MaxWorkerCount(50000), IdleWorkerTimeout(30000), Daemon(false) {}

  /* Construct from argc/argv. */
  TCmd(int argc, char *argv[])
      : TCmd() {
    Parse(argc, argv, TMeta());
  }

  /* Parameters for the scheduler policy. */
  size_t MinWorkerCount, MaxWorkerCount, IdleWorkerTimeout;

  /* If true, then run as a daemon. */
  bool Daemon;

  private:

  /* Our meta-type. */
  class TMeta final
      : public TServer::TCmd::TMeta {
    public:

    /* Registers our fields. */
    TMeta()
        : TServer::TCmd::TMeta("The Stig server.") {
      Param(
          &TCmd::MinWorkerCount, "min_worker_count", Optional, "min_worker_count\0",
          "The minimum number of worker threads to maintain in the thread pool."
      );
      Param(
          &TCmd::MaxWorkerCount, "max_worker_count", Optional, "max_worker_count\0",
          "The maximum number of worker threads to maintain in the thread pool."
      );
      Param(
          &TCmd::IdleWorkerTimeout, "idle_worker_timeout", Optional, "idle_worker_timeout\0",
          "The maximum number of milliseconds a worker will wait for a job before self-destructing."
      );
      Param(
          &TCmd::Daemon, "daemon", Optional, "daemon\0d\0",
          "Run as a daemon.  The pid of the spawned daemon process is returned on stdout."
      );
    }

  };  // TCmd::TMeta

};  // TCmd

static void LaunchServer(TScheduler *scheduler, const ::TCmd &cmd, shared_ptr<TServer> &server) {
  DEBUG_LOG("main: constructing server");
  server = make_shared<TServer>(scheduler, cmd);
  DEBUG_LOG("main: server constructed");
}

int main(int argc, char *argv[]) {
  ::TCmd cmd(argc, argv);
  TLog log(cmd);
  if (cmd.Daemon) {
    auto pid = Server::Daemonize();
    if (pid) {
      cout << pid << endl;
      return EXIT_SUCCESS;
    }
  }
  std::shared_ptr<TServer> server = nullptr;
  TScheduler::TPolicy(cmd.MinWorkerCount, cmd.MaxWorkerCount, milliseconds(cmd.IdleWorkerTimeout), false).RunUntilCtrlC(bind(LaunchServer, _1, cref(cmd), ref(server)));
  return EXIT_SUCCESS;
}
