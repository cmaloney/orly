/* <stig/code_gen/top_func.cc>

   Implements <stig/code_gen/top_func.h>

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

#include <stig/code_gen/top_func.h>

#include <stig/code_gen/scope.h>

using namespace Stig;
using namespace Stig::CodeGen;

bool TTopFunc::IsTopLevel() const {
  return true;
}

void TTopFunc::WriteDecl(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  //NOTE: We prefix with 'F' to make the function name never conflict with a C++ builtin.
  out << GetReturnType() << ' ';
  WriteCcName(out);
  out << "(Stig::Package::TContext &ctx";
  //TODO: The HasArgs here feels sort of hacky...
  if (HasArgs()) {
    out << ", ";
  }
  WriteArgs(out);
  out << ')';
}

void TTopFunc::WriteDef(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  WriteDecl(out);
  WriteBody(out);
}

TTopFunc::TTopFunc(const L0::TPackage *package) : TFunction(package, TIdScope::New()) {}