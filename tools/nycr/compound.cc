/* <tools/nycr/compound.cc>

   Implements <tools/nycr/compound.h>.

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

#include <tools/nycr/compound.h>

#include <algorithm>
#include <cassert>

#include <tools/nycr/all_decls.h>
#include <tools/nycr/symbol/anonymous_member.h>
#include <tools/nycr/symbol/error_member.h>
#include <tools/nycr/symbol/named_member.h>

using namespace std;
using namespace Tools::Nycr;

TCompound::TCompound(const Syntax::TName *name, const Syntax::TOptSuper *opt_super, const Syntax::TOptRhs *opt_rhs)
    : TFinal(name, opt_super) {
  const Syntax::TRhs *rhs = TryGetNode<Syntax::TRhs, Syntax::TNoRhs>(opt_rhs);
  if (rhs) {
    const Syntax::TMemberSeq *member_seq = rhs->GetMemberSeq();
    do {
      class TMemberVisitor : public Syntax::TMember::TVisitor {
        public:
        TMemberVisitor(TMember *&member) : Member(member) {}
        virtual void operator()(const Syntax::TAnonymousMember *that) const { Member = new TAnonymousMember(that); }
        virtual void operator()(const Syntax::TNamedMember *that) const     { Member = new TNamedMember(that); }
        virtual void operator()(const Syntax::TErrorMember *) const         { Member = new TErrorMember(); }
        private:
        TMember *&Member;
      };
      TMember *member;
      member_seq->GetMember()->Accept(TMemberVisitor(member));
      Members.push_back(member);
      member_seq = TryGetNode<Syntax::TMemberSeq, Syntax::TNoMemberSeq>(member_seq->GetOptMemberSeq());
    } while (member_seq);
    Operator.Set<Syntax::TOperRef, Syntax::TNoOperRef>(rhs->GetOptOperRef());
  }
}

TCompound::~TCompound() {
  assert(this);
  for (auto iter = Members.begin(); iter != Members.end(); ++iter) {
    delete *iter;
  }
}

void TCompound::BuildMembers() {
  assert(this);
  Symbol::TCompound *compound = GetSymbolAsCompound();
  assert(compound);
  for (auto iter = Members.begin(); iter != Members.end(); ++iter) {
    (*iter)->Build(compound);
  }
  GetSymbolAsCompound()->SetOperator(TryGetOperator());
}

Symbol::TKind *TCompound::GetSymbolAsKind() const {
  assert(this);
  return GetSymbolAsCompound();
}

void TCompound::ForEachRef(const function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  TFinal::ForEachRef(cb);
  for (auto iter = Members.begin(); iter != Members.end(); ++iter) {
    (*iter)->ForEachRef(cb);
  }
  cb(Operator);
}

void TCompound::TErrorMember::Build(Symbol::TCompound *compound) {
  assert(this);
  new Symbol::TErrorMember(compound);
}

void TCompound::TErrorMember::ForEachRef(const std::function<void (TAnyRef &)> &) {}

void TCompound::TMemberWithKind::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  cb(Kind);
}

void TCompound::TAnonymousMember::Build(Symbol::TCompound *compound) {
  assert(this);
  new Symbol::TAnonymousMember(compound, GetKind());
}

void TCompound::TNamedMember::Build(Symbol::TCompound *compound) {
  assert(this);
  new Symbol::TNamedMember(compound, GetKind(), Name->GetLexeme().GetText());
}

const char *TDecl::TInfo<TCompound>::Name = "compound";
