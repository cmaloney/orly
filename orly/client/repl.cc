/* <orly/client/repl.cc>

   Implements <orly/client/repl.h>.

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

#include <orly/client/repl.h>

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include <unistd.h>

#include <base/repl.h>
#include <orly/protocol.h>
#include <orly/client/program/interpret_stmt.h>
#include <orly/client/program/parse_stmt.h>

using namespace std;
using namespace std::chrono;
using namespace Base;
using namespace Socket;
using namespace Orly;
using namespace Orly::Client;
using namespace Orly::Client::Program;

namespace {

  /* Populate the suggestion and description of each of the top-level stmts. */
  class TGetInfo : public TTop::TVisitor {
    public:
    explicit TGetInfo(std::vector<TInfo> &infos) : Infos(infos) {}
    virtual ~TGetInfo() {}
    virtual void operator()(const TImage *) const override {}
    virtual void operator()(const TTailStmt *) const override {
      Infos.push_back({"tail", "tail;", "Tail the server"});
    }
    virtual void operator()(const TSetTtlStmt *) const override {
      Infos.push_back(
          {"set ttl", "set ttl {session_id} to 3000;", "Set time to live"});
    }
    virtual void operator()(const TTryStmt *) const override {
      Infos.push_back({"try",
                       "try {pov_id} package method <{.name: val}>;",
                       "Try invoking a method"});
    }
    virtual void operator()(const TSetUserIdStmt *) const override {
      Infos.push_back({"set user", "set user {user_id};", "Set a user id"});
    }
    virtual void operator()(const TPovStatusStmt *) const override {
      Infos.push_back({"pause", "pause {pov_id};", "Pause a point of view"});
      Infos.push_back(
          {"unpause", "unpause {pov_id};", "Unpause a point of view"});
    }
    virtual void operator()(const TPovConsStmt *) const override {
      Infos.push_back(
          {"new", "new fast private pov;", "Create a new point of view"});
    }
    virtual void operator()(const TBeginImportStmt *) const override {
      Infos.push_back({"begin import", "begin import;", "Begin an import"});
    }
    virtual void operator()(const TInstallStmt *) const override {
      Infos.push_back(
          {"install", "install package.1;", "Install a package"});
    }
    virtual void operator()(const TResumeSessionStmt *) const override {}
    virtual void operator()(const TCompileStmt *) const override {
      Infos.push_back(
          {"compile", R"~~(compile "x = 42;";)~~", "Compile a program"});
    }
    virtual void operator()(const TEchoStmt *) const override {
      Infos.push_back(
          {"echo", "echo 'Hello world!';", "Display a line of text"});
    }
    virtual void operator()(const TNewSessionStmt *) const override {}
    virtual void operator()(const TUninstallStmt *) const override {
      Infos.push_back(
          {"uninstall", "uninstall package.1;", "Uninstall a package"});
    }
    virtual void operator()(const TExitStmt *) const override {
      Infos.push_back({"exit", "exit;", "Exit from orly console"});
    }
    virtual void operator()(const TGetSourceStmt *) const override {}
    virtual void operator()(const TListPackageStmt *) const override {}
    virtual void operator()(const TEndImportStmt *) const override {
      Infos.push_back({"end import", "end import;", "End an import"});
    }
    virtual void operator()(const TImportStmt *) const override {}
    virtual void operator()(const TListSchemaStmt *) const override {}
    private:
    std::vector<TInfo> &Infos;
  };  // TGetInfo

  /* This is used in GetInfosImpl to make sure that the list of classes provided
     in GetInfos covers all of TTop::TVisitor's cases. */
  template <typename T, typename... Ts>
  struct TCheck;

  /* Base case. */
  template <typename T>
  struct TCheck<T> : public TTop::TVisitor {
    virtual void operator()(const T *) const override {}
  };  // TCheck<T>

  /* Recursive case. */
  template <typename T, typename... Ts>
  struct TCheck : TCheck<Ts...> {
    using TCheck<Ts...>::operator();
    virtual void operator()(const T *) const override {}
  };  // TCheck<T, Ts...>

  /* Check that the Ts... covers the requirements of TTop::TVisitor. */
  template <typename... Ts>
  void GetInfosImpl(const TGetInfo &get_info) {
    TCheck<Ts...>{};
    std::initializer_list<int>(
        {(get_info(static_cast<const Ts *>(nullptr)), 0)...});
  }

  /* Returns the vector of 'TInfo's. */
  const auto &GetInfos() {
    static std::vector<TInfo> infos = []() {
      std::vector<TInfo> result;
      result.push_back({"help", "help", "Display this text"});
      result.push_back({"?", "?", "Alias for 'help'"});
      GetInfosImpl<TImage,
                   TTailStmt,
                   TSetTtlStmt,
                   TTryStmt,
                   TSetUserIdStmt,
                   TPovStatusStmt,
                   TPovConsStmt,
                   TBeginImportStmt,
                   TInstallStmt,
                   TResumeSessionStmt,
                   TCompileStmt,
                   TEchoStmt,
                   TNewSessionStmt,
                   TUninstallStmt,
                   TExitStmt,
                   TGetSourceStmt,
                   TListPackageStmt,
                   TEndImportStmt,
                   TImportStmt,
                   TListSchemaStmt>(TGetInfo(result));
      return result;
    }();
    return infos;
  }
}

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
    : TLog::TCmd::TMeta("The Orly client shell.") {
  Param(
      &TCmd::ServerAddress, "server_address", Optional, "server_address\0sa\0",
      "The address where the Orly server can be found."
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
    : Client(make_shared<TClient>(cmd.ServerAddress, cmd.SessionId, seconds(cmd.TimeToLive))) {}

TRepl::~TRepl() {
  assert(this);
}

int TRepl::Run() {
  assert(this);
  Repl("orly> ", GetInfos(), [this](const string &str) {
    bool keep_going = true;
    try {
      ParseStmtStr(str.c_str(), [this, &keep_going](const TStmt *stmt) { keep_going = InterpretStmt(stmt, Client); });
    }
    catch (const exception &ex) {
      cout << ex.what() << endl;
    }
    return keep_going;
  });
  if (Client->GetSessionId()) {
    cout << "(session_id = " << Client->GetSessionId() << ", time_to_live = " << Client->GetTimeToLive().count() << " sec(s))" << endl;
  } else {
    cout << "(no session)" << endl;
  }
  return EXIT_SUCCESS;
}
