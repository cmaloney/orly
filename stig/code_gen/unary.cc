/* <stig/code_gen/unary.cc>

   Implements <stig/code_gen/unary.h>

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

#include <stig/code_gen/unary.h>
#include <stig/type/unwrap.h>

using namespace Stig;
using namespace Stig::CodeGen;

TUnary::TUnary(const L0::TPackage *package,
               const Type::TType &ret_type,
               TOp op,
               const TInline::TPtr &expr)
    : TInline(package, ret_type),
      Op(op),
      Expr(expr) {}

void TUnary::WriteExpr(TCppPrinter &out) const {
  switch(Op) {
    case AddressOf: Postfix(out, "GetOptAddr");
      break;
    case Cast: out << "CastAs<" << GetReturnType() << ", " << Expr->GetReturnType() << ">::Do(" << Expr << ')';
      break;
    case Ceiling: Call(out, "ceil");
      break;
    #if 0
    case Exists: {
      const Base::TUuid &index_id = Package->GetIndexIdFor(Expr->GetReturnType(), );
      char uuid[37];
      index_id.Format(uuid);
      out << "Exists(ctx.GetFlux(), " << Expr << ", Base::TUuid(\"" << uuid << "\")";
      break;
    }
    #endif
    case Floor: Call(out, "floor");
      break;
    case IsEmpty: Call(out, "IsEmpty");
      break;
    case IsKnown: Call(out, "IsKnown");
      break;
    case IsUnknown: Call(out, "IsUnknown");
      break;
    case Known: Postfix(out, "GetVal");
      break;
    case LengthOf: Call(out, "GetSize");
      break;
    case Log: Call(out, "log");
      break;
    case Log2: out << "(log(" << Expr << ")/log(2))";
      break;
    case Log10: Call(out, "log10");
      break;
    case Negative: out << "(-" << Expr << ')';
      break;
    case Not: Call(out, "Not");
      break;
    case Read: {
      const Base::TUuid &index_id = Package->GetIndexIdFor(Expr->GetReturnType(), Type::UnwrapOptional(Type::UnwrapMutable(GetReturnType())));
      char uuid[37];
      index_id.FormatUnderscore(uuid);
      out << "Read<" << Type::UnwrapMutable(GetReturnType()) << ">(ctx.GetFlux(), " << Expr << ", " << TStigNamespace(Package->GetNamespace()) << "::My" << uuid << ")";
      break;
    }
    case ReverseOf: Call(out, "Reverse");
      break;
    case SequenceOf: Call(out, "MakeGenerator");
      break;
    case TimeDiffObj: Call(out, "Rt::Objects::TObjO6i3dayi4hourb10is_forwardi6minutei10nanosecondi6second");
      break;
    case TimePntObj: Call(out, "Rt::Objects::TObjO8i3dayi4houri6minutei5monthi10nanosecondi6secondi10utc_offseti4year");
      break;
    case ToLower: Call(out, "Stig::Rt::ToLower");
      break;
    case ToUpper: Call(out, "Stig::Rt::ToUpper");
      break;
    case UnwrapMutable: Postfix(out, "GetVal");
      break;
  }
}

void TUnary::Call(TCppPrinter &out, const char *func) const {
  assert(&out);
  assert(func);
  out << func << '(' << Expr << ')';
}

void TUnary::Postfix(TCppPrinter &out, const char *func) const {
  assert(&out);
  assert(func);
  out << Expr << '.' << func << "()";
}