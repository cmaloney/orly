/* <orly/code_gen/export_func.cc>

   Implements <orly/code_gen/export_func.h>

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/code_gen/export_func.h>

#include <orly/code_gen/scope.h>

using namespace Orly::CodeGen;

//TODO: The namespace passing around here is ugly and feels very, very backwards. Probably we should just have a pointer
//      to the package and get the namespace out of it.
TExportFunc::TPtr TExportFunc::New(const L0::TPackage *package,
                                   const Symbol::TFunction::TPtr &symbol) {
  return TExportFunc::TPtr(new TExportFunc(package, symbol));
}

void TExportFunc::WriteCcName(TCppPrinter &out) const {
  assert(this);

  out << 'F' << GetName();
}

void TExportFunc::WriteName(TCppPrinter &out) const {
  out << Package->GetName();
  if (!Package->GetName().Name.empty()) {
    out << "::";
  }
  WriteCcName(out);
}

//TODO: The seeming double call of the TFunction ctor is sort of fugly. Esp. how we pass around the new IdScope.
TExportFunc::TExportFunc(const L0::TPackage *package, const Symbol::TFunction::TPtr &symbol)
    : TFunction(package, TIdScope::New()), TTopFunc(package), TSymbolFunc(package, symbol, CodeScope.GetIdScope()) {}