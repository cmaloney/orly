/* <stig/client/program/interpret_stmt.cc>

   Implements <stig/client/program/interpret_stmt.h>.

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

#include <stig/client/program/interpret_stmt.h>

#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>

#include <base/debug_log.h>
#include <stig/atom/kit2.h>
#include <stig/atom/suprena.h>
#include <stig/client/program/translate_expr.h>
#include <stig/sabot/state_dumper.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::Atom;
using namespace Stig::Client::Program;

bool Stig::Client::Program::InterpretStmt(const TStmt *stmt, const shared_ptr<TClient> &client) {
  class visitor_t final : public TStmt::TVisitor {
    public:
    visitor_t(bool &result, const shared_ptr<TClient> &client) : Result(result), Client(client) {}
    // non-db
    virtual void operator()(const TExitStmt *) const override {
      Result = false;
    }
    virtual void operator()(const TEchoStmt *that) const override {
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      ostringstream strm;
      Sabot::State::TAny::TWrapper(NewStateSabot(that->GetExpr(), state_alloc))->Accept(Sabot::TStateDumper(strm));
      cout << **(Client->Echo(strm.str())) << endl;
    }
    virtual void operator()(const TTailStmt */*that*/) const override {
      Client->TailGlobalPov()->Sync();
    }
    // import
    virtual void operator()(const TBeginImportStmt *) const override {
      Client->BeginImport()->Sync();
    }
    virtual void operator()(const TEndImportStmt *) const override {
      Client->EndImport()->Sync();
    }
    virtual void operator()(const TImportStmt *) const override {
      // cout << **(Client->Import(that->GetFile()->GetLexeme().AsDoubleQuotedString(), that->GetXactCount()->GetLexeme().AsInt())) << endl;
    }
    // packages
    virtual void operator()(const TInstallStmt *that) const override {
      vector<string> package_name;
      uint64_t version;
      TranslatePackage(package_name, version, that->GetPackageName());
      Client->InstallPackage(package_name, version)->Sync();
    }
    virtual void operator()(const TUninstallStmt *that) const override {
      vector<string> package_name;
      uint64_t version;
      TranslatePackage(package_name, version, that->GetPackageName());
      Client->UnInstallPackage(package_name, version)->Sync();
    }
    // pov
    virtual void operator()(const TPovConsStmt *that) const override {
      bool is_safe = dynamic_cast<const TSafeGuarantee *>(that->GetPovGuarantee()) != nullptr;
      bool is_shared = dynamic_cast<const TSharedKind *>(that->GetPovKind()) != nullptr;
      Base::TOpt<Base::TUuid> parent_id;
      auto parent = dynamic_cast<const TParent *>(that->GetOptParent());
      if (parent) {
        auto id_expr = parent->GetIdExpr();
        string tmp = id_expr->GetLexeme().GetText().substr(1, id_expr->GetLexeme().GetText().size() - 2);
        parent_id = TUuid(tmp.c_str());
      }
      shared_ptr<Rpc::TFuture<TUuid>> pov_id;
      if (is_safe) {
        if (is_shared) {
          pov_id = Client->NewSafeSharedPov(parent_id);
        } else {
          pov_id = Client->NewSafePrivatePov(parent_id);
        }
      } else {
        if (is_shared) {
          pov_id = Client->NewFastSharedPov(parent_id);
        } else {
          pov_id = Client->NewFastPrivatePov(parent_id);
        }
      }
      cout << "pov_id = " << **pov_id << endl;
    }
    virtual void operator()(const TPovStatusStmt *that) const override {
      bool paused = dynamic_cast<const TPauseKind *>(that->GetStatusKind()) != nullptr;
      string tmp = that->GetIdExpr()->GetLexeme().GetText().substr(1, that->GetIdExpr()->GetLexeme().GetText().size() - 2);
      Base::TUuid id(tmp.c_str());
      if (paused) {
        Client->PausePov(id)->Sync();
      } else {
        Client->UnpausePov(id)->Sync();
      }
    }
    // try
    virtual void operator()(const TTryStmt *that) const override {
      string tmp = that->GetPovId()->GetLexeme().GetText().substr(1, that->GetPovId()->GetLexeme().GetText().size() - 2);
      Base::TUuid pov_id(tmp.c_str());
      vector<string> fq_name;
      TranslatePathName(fq_name, that->GetPackage());
      TClosure closure(that->GetMethodName()->GetLexeme().GetText());
      auto list = dynamic_cast<const TObjMemberList *>(that->GetArgs()->GetOptObjMemberList());
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      while (list) {
        auto member = list->GetObjMember();
        Sabot::State::TAny::TWrapper state(NewStateSabot(member->GetExpr(), state_alloc));
        closure.AddArgBySabot(member->GetName()->GetLexeme().GetText(), state);
        auto tail = dynamic_cast<const TObjMemberListTail *>(list->GetOptObjMemberListTail());
        list = tail ? tail->GetObjMemberList() : nullptr;
      }
      auto result = Client->Try(pov_id, fq_name, closure);
      Sabot::State::TAny::TWrapper((*result)->GetValue().NewState((*result)->GetArena().get(), state_alloc))->Accept(Sabot::TStateDumper(cout));
      cout << endl;
    }
    // misc
    virtual void operator()(const TSetTtlStmt *that) const override {
      string tmp = that->GetIdExpr()->GetLexeme().GetText().substr(1, that->GetIdExpr()->GetLexeme().GetText().size() - 2);
      Base::TUuid id(tmp.c_str());
      int ttl = that->GetIntExpr()->GetLexeme().AsInt();
      Client->SetTimeToLive(id, chrono::seconds(ttl))->Sync();
    }
    virtual void operator()(const TSetUserIdStmt *that) const override {
      string tmp = that->GetIdExpr()->GetLexeme().GetText().substr(1, that->GetIdExpr()->GetLexeme().GetText().size() - 2);
      Base::TUuid id(tmp.c_str());
      Client->SetUserId(id)->Sync();
    }
    #if 0
    virtual void operator()(const TCallStmt *that) const override {
      vector<string> function_name;
      TranslatePathName(function_name, that->GetNameList());
      TFastArena arena;
      Atom::TCore obj_core;
      TranslateExpr(&arena, &obj_core, that->GetObjExpr());
      cout << "Will Call : ";
      bool f = true;
      for (const auto &iter : function_name) {

        cout << (f ? "" : "/") << iter;;
        f = false;
      }
      cout << " with [" << Atom::TAtom(obj_core, &arena) << "]" << endl;
    }
    #endif
    private:
    bool &Result;
    const shared_ptr<TClient> &Client;
  };
  assert(stmt);
  bool result = true;
  stmt->Accept(visitor_t(result, client));
  return result;
}
