/* <stig/code_gen/binary_scoped_rhs.cc>

   Implements <stig/code_gen/binary_scoped_rhs.h>

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

#include <stig/code_gen/binary_scoped_rhs.h>

using namespace Stig::CodeGen;

TBinaryScopedRhs::TPtr TBinaryScopedRhs::New(const L0::TPackage *package,
                                             const Type::TType &ret_type,
                                             TOp op,
                                             const TInline::TPtr &lhs,
                                             const Expr::TExpr::TPtr &rhs) {
  return TBinaryScopedRhs::TPtr(new TBinaryScopedRhs(package, ret_type, op, lhs, rhs));
}

TBinaryScopedRhs::TBinaryScopedRhs(const L0::TPackage *package,
                                   const Type::TType &ret_type,
                                   TOp op,
                                   const TInline::TPtr &lhs,
                                   const Expr::TExpr::TPtr &rhs)
      : TInline(package, ret_type),
        Op(op),
        Lhs(lhs),
        Rhs(TInlineScope::New(package, rhs)) {
  assert(&lhs);
  assert(lhs);
}

void TBinaryScopedRhs::WriteExpr(TCppPrinter &out) const {
  switch (Op) {
    case AndThen:
      out << "AndThen(" << Lhs << ", " << Rhs << ')';
      break;
    case OrElse:
      out << "OrElse(" << Lhs << ", " << Rhs << ')';
      break;
  }  // switch (Op)
}
