/* <orly/synth/get_pos_range.cc>

   Implements <orly/synth/get_pos_range.h>

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/synth/get_pos_range.h>

#include <orly/synth/cst_utils.h>

using namespace Orly;

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TAddrCtor *that) {
  return Orly::Synth::GetPosRange(that->GetOpenAddr(), that->GetCloseAddr());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TAssertExpr *that) {
  return Orly::Synth::GetPosRange(that->GetOpenParen(), that->GetCloseBrace());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TBuiltInRandom *that) {
  return Orly::Synth::GetPosRange(that->GetRandomKwd());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TBuiltInReplace *that) {
  return Orly::Synth::GetPosRange(that->GetReplaceKwd());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TCollatedByExpr *that) {
  return Orly::Synth::GetPosRange(that->GetSeq(), that->GetHaving());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TCollectedByExpr *that) {
  return Orly::Synth::GetPosRange(that->GetSeq(), that->GetCollect());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TDbKeysExpr *that) {
  return Orly::Synth::GetPosRange(that->GetKeysKwd(), that->GetCloseAddr());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TDeleteStmt *that) {
  return Orly::Synth::GetPosRange(that->GetDeleteKwd(), that->GetSemi());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TDictCtor *that) {
  return Orly::Synth::GetPosRange(that->GetOpenBrace(), that->GetCloseBrace());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TEffectingExpr *that) {
  return Orly::Synth::GetPosRange(that->GetOpenParen(), that->GetStmtBlock()->GetCloseBrace());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TEmptyCtor *that) {
  return Orly::Synth::GetPosRange(that->GetEmptyKwd() /*, that->GetType() */);
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TFreeDbKeysMember *that) {
  return Orly::Synth::GetPosRange(that->GetFreeKwd(), that->GetCloseParen());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TFuncDef *that) {
  return Orly::Synth::GetPosRange(that->GetName(), that->GetSemi());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TGivenExpr *that) {
  return Orly::Synth::GetPosRange(that->GetGivenKwd(), that->GetCloseParen());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TIfExpr *that) {
  return Orly::Synth::GetPosRange(that->GetOpenParen(), that->GetCloseParen());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TIfStmt *that) {
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
  return Orly::Synth::GetPosRange(that->GetIfKwd(), close_brace);
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixAndThen *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixBitwiseAnd *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixBitwiseOr *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixBitwiseXor *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixDiv *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixEq *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixExp *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixFilter *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixGt *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixGtEq *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixIn *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixLogicalAnd *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixLogicalOr *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixLogicalXor *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixLt *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixLtEq *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixMatch *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixMinus *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixMod *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixMul *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixNeq *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixOrElse *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixPlus *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixReduce *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixSort *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixTake *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixSkip *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixSplit *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInfixWhile *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TInstallerDef *that) {
  return Orly::Synth::GetPosRange(that->GetPackageKwd(), that->GetSemi());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TLabeledTestCase *that) {
  return Orly::Synth::GetPosRange(that->GetName(), that->GetSemi());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TLhsExpr *that) {
  return Orly::Synth::GetPosRange(that->GetLhsKwd());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TListCtor *that) {
  return Orly::Synth::GetPosRange(that->GetOpenBracket(), that->GetCloseBracket());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TLiteralExpr *that) {
  class TVisitor
    : public Package::Syntax::TLiteral::TVisitor {
      NO_COPY_SEMANTICS(TVisitor);
      public:
      TVisitor(TPosRange &pos_range)
        : PosRange(pos_range) {}
      virtual void operator()(const Package::Syntax::TDoubleQuotedRawStrLiteral *that) const { PosRange = Orly::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TDoubleQuotedStrLiteral    *that) const { PosRange = Orly::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TFalseKwd                  *that) const { PosRange = Orly::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TIdLiteral                 *that) const { PosRange = Orly::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TIntLiteral                *that) const { PosRange = Orly::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TRealLiteral               *that) const { PosRange = Orly::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TSingleQuotedRawStrLiteral *that) const { PosRange = Orly::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TSingleQuotedStrLiteral    *that) const { PosRange = Orly::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TTimeDiffLiteral           *that) const { PosRange = Orly::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TTimePntLiteral            *that) const { PosRange = Orly::Synth::GetPosRange(that); }
      virtual void operator()(const Package::Syntax::TTrueKwd                   *that) const { PosRange = Orly::Synth::GetPosRange(that); }
      private:
      TPosRange &PosRange;
    };  // TVisitor
  TPosRange pos_range;
  that->GetLiteral()->Accept(TVisitor(pos_range));
  return pos_range;
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TMutateStmt *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetSemi());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TNewStmt *that) {
  return Orly::Synth::GetPosRange(that->GetNewKwd(), that->GetSemi());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TNowExpr *that) {
  return Orly::Synth::GetPosRange(that->GetNowKwd());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TObjCtor *that) {
  return Orly::Synth::GetPosRange(that->GetOpenObj(), that->GetCloseObj());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TParenExpr *that) {
  return Orly::Synth::GetPosRange(that->GetOpenParen(), that->GetCloseParen());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPostfixAddrMember *that) {
  return Orly::Synth::GetPosRange(that->GetExpr(), that->GetIntLiteral());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPostfixCall *that) {
  return Orly::Synth::GetPosRange(that->GetExpr(), that->GetCloseParen());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPostfixCast *that) {
  return Orly::Synth::GetPosRange(that->GetExpr() /* , that->GetType() */);
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPostfixIsEmpty *that) {
  return Orly::Synth::GetPosRange(that->GetExpr(), that->GetEmptyKwd());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPostfixIsKnown *that) {
  return Orly::Synth::GetPosRange(that->GetExpr(), that->GetIsKnown()->GetKnownKwd());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPostfixIsKnownExpr *that) {
  return Orly::Synth::GetPosRange(that->GetLhs(), that->GetRhs());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPostfixIsUnknown *that) {
  return Orly::Synth::GetPosRange(that->GetExpr(), that->GetUnknownKwd());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPostfixObjMember *that) {
  return Orly::Synth::GetPosRange(that->GetExpr(), that->GetName());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPostfixOptCheckpoint *that) {
  return Orly::Synth::GetPosRange(that->GetExpr(), that->GetQuestionMark());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPostfixSlice *that) {
  return Orly::Synth::GetPosRange(that->GetExpr(), that->GetCloseBracket());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPrefixAddrOf *that) {
  return Orly::Synth::GetPosRange(that->GetAmpersand(), that->GetExpr());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPrefixExists *that) {
  return Orly::Synth::GetPosRange(that->GetExistsKwd(), that->GetExpr());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPrefixKnown *that) {
  return Orly::Synth::GetPosRange(that->GetKnownKwd(), that->GetExpr());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPrefixLengthOf *that) {
  return Orly::Synth::GetPosRange(that->GetLengthKwd(), that->GetExpr());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPrefixLogicalNot *that) {
  return Orly::Synth::GetPosRange(that->GetNotKwd(), that->GetExpr());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPrefixMinus *that) {
  return Orly::Synth::GetPosRange(that->GetMinus(), that->GetExpr());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPrefixPlus *that) {
  return Orly::Synth::GetPosRange(that->GetPlus(), that->GetExpr());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPrefixReverseOf *that) {
  return Orly::Synth::GetPosRange(that->GetReverseKwd(), that->GetExpr());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPrefixSequence *that) {
  return Orly::Synth::GetPosRange(that->GetStars(), that->GetExpr());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPrefixStart *that) {
  return Orly::Synth::GetPosRange(that->GetStartKwd(), that->GetExpr());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TPrefixTimeObj *that) {
  return Orly::Synth::GetPosRange(that->GetTimeObjKwd(), that->GetExpr());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TRangeCtor *that) {
  class TVisitor
    : public Package::Syntax::TRangeEnd::TVisitor {
      NO_COPY_SEMANTICS(TVisitor);
      public:
      TVisitor(const Package::Syntax::TOpenBracket *open_bracket, TPosRange &pos_range)
        : OpenBracket(open_bracket), PosRange(pos_range) {}
      virtual void operator()(const Package::Syntax::TClosedRangeEnd *that) const {
        PosRange = Orly::Synth::GetPosRange(OpenBracket, that->GetCloseBracket());
      }
      virtual void operator()(const Package::Syntax::TOpenRangeEnd *that) const {
        PosRange = Orly::Synth::GetPosRange(OpenBracket, that->GetCloseParen());
      }
      virtual void operator()(const Package::Syntax::TUndefinedRangeEnd *that) const {
        PosRange = Orly::Synth::GetPosRange(OpenBracket, that->GetCloseParen());
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
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TReadExpr *that) {
  return Orly::Synth::GetPosRange(that->GetStar(), that->GetCloseParen());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TRefExpr *that) {
  return Orly::Synth::GetPosRange(that->GetName());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TRhsExpr *that) {
  return Orly::Synth::GetPosRange(that->GetRhsKwd());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TSessionIdExpr *that) {
  return Orly::Synth::GetPosRange(that->GetSessionIdKwd());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TSetCtor *that) {
  return Orly::Synth::GetPosRange(that->GetOpenBrace(), that->GetCloseBrace());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TStmtBlock *that) {
  return Orly::Synth::GetPosRange(that->GetOpenBrace(), that->GetCloseBrace());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TTestCaseBlock *that) {
  return Orly::Synth::GetPosRange(that->GetOpenBrace(), that->GetCloseBrace());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TTestDef *that) {
  // TODO: account for the optional with clause
  return Orly::Synth::GetPosRange(that->GetTestKwd(), that->GetSemi());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TTestKvEntry *that) {
  return Orly::Synth::GetPosRange(that->GetKey(), that->GetSemi());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TThatExpr *that) {
  return Orly::Synth::GetPosRange(that->GetThatKwd());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TTimeDiffCtor *that) {
  return Orly::Synth::GetPosRange(that->GetTimeDiffKwd());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TTimePntCtor *that) {
  return Orly::Synth::GetPosRange(that->GetTimePntKwd());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TUninstallerDef *that) {
  return Orly::Synth::GetPosRange(that->GetNotKwd(), that->GetSemi());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TUnknownCtor *that) {
  return Orly::Synth::GetPosRange(that->GetUnknownKwd() /* , that->GetType */ );
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TUnlabeledTestCase *that) {
  return Orly::Synth::GetPosRange(that->GetExpr(), that->GetSemi());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TUpgraderDef *that) {
  return Orly::Synth::GetPosRange(that->GetPackageKwd(), that->GetSemi());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TUserIdExpr *that) {
  return Orly::Synth::GetPosRange(that->GetUserIdKwd());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TWhereExpr *that) {
  return Orly::Synth::GetPosRange(that->GetOpenParen(), that->GetCloseBrace());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TWithClause *that) {
  return Orly::Synth::GetPosRange(that->GetWithKwd(), that->GetCloseBrace());
}

template <>
TPosRange Orly::Synth::GetPosRange(const Package::Syntax::TExpr *expr) {
  class TPosRangeVisitor
      : public Package::Syntax::TExpr::TVisitor {
    NO_COPY_SEMANTICS(TPosRangeVisitor);
    public:
    TPosRangeVisitor(TPosRange &pos_range)
        : PosRange(pos_range) {}
    virtual void operator()(const Package::Syntax::TAddrCtor             *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TAssertExpr           *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TBuiltInCeiling       *that) const { PosRange = Orly::Synth::GetPosRange(that->GetCeilingKwd()); }
    virtual void operator()(const Package::Syntax::TBuiltInFloor         *that) const { PosRange = Orly::Synth::GetPosRange(that->GetFloorKwd()); }
    virtual void operator()(const Package::Syntax::TBuiltInLog           *that) const { PosRange = Orly::Synth::GetPosRange(that->GetLogKwd()); }
    virtual void operator()(const Package::Syntax::TBuiltInLog2          *that) const { PosRange = Orly::Synth::GetPosRange(that->GetLog2Kwd()); }
    virtual void operator()(const Package::Syntax::TBuiltInLog10         *that) const { PosRange = Orly::Synth::GetPosRange(that->GetLog10Kwd()); }
    virtual void operator()(const Package::Syntax::TBuiltInRandom        *that) const { PosRange = Orly::Synth::GetPosRange(that->GetRandomKwd()); }
    virtual void operator()(const Package::Syntax::TBuiltInReplace       *that) const { PosRange = Orly::Synth::GetPosRange(that->GetReplaceKwd()); }
    virtual void operator()(const Package::Syntax::TBuiltInToLower       *that) const { PosRange = Orly::Synth::GetPosRange(that->GetToLowerKwd()); }
    virtual void operator()(const Package::Syntax::TBuiltInToUpper       *that) const { PosRange = Orly::Synth::GetPosRange(that->GetToUpperKwd()); }
    virtual void operator()(const Package::Syntax::TCollatedByExpr       *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TCollectedByExpr      *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TDbKeysExpr           *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TDeleteStmt           *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TDictCtor             *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TEffectingExpr        *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TEmptyCtor            *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TGivenExpr            *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TIfExpr               *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixAndThen         *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixBitwiseAnd      *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixBitwiseOr       *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixBitwiseXor      *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixDiv             *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixEq              *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixExp             *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixFilter          *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixGt              *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixGtEq            *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixIn              *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixLogicalAnd      *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixLogicalOr       *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixLogicalXor      *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixLt              *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixLtEq            *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixMatch           *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixMinus           *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixMod             *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixMul             *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixNeq             *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixOrElse          *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixPlus            *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixReduce          *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixSort            *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixTake            *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixSkip            *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixSplit           *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TInfixWhile           *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TLhsExpr              *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TListCtor             *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TLiteralExpr          *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TNowExpr              *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TObjCtor              *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TParenExpr            *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixAddrMember    *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixCall          *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixCast          *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixIsEmpty       *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixIsKnown       *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixIsKnownExpr   *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixIsUnknown     *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixObjMember     *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixOptCheckpoint *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPostfixSlice         *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixAddrOf         *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixExists         *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixKnown          *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixLengthOf       *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixLogicalNot     *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixMinus          *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixPlus           *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixReverseOf      *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixSequence       *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixStart          *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TPrefixTimeObj        *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TRangeCtor            *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TReadExpr             *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TRefExpr              *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TRhsExpr              *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TSessionIdExpr        *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TSetCtor              *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TStmtBlock            *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TThatExpr             *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TTimeDiffCtor         *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TTimePntCtor          *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TUnknownCtor          *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TUserIdExpr           *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    virtual void operator()(const Package::Syntax::TWhereExpr            *that) const { PosRange = Orly::Synth::GetPosRange(that); }
    private:
    TPosRange &PosRange;
  };  // TPosRangeVisitor
  TPosRange pos_range;
  expr->Accept(TPosRangeVisitor(pos_range));
  return pos_range;
}
