/* <stig/synth/given_collector.h>

   TODO

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

#include <stack>
#include <unordered_set>

#include <stig/stig.package.cst.h>
#include <stig/synth/cst_utils.h>

namespace Stig {

  namespace Synth {

    /* TODO */
    class TGivenCollector
        : public Package::Syntax::TExpr::TVisitor {
      public:

      /* A convenience typedef */
      typedef std::unordered_set<const Package::Syntax::TGivenExpr *> TGivenSet;

      /* Traverse the expression tree and collect givens */
      static void CollectGivens(const Package::Syntax::TExpr *root, TGivenSet &givens) {
        TStack stack;
        TGivenCollector visitor(stack, givens);
        stack.push(root);
        while (!stack.empty()) {
          const auto &expr = stack.top();
          stack.pop();
          expr->Accept(visitor);
        }
      }

      private:

      /* A convenience typedef */
      typedef std::stack<const Package::Syntax::TExpr *> TStack;

      /* Constructor. Private to force users to use the static CollectGivens() function */
      TGivenCollector(TStack &stack, TGivenSet &givens)
          : Stack(stack), Givens(givens) {}

      /* Insert given expr */
      virtual void operator()(const Package::Syntax::TGivenExpr *that) const {
        auto result = Givens.insert(that);
        assert(result.second);
      }

      /* Other exprs */
      virtual void operator()(const Package::Syntax::TAddrCtor *that) const {
        ForEach<Package::Syntax::TAddrMember>(that->GetOptAddrMemberList(),
            [this](const Package::Syntax::TAddrMember *addr_member) -> bool {
              Push(addr_member->GetExpr());
              return true;
            });
      }
      virtual void operator()(const Package::Syntax::TAssertExpr *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TBuiltInCeiling *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TBuiltInFloor *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TBuiltInLog *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TBuiltInLog2 *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TBuiltInLog10 *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TBuiltInReplace *) const { /* DO NOTHING */ }
      virtual void operator()(const Package::Syntax::TBuiltInRandom *) const { /* DO NOTHING */ }
      virtual void operator()(const Package::Syntax::TBuiltInToLower *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TBuiltInToUpper *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TCollatedByExpr *that) const {
        Push(that->GetSeq(), that->GetReduce(), that->GetHaving());
      }
      virtual void operator()(const Package::Syntax::TCollectedByExpr *that) const {
        Push(that->GetSeq(), that->GetCollect());
      }
      virtual void operator()(const Package::Syntax::TDbKeysExpr *that) const {
        class TDbKeysMemberVisitor
            : public Package::Syntax::TDbKeysMember::TVisitor {
          NO_COPY_SEMANTICS(TDbKeysMemberVisitor);
          public:
          TDbKeysMemberVisitor(const TGivenCollector *given_collector)
              : GivenCollector(given_collector) {}
          virtual void operator()(const Package::Syntax::TFixedDbKeysMember *that) const {
            GivenCollector->Push(that->GetExpr());
          }
          virtual void operator()(const Package::Syntax::TFreeDbKeysMember *) const { /* DO NOTHING */ }
          private:
          const TGivenCollector *GivenCollector;
        };  // TDbKeysMemberVisitor
        TDbKeysMemberVisitor visitor(this);
        ForEach<Package::Syntax::TDbKeysMember>(that->GetOptDbKeysMemberList(),
            [&visitor](const Package::Syntax::TDbKeysMember *db_keys_member) -> bool {
              db_keys_member->Accept(visitor);
              return true;
            });
      }
      virtual void operator()(const Package::Syntax::TDictCtor *that) const {
        ForEach<Package::Syntax::TDictMember>(that->GetDictMemberList(),
            [this](const Package::Syntax::TDictMember *dict_member) -> bool {
              Push(dict_member->GetKey(), dict_member->GetValue());
              return true;
            });
      }
      virtual void operator()(const Package::Syntax::TEffectingExpr *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TEmptyCtor *) const { /* DO NOTHING */ }
      virtual void operator()(const Package::Syntax::TIfExpr *that) const { Push(that->GetTrueCase(), that->GetPredicate(), that->GetFalseCase()); }
      virtual void operator()(const Package::Syntax::TInfixAndThen *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixBitwiseAnd *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixBitwiseOr *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixBitwiseXor *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixDiv *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixEq *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixExp *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixFilter *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixGt *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixGtEq *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixIn *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixLogicalAnd *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixLogicalOr *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixLogicalXor *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixLt *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixLtEq *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixMatch *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixMinus *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixMod *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixMul *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixNeq *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixOrElse *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixPlus *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixReduce *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixSort *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixTake *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixSkip *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixSplit *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TInfixWhile *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TLhsExpr *) const { /* DO NOTHING */ }
      virtual void operator()(const Package::Syntax::TListCtor *that) const { Push(that->GetExprList()); }
      virtual void operator()(const Package::Syntax::TLiteralExpr *) const { /* DO NOTHING */ }
      virtual void operator()(const Package::Syntax::TNowExpr *) const { /* DO NOTHING */ }
      virtual void operator()(const Package::Syntax::TObjCtor *that) const {
        ForEach<Package::Syntax::TObjMember>(that->GetOptObjMemberList(),
            [this](const Package::Syntax::TObjMember *obj_member) -> bool {
              Push(obj_member->GetExpr());
              return true;
            });
      }
      virtual void operator()(const Package::Syntax::TParenExpr *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPostfixAddrMember *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPostfixCall *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPostfixCast *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPostfixIsEmpty *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPostfixIsKnown *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPostfixIsKnownExpr *that) const { Push(that->GetLhs(), that->GetRhs()); }
      virtual void operator()(const Package::Syntax::TPostfixIsUnknown *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPostfixObjMember *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPostfixOptCheckpoint *) const { /* DO NOTHING */ }
      virtual void operator()(const Package::Syntax::TPostfixSlice *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPrefixAddrOf *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPrefixExists *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPrefixKnown *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPrefixLengthOf *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPrefixLogicalNot *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPrefixMinus *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPrefixPlus *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPrefixReverseOf *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPrefixSequence *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPrefixStart *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TPrefixTimeObj *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TRangeCtor *that) const {
        class TRangeEndVisitor
            : public Package::Syntax::TRangeEnd::TVisitor {
          NO_COPY_SEMANTICS(TRangeEndVisitor);
          public:
          TRangeEndVisitor(const TGivenCollector *given_collector)
              : GivenCollector(given_collector) {}
          virtual void operator()(const Package::Syntax::TOpenRangeEnd *that) const {
            GivenCollector->Push(that->GetExpr());
          }
          virtual void operator()(const Package::Syntax::TClosedRangeEnd *that) const {
            GivenCollector->Push(that->GetExpr());
          }
          virtual void operator()(const Package::Syntax::TUndefinedRangeEnd *) const { /* DO NOTHING */ }
          private:
          const TGivenCollector *GivenCollector;
        };  // TRangeEndVisitor
        Push(that->GetExpr());
        const auto &stride = TryGetNode<Package::Syntax::TRangeStride,
                                        Package::Syntax::TNoRangeStride>(that->GetOptRangeStride());
        if (stride) {
          Push(stride->GetExpr());
        }
        that->GetRangeEnd()->Accept(TRangeEndVisitor(this));
      }
      virtual void operator()(const Package::Syntax::TReadExpr *that) const { Push(that->GetExpr()); }
      virtual void operator()(const Package::Syntax::TRefExpr *) const { /* DO NOTHING */ }
      virtual void operator()(const Package::Syntax::TRhsExpr *) const { /* DO NOTHING */ }
      virtual void operator()(const Package::Syntax::TSessionIdExpr *) const { /* DO NOTHING */ }
      virtual void operator()(const Package::Syntax::TSetCtor *that) const { Push(that->GetExprList()); }
      virtual void operator()(const Package::Syntax::TThatExpr *) const { /* DO NOTHING */ }
      virtual void operator()(const Package::Syntax::TTimeDiffCtor *) const { /* DO NOTHING */ }
      virtual void operator()(const Package::Syntax::TTimePntCtor *) const { /* DO NOTHING */ }
      virtual void operator()(const Package::Syntax::TUnknownCtor *) const { /* DO NOTHING */ }
      virtual void operator()(const Package::Syntax::TUserIdExpr *) const { /* DO NOTHING */ }
      virtual void operator()(const Package::Syntax::TWhereExpr *that) const { Push(that->GetExpr()); }

      /* Push expr onto the stack */
      void Push(const Package::Syntax::TExpr *expr) const {
        assert(expr);
        Stack.push(expr);
      }

      /* Push arbitrary number of exprs onto the stack */
      template <typename TFirst, typename... TRest>
      void Push(const TFirst &first, const TRest &... rest) const {
        Push(first);
        Push(rest...);
      }

      void Push(const Package::Syntax::TExprList *expr_list) const {
        ForEach<Package::Syntax::TExpr>(expr_list,
            [this](const Package::Syntax::TExpr *expr) -> bool {
              Push(expr);
              return true;
            });
      }

      /* BFS Stack */
      TStack &Stack;

      /* A collection of given expressions */
      TGivenSet &Givens;

    };  // TGivenCollector

  }  // Synth

}  // Stig
