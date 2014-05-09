/* <orly/server/ws.exercise.cc>

   Runs the <orly/server/ws_test_server.h> until SIGINT.

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

#include <unistd.h>

#include <orly/server/ws_test_server.h>
#include <signal/handler_installer.h>
#include <signal/masker.h>
#include <signal/set.h>

using namespace std;
using namespace Orly::Server;
using namespace Signal;

/* Very main.  So running.  Wow. */
int main(int argc, char *argv[]) {
  /* Get our port number from the command line, or use the default. */
  in_port_t port_number = (argc >= 2) ? atoi(argv[1]) : 8080;
  /* Handle SIGINT. */
  THandlerInstaller handle_sigint(SIGINT);
  TMasker mask_all_but_sigint(*TSet(TSet::Exclude, { SIGINT }));
  /* Make an el-fake-o server and run the websocket interface against it. */
  TWsTestServer ws_test_server(port_number);
  /* Wait to be told to stop. */
  pause();
  return EXIT_SUCCESS;
}