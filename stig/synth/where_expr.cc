/* <stig/synth/where_expr.cc>

   Implements <stig/synth/where_expr.h>.

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

#include <stig/synth/where_expr.h>

#include <base/assert_true.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Synth;

TWhereExpr::TWhereExpr(
    const TExprFactory *expr_factory,
    const Package::Syntax::TWhereExpr *where_expr)
      : TScope(Base::AssertTrue(expr_factory)->OuterScope),
        WhereExpr(Base::AssertTrue(where_expr)) {
  TExprFactory local_expr_factory = *expr_factory;
  local_expr_factory.OuterScope = this;
  TLocalDefFactory::NewDefs(&local_expr_factory, WhereExpr->GetOptDefSeq());
  Expr = local_expr_factory.NewExpr(WhereExpr->GetExpr());
}

TWhereExpr::~TWhereExpr() {
  assert(this);
  delete Expr;
}

Expr::TExpr::TPtr TWhereExpr::Build() const {
  assert(this);
  assert(Expr);
  Symbol->SetExpr(Expr->Build());
  return Symbol;
}

void TWhereExpr::BuildSymbol() {
  assert(this);
  assert(!Symbol);
  Symbol = Expr::TWhere::New(GetPosRange(WhereExpr));
}

void TWhereExpr::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  cb(this);
  Expr->ForEachInnerScope(cb);
}

void TWhereExpr::ForEachControlledRef(const std::function<void (TAnyRef &)> &cb) const {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachRef(cb);
}

Symbol::TScope::TPtr TWhereExpr::GetScopeSymbol() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}

Expr::TWhere::TPtr TWhereExpr::GetSymbol() const {
  assert(this);
  assert(Symbol);
  return Symbol;
}

bool TWhereExpr::HasSymbol() const {
  assert(this);
  return Symbol.get();
}

void TWhereExpr::TLocalDefFactory::NewDefs(
    const TExprFactory *expr_factory,
    const Package::Syntax::TOptDefSeq *opt_def_seq) {
  TWhereExpr::TLocalDefFactory(expr_factory).TDefFactory::NewDefs(opt_def_seq);
}

TWhereExpr::TLocalDefFactory::TLocalDefFactory(const TExprFactory *expr_factory)
    : TDefFactory(expr_factory) {}

void TWhereExpr::TLocalDefFactory::operator()(const Package::Syntax::TInstallerDef *that) const {
  assert(this);
  OnTopLevel("an installer", GetPosRange(that));
}

void TWhereExpr::TLocalDefFactory::operator()(const Package::Syntax::TUninstallerDef *that) const {
  assert(this);
  OnTopLevel("an uninstaller", GetPosRange(that));
}

void TWhereExpr::TLocalDefFactory::operator()(const Package::Syntax::TUpgraderDef *that) const {
  assert(this);
  OnTopLevel("an upgrader", GetPosRange(that));
}

void TWhereExpr::TLocalDefFactory::OnTopLevel(const char *desc, const TPosRange &pos_range) const {
  assert(this);
  assert(desc);
  assert(&pos_range);
  Tools::Nycr::TError::TBuilder(pos_range) << desc << " is not allowed within a where clause";
}
