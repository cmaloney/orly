/* <tools/nycr/kind.cc>

   Implements <tools/nycr/kind.h>.

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

#include <tools/nycr/kind.h>

#include <cassert>

#include <base/no_default_case.h>
#include <tools/nycr/all_decls.h>

using namespace std;
using namespace Tools::Nycr;

Symbol::TBase *TKind::TryGetBaseSymbol() const {
  assert(this);
  TBase *base = *Super;
  return base ? base->GetSymbolAsBase() : 0;
}

TKind::TKind(const Syntax::TName *name, const Syntax::TOptSuper *opt_super)
    : TDecl(name) {
  Super.Set<Syntax::TSuper, Syntax::TNoSuper>(opt_super);
}

void TKind::ForEachPred(int pass, const function<void (TDecl *)> &cb) {
  assert(this);
  assert(&cb);
  switch (pass) {
    case 1: {
      break;
    }
    case 2: {
      cb(*Super);
      break;
    }
    case 3: {
      break;
    }
    NO_DEFAULT_CASE;
  }
}

void TKind::ForEachRef(const function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  cb(Super);
}

const char *TDecl::TInfo<TKind>::Name = "kind";
