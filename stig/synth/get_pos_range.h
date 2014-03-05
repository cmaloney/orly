/* <stig/synth/get_pos_range.h>

   GetPosRange() over the CST

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

#pragma once

#include <cassert>

#include <stig/pos_range.h>
#include <stig/stig.checkpoint.cst.h>
#include <stig/stig.package.cst.h>

namespace Stig {

  namespace Synth {

    template <typename TNode>
    TPosRange GetPosRange(const TNode *node) {
      assert(node);
      return node->GetLexeme().GetPosRange();
    }

    template <typename TLhsNode, typename TRhsNode>
    TPosRange GetPosRange(const TLhsNode *lhs, const TRhsNode *rhs) {
      assert(lhs);
      assert(rhs);
      return TPosRange(GetPosRange(lhs), GetPosRange(rhs));
    }

    template <>
    TPosRange GetPosRange(const Package::Syntax::TAddrCtor *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TAssertExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TBuiltInRandom *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TBuiltInReplace *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TCollatedByExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TCollectedByExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TDbKeysExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TDeleteStmt *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TDictCtor *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TEffectingExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TEmptyCtor *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TFreeDbKeysMember *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TFuncDef *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TGivenExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TIfExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TIfStmt *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixAndThen *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixBitwiseAnd *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixBitwiseOr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixBitwiseXor *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixDiv *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixEq *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixExp *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixFilter *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixGt *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixGtEq *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixIn *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixLogicalAnd *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixLogicalOr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixLogicalXor *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixLt *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixLtEq *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixMatch *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixMinus *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixMod *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixMul *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixNeq *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixOrElse *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixPlus *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixReduce *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixSort *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixTake *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixSkip *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixSplit *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInfixWhile *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TInstallerDef *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TLabeledTestCase *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TLhsExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TListCtor *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TLiteralExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TMutateStmt *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TNewStmt *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TNowExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TObjCtor *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TParenExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPostfixAddrMember *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPostfixCall *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPostfixCast *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPostfixIsEmpty *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPostfixIsKnown *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPostfixIsKnownExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPostfixIsUnknown *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPostfixObjMember *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPostfixOptCheckpoint *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPostfixSlice *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPrefixAddrOf *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPrefixExists *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPrefixKnown *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPrefixLengthOf *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPrefixLogicalNot *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPrefixMinus *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPrefixPlus *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPrefixReverseOf *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPrefixSequence *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPrefixStart *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TPrefixTimeObj *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TRangeCtor *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TReadExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TRefExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TRhsExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TSessionIdExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TSetCtor *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TStmtBlock *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TTestCaseBlock *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TTestDef *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TTestKvEntry *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TThatExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TTimeDiffCtor *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TTimePntCtor *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TUninstallerDef *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TUnknownCtor *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TUnlabeledTestCase *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TUpgraderDef *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TUserIdExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TWhereExpr *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TWithClause *that);

    template <>
    TPosRange GetPosRange(const Package::Syntax::TExpr *that);

  }  // Synth

}  // Stig
