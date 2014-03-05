/* <tools/nycr/operator.cc>

   Implements <tools/nycr/operator.h>.

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

#include <tools/nycr/operator.h>

#include <cassert>

#include <base/no_default_case.h>
#include <tools/nycr/all_decls.h>
#include <tools/nycr/symbol/prec_level.h>

using namespace std;
using namespace Tools::Nycr;

TOperator::TOperator(const Syntax::TOper *decl)
    : TAtom(decl->GetName(), decl->GetOptSuper(), decl->GetPattern()), PrecLevel(decl->GetPrecLevelRef()), Symbol(0) {
  class TAssocVisitor : public Syntax::TAssoc::TVisitor {
    public:
    TAssocVisitor(Symbol::TOperator::TAssoc &assoc) : Assoc(assoc) {}
    virtual void operator()(const Syntax::TRightKwd *) const { Assoc = Symbol::TOperator::Right; }
    virtual void operator()(const Syntax::TLeftKwd *) const { Assoc = Symbol::TOperator::Left; }
    virtual void operator()(const Syntax::TNonassocKwd *) const { Assoc = Symbol::TOperator::NonAssoc; }
    private:
    Symbol::TOperator::TAssoc &Assoc;
  };
  decl->GetAssoc()->Accept(TAssocVisitor(Assoc));
}

void TOperator::ForEachRef(const function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  TAtom::ForEachRef(cb);
  cb(PrecLevel);
}

Symbol::TAtom *TOperator::GetSymbolAsAtom() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}

bool TOperator::Build(int pass) {
  assert(this);
  bool is_done;
  switch (pass) {
    case 1: {
      is_done = false;
      break;
    }
    case 2: {
      Symbol = new Symbol::TOperator(GetNameText(), TryGetBaseSymbol(), GetPatternText(), GetPri(), PrecLevel->GetSymbol(), Assoc);
      is_done = true;
      break;
    }
    NO_DEFAULT_CASE;
  }
  return is_done;
}

const char *TDecl::TInfo<TOperator>::Name = "operator";
