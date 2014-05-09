/* <orly/server/ws.exercise.cc>

   Exercise for <orly/server/ws.h>.

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
#include <cstdlib>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <unistd.h>

#include <orly/atom/suprena.h>
#include <signal/handler_installer.h>
#include <signal/masker.h>
#include <signal/set.h>

using namespace std;
using namespace Base;
using namespace Orly;
using namespace Orly::Atom;
using namespace Orly::Server;
using namespace Signal;

/* Our finalization of TWs::TSessionManager. */
class TSessionManager
    : public TWs::TSessionManager {
  public:

  /* Do-little. */
  TSessionManager() {}

  /* Construct a new session and pin it. */
  virtual TWs::TSessionPin *NewSession() override {
    assert(this);
    TUuid id(TUuid::Best);
    return (Sessions[id] = unique_ptr<TSession>(new TSession(id)))->NewPin();
  }

  /* Look up an existing session and pin it. */
  virtual TWs::TSessionPin *ResumeSession(const Base::TUuid &id) {
    assert(this);
    auto iter = Sessions.find(id);
    if (iter == Sessions.end()) {
      throw invalid_argument("unknown session");
    }
    return iter->second->NewPin();
  }

  private:

  /* A session object, which survives multiple connect/disconnect events.
     In a real server, this would have a time-to-live and be managed as part of an
     in-memory cache.  But we're a fake server so we just let them hang around
     forever. */
  class TSession final {
    NO_COPY(TSession);
    public:

    /* Cache the id as our own and start out without a pin. */
    explicit TSession(const TUuid &id)
        : Id(id), Pin(nullptr) {}

    /* Do-little. */
    void BeginImport() const {}

    /* Do-little. */
    void EndImport() const {}

    /* The id by which we may be looked up and resumed later. */
    const Base::TUuid &GetId() const {
      assert(this);
      return Id;
    }

    /* Do-little. */
    void Import(const string &/*path*/, uint64_t /*count*/) const {}

    /* Do-little. */
    void InstallPackage(const vector<string> &/*name*/, uint64_t /*version*/) {}

    /* Create a new pin in this session.  If it's already pinned, throw. */
    TWs::TSessionPin *NewPin() {
      return new TPin(this);
    }

    /* We fake this by just generating a random id. */
    TUuid NewPov(bool /*is_safe*/, bool /*is_shared*/, const TOpt<TUuid> &/*parent_id*/) {
      assert(this);
      return TUuid(TUuid::Best);
    }

    /* Do-little. */
    void PausePov(const Base::TUuid &/*pov_id*/) const {}

    /* Do-little. */
    void SetTtl(const TUuid &/*durable_id*/, const chrono::seconds &/*ttl*/) const {}

    /* Do-little. */
    void SetUserId(const TUuid &/*user_id*/) const {}

    /* Do-little. */
    void Tail() const {}

    /* We fake this by just always returning 98.6. */
    TMethodResult Try(const TMethodRequest &/*method_request*/) {
      assert(this);
      void *alloc = alloca(Sabot::State::GetMaxStateSize());
      TSuprena arena;
      return TMethodResult(&arena, TCore(98.6, &arena, alloc), TOpt<TTracker>());
    }

    /* Do-little. */
    void UninstallPackage(const vector<string> &/*name*/, uint64_t /*version*/) {}

    /* Do-little. */
    void UnpausePov(const Base::TUuid &/*pov_id*/) const {}

    private:

    /* Our finalization of TWs::TSessionPin.
       It redirects request to the session object it is pinning. */
    class TPin final
        : public TWs::TSessionPin {
      public:

      /* Pin the given session. */
      TPin(TSession *session)
          : Session(session) {
        if (session->Pin) {
          throw logic_error("session already pinned");
        }
        session->Pin = this;
      }

      /* Unpin the given session.  In a real server, the session would now
            (1) be available to be removed from memory and
            (2) start running its time-to live clock, at the end of which it
                would be available to be removed from disk. */
      virtual ~TPin() {
        assert(this);
        if (Session->Pin != this) {
          throw logic_error("session pin is messed up");
        }
        Session->Pin = nullptr;
      }

      /* Pass-throughs to the session object we have pinned. */
      virtual void BeginImport() const override { Session->BeginImport(); }
      virtual void EndImport() const override { Session->EndImport(); }
      virtual const TUuid &GetId() const override { return Session->GetId(); }
      virtual void Import(const string &path, uint64_t count) const override { Session->Import(path, count); }
      virtual void InstallPackage(const vector<string> &name, uint64_t version) const override { Session->InstallPackage(name, version); }
      virtual TUuid NewPov(bool is_safe, bool is_shared, const TOpt<TUuid> &parent_id) const override { return Session->NewPov(is_safe, is_shared, parent_id); }
      virtual void PausePov(const TUuid &pov_id) const override { Session->PausePov(pov_id); }
      virtual void SetTtl(const TUuid &durable_id, const chrono::seconds &ttl) const override { Session->SetTtl(durable_id, ttl); }
      virtual void SetUserId(const TUuid &user_id) const override { Session->SetUserId(user_id); }
      virtual void Tail() const override { Session->Tail(); }
      virtual TMethodResult Try(const TMethodRequest &method_request) const override { return Session->Try(method_request); }
      virtual void UninstallPackage(const vector<string> &name, uint64_t version) const override { Session->UninstallPackage(name, version); }
      virtual void UnpausePov(const TUuid &pov_id) const override { Session->UnpausePov(pov_id); }

      private:

      /* The session object we have pinned. */
      TSession *Session;

    };  // TSessionManager::TSession::TPin

    /* See accessor. */
    TUuid Id;

    /* Our current pin, if any. */
    TPin *Pin;

  };  // TSessionManager::TSession

  /* All the sessions currently in memory. */
  unordered_map<TUuid, unique_ptr<TSession>> Sessions;

};  // TSessionManager

int main(int argc, char *argv[]) {
  /* Get our port number from the command line, or use the default. */
  in_port_t port_number = (argc >= 2) ? atoi(argv[1]) : 8080;
  /* Handle SIGINT. */
  THandlerInstaller handle_sigint(SIGINT);
  TMasker mask_all_but_sigint(*TSet(TSet::Exclude, { SIGINT }));
  /* Make an el-fake-o server and run the websocket interface against it. */
  TSessionManager session_mngr;
  unique_ptr<TWs> ws(TWs::New(&session_mngr, port_number));
  /* Wait to be told to stop. */
  pause();
  return EXIT_SUCCESS;
}
