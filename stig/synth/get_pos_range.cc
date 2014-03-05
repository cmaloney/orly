/* <stig/synth/get_pos_range.cc>

   Implements <stig/synth/get_pos_range.h>

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

#include <stig/synth/get_pos_range.h>

#include <stig/synth/cst_utils.h>

using namespace Stig;

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TAddrCtor *that) {
  return Stig::Synth::GetPosRange(that->GetOpenAddr(), that->GetCloseAddr());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TAssertExpr *that) {
  return Stig::Synth::GetPosRange(that->GetOpenParen(), that->GetCloseBrace());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TBuiltInRandom *that) {
  return Stig::Synth::GetPosRange(that->GetRandomKwd());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TBuiltInReplace *that) {
  return Stig::Synth::GetPosRange(that->GetReplaceKwd());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TCollatedByExpr *that) {
  return Stig::Synth::GetPosRange(that->GetSeq(), that->GetHaving());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TCollectedByExpr *that) {
  return Stig::Synth::GetPosRange(that->GetSeq(), that->GetCollect());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TDbKeysExpr *that) {
  return Stig::Synth::GetPosRange(that->GetKeysKwd(), that->GetCloseAddr());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TDeleteStmt *that) {
  return Stig::Synth::GetPosRange(that->GetDeleteKwd(), that->GetSemi());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TDictCtor *that) {
  return Stig::Synth::GetPosRange(that->GetOpenBrace(), that->GetCloseBrace());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TEffectingExpr *that) {
  return Stig::Synth::GetPosRange(that->GetOpenParen(), that->GetStmtBlock()->GetCloseBrace());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TEmptyCtor *that) {
  return Stig::Synth::GetPosRange(that->GetEmptyKwd() /*, that->GetType() */);
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TFreeDbKeysMember *that) {
  return Stig::Synth::GetPosRange(that->GetFreeKwd(), that->GetCloseParen());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TFuncDef *that) {
  return Stig::Synth::GetPosRange(that->GetName(), that->GetSemi());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TGivenExpr *that) {
  return Stig::Synth::GetPosRange(that->GetGivenKwd(), that->GetCloseParen());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TIfExpr *that) {
  return Stig::Synth::GetPosRange(that->GetOpenParen(), that->GetCloseParen());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TIfStmt *that) {
  const Package::Syntax::TCloseBrace *close_brace = that->GetStmtBlock()->GetCloseBrace();
  ForEach<Package::Syntax::TElseIfClause>(that->GetOptElseIfClauseSeq(),
      [&close_brace](const Package::Syntax::TElseIfClause *else_if_clause) -> bool {
        close_brace = else_if_clause->GetStmtBlock()->GetCloseBrace();
        return true;
      });
  auto else_clause = TryGetNode<Package::Syntax::TElseClause, Package::Syntax::TNoElseClause>(that->GetOptElseClause());
  if (else_clause) {
    close_brace = else_clause->GetStmtBlock()->GetCloseBrace();
  }
  return Stig::Synth::GetPosRange(that->GetIfKwd(), close_brace);
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixAndThen *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixBitwiseAnd *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixBitwiseOr *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixBitwiseXor *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixDiv *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixEq *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixExp *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixFilter *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixGt *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixGtEq *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixIn *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixLogicalAnd *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixLogicalOr *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixLogicalXor *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixLt *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixLtEq *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixMatch *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixMinus *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixMod *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixMul *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixNeq *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixOrElse *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixPlus *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixReduce *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixSort *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixTake *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixSkip *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixSplit *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInfixWhile *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TInstallerDef *that) {
  return Stig::Synth::GetPosRange(that->GetPackageKwd(), that->GetSemi());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TLabeledTestCase *that) {
  return Stig::Synth::GetPosRange(that->GetName(), that->GetSemi());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TLhsExpr *that) {
  return Stig::Synth::GetPosRange(that->GetLhsKwd());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TListCtor *that) {
  return Stig::Synth::GetPosRange(that->GetOpenBracket(), that->GetCloseBracket());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TLiteralExpr *that) {
  class TVisitor
    : public Package::Syntax::TLiteral::TVisitor {
      NO_COPY_SEMANTICS(TVisitor);
      public:
      TVisitor(TPosRange &pos_range)
        : PosRange(pos_range) {}
      virtual void operator()(const Package::Syntax::TDoubleQuotedRawStrLiteral *that) const { PosRange = Stig::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TDoubleQuotedStrLiteral    *that) const { PosRange = Stig::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TFalseKwd                  *that) const { PosRange = Stig::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TIdLiteral                 *that) const { PosRange = Stig::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TIntLiteral                *that) const { PosRange = Stig::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TRealLiteral               *that) const { PosRange = Stig::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TSingleQuotedRawStrLiteral *that) const { PosRange = Stig::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TSingleQuotedStrLiteral    *that) const { PosRange = Stig::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TTimeDiffLiteral           *that) const { PosRange = Stig::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TTimePntLiteral            *that) const { PosRange = Stig::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TTrueKwd                   *that) const { PosRange = Stig::Synth::GetPosRange(that); }
      private:
      TPosRange &PosRange;
    };  // TVisitor
  TPosRange pos_range;
  that->GetLiteral()->Accept(TVisitor(pos_range));
  return pos_range;
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TMutateStmt *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetSemi());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TNewStmt *that) {
  return Stig::Synth::GetPosRange(that->GetNewKwd(), that->GetSemi());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TNowExpr *that) {
  return Stig::Synth::GetPosRange(that->GetNowKwd());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TObjCtor *that) {
  return Stig::Synth::GetPosRange(that->GetOpenObj(), that->GetCloseObj());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TParenExpr *that) {
  return Stig::Synth::GetPosRange(that->GetOpenParen(), that->GetCloseParen());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPostfixAddrMember *that) {
  return Stig::Synth::GetPosRange(that->GetExpr(), that->GetIntLiteral());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPostfixCall *that) {
  return Stig::Synth::GetPosRange(that->GetExpr(), that->GetCloseParen());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPostfixCast *that) {
  return Stig::Synth::GetPosRange(that->GetExpr() /* , that->GetType() */);
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPostfixIsEmpty *that) {
  return Stig::Synth::GetPosRange(that->GetExpr(), that->GetEmptyKwd());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPostfixIsKnown *that) {
  return Stig::Synth::GetPosRange(that->GetExpr(), that->GetIsKnown()->GetKnownKwd());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPostfixIsKnownExpr *that) {
  return Stig::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPostfixIsUnknown *that) {
  return Stig::Synth::GetPosRange(that->GetExpr(), that->GetUnknownKwd());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPostfixObjMember *that) {
  return Stig::Synth::GetPosRange(that->GetExpr(), that->GetName());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPostfixOptCheckpoint *that) {
  return Stig::Synth::GetPosRange(that->GetExpr(), that->GetQuestionMark());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPostfixSlice *that) {
  return Stig::Synth::GetPosRange(that->GetExpr(), that->GetCloseBracket());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPrefixAddrOf *that) {
  return Stig::Synth::GetPosRange(that->GetAmpersand(), that->GetExpr());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPrefixExists *that) {
  return Stig::Synth::GetPosRange(that->GetExistsKwd(), that->GetExpr());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPrefixKnown *that) {
  return Stig::Synth::GetPosRange(that->GetKnownKwd(), that->GetExpr());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPrefixLengthOf *that) {
  return Stig::Synth::GetPosRange(that->GetLengthKwd(), that->GetExpr());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPrefixLogicalNot *that) {
  return Stig::Synth::GetPosRange(that->GetNotKwd(), that->GetExpr());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPrefixMinus *that) {
  return Stig::Synth::GetPosRange(that->GetMinus(), that->GetExpr());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPrefixPlus *that) {
  return Stig::Synth::GetPosRange(that->GetPlus(), that->GetExpr());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPrefixReverseOf *that) {
  return Stig::Synth::GetPosRange(that->GetReverseKwd(), that->GetExpr());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPrefixSequence *that) {
  return Stig::Synth::GetPosRange(that->GetStars(), that->GetExpr());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPrefixStart *that) {
  return Stig::Synth::GetPosRange(that->GetStartKwd(), that->GetExpr());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TPrefixTimeObj *that) {
  return Stig::Synth::GetPosRange(that->GetTimeObjKwd(), that->GetExpr());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TRangeCtor *that) {
  class TVisitor
    : public Package::Syntax::TRangeEnd::TVisitor {
      NO_COPY_SEMANTICS(TVisitor);
      public:
      TVisitor(const Package::Syntax::TOpenBracket *open_bracket, TPosRange &pos_range)
        : OpenBracket(open_bracket), PosRange(pos_range) {}
      virtual void operator()(const Package::Syntax::TClosedRangeEnd *that) const {
        PosRange = Stig::Synth::GetPosRange(OpenBracket, that->GetCloseBracket());
      }
      virtual void operator()(const Package::Syntax::TOpenRangeEnd *that) const {
        PosRange = Stig::Synth::GetPosRange(OpenBracket, that->GetCloseParen());
      }
      virtual void operator()(const Package::Syntax::TUndefinedRangeEnd *that) const {
        PosRange = Stig::Synth::GetPosRange(OpenBracket, that->GetCloseParen());
      }
      private:
      const Package::Syntax::TOpenBracket *OpenBracket;
      TPosRange &PosRange;
    };  // TVisitor
  TPosRange pos_range;
  that->GetRangeEnd()->Accept(TVisitor(that->GetOpenBracket(), pos_range));
  return pos_range;
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TReadExpr *that) {
  return Stig::Synth::GetPosRange(that->GetStar(), that->GetCloseParen());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TRefExpr *that) {
  return Stig::Synth::GetPosRange(that->GetName());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TRhsExpr *that) {
  return Stig::Synth::GetPosRange(that->GetRhsKwd());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TSessionIdExpr *that) {
  return Stig::Synth::GetPosRange(that->GetSessionIdKwd());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TSetCtor *that) {
  return Stig::Synth::GetPosRange(that->GetOpenBrace(), that->GetCloseBrace());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TStmtBlock *that) {
  return Stig::Synth::GetPosRange(that->GetOpenBrace(), that->GetCloseBrace());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TTestCaseBlock *that) {
  return Stig::Synth::GetPosRange(that->GetOpenBrace(), that->GetCloseBrace());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TTestDef *that) {
  // TODO: account for the optional with clause
  return Stig::Synth::GetPosRange(that->GetTestKwd(), that->GetSemi());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TTestKvEntry *that) {
  return Stig::Synth::GetPosRange(that->GetKey(), that->GetSemi());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TThatExpr *that) {
  return Stig::Synth::GetPosRange(that->GetThatKwd());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TTimeDiffCtor *that) {
  return Stig::Synth::GetPosRange(that->GetTimeDiffKwd());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TTimePntCtor *that) {
  return Stig::Synth::GetPosRange(that->GetTimePntKwd());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TUninstallerDef *that) {
  return Stig::Synth::GetPosRange(that->GetNotKwd(), that->GetSemi());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TUnknownCtor *that) {
  return Stig::Synth::GetPosRange(that->GetUnknownKwd() /* , that->GetType */ );
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TUnlabeledTestCase *that) {
  return Stig::Synth::GetPosRange(that->GetExpr(), that->GetSemi());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TUpgraderDef *that) {
  return Stig::Synth::GetPosRange(that->GetPackageKwd(), that->GetSemi());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TUserIdExpr *that) {
  return Stig::Synth::GetPosRange(that->GetUserIdKwd());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TWhereExpr *that) {
  return Stig::Synth::GetPosRange(that->GetOpenParen(), that->GetCloseBrace());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TWithClause *that) {
  return Stig::Synth::GetPosRange(that->GetWithKwd(), that->GetCloseBrace());
}

template <>
TPosRange Stig::Synth::GetPosRange(const Package::Syntax::TExpr *expr) {
  class TPosRangeVisitor
      : public Package::Syntax::TExpr::TVisitor {
    NO_COPY_SEMANTICS(TPosRangeVisitor);
    public:
    TPosRangeVisitor(TPosRange &pos_range)
        : PosRange(pos_range) {}
    virtual void operator()(const Package::Syntax::TAddrCtor             *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TAssertExpr           *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TBuiltInCeiling       *that) const { PosRange = Stig::Synth::GetPosRange(that->GetCeilingKwd()); }
    virtual void operator()(const Package::Syntax::TBuiltInFloor         *that) const { PosRange = Stig::Synth::GetPosRange(that->GetFloorKwd()); }
    virtual void operator()(const Package::Syntax::TBuiltInLog           *that) const { PosRange = Stig::Synth::GetPosRange(that->GetLogKwd()); }
    virtual void operator()(const Package::Syntax::TBuiltInLog2          *that) const { PosRange = Stig::Synth::GetPosRange(that->GetLog2Kwd()); }
    virtual void operator()(const Package::Syntax::TBuiltInLog10         *that) const { PosRange = Stig::Synth::GetPosRange(that->GetLog10Kwd()); }
    virtual void operator()(const Package::Syntax::TBuiltInRandom        *that) const { PosRange = Stig::Synth::GetPosRange(that->GetRandomKwd()); }
    virtual void operator()(const Package::Syntax::TBuiltInReplace       *that) const { PosRange = Stig::Synth::GetPosRange(that->GetReplaceKwd()); }
    virtual void operator()(const Package::Syntax::TBuiltInToLower       *that) const { PosRange = Stig::Synth::GetPosRange(that->GetToLowerKwd()); }
    virtual void operator()(const Package::Syntax::TBuiltInToUpper       *that) const { PosRange = Stig::Synth::GetPosRange(that->GetToUpperKwd()); }
    virtual void operator()(const Package::Syntax::TCollatedByExpr       *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TCollectedByExpr      *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TDbKeysExpr           *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TDeleteStmt           *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TDictCtor             *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TEffectingExpr        *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TEmptyCtor            *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TGivenExpr            *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TIfExpr               *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixAndThen         *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixBitwiseAnd      *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixBitwiseOr       *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixBitwiseXor      *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixDiv             *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixEq              *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixExp             *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixFilter          *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixGt              *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixGtEq            *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixIn              *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixLogicalAnd      *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixLogicalOr       *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixLogicalXor      *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixLt              *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixLtEq            *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixMatch           *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixMinus           *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixMod             *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixMul             *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixNeq             *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixOrElse          *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixPlus            *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixReduce          *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixSort            *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixTake            *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixSkip            *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixSplit           *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixWhile           *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TLhsExpr              *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TListCtor             *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TLiteralExpr          *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TNowExpr              *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TObjCtor              *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TParenExpr            *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixAddrMember    *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixCall          *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixCast          *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixIsEmpty       *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixIsKnown       *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixIsKnownExpr   *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixIsUnknown     *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixObjMember     *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixOptCheckpoint *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixSlice         *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixAddrOf         *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixExists         *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixKnown          *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixLengthOf       *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixLogicalNot     *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixMinus          *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixPlus           *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixReverseOf      *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixSequence       *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixStart          *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixTimeObj        *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TRangeCtor            *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TReadExpr             *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TRefExpr              *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TRhsExpr              *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TSessionIdExpr        *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TSetCtor              *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TStmtBlock            *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TThatExpr             *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TTimeDiffCtor         *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TTimePntCtor          *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TUnknownCtor          *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TUserIdExpr           *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TWhereExpr            *that) const { PosRange = Stig::Synth::GetPosRange(that); }
    private:
    TPosRange &PosRange;
  };  // TPosRangeVisitor
  TPosRange pos_range;
  expr->Accept(TPosRangeVisitor(pos_range));
  return pos_range;
}
