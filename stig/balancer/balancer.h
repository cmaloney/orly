/* <stig/balancer/balancer.h>

   TODO

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

#pragma once

#include <cassert>

#include <base/cmd.h>
#include <base/fd.h>
#include <base/io_utils.h>
#include <base/log.h>
#include <base/no_copy_semantics.h>
#include <base/scheduler.h>
#include <socket/address.h>

namespace Stig {

  namespace Balancer {

    /* TODO */
    class TBalancer {
      NO_COPY_SEMANTICS(TBalancer);
      public:

      /* Command-line arguments. */
      class TCmd : public Base::TLog::TCmd {
        public:

        /* Construct with defaults. */
        TCmd() : PortNumber(19380), ConnectionBacklog(5000) {}

        /* Construct from argc/argv. */
        TCmd(int argc, char *argv[])
            : TCmd() {
          Parse(argc, argv, TMeta());
        }

        /* The port on which we respond to TCP. */
        in_port_t PortNumber;

        /* The maximum number of connection requests to backlog against MainSocket. */
        int ConnectionBacklog;

        private:

        /* Our meta-type. */
        class TMeta
            : public Base::TCmd::TMeta {
          public:

          /* Registers our fields. */
          TMeta()
              : Base::TCmd::TMeta("The TCP Traffic Balancer.") {
            Param(
                &TCmd::PortNumber, "port", Optional, "port\0",
                "The port on which we listen for incoming traffic."
            );
            Param(
                &TCmd::ConnectionBacklog, "connection_backlog", Optional, "connection_backlog\0cb\0",
                "The maximum number of client connection requests to backlog."
            );
          }

        };  // TCmd::TMeta

      };  // TCmd

      /* TODO */
      virtual ~TBalancer();

      protected:

      /* TODO */
      TBalancer(Base::TScheduler *scheduler, const TCmd &cmd);

      /* Accepts connections from clients on our main socket.  Launched as a thread by the constructor. */
      void AcceptClientConnections();

      /* Serves a client on the given fd.  Launched as a thread by AcceptClientConnections() when a client connects. */
      void ServeClient(Base::TFd &fd, const Socket::TAddress &client_address);

      /* TODO */
      virtual const Socket::TAddress &ChooseHost() = 0;

      /* TODO */
      virtual void OnError(const std::exception &ex) = 0;

      private:

      /* The scheduler we use to launch jobs.  Set by the constructor and never changed. */
      Base::TScheduler *const Scheduler;

      /* The socket on which AcceptClientConnections() listens. */
      Base::TFd MainSocket;

    };  // TBalancer

  }   // Balancer

}  // Stig