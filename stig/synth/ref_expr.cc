/* <stig/synth/ref_expr.cc>

   Implements <stig/synth/ref_expr.h>.

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

#include <base/assert_true.h>
#include <stig/expr/ref.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/ref_expr.h>

using namespace Stig;
using namespace Stig::Synth;

TRefExpr::TRefExpr(const Package::Syntax::TRefExpr *ref_expr)
    : RefExpr(Base::AssertTrue(ref_expr)),
      FuncDef(RefExpr->GetName()) {}

Expr::TExpr::TPtr TRefExpr::Build() const {
  assert(this);
  return Expr::TRef::New(FuncDef->GetSymbol()->GetResultDefs()[0], GetPosRange(RefExpr));
}

void TRefExpr::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  cb(FuncDef);
}
