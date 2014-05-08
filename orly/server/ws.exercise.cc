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
#include <memory>
#include <unordered_map>

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

class TSessionManager
    : public TWs::TSessionManager {
  public:

  TSessionManager() {}

  virtual TWs::TSessionPin *NewSession() override {
    assert(this);
    TUuid id(TUuid::Best);
    return (Sessions[id] = unique_ptr<TSession>(new TSession(id)))->NewPin();
  }

  virtual TWs::TSessionPin *ResumeSession(const Base::TUuid &id) {
    assert(this);
    auto iter = Sessions.find(id);
    if (iter == Sessions.end()) {
      throw invalid_argument("unknown session");
    }
    return iter->second->NewPin();
  }

  private:

  class TSession final {
    NO_COPY(TSession);
    public:

    explicit TSession(const TUuid &id)
        : Id(id), Pin(nullptr) {}

    const Base::TUuid &GetId() const {
      assert(this);
      return Id;
    }

    TWs::TSessionPin *NewPin() {
      return new TPin(this);
    }

    TUuid NewPov(bool /*is_safe*/, bool /*is_shared*/, const TOpt<TUuid> &/*parent_id*/) {
      assert(this);
      return TUuid(TUuid::Best);
    }

    TMethodResult Try(const TMethodRequest &/*method_request*/) {
      assert(this);
      void *alloc = alloca(Sabot::State::GetMaxStateSize());
      TSuprena arena;
      return TMethodResult(&arena, TCore(98.6, &arena, alloc), TOpt<TTracker>());
    }

    private:

    class TPin final
        : public TWs::TSessionPin {
      public:

      TPin(TSession *session)
          : Session(session) {
        if (session->Pin) {
          throw logic_error("session already pinned");
        }
        session->Pin = this;
      }

      virtual ~TPin() {
        assert(this);
        if (Session->Pin != this) {
          throw logic_error("session pin is messed up");
        }
        Session->Pin = nullptr;
      }

      virtual const TUuid &GetId() const override {
        assert(this);
        return Session->Id;
      }

      virtual TUuid NewPov(bool is_safe, bool is_shared, const TOpt<TUuid> &parent_id) const override {
        assert(this);
        return Session->NewPov(is_safe, is_shared, parent_id);
      }

      virtual TMethodResult Try(const TMethodRequest &method_request) const override {
        assert(this);
        return Session->Try(method_request);
      }

      private:

      TSession *Session;

    };  // TSessionManager::TSession::TPin

    TUuid Id;

    TPin *Pin;

  };  // TSessionManager::TSession

  unordered_map<TUuid, unique_ptr<TSession>> Sessions;

};  // TSessionManager

int main(int, char *[]) {
  THandlerInstaller handle_sigint(SIGINT);
  TMasker mask_all_but_sigint(*TSet(TSet::Exclude, { SIGINT }));
  TSessionManager session_mngr;
  unique_ptr<TWs> ws(TWs::New(&session_mngr, 8081));
  pause();
  return 0;
}
