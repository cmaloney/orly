/* <orly/server/ws.cc>

   Implements <orly/server/ws.h>.

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

#include <cassert>
#include <map>
#include <memory>
#include <thread>
#include <sstream>
#include <string>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <orly/client/program/parse_stmt.h>
#include <orly/client/program/translate_expr.h>
#include <orly/indy/key.h>
#include <orly/sabot/state_dumper.h>
#include <orly/sabot/type_dumper.h>

using namespace std;
using namespace std::placeholders;

using namespace Base;

using namespace Orly;
using namespace Orly::Client::Program;
using namespace Orly::Sabot;
using namespace Orly::Server;

/* The implementation of the TWs interface declared in the header. */
class TWsImpl final
    : public TWs {
  public:

  /* Starts up the server. */
  TWsImpl(TSessionManager *session_mngr, in_port_t port_number)
      : SessionManager(session_mngr) {
    assert(session_mngr);
    AsioServer.clear_access_channels(websocketpp::log::alevel::all);
    AsioServer.clear_error_channels(websocketpp::log::elevel::all);
    AsioServer.init_asio();
    AsioServer.set_open_handler(bind(&TWsImpl::OnOpen, this, _1));
    AsioServer.set_close_handler(bind(&TWsImpl::OnClose, this, _1));
    AsioServer.set_message_handler(bind(&TWsImpl::OnMsg, this, _1, _2));
    AsioServer.listen(port_number);
    AsioServer.start_accept();
    BgThread = thread(&TWsImpl::BgMain, this);      
  }

  /* Shuts down the server. */
  virtual ~TWsImpl() {
    assert(this);
    AsioServer.stop();
    BgThread.join();
  }

  private:

  /* Pull in some types from the websocket ASIO implementation. */
  using TAsioServer = websocketpp::server<websocketpp::config::asio>;
  using TMsgPtr = TAsioServer::message_ptr;
  using TConnHndl = websocketpp::connection_hdl;

  /* Constructed by TWsImpl::OnOpen(), destroyed by TWsImpl::OnClose(). */
  class TConn final {
    NO_COPY(TConn);
    public:

    /* Cache the args. */
    TConn(TWsImpl *ws, TConnHndl hndl)
        : Ws(ws), Hndl(hndl), Exiting(false) {}

    /* True iff. we have processed an exit statement and so want the TCP
       connection to close. */
    bool IsExiting() const noexcept {
      assert(this);
      return Exiting;
    }

    /* Called by TWsImpl::OnMsg(). Parses and interprets a statement sent
       to us as a text message. */
    string OnMsg(TMsgPtr msg) {
      assert(this);
      assert(msg);
      ostringstream strm;
      ParseStmtStr(
          msg->get_payload().c_str(),
          [this, &strm](const TStmt *stmt) {
             stmt->Accept(TStmtVisitor(this, strm));
          }
      );
      return strm.str();
    }

    private:

    /* Interpret a statement. */
    class TStmtVisitor final
        : public TStmt::TVisitor {
      public:

      /* Cache the args. */
      TStmtVisitor(TConn *conn, ostream &strm)
          : Conn(conn), Strm(strm) {}

      /* Echo. */
      virtual void operator()(const TEchoStmt *stmt) const override {
        assert(this);
        assert(stmt);
        void *alloc = alloca(SabotStateSize);
        TWrapper(NewStateSabot(stmt->GetExpr(), alloc))->Accept(TStateDumper(Strm));
      }

      /* Exit. */
      virtual void operator()(const TExitStmt *) const override {
        assert(this);
        Conn->Exiting = true;
      }

      /* New session. */
      virtual void operator()(const TNewSessionStmt *) const override {
        assert(this);
        if (Conn->Session) {
          throw invalid_argument("session already established");
        }
        Conn->Session.reset(Conn->Ws->SessionManager->NewSession());
        Strm << Conn->Session->GetId();
      }

      /* Resume session. */
      virtual void operator()(const TResumeSessionStmt *stmt) const override {
        assert(this);
        assert(stmt);
        if (Conn->Session) {
          throw invalid_argument("session already established");
        }
        Conn->Session.reset(Conn->Ws->SessionManager->ResumeSession(Translate(stmt->GetIdExpr())));
        Strm << Conn->Session->GetId();
      }

      /* Set user id. */
      virtual void operator()(const TSetUserIdStmt *stmt) const override {
        assert(this);
        assert(stmt);
        TUuid user_id = Translate(stmt->GetIdExpr());
        GetSession()->SetUserId(user_id);
      }

      /* Set time-to-live. */
      virtual void operator()(const TSetTtlStmt *stmt) const override {
        assert(this);
        assert(stmt);
        TUuid durable_id = Translate(stmt->GetIdExpr());
        chrono::seconds ttl(stmt->GetIntExpr()->GetLexeme().AsInt());
        GetSession()->SetTtl(durable_id, ttl);
      }

      /* Install package. */
      virtual void operator()(const TInstallStmt *stmt) const override {
        assert(this);
        assert(stmt);
        vector<string> package_name;
        uint64_t version;
        TranslatePackage(package_name, version, stmt->GetPackageName());
        GetSession()->InstallPackage(package_name, version);
      }

      /* Uninstall package. */
      virtual void operator()(const TUninstallStmt *stmt) const override {
        assert(this);
        assert(stmt);
        vector<string> package_name;
        uint64_t version;
        TranslatePackage(package_name, version, stmt->GetPackageName());
        GetSession()->UninstallPackage(package_name, version);
      }

      /* New pov. */
      virtual void operator()(const TPovConsStmt *stmt) const override {
        assert(this);
        assert(stmt);
        bool is_safe = dynamic_cast<const TSafeGuarantee *>(stmt->GetPovGuarantee()) != nullptr;
        bool is_shared = dynamic_cast<const TSharedKind *>(stmt->GetPovKind()) != nullptr;
        TOpt<TUuid> parent_id;
        auto parent = dynamic_cast<const TParent *>(stmt->GetOptParent());
        if (parent) {
          parent_id = Translate(parent->GetIdExpr());
        }
        Strm << GetSession()->NewPov(is_safe, is_shared, parent_id);
      }

      /* Try call. */
      virtual void operator()(const TTryStmt *stmt) const override {
        assert(this);
        assert(stmt);
        TUuid pov_id = Translate(stmt->GetPovId());
        vector<string> fq_name;
        TranslatePathName(fq_name, stmt->GetPackage());
        TClosure closure(stmt->GetMethodName()->GetLexeme().GetText());
        auto list = dynamic_cast<const TObjMemberList *>(stmt->GetArgs()->GetOptObjMemberList());
        void *alloc = alloca(SabotStateSize);
        while (list) {
          auto member = list->GetObjMember();
          TWrapper state(NewStateSabot(member->GetExpr(), alloc));
          closure.AddArgBySabot(member->GetName()->GetLexeme().GetText(), state);
          auto tail = dynamic_cast<const TObjMemberListTail *>(list->GetOptObjMemberListTail());
          list = tail ? tail->GetObjMemberList() : nullptr;
        }
        TMethodResult result = GetSession()->Try(TMethodRequest(pov_id, fq_name, closure));
        Indy::TKey(result.GetValue(), result.GetArena().get()).Dump(Strm);
      }

      /* Pause or unpause a pov. */
      virtual void operator()(const TPovStatusStmt *stmt) const override {
        assert(this);
        assert(stmt);
        bool is_pause = dynamic_cast<const TPauseKind *>(stmt->GetStatusKind()) != nullptr;
        TUuid pov_id = Translate(stmt->GetIdExpr());
        if (is_pause) {
          GetSession()->PausePov(pov_id);
        } else {
          GetSession()->UnpausePov(pov_id);
        }
      }

      /* Tail the global pov. */
      virtual void operator()(const TTailStmt *) const override {
        assert(this);
        GetSession()->Tail();
      }

      /* Begin bulk import mode. */
      virtual void operator()(const TBeginImportStmt *) const override {
        assert(this);
        GetSession()->BeginImport();
      }

      /* End bulk import mode. */
      virtual void operator()(const TEndImportStmt *) const override {
        assert(this);
        GetSession()->EndImport();
      }

      /* Import bulk data. */
      virtual void operator()(const TImportStmt *stmt) const override {
        assert(this);
        assert(stmt);
        string path = stmt->GetFile()->GetLexeme().AsDoubleQuotedString();
        uint64_t count = stmt->GetXactCount()->GetLexeme().AsInt();
        GetSession()->Import(path, count);
      }

      private:

      /* The size used to alloca() space for a sabot of state. */
      static constexpr auto SabotStateSize = Orly::Sabot::State::GetMaxStateSize();

      /* Pull in some stuff from sabot. */
      using TStateDumper = Orly::Sabot::TStateDumper;
      using TWrapper = Orly::Sabot::State::TAny::TWrapper;

      /* The session we are using in this connection.  Never null.
         If no session has yet been established for this connection, throw. */
      TSessionPin *GetSession() const {
        assert(this);
        if (!Conn->Session) {
          throw invalid_argument("session not yet established");
        }
        return Conn->Session.get();
      }

      /* Translate an Orlyscript id into a TUuid. */
      static TUuid Translate(const TIdExpr *id_expr) {
        assert(id_expr);
        return TUuid(id_expr->GetLexeme().GetText().substr(1, id_expr->GetLexeme().GetText().size() - 2).c_str());
      }

      /* The connection on whose behalf we're translating. */
      TConn *Conn;

      /* The stream to which to write the result of our interpretation. */
      ostream &Strm;

    };  // TWsImpl::TStmtVisitor

    /* The server of which this connection is a part. */
    TWsImpl *Ws;

    /* The handle of this connection, used to identify its TCP socket. */
    TConnHndl Hndl;

    /* See accessor. */
    bool Exiting;

    /* The session established for this connection, if any. */
    unique_ptr<TSessionPin> Session;

  };  // TWsImpl::TConn

  /* The entry point of the background thread launched from the ctor.
     This will run until stopped by the dtor. */
  void BgMain() {
    assert(this);
    AsioServer.run();
    for (const auto &item: Conns) {
      delete item.second;
    }
  }

  /* Called by the ws framework when it accepts a new TCP connection. */
  void OnOpen(TConnHndl conn_hndl) {
    assert(this);
    unique_ptr<TConn> temp(new TConn(this, conn_hndl));
    if (!Conns.insert(make_pair(conn_hndl, temp.get())).second) {
      throw invalid_argument("cannot reopen open connection");
    }
    temp.release();
  }
   
  /* Called by the ws framework when it closes a TCP connection (or when the
     connection is closed by the client). */
  void OnClose(TConnHndl conn_hndl) {
    assert(this);
    auto iter = Conns.find(conn_hndl);
    if (iter == Conns.end()) {
      throw invalid_argument("cannot close unopen connection");
    }
    delete iter->second;
    Conns.erase(iter);
  }

  /* Called by the ws framework each time a message arrives. */
  void OnMsg(TConnHndl conn_hndl, TMsgPtr msg) {
    assert(this);
    /* Find the connection object established by the OnOpen() call. */
    auto iter = Conns.find(conn_hndl);
    if (iter == Conns.end()) {
      throw invalid_argument("cannot use unopen connection");
    }
    /* Pass the message to the connection object for processing. */
    TConn *conn = iter->second;
    const char *prefix;
    string reply;
    try {
      reply = conn->OnMsg(msg);
      prefix = "ok";
    } catch (const exception &ex) {
      reply = ex.what();
      prefix = "ex";
    }
    /* Format the reply and send it back to the client. */
    ostringstream strm;
    strm << prefix;
    if (!reply.empty()) {
      strm << ':' << reply;
    }
    AsioServer.send(conn_hndl, strm.str(), websocketpp::frame::opcode::text);    
    if (conn->IsExiting()) {
      AsioServer.close(conn_hndl, websocketpp::close::status::normal, "");
    }
  }

  /* The session manager interface passed to us at construction time. */
  TSessionManager *SessionManager;

  /* This object handles the mechanics of the websocket protocol. */
  TAsioServer AsioServer;

  /* The background thread launched by the ctor. */
  thread BgThread;

  /* The sessions currently open. */
  map<TConnHndl, TConn *, owner_less<TConnHndl>> Conns;

};  // TWsImpl

TWs *TWs::New(TSessionManager *session_mngr, in_port_t port_number) {
  return new TWsImpl(session_mngr, port_number);
}