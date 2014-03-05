/* <tools/nycr/prec_level.cc>

   Implements <tools/nycr/prec_level.h>.

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

#include <tools/nycr/prec_level.h>

#include <cassert>

#include <base/no_default_case.h>
#include <tools/nycr/all_decls.h>
#include <tools/nycr/symbol/prec_level.h>

using namespace Tools::Nycr;

size_t TPrecLevel::NextIdx = 0;

TPrecLevel::TPrecLevel(const Syntax::TPrecLevel *decl)
  : TDecl(decl->GetName()), Idx(NextIdx++), Symbol(0) {}

bool TPrecLevel::Build(int pass) {
  assert(this);
  switch (pass) {
    case 1: {
      Symbol = new Symbol::TPrecLevel(GetNameText(), Idx);
      break;
    }
    NO_DEFAULT_CASE;
  }
  return true;
}

const char *TDecl::TInfo<TPrecLevel>::Name = "prec level";
