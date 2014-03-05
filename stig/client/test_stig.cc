/* <stig/client/test_stig.cc>

   A runnable server that implements Stig's client-server protocol, but doesn't actually do much work.
   Use this to test your clients.
   The server runs until it receives a ctrl-c.

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

#include <signal/handler_installer.h>
#include <signal/masker.h>
#include <signal/set.h>
#include <stig/client/test_server.h>

using namespace std;
using namespace Signal;
using namespace Stig::Client;

int main(int argc, char *argv[]) {
  /* Mask out all signals and install a do-nothing handler for ctrl-c. */
  TMasker masker(*TSet(TSet::Full));
  THandlerInstaller handler(SIGINT);
  /* Parse the command-line into a temporary object, then use it to start the server. */
  unique_ptr<TTestServer> test_server;
  /* extra */ {
    TTestServer::TCmd cmd(argc, argv);
    test_server.reset(new TTestServer(cmd));
  }
  /* Wait for ctrl-c, then stop the server and exit. */
  sigsuspend(&*TSet(TSet::Exclude, { SIGINT }));
  test_server.reset();
  return EXIT_SUCCESS;
}
