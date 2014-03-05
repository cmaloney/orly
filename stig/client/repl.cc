/* <stig/client/repl.cc>

   Implements <stig/client/repl.h>.

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

#include <stig/client/repl.h>

#include <cassert>
#include <exception>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>

#include <unistd.h>

#include <base/error_utils.h>
#include <base/io_utils.h>
#include <stig/protocol.h>
#include <stig/client/program/interpret_stmt.h>
#include <stig/client/program/parse_stmt.h>
#include <tools/nycr/error.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Socket;
using namespace Stig;
using namespace Stig::Client;
using namespace Stig::Client::Program;
using namespace Tools;

int TRepl::Main(int argc, char *argv[]) NO_THROW {
  int result;
  try {
    TCmd cmd(argc, argv);
    TLog log(cmd);
    result = TRepl(cmd).Run();
  } catch (const exception &ex) {
    cerr << "error: " << ex.what() << endl;
    result = EXIT_FAILURE;
  } catch (...) {
    cerr << "error: something weird happened" << endl;
    result = EXIT_FAILURE;
  }
  return result;
}

void TRepl::TClient::OnPovFailed(const Base::TUuid &/*repo_id*/) {}

void TRepl::TClient::OnUpdateAccepted(const Base::TUuid &/*repo_id*/, const Base::TUuid &/*tracking_id*/) {}

void TRepl::TClient::OnUpdateReplicated(const Base::TUuid &/*repo_id*/, const Base::TUuid &/*tracking_id*/) {}

void TRepl::TClient::OnUpdateDurable(const Base::TUuid &/*repo_id*/, const Base::TUuid &/*tracking_id*/) {}

void TRepl::TClient::OnUpdateSemiDurable(const Base::TUuid &/*repo_id*/, const Base::TUuid &/*tracking_id*/) {}


TRepl::TCmd::TCmd()
    : ServerAddress(TAddress::IPv4Loopback, DefaultPortNumber), TimeToLive(3600) {}

TRepl::TCmd::TCmd(int argc, char *argv[])
    : TCmd() {
  Parse(argc, argv, TMeta());
}

TRepl::TCmd::TMeta::TMeta()
    : TLog::TCmd::TMeta("The Stig client shell.") {
  Param(
      &TCmd::ServerAddress, "server_address", Optional, "server_address\0sa\0",
      "The address where the Stig server can be found."
  );
  Param(
      &TCmd::SessionId, "session_id", Optional, "session_id\0sid\0",
      "The id of the session with which to connect."
  );
  Param(
      &TCmd::TimeToLive, "time_to_live", Optional, "time_to_live\0ttl\0",
      "The minimum number of seconds your session will persist after you disconnect."
  );
}

TRepl::TRepl(const TCmd &cmd)
    : Client(make_shared<TClient>(cmd.ServerAddress, cmd.SessionId, seconds(cmd.TimeToLive))), SigIntHandler(SIGINT) {
  /* Change the virtual characters for EOF and INTR. */
  IfLt0(tcgetattr(0, &OldTermios));
  termios new_termios = OldTermios;
  new_termios.c_cc[VEOF]  = 3; // ctrl-c is our EOF
  new_termios.c_cc[VINTR] = 4; // ctrl-d is out INTR
  IfLt0(tcsetattr(0, TCSANOW, &new_termios));
}

TRepl::~TRepl() {
  assert(this);
  tcsetattr(0, TCSANOW, &OldTermios);
}

bool TRepl::ForEachStr(const function<bool (const string &)> &cb) {
  assert(this);
  assert(&cb);
  ostringstream strm;
  bool
      is_intr,
      is_cont = false;
  for (;;) {
    /* Get a line from the user.  This could be the start of a new string or a continuation of an existing string. */
    cout << (!is_cont ? "stig" : " ...") << "> ";
    cout.flush();
    char line[256];
    size_t size;
    try {
      size = ReadAtMost(0, line, sizeof(line));
    } catch (const system_error &error) {
      if (!WasInterrupted(error)) {
        throw;
      }
      /* The user hit INTR, so it's time to bail. */
      cout << endl;
      is_intr = true;
      break;
    }
    if (!size || line[size - 1] != 10) {
      /* The user hit EOF, so clear the terminal and go back to the prompt. */
      strm.str("");
      is_cont = false;
      cout << endl;
    } else if (size > 1) {
      /* We got something more than a blank line, so append it to the string.  (Remember, the last character is the LF.) */
      strm.write(line, size);
      is_cont = (line[size - 2] != ';');
      if (!is_cont) {
        /* It ended with a semi, so we have the user's whole string. */
        is_intr = cb(strm.str());
        if (!is_intr) {
          break;
        }
        strm.str("");
        is_cont = false;
      }
    }
  }
  return is_intr;
}

int TRepl::Run() {
  assert(this);
  ForEachStr(
      [this](const string &str) {
        bool keep_going = true;
        try {
          ParseStmtStr(
              str.c_str(),
              [this, &keep_going](const TStmt *stmt) {
                keep_going = InterpretStmt(stmt, Client);
              }
          );
        } catch (const exception &ex) {
          cout << ex.what() << endl;
        }
        return keep_going;
      }
  );
  if (Client->GetSessionId()) {
    cout << "(session_id = " << Client->GetSessionId() << ", time_to_live = " << Client->GetTimeToLive().count() << " sec(s))" << endl;
  } else {
    cout << "(no session)" << endl;
  }
  return EXIT_SUCCESS;
}
