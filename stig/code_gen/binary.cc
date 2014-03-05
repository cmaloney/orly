/* <stig/code_gen/binary.cc>

   Implements <stig/code_gen/binary.h>

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

#include <stig/code_gen/binary.h>

using namespace Stig::CodeGen;

TBinary::TBinary(const L0::TPackage *package, const Type::TType &ret_type, TOp op, const TInline::TPtr &lhs, const TInline::TPtr &rhs)
      : TInline(package, ret_type), Op(op), Lhs(lhs), Rhs(rhs) {
  assert(&lhs);
  assert(lhs);
  assert(&rhs);
  assert(rhs);
}

void TBinary::WriteExpr(TCppPrinter &out) const {
  switch (Op) {
    case Add: Infix(out, '+');
      break;
    case And: Call(out, "And");
      break;
    case Div: Call(out, "Div");
      break;
    case EqEq: Call(out, "EqEq");
      break;
    case Exponent: Call(out, "pow");
      break;
    case Gt: Call(out, "Gt");
      break;
    case GtEq: Call(out, "GtEq");
      break;
    case In: CallReverse(out, "Contains");
      break;
    case Intersection: Infix(out, '&');
      break;
    case IsKnownExpr: Call(out, "IsKnownExpr");
      break;
    case Lt: Call(out, "Lt");
      break;
    case LtEq: Call(out, "LtEq");
      break;
    case Modulo: Infix(out, '%');
      break;
    case Mult: Infix(out, '*');
      break;
    case Neq: Call(out, "Neq");
      break;
    case Or: Call(out, "Or");
      break;
    case Sub: Infix(out, '-');
      break;
    case SymmetricDiff: Infix(out, '^');
      break;
    case Union: Infix(out, '|');
      break;
    case Xor: Call(out, "Xor");
      break;
  }
}

void TBinary::Template(TCppPrinter &out, const char *name) const {
  assert(&out);
  assert(name);
  out << name << '<' << GetReturnType() << ">(" << Lhs << ", " << Rhs << ')';
}

void TBinary::Infix(TCppPrinter &out, char op) const {
  assert(this);
  assert(&out);
  out << '(' << Lhs << ' ' << op << ' ' << Rhs << ')';
}

void TBinary::Call(TCppPrinter &out, const char *func) const {
  assert(&out);
  assert(func);
  out << func << '(' << Lhs << ", " << Rhs << ')';
}

void TBinary::CallReverse(TCppPrinter &out, const char *func) const {
  assert(&out);
  assert(func);
  out << func << '(' << Rhs << ", " << Lhs << ')';
}