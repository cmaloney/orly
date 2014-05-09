/* <orly/server/ws_test_server.h>

   A do-little server for testing the websockets layer.

   Copyright 2010-2014 OrlyAtomics, Inc.

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

#include <base/class_traits.h>
#include <orly/server/ws.h>

namespace Orly {

  namespace Server {

    /* A do-little server for testing the websockets layer. */
    class TWsTestServer final {
      NO_COPY(TWsTestServer);
      public:

      /* Startup the server on the given port. */
      TWsTestServer(in_port_t port_number = 8080);

      /* Shutdown the server. */
      ~TWsTestServer();

      private:

      /* Defined in the .cc file. */
      class TSessionManager;

      /* The session manager we provide to the server object. */
      TSessionManager *SessionManager;

      /* The server object we wrap. */
      TWs *Ws;

    };  // TWsTestServer;

  }  // Server

}  // Orly