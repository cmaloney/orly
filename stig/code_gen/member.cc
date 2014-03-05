/* <stig/code_gen/member.cc>

   Implements <stig/code_gen/member.h>

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

#include <stig/code_gen/member.h>

#include <stig/type/mutable.h>

using namespace Stig::CodeGen;

void TMutableRewrap::WriteExpr(TCppPrinter &out) const {
  assert(this);
  assert(&out);
  if(GetReturnType().Is<Type::TMutable>()) {
    out << "[](const " << Src->GetReturnType() << " &src) {" << Eol
        << "  return MakeMutable(src.GetOptAddr(), src.GetParts(), src.GetVal()";
    WritePostfixOp(out);
    out << ");" << Eol
        << "}(" << Src << ")";
  } else {
    out << Src;
    WritePostfixOp(out);
  }
}

TMutableRewrap::TMutableRewrap(const L0::TPackage *package, const Type::TType &return_type, const TInline::TPtr &src)
    : TInline(package, return_type), Src(src) {}

TAddrMember::TAddrMember(const L0::TPackage *package, const Type::TType &return_type, const TInline::TPtr &addr, size_t index)
    : TMutableRewrap(package, return_type, addr), Index(index) {}

void TAddrMember::WritePartId(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  out << Index;
}

void TAddrMember::WritePostfixOp(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  out << ".Get<" << Index << ">()";
}

void TAddrMember::WriteExpr(TCppPrinter &out) const {
  assert(this);
  assert(&out);
  if(GetReturnType().Is<Type::TMutable>()) {
    out << "[](const " << Src->GetReturnType() << " &src) {" << Eol
        << "  return MakeMutable(src.GetOptAddr(), src.GetParts(), Rt::UnwrapDesc(std::get<" << Index << ">(src.GetVal())));" << Eol
        << "}(" << Src << ")";
  } else {
    out << "Rt::UnwrapDesc(std::get<" << Index << ">(" << Src << "))";
  }
}

TObjMember::TObjMember(const L0::TPackage *package, const Type::TType &return_type, const TInline::TPtr &obj, const std::string &name)
    : TMutableRewrap(package, return_type, obj), Name(name) {}

void TObjMember::WritePartId(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  out << Name;
}

void TObjMember::WritePostfixOp(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  out << ".GetV" << Name << "()";
}
