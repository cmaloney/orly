/* <tools/nycr/build.cc>

   Implements <tools/nycr/symbol/build.h>.

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

#include <tools/nycr/build.h>

#include <tools/nycr/base.h>
#include <tools/nycr/decl.h>
#include <tools/nycr/keyword.h>
#include <tools/nycr/language.h>
#include <tools/nycr/operator.h>
#include <tools/nycr/prec_level.h>
#include <tools/nycr/rule.h>

using namespace Tools::Nycr;

void Tools::Nycr::Build(const Syntax::TNycr *nycr) {
  assert(nycr);
  TDecl::DeleteEach();
  try {
    const Syntax::TOptDeclSeq *opt_decl_seq = nycr->GetOptDeclSeq();
    for (;;) {
      class TDeclVisitor : public Syntax::TDecl::TVisitor {
        public:
        TDeclVisitor() {}
        virtual void operator()(const Syntax::TBadDecl *) const {}
        virtual void operator()(const Syntax::TBase *that) const      { new TBase(that); }
        virtual void operator()(const Syntax::TKeyword *that) const   { new TKeyword(that); }
        virtual void operator()(const Syntax::TLanguage *that) const  { new TLanguage(that); }
        virtual void operator()(const Syntax::TOper *that) const      { new TOperator(that); }
        virtual void operator()(const Syntax::TPrecLevel *that) const { new TPrecLevel(that); }
        virtual void operator()(const Syntax::TRule *that) const      { new TRule(that); }
      };
      const Syntax::TDeclSeq *decl_seq = TryGetNode<Syntax::TDeclSeq, Syntax::TNoDeclSeq>(opt_decl_seq);
      if (!decl_seq) {
        break;
      }
      decl_seq->GetDecl()->Accept(TDeclVisitor());
      opt_decl_seq = decl_seq->GetOptDeclSeq();
    }
    TDecl::BindAndBuildEach();
  } catch (...) {
    TDecl::DeleteEach();
    throw;
  }
  TDecl::DeleteEach();
}
