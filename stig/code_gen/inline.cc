/* <stig/code_gen/inline.cc>

   Implements <stig/code_gen/inline.h>

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

#include <stig/code_gen/inline.h>

#include <stig/type.h>

using namespace Stig::CodeGen;


/* Writes the common subexpression eliminated variant of this inline. */
void TInline::Write(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  if (Id) {
    out << *Id;
  } else {
    WriteExpr(out);
  }
}

const TId<TIdKind::Var> &TInline::GetId() const {
  assert(this);
  assert(Id);

  return *Id;
}

Stig::Type::TType TInline::GetReturnType() const {
  assert(this);

  return ReturnType;
}

bool TInline::HasId() const {
  assert(this);
  return Id;
}

//Used by common sub expression eliminator.
void TInline::SetCommonSubexpressionId(TId<TIdKind::Var> &&id) const {
  assert(this);
  assert(!Id);

  Id = std::move(id);
}

TInline::TInline(const L0::TPackage *package, Type::TType type) : Package(package), ReturnType(type) {}

TCppPrinter &Stig::CodeGen::operator<<(TCppPrinter &out, const std::shared_ptr<const Stig::CodeGen::TInline> &ptr) {
  assert(&ptr);
  assert(ptr);

  ptr->Write(out);

  return out;
}