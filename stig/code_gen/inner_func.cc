/* <stig/code_gen/inner_func.cc>

   Implements <stig/code_gen/inner_func.h>

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

#include <stig/code_gen/inner_func.h>

using namespace std;
using namespace Stig;
using namespace Stig::CodeGen;

TInnerFunc::TPtr TInnerFunc::New(const L0::TPackage *package, const Symbol::TFunction::TPtr &symbol, const TIdScope::TPtr &id_scope) {
  return TInnerFunc::TPtr(new TInnerFunc(package, symbol, id_scope));
}

/* TODO: The somewhat excessive passing of id_scope here is somewhat excessive/ugly */
TInnerFunc::TInnerFunc(const L0::TPackage *package, const Symbol::TFunction::TPtr &symbol, const TIdScope::TPtr &id_scope)
    : TFunction(package, id_scope), TInlineFunc(package, id_scope), TSymbolFunc(package, symbol, id_scope) {}