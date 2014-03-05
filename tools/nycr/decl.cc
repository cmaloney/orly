/* <tools/nycr/decl.cc>

   Implements <tools/nycr/decl/decl.h>.

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

#include <tools/nycr/decl.h>

#include <base/no_default_case.h>
#include <base/thrower.h>
#include <tools/nycr/error.h>

using namespace std;
using namespace Tools::Nycr;

void TDecl::BindAndBuildEach() {
  ForEachUniqueDecl([](TDecl *decl){ decl->Bind(); });
  int pass = 0;
  bool again;
  do {
    if (TError::GetFirstError()) {
      break;
    }
    ++pass;
    again = false;
    ForEachUniqueDecl([pass, &again](TDecl *decl){ decl->BuildPredsAndSelf(pass, again); });
  } while (again);
}

void TDecl::DeleteEach() {
  for (auto iter = DeclsByName.begin(); iter != DeclsByName.end(); ++iter) {
    const vector<TDecl *> &decls = iter->second;
    for (size_t i = 0; i < decls.size(); ++i) {
      delete decls[i];
    }
  }
  DeclsByName.clear();
}

TDecl::TAnyRef::~TAnyRef() {}

void TDecl::TAnyRef::Bind() {
  assert(this);
  assert(!IsBound);
  if (Name) {
    TDecl *decl = TryGetDecl(Name);
    if (decl) {
      const char *decl_type_name = SetDecl(decl);
      if (decl_type_name) {
        TError::TBuilder(Name->GetLexeme().GetPosRange())
            << '"' << Name->GetLexeme().GetText() << "\" does not refer to \"" << decl_type_name << '"';
      }
    }
  }
  IsBound = true;
}

TDecl::TDecl(const Syntax::TName *name)
    : Name(name), Readiness(0), State(Unstarted) {
  assert(name);
  const TLexeme &lexeme = name->GetLexeme();
  const string &name_str = lexeme.GetText();
  static const vector<TDecl *> no_decls;
  vector<TDecl *> &decls = DeclsByName.insert(make_pair(name_str, no_decls)).first->second;
  if (!decls.empty()) {
    TError::TBuilder(lexeme.GetPosRange())
        << "duplicate name \"" << name_str
        << "\" previously declared at " << (decls[0]->Name->GetLexeme().GetPosRange());
  }
  decls.push_back(this);
}

TDecl::~TDecl() {}

void TDecl::ForEachPred(int, const function<void (TDecl *)> &) {}

void TDecl::ForEachRef(const function<void (TAnyRef &)> &) {}

void TDecl::Bind() {
  assert(this);
  ForEachRef([this](TAnyRef &ref){ ref.Bind(); });
}

void TDecl::BuildPredsAndSelf(int pass, bool &again) {
  assert(this);
  assert(&again);
  if (Readiness < pass) {
    switch (State) {
      case Unstarted: {
        State = Started;
        ForEachPred(pass, [pass, &again](TDecl *decl){
          if (decl) {
            decl->BuildPredsAndSelf(pass, again);
          }
        });
        if (Build(pass)) {
          State = Finished;
        } else {
          State = Unstarted;
          again = true;
        }
        Readiness = pass;
        break;
      }
      case Started: {
        THROW << "cycle in build depenencies, pass " << pass << ", \"" << Name->GetLexeme().GetText() << '"';
      }
      case Finished: {
        Readiness = pass;
        break;
      }
      NO_DEFAULT_CASE;
    }
  }
}

void TDecl::ForEachUniqueDecl(const function<void (TDecl *)> &cb) {
  assert(&cb);
  for (auto iter = DeclsByName.begin(); iter != DeclsByName.end(); ++iter) {
    const vector<TDecl *> &decls = iter->second;
    if (decls.size() == 1) {
      cb(decls[0]);
    }
  }
}

TDecl *TDecl::TryGetDecl(const Syntax::TName *name) {
  assert(name);
  const TLexeme &lexeme = name->GetLexeme();
  const string &text = lexeme.GetText();
  auto iter = DeclsByName.find(text);
  if (iter == DeclsByName.end()) {
    TError::TBuilder(lexeme.GetPosRange()) << '"' << text << "\" is not declared";
    return 0;
  }
  const vector<TDecl *> &decls = iter->second;
  assert(!decls.empty());
  if (decls.size() > 1) {
    TError::TBuilder(lexeme.GetPosRange()) << '"' << text << "\" is declared multiple times";
    return 0;
  }
  return decls[0];
}

map<string, vector<TDecl *>> TDecl::DeclsByName;
