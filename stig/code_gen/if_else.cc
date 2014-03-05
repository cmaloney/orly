/* <stig/code_gen/if_else.cc>

   Implements <stig/code_gen/if_else.h>

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

#include <stig/code_gen/if_else.h>

using namespace Stig;
using namespace Stig::CodeGen;

TIfElse::TPtr TIfElse::New(
    const L0::TPackage *package,
    const Type::TType &ret_type,
    const Expr::TExpr::TPtr &true_case,
    const TInline::TPtr &predicate,
    const Expr::TExpr::TPtr &false_case) {
  return TPtr(new TIfElse(package, ret_type, true_case, predicate, false_case));
}

void TIfElse::WriteExpr(TCppPrinter &out) const {
  assert(&out);

  //TODO: Building into the sum type if true case and false case return different types.
  out << "Predicate<" << GetReturnType() << ">(ctx, " << Predicate << ", " << True;

  out << ", /* else */" << False;

  out << ')';
}

TIfElse::TIfElse(const L0::TPackage *package,
                 const Type::TType &ret_type,
                 const Expr::TExpr::TPtr &true_case,
                 const TInline::TPtr &predicate,
                 const Expr::TExpr::TPtr &false_case)
  : TInline(package, ret_type), Predicate(predicate), True(TInlineScope::New(package, true_case)),
    False(TInlineScope::New(package, false_case)), InDependsOn(false) {}