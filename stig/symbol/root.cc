/* <stig/symbol/root.cc>

   Implements <stig/symbol/root.h>

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

#include <stig/symbol/root.h>

using namespace Stig;
using namespace Stig::Symbol;

TRoot::TRoot()  // Should only be used by TFunction
    : Expr(nullptr) {}

TRoot::TRoot(const Expr::TExpr::TPtr &expr)
    : Expr(Base::AssertTrue(expr)) {
  Expr->SetExprParent(this);
}

TRoot::~TRoot() {
  assert(this);
  if (Expr) {
    Expr->UnsetExprParent(this);
  }
}

const Expr::TExpr::TPtr &TRoot::GetExpr() const {
  assert(this);
  assert(Expr);
  return Expr;
}

void TRoot::SetExpr(const Expr::TExpr::TPtr &expr) {
  assert(this);
  assert(expr);
  assert(!Expr);
  Expr = expr;
  Expr->SetExprParent(this);
}
