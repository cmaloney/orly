/* <stig/synth/func_def.cc>

   Implements <stig/synth/func_def.h>.

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

#include <stig/synth/func_def.h>

#include <base/assert_true.h>
#include <base/no_default_case.h>
#include <stig/symbol/given_param_def.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>
#include <stig/synth/new_type.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Synth;

TFuncDef::TFuncDef(TScope *scope, const Package::Syntax::TFuncDef *func_def)
    : TDef(scope, Base::AssertTrue(func_def)->GetName()),
      FuncDef(Base::AssertTrue(func_def)),
      Expr(nullptr) {}

TFuncDef::~TFuncDef() {
  assert(this);
  delete Expr;
}

void TFuncDef::AddParamName(const TName &name) {
  assert(this);
  auto result = ParamNames.emplace(name);
  if (!result.second) {
    Tools::Nycr::TError::TBuilder(name.GetPosRange())
        << '"' << name.GetText() << "\" is a duplicate parameter name";
  }
}

TAction TFuncDef::Build(int pass) {
  assert(this);
  TAction action;
  switch (pass) {
    case 1: {
      action = Continue;
      break;
    }
    case 2: {
      Symbol = Symbol::TFunction::New(
                 GetOuterScope()->GetScopeSymbol(), /* scope */
                 GetName().GetText(), /* name */
                 GetPosRange(FuncDef)); /* pos_range */
      Symbol::TResultDef::New(Symbol, GetName().GetText(), GetPosRange(FuncDef));
      action = Continue;
      break;
    }
    case 3: {
      BuildSecondarySymbol();
      action = Continue;
      break;
    }
    case 4: {
      Symbol->SetExpr(Expr->Build());
      action = Finish;
      break;
    }
    NO_DEFAULT_CASE;
  }
  return action;
}

void TFuncDef::BuildSecondarySymbol() {}

void TFuncDef::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(Expr);
  Expr->ForEachInnerScope(cb);
}

void TFuncDef::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(Expr);
  Expr->ForEachRef(cb);
}

Symbol::TFunction::TPtr TFuncDef::GetSymbol() const {
  assert(this);
  return Symbol;
}

void TFuncDef::SetExpr(TExpr *expr) {
  assert(this);
  assert(expr);
  assert(!Expr);
  Expr = expr;
}

const char *TDef::TInfo<TFuncDef>::Name = "a function defintion";

TParamFuncDef::TParamFuncDef(
    const TExprFactory *expr_factory,
    const Package::Syntax::TFuncDef *func_def,
    const Package::Syntax::TGivenExpr *given_expr)
      : TFuncDef(Base::AssertTrue(expr_factory)->OuterScope, func_def),
        EnclosingFunc(Base::AssertTrue(expr_factory->EnclosingFunc)),
        GivenExpr(Base::AssertTrue(given_expr)),
        Type(NewType(GivenExpr->GetType())) {
  EnclosingFunc->AddParamName(TName(FuncDef->GetName()));
  TExprFactory local_expr_factory = *expr_factory;
  local_expr_factory.ParamFunc = this;
  SetExpr(local_expr_factory.NewExpr(FuncDef->GetExpr()));
}

void TParamFuncDef::BuildSecondarySymbol() {
  assert(this);
  ParamDefSymbol = Symbol::TGivenParamDef::New(
                       EnclosingFunc->GetSymbol(), /* function */
                       GetName().GetText(),        /* name */
                       Type->GetSymbolicType(),    /* type */
                       GetPosRange(GivenExpr));    /* pos_range */
}

const Symbol::TParamDef::TPtr &TParamFuncDef::GetParamDefSymbol() const {
  assert(this);
  return ParamDefSymbol;
}

TType *TParamFuncDef::GetType() const {
  assert(this);
  return Type;
}

const char *TDef::TInfo<TParamFuncDef>::Name = "a parameter function defintion";

TPureFuncDef::TPureFuncDef(const TExprFactory *expr_factory, const Package::Syntax::TFuncDef *func_def)
    : TFuncDef(Base::AssertTrue(expr_factory)->OuterScope, func_def) {
  TExprFactory local_expr_factory = *expr_factory;
  local_expr_factory.EnclosingFunc = this;
  SetExpr(local_expr_factory.NewExpr(FuncDef->GetExpr()));
}
