/* <stig/code_gen/inline_func.cc>

   Implements <stig/code_gen/inline_func.h>

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

#include <stig/code_gen/inline_func.h>

using namespace Stig::CodeGen;

const TId<TIdKind::Func> &TInlineFunc::GetId() const {
  assert(this);
  return Id;
}

bool TInlineFunc::IsTopLevel() const {
  return false;
}

void TInlineFunc::WriteDecl(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  out << "std::function<" << GetReturnType() << " (";
  WriteArgs(out);
  out << ")> " << Id << "/* " << GetName() << " */";
}

void TInlineFunc::WriteDef(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  WriteName(out);
  out << " = [=, &ctx](";
  WriteArgs(out);
  //The return type here isn't strictly necessary, but it makes the C++ compiler errors better.
  out << ") -> " << GetReturnType();
  WriteBody(out);
}

void TInlineFunc::WriteName(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  out << Id;
}

TInlineFunc::TInlineFunc(const L0::TPackage *package, const TIdScope::TPtr &id_scope) : TFunction(package, id_scope), Id(id_scope->NewFunc()) {}
