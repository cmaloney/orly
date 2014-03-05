/* <stig/server/failover_balancer.cc>

   A failover balancer.

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

#include <stig/balancer/failover_test_balancer.h>

using namespace std;
using namespace chrono;
using namespace Stig::Balancer;

/* Command-line arguments. */
class TCmd final
    : public TBalancer::TCmd {
  public:

  /* Construct with defaults. */
  TCmd() {}

  /* Construct from argc/argv. */
  TCmd(int argc, char *argv[])
      : TCmd() {
    Parse(argc, argv, TMeta());
  }

  /* TODO  */
  Socket::TAddress Server1Addr;
  Socket::TAddress Server2Addr;

  private:

  /* Our meta-type. */
  class TMeta final
      : public Base::TCmd::TMeta {
    public:

    /* Registers our fields. */
    TMeta()
        : Base::TCmd::TMeta("The failover balancer.") {
      Param(
          &TCmd::Server1Addr, "server_1_addr", Required, "server_1_addr\0",
          "Address of server 1."
      );
      Param(
          &TCmd::Server2Addr, "server_2_addr", Required, "server_2_addr\0",
          "Address of server 2."
      );
    }

  };  // TCmd::TMeta

};  // TCmd


int main(int argc, char *argv[]) {
  ::TCmd cmd(argc, argv);
  Base::TLog log(cmd);
  const size_t check_interval_milli = 100;
  const Base::TScheduler::TPolicy scheduler_policy(4, 1000, milliseconds(1000));
  Base::TScheduler scheduler;
  scheduler.SetPolicy(scheduler_policy);
  TFailoverTestBalancer balancer(&scheduler, cmd, check_interval_milli);
  balancer.RegisterHost(cmd.Server1Addr);
  balancer.RegisterHost(cmd.Server2Addr);
  pause();
}