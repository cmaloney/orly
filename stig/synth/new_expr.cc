/* <stig/synth/new_expr.cc>

   Implements <stig/synth/new_expr.h>.

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

#include <stig/synth/new_expr.h>

#include <cassert>
#include <map>

#include <base/assert_true.h>
#include <stig/error.h>
#include <stig/expr.h>
#include <stig/synth/addr_ctor.h>
#include <stig/synth/addr_member_expr.h>
#include <stig/synth/affix_expr.h>
#include <stig/synth/assert_expr.h>
#include <stig/synth/collated_by_expr.h>
#include <stig/synth/collected_by_expr.h>
#include <stig/synth/db_keys_expr.h>
#include <stig/synth/dict_ctor.h>
#include <stig/synth/effecting_expr.h>
#include <stig/synth/empty_ctor.h>
#include <stig/synth/exists_ctor.h>
#include <stig/synth/filter_expr.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/given_expr.h>
#include <stig/synth/if_else_expr.h>
#include <stig/synth/infix_expr.h>
#include <stig/synth/lhs_expr.h>
#include <stig/synth/list_ctor.h>
#include <stig/synth/literal_expr.h>
#include <stig/synth/now_expr.h>
#include <stig/synth/obj_ctor.h>
#include <stig/synth/obj_member_expr.h>
#include <stig/synth/postfix_call.h>
#include <stig/synth/postfix_cast.h>
#include <stig/synth/postfix_is_known.h>
#include <stig/synth/postfix_is_known_expr.h>
#include <stig/synth/postfix_slice.h>
#include <stig/synth/prefix_start.h>
#include <stig/synth/random_int.h>
#include <stig/synth/range_ctor.h>
#include <stig/synth/read_expr.h>
#include <stig/synth/reduce_expr.h>
#include <stig/synth/ref_expr.h>
#include <stig/synth/rhs_expr.h>
#include <stig/synth/set_ctor.h>
#include <stig/synth/session_id_expr.h>
#include <stig/synth/sort_expr.h>
#include <stig/synth/startable_expr.h>
#include <stig/synth/str_replace.h>
#include <stig/synth/that_expr.h>
#include <stig/synth/thatable_expr.h>
#include <stig/synth/time_diff_ctor.h>
#include <stig/synth/time_pnt_ctor.h>
#include <stig/synth/unknown_ctor.h>
#include <stig/synth/user_id_expr.h>
#include <stig/synth/where_expr.h>
#include <stig/synth/while_expr.h>

using namespace Base;
using namespace Stig;
using namespace Stig::Package;
using namespace Stig::Synth;

/* TODO */
TExprFactory::TExprFactory(
    TScope *outer_scope,
    TFuncDef *enclosing_func,
    TParamFuncDef *param_func,
    TLhsRhsableExpr *lhsrhsable_expr,
    TStartableExpr *startable_expr,
    TThatableExpr *thatable_expr)
      : EnclosingFunc(enclosing_func),
        OuterScope(Base::AssertTrue(outer_scope)),
        ParamFunc(param_func),
        LhsRhsableExpr(lhsrhsable_expr),
        StartableExpr(startable_expr),
        ThatableExpr(thatable_expr) {}

TExpr *TExprFactory::NewExpr(const Package::Syntax::TExpr *root) const {
  class TExprVisitor
      : public Syntax::TExpr::TVisitor {
    public:
    TExprVisitor(const TExprFactory *expr_factory, TExpr *&out)
        : ExprFactory(Base::AssertTrue(expr_factory)), Out(out) {}
    virtual void operator()(const Syntax::TAddrCtor *that) const { Out = new TAddrCtor(ExprFactory, that); }
    virtual void operator()(const Syntax::TAssertExpr *that) const { Out = new TAssertExpr(ExprFactory, that); }
    virtual void operator()(const Syntax::TBuiltInCeiling *that) const { OnAffix(that->GetExpr(), Expr::TCeiling::New, GetPosRange(that->GetCeilingKwd())); }
    virtual void operator()(const Syntax::TBuiltInFloor *that) const { OnAffix(that->GetExpr(), Expr::TFloor::New, GetPosRange(that->GetFloorKwd())); }
    virtual void operator()(const Syntax::TBuiltInLog *that) const { OnAffix(that->GetExpr(), Expr::TLog::New, GetPosRange(that->GetLogKwd())); }
    virtual void operator()(const Syntax::TBuiltInLog2 *that) const { OnAffix(that->GetExpr(), Expr::TLog2::New, GetPosRange(that->GetLog2Kwd())); }
    virtual void operator()(const Syntax::TBuiltInLog10 *that) const { OnAffix(that->GetExpr(), Expr::TLog10::New, GetPosRange(that->GetLog10Kwd())); }
    //    virtual void operator()(const Syntax::TBuiltInReplace *that) const { OnAffix(that->GetExpr(), Expr::TStrReplace::New, GetPosRange(that->GetReplaceKwd())); }
    virtual void operator()(const Syntax::TBuiltInReplace *that) const { Out = new TStrReplace(that); };
    virtual void operator()(const Syntax::TBuiltInRandom *that) const { Out = new TRandomInt(that); }
    virtual void operator()(const Syntax::TBuiltInToLower *that) const { OnAffix(that->GetExpr(), Expr::TToLower::New, GetPosRange(that->GetToLowerKwd())); }
    virtual void operator()(const Syntax::TBuiltInToUpper *that) const { OnAffix(that->GetExpr(), Expr::TToUpper::New, GetPosRange(that->GetToUpperKwd())); }
    virtual void operator()(const Syntax::TCollatedByExpr *that) const {
      Out = new TCollatedByExpr(ExprFactory, that);
    }
    virtual void operator()(const Syntax::TCollectedByExpr *that) const {
      Out = new TCollectedByExpr(ExprFactory, that);
    }
    virtual void operator()(const Syntax::TDbKeysExpr *that) const { Out = new TDbKeysExpr(ExprFactory, that); }
    virtual void operator()(const Syntax::TDictCtor *that) const { Out = new TDictCtor(ExprFactory, that); }
    virtual void operator()(const Syntax::TEffectingExpr *that) const { Out = new TEffectingExpr(ExprFactory, that); }
    virtual void operator()(const Syntax::TEmptyCtor *that) const { Out = new TEmptyCtor(that); }
    virtual void operator()(const Syntax::TGivenExpr *that) const { Out = new TGivenExpr(ExprFactory->ParamFunc, that); }
    virtual void operator()(const Syntax::TIfExpr *that) const {
      TExpr *true_expr = nullptr;
      TExpr *pred_expr = nullptr;
      TExpr *false_expr = nullptr;
      try {
        true_expr = ExprFactory->NewExpr(that->GetTrueCase());
        pred_expr = ExprFactory->NewExpr(that->GetPredicate());
        false_expr = ExprFactory->NewExpr(that->GetFalseCase());
        Out = new TIfElseExpr(true_expr, pred_expr, false_expr, that);
      } catch (...) {
        delete true_expr;
        delete pred_expr;
        delete false_expr;
      }
    }
    virtual void operator()(const Syntax::TInfixAndThen *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TAndThen::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixBitwiseAnd *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TIntersection::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixBitwiseOr *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TUnion::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixBitwiseXor *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TSymmetricDiff::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixDiv *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TDiv::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixEq *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TEqEq::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixExp *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TExp::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixFilter *that) const { Out = new TFilterExpr(ExprFactory, that); }
    virtual void operator()(const Syntax::TInfixGt *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TGt::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixGtEq *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TGtEq::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixIn *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TIn::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixLogicalAnd *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TAnd::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixLogicalOr *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TOr::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixLogicalXor *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TXor::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixLt *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TLt::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixLtEq *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TLtEq::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixMatch *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TMatch::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixMinus *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TSub::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixMod *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TMod::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixMul *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TMult::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixNeq *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TNeq::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixOrElse *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TOrElse::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixPlus *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TAdd::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixReduce *that) const { Out = new TReduceExpr(ExprFactory, that); }
    virtual void operator()(const Syntax::TInfixSort *that) const { Out = new TSortExpr(ExprFactory, that); }
    virtual void operator()(const Syntax::TInfixTake *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TTake::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixSkip *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TSkip::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixSplit *that) const { OnInfix(that->GetLhs(), that->GetRhs(), Expr::TSplit::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TInfixWhile *that) const { Out = new TWhileExpr(ExprFactory, that); }
    virtual void operator()(const Syntax::TLhsExpr *that) const { Out = new TLhsExpr(ExprFactory, that); }
    virtual void operator()(const Syntax::TListCtor *that) const { Out = new TListCtor(ExprFactory, that); }
    virtual void operator()(const Syntax::TLiteralExpr *that) const { Out = new TLiteralExpr(that); }
    virtual void operator()(const Syntax::TNowExpr *that) const { Out = new TNowExpr(that); }
    virtual void operator()(const Syntax::TObjCtor *that) const { Out = new TObjCtor(ExprFactory, that); }
    virtual void operator()(const Syntax::TParenExpr *that) const { Out = ExprFactory->NewExpr(that->GetExpr()); }
    virtual void operator()(const Syntax::TPostfixAddrMember *that) const { Out = new TAddrMemberExpr(ExprFactory, that); }
    virtual void operator()(const Syntax::TPostfixCall *that) const { Out = new TPostfixCall(ExprFactory, that); }
    virtual void operator()(const Syntax::TPostfixCast *that) const { Out = new TPostfixCast(ExprFactory, that); }
    virtual void operator()(const Syntax::TPostfixIsEmpty *that) const { OnAffix(that->GetExpr(), Expr::TIsEmpty::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TPostfixIsKnown *that) const { Out = new TPostfixIsKnown(ExprFactory, that); }
    virtual void operator()(const Syntax::TPostfixIsKnownExpr *that) const { Out = new TPostfixIsKnownExpr(ExprFactory, that); }
    virtual void operator()(const Syntax::TPostfixIsUnknown *that) const { OnAffix(that->GetExpr(), Expr::TIsUnknown::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TPostfixObjMember *that) const { Out = new TObjMemberExpr(ExprFactory, that); }
    virtual void operator()(const Syntax::TPostfixOptCheckpoint *that) const {
      throw TCompileError(HERE, GetPosRange(that), "expr ? is not allowed in stig programs. It is available only in checkpoints.");
    }
    virtual void operator()(const Syntax::TPostfixSlice *that) const { Out = new TPostfixSlice(ExprFactory, that); }
    virtual void operator()(const Syntax::TPrefixAddrOf *that) const { OnAffix(that->GetExpr(), Expr::TAddrOf::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TPrefixExists *that) const { Out = new TExistsCtor(ExprFactory, that); }
    virtual void operator()(const Syntax::TPrefixKnown  *that) const { OnAffix(that->GetExpr(), Expr::TKnown::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TPrefixLengthOf *that) const { OnAffix(that->GetExpr(), Expr::TLengthOf::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TPrefixLogicalNot *that) const { OnAffix(that->GetExpr(), Expr::TNot::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TPrefixMinus *that) const { OnAffix(that->GetExpr(), Expr::TNegative::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TPrefixPlus *that) const { OnAffix(that->GetExpr(), Expr::TPositive::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TPrefixReverseOf *that) const { OnAffix(that->GetExpr(), Expr::TReverseOf::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TPrefixSequence *that) const { OnAffix(that->GetExpr(), Expr::TSequenceOf::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TPrefixStart *that) const { Out = new TPrefixStart(ExprFactory, that); }
    virtual void operator()(const Syntax::TPrefixTimeObj *that) const { OnAffix(that->GetExpr(), Expr::TTimeObj::New, GetPosRange(that)); }
    virtual void operator()(const Syntax::TRangeCtor *that) const { Out = new TRangeCtor(ExprFactory, that); }
    virtual void operator()(const Syntax::TReadExpr *that) const { Out = new TReadExpr(ExprFactory, that); }
    virtual void operator()(const Syntax::TRefExpr *that) const { Out = new TRefExpr(that); }
    virtual void operator()(const Syntax::TRhsExpr *that) const { Out = new TRhsExpr(ExprFactory, that); }
    virtual void operator()(const Syntax::TSessionIdExpr *that) const { Out = new TSessionIdExpr(that); }
    virtual void operator()(const Syntax::TSetCtor *that) const { Out = new TSetCtor(ExprFactory, that); }
    virtual void operator()(const Syntax::TThatExpr *that) const { Out = new TThatExpr(ExprFactory, that); }
    virtual void operator()(const Syntax::TTimeDiffCtor *that) const { Out = new TTimeDiffCtor(that); }
    virtual void operator()(const Syntax::TTimePntCtor *that) const { Out = new TTimePntCtor(that); }
    virtual void operator()(const Syntax::TUnknownCtor *that) const { Out = new TUnknownCtor(that); }
    virtual void operator()(const Syntax::TUserIdExpr *that) const { Out = new TUserIdExpr(that); }
    virtual void operator()(const Syntax::TWhereExpr *that) const { Out = new TWhereExpr(ExprFactory, that); }
    private:
    void OnAffix(const Syntax::TExpr *that, TAffixExpr::TNew new_, const TPosRange &pos_range) const {
      TExpr *expr = nullptr;
      try {
        expr = ExprFactory->NewExpr(that);
        Out = new TAffixExpr(expr, new_, pos_range);
      } catch (...) {
        delete expr;
        throw;
      }
    }
    void OnInfix(const Syntax::TExpr *lhs, const Syntax::TExpr *rhs, TInfixExpr::TNew new_, const TPosRange &pos_range) const {
      TExpr *lhs_expr = nullptr;
      TExpr *rhs_expr = nullptr;
      try {
        lhs_expr = ExprFactory->NewExpr(lhs);
        rhs_expr = ExprFactory->NewExpr(rhs);
        Out = new TInfixExpr(lhs_expr, rhs_expr, new_, pos_range);
      } catch (...) {
        delete lhs_expr;
        delete rhs_expr;
        throw;
      }
    }
    const TExprFactory *ExprFactory;
    TExpr *&Out;
  };  // TExprVisitor
  assert(root);
  TExpr *expr = nullptr;
  try {
    root->Accept(TExprVisitor(this, expr));
  } catch (...) {
    delete expr;
    throw;
  }
  return expr;
}
