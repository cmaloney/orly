/* <orly/server/ws.test.cc>

   Unit test for <orly/server/ws.h>.

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

#include <orly/server/ws.h>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <orly/server/ws_test_server.h>
#include <test/kit.h>

using namespace std;
using namespace Orly::Server;

using TAsioClient = websocketpp::client<websocketpp::config::asio_client>;
using TConnHndl = websocketpp::connection_hdl;
using TMsgPtr = TAsioClient::message_ptr;

const string UriPrefix = "ws://127.0.0.1:";

FIXTURE(Typical) {
  /* TODO: This loop-and-retry approach is a hack. The server isn't releasing
     its port number properly, probably because I'm not using the library correctly.
     I also don't know how to do a bind-to-any-port style server with this library,
     so, to get repeatable success, we have to port-probe. */
  in_port_t port_number = 8080;
  int retry_limit = 100;
  string reply;
  do {
    try {
      TWsTestServer ws_test_server(port_number);
      TAsioClient client;
      client.clear_access_channels(websocketpp::log::alevel::all);
      client.clear_error_channels(websocketpp::log::elevel::all);
      client.init_asio();
      client.set_open_handler(
        [&client](TConnHndl conn_hndl) {
          client.send(conn_hndl, "echo 'hello';", websocketpp::frame::opcode::text);    
        }
      );
      client.set_message_handler(
        [&client, &reply](TConnHndl conn_hndl, TMsgPtr msg_ptr) {
          reply = msg_ptr->get_payload();
          client.close(conn_hndl, websocketpp::close::status::going_away, "");
        }
      );
      websocketpp::lib::error_code ec;
      auto conn = client.get_connection(UriPrefix + to_string(port_number), ec);
      client.connect(conn);
      client.run();
    } catch (...) {
      --retry_limit;
      if (!retry_limit) {
        throw;
      }
      ++port_number;
    }
  } while (reply.empty());
  EXPECT_EQ(reply, "ok:\"hello\"");
}