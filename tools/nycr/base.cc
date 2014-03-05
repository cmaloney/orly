/* <tools/nycr/base.cc>

   Implements <tools/nycr/base.h>.

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

#include <tools/nycr/base.h>

#include <base/no_default_case.h>
#include <tools/nycr/all_decls.h>

using namespace std;
using namespace Tools::Nycr;

TBase::TBase(const Syntax::TBase *decl)
  : TKind(decl->GetName(), decl->GetOptSuper()), Symbol(0) {}

Symbol::TKind *TBase::GetSymbolAsKind() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}

bool TBase::Build(int pass) {
  assert(this);
  bool is_done;
  switch (pass) {
    case 1: {
      is_done = false;
      break;
    }
    case 2: {
      Symbol = new Symbol::TBase(GetNameText(), TryGetBaseSymbol());
      is_done = true;
      break;
    }
    NO_DEFAULT_CASE;
  }
  return is_done;
}

const char *TDecl::TInfo<TBase>::Name = "base";
