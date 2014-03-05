/* <stig/code_gen/scope.cc>

   Implements <stig/code_gen/scope.h>

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

#include <stig/code_gen/scope.h>

#include <stig/code_gen/effect.h>
#include <stig/code_gen/interner.h>
#include <stig/code_gen/effect.h>

using namespace Stig::CodeGen;


TIdScope::TPtr TIdScope::New() {
  return TPtr(new TIdScope());
}

TId<TIdKind::Arg> TIdScope::NewArg() {
  assert(this);
  return Arg.New();
}

TId<TIdKind::Func> TIdScope::NewFunc() {
  assert(this);
  return Func.New();
}

TId<TIdKind::Var> TIdScope::NewVar() {
  assert(this);
  return Var.New();
}

TCodeScope::TCodeScope(const TIdScope::TPtr &id_scope)
    : IdScope(id_scope), Interner(new TInterner(this)), Stmts(new TStmtBlock()) {}

TCodeScope::~TCodeScope() {
  delete Interner;
}

void TCodeScope::AddAssertion(const std::string &name, const TInline::TPtr &assertion) {
  assert(this);
  assert(&name);
  assert(&assertion);

  Assertions.push_back(std::make_pair(name, assertion));
}

/* Makes a new id, and adds the given inline to our list of locals. */
void TCodeScope::AddLocal(const TInline::TPtr &inline_) {
  assert(this);
  assert(&inline_);

  //Fast exit if we already have been common subexpression eliminated.
  if(inline_->HasId()) {
    return;
  }
  //TODO: Re-enable after sorting out dependency ordering issues.
  /*
  inline_->SetCommonSubexpressionId(IdScope->NewVar());
  Locals.push_back(inline_);
  */
}

TIdScope::TPtr TCodeScope::GetIdScope() const {
  assert(this);

  return IdScope;
}

TInterner *TCodeScope::GetInterner() const {
  assert(this);
  return Interner;
}

const TCodeScope::TLocals &TCodeScope::GetLocals() const {
  assert(this);

  return Locals;
}

TStmtBlock &TCodeScope::GetStmts() {
  assert(this);

  return *Stmts;
}
const TStmtBlock &TCodeScope::GetStmts() const {
  assert(this);

  return *Stmts;
}


TId<TIdKind::Arg> TCodeScope::NewArg() {
  assert(this);

  return IdScope->NewArg();
}

void TCodeScope::WriteStart(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  for(auto &it: Locals) {
    out << "auto " << it->GetId() << " = ";
    it->WriteExpr(out);
    out << ';' << Eol;
  }

  if (!Locals.empty()) {
    out << Eol;
  }

  for(auto &it: Assertions) {
    out << "Assert(\"" << it.first << "\", " << it.second << ");" << Eol;
  }

  if(!Assertions.empty()) {
    out << Eol;
  }

  if(!Stmts->IsEmpty()) {
    Stmts->Write(out);
    out << Eol;
  }

}