/* <stig/synth/literal_expr.cc>

   Implements <stig/synth/literal_expr.h>.

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

#include <stig/synth/literal_expr.h>

#include <base/assert_true.h>
#include <stig/expr/literal.h>
#include <stig/synth/get_pos_range.h>
#include <stig/var.h>
#include <tools/nycr/lexeme.h>

using namespace Stig;
using namespace Stig::Synth;
using namespace Tools::Nycr;

TLiteralExpr::TLiteralExpr(const Package::Syntax::TLiteralExpr *literal_expr)
    : LiteralExpr(Base::AssertTrue(literal_expr)) {}

Expr::TExpr::TPtr TLiteralExpr::Build() const {
  class TLiteralVisitor
      : public Package::Syntax::TLiteral::TVisitor {
    public:
    TLiteralVisitor(Expr::TExpr::TPtr &expr)
        : Expr(expr) {}
    virtual void operator()(const Package::Syntax::TTimeDiffLiteral *that) const {
      SetExpr(Var::TVar(that->GetLexeme().AsTimeDiff()), GetPosRange(that));
    }
    virtual void operator()(const Package::Syntax::TTimePntLiteral *that) const {
      SetExpr(Var::TVar(that->GetLexeme().AsTimePnt()), GetPosRange(that));
    }
    virtual void operator()(const Package::Syntax::TTrueKwd *that) const {
      SetExpr(Var::TVar(true), GetPosRange(that));
    }
    virtual void operator()(const Package::Syntax::TFalseKwd *that) const {
      SetExpr(Var::TVar(false), GetPosRange(that));
    }
    virtual void operator()(const Package::Syntax::TIntLiteral *that) const {
      SetExpr(Var::TVar(that->GetLexeme().AsInt()), GetPosRange(that));
    }
    virtual void operator()(const Package::Syntax::TIdLiteral *that) const {
      SetExpr(Var::TVar(TUUID(TLexeme::RemoveCurlyBraces(that->GetLexeme().GetText()).c_str())), GetPosRange(that));
    }
    virtual void operator()(const Package::Syntax::TRealLiteral *that) const {
      SetExpr(Var::TVar(that->GetLexeme().AsDouble()), GetPosRange(that));
    }
    virtual void operator()(const Package::Syntax::TSingleQuotedStrLiteral *that) const {
      SetExpr(Var::TVar(that->GetLexeme().AsSingleQuotedString()), GetPosRange(that));
    }
    virtual void operator()(const Package::Syntax::TDoubleQuotedRawStrLiteral *that) const {
      SetExpr(Var::TVar(that->GetLexeme().AsDoubleQuotedRawString()), GetPosRange(that));
    }
    virtual void operator()(const Package::Syntax::TSingleQuotedRawStrLiteral *that) const {
      SetExpr(Var::TVar(that->GetLexeme().AsSingleQuotedRawString()), GetPosRange(that));
    }
    virtual void operator()(const Package::Syntax::TDoubleQuotedStrLiteral *that) const {
      SetExpr(Var::TVar(that->GetLexeme().AsDoubleQuotedString()), GetPosRange(that));
    }
    private:
    void SetExpr(const Var::TVar &val, const TPosRange &pos_range) const {
      Expr = Expr::TLiteral::New(val, pos_range);
    }
    Expr::TExpr::TPtr &Expr;
  };  // TLiteralVisitor
  assert(this);
  Expr::TExpr::TPtr expr;
  LiteralExpr->GetLiteral()->Accept(TLiteralVisitor(expr));
  return expr;
}
