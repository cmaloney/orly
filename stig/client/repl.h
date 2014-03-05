/* <stig/client/repl.h>

   Runs the Read-Execute-Print Loop for the Stig client.

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

#include <functional>
#include <memory>
#include <string>
#include <thread>

#include <termios.h>

#include <base/cmd.h>
#include <base/log.h>
#include <base/no_copy_semantics.h>
#include <base/no_throw.h>
#include <base/opt.h>
#include <base/uuid.h>
#include <signal/handler_installer.h>
#include <stig/client/client.h>
#include <socket/address.h>

namespace Stig {

  namespace Client {

    /* Runs the Read-Execute-Print Loop for the Stig client. */
    class TRepl {
      NO_COPY_SEMANTICS(TRepl);
      public:

      /* Run the REPL. */
      static int Main(int argc, char *argv[]) NO_THROW;

      private:

      /* TODO */
      class TClient final
          : public Client::TClient {
        public:

        /* TODO */
        TClient(const Socket::TAddress &server_address, const Base::TOpt<Base::TUuid> &session_id, const std::chrono::seconds &time_to_live)
            : Client::TClient(server_address, session_id, time_to_live) {}

        private:

        /* TODO */
        virtual void OnPovFailed(const Base::TUuid &repo_id) override;

        /* TODO */
        virtual void OnUpdateAccepted(const Base::TUuid &repo_id, const Base::TUuid &tracking_id) override;

        /* TODO */
        virtual void OnUpdateReplicated(const Base::TUuid &repo_id, const Base::TUuid &tracking_id) override;

        /* TODO */
        virtual void OnUpdateDurable(const Base::TUuid &repo_id, const Base::TUuid &tracking_id) override;

        /* TODO */
        virtual void OnUpdateSemiDurable(const Base::TUuid &repo_id, const Base::TUuid &tracking_id) override;

      };  // TRepl::TClient

      /* Command-line arguments. */
      class TCmd
          : public Base::TLog::TCmd {
        public:

        /* Construct with defaults. */
        TCmd();

        /* Construct from argc/argv. */
        TCmd(int argc, char *argv[]);

        /* The address where the Stig server can be found. */
        Socket::TAddress ServerAddress;

        /* The session id to use.  If not given, then we'll start a new session. */
        Base::TOpt<Base::TUuid> SessionId;

        /* The minimum number of seconds our session should live after we leave. */
        uint32_t TimeToLive;

        private:

        /* Our meta-type. */
        class TMeta
            : public Base::TLog::TCmd::TMeta {
          public:

          /* Registers our fields. */
          TMeta();

        };  // TRepl::TCmd::TMeta

      };  // TRepl::TCmd

      /* Sets up the terminal. */
      TRepl(const TCmd &cmd);

      /* Tears down the terminal. */
      ~TRepl();

      /* Call back for each string entered at the terminal.
         Keeps going until the user hits INTR or until the callback returns false.
         Returns true iff. exit was caused by INTR. */
      bool ForEachStr(const std::function<bool (const std::string &)> &cb);

      /* Run the REPL. */
      int Run();

      /* Our end of the full-duplex RPC conversation with the server. */
      std::shared_ptr<TClient> Client;

      /* This handler prevents SIGINT from shutting down our process. */
      Signal::THandlerInstaller SigIntHandler;

      /* We restore to this version of the terminal settings when we destruct. */
      termios OldTermios;

    };  // TRepl

  }  // Client

}  // Stig
