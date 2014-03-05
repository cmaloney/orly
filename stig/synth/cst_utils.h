/* <stig/synth/cst_utils.h>

   Utilities for navigating the CST.

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
#include <functional>

#include <base/no_construction.h>
#include <stig/stig.checkpoint.cst.h>
#include <stig/stig.package.cst.h>

namespace Stig {

  namespace Synth {

    /* Unravel the TOptFoo / TFoo / TNoFoo pattern. */
    template <typename TNode, typename TNoNode, typename TOptNode>
    const TNode *TryGetNode(const TOptNode *opt_node) {
      assert(opt_node);
      class TVisitor
          : public TOptNode::TVisitor {
        public:
        TVisitor(const TNode *&node)
            : Node(node) {}
        virtual void operator()(const TNode   *that) const { Node = that; }
        virtual void operator()(const TNoNode *)     const {}
        private:
        const TNode *&Node;
      };
      const TNode *node = nullptr;
      opt_node->Accept(TVisitor(node));
      return node;
    }

    template <typename TItem>
    struct TTypeDependentFalse {
      static const bool False=false;
    };

    template <typename TItem>
    struct ItemInfo {
      static_assert(TTypeDependentFalse<TItem>::False, "You must specialize TItemInfo for TItem");
    };

    /* Call the given function on each item in a list. */
    template <typename TItem>
    void ForEach(
        const typename ItemInfo<TItem>::TNode *node,
        const std::function<bool (const TItem *)> &cb) {
      assert(node);
      assert(&cb);
      assert(cb);
      while (cb(ItemInfo<TItem>::GetItem(node)) && (node = ItemInfo<TItem>::TryGetNext(node)));
    }

    /* Call the given function on each item in an optional list. */
    template <typename TItem>
    void ForEach(
        const typename ItemInfo<TItem>::TOptNode *opt_node,
        const std::function<bool (const TItem *)> &cb) {
      assert(opt_node);
      assert(&cb);
      assert(cb);
      auto node = TryGetNode<typename ItemInfo<TItem>::TNode,
                             typename ItemInfo<TItem>::TNoNode>(opt_node);
      if (node) {
        ForEach(node, cb); // Call the above version
      }
    }

    template <>
    struct ItemInfo<Checkpoint::Syntax::TName> {
      NO_CONSTRUCTION(ItemInfo);

      typedef Checkpoint::Syntax::TNoPackageNameMemberListTail TNoListTailNode;
      typedef Checkpoint::Syntax::TPackageNameMemberListTail   TListTailNode;
      typedef Checkpoint::Syntax::TPackageNameMemberList       TNode;
      typedef Checkpoint::Syntax::TName                        TItem;

      static const TItem *GetItem(const TNode *node) {
        assert(node);
        return node->GetName();
      }

      static const TNode *TryGetNext(const TNode *node) {
        assert(node);
        auto list_tail = TryGetNode<TListTailNode, TNoListTailNode>(node->GetOptPackageNameMemberListTail());
        return list_tail ? list_tail->GetPackageNameMemberList() : nullptr;
      }

    };  // ItemInfo<Checkpoint::Syntax::TName>

    template <>
    struct ItemInfo<Checkpoint::Syntax::TCheckpointStmt> {
      NO_CONSTRUCTION(ItemInfo);

      typedef Checkpoint::Syntax::TOptCheckpointStmtSeq TOptNode;
      typedef Checkpoint::Syntax::TNoCheckpointStmtSeq  TNoNode;
      typedef Checkpoint::Syntax::TCheckpointStmtSeq    TNode;
      typedef Checkpoint::Syntax::TCheckpointStmt       TItem;

      static const TItem *GetItem(const TNode *node) {
        assert(node);
        return node->GetCheckpointStmt();
      }

      static const TNode *TryGetNext(const TNode *node) {
        assert(node);
        return TryGetNode<TNode, TNoNode>(node->GetOptCheckpointStmtSeq());
      }

    };  // ItemInfo<Checkpoint::Syntax::TCheckpointStmt>

    template <>
    struct ItemInfo<Package::Syntax::TAddrMember> {
      NO_CONSTRUCTION(ItemInfo);

      typedef Package::Syntax::TNoAddrMemberListTail  TNoListTailNode;
      typedef Package::Syntax::TAddrMemberListTail    TListTailNode;
      typedef Package::Syntax::TOptAddrMemberList     TOptNode;
      typedef Package::Syntax::TNoAddrMemberList      TNoNode;
      typedef Package::Syntax::TAddrMemberList        TNode;
      typedef Package::Syntax::TAddrMember            TItem;

      static const TItem *GetItem(const TNode *node) {
        assert(node);
        return node->GetAddrMember();
      }

      static const TNode *TryGetNext(const TNode *node) {
        assert(node);
        auto list_tail = TryGetNode<TListTailNode, TNoListTailNode>(node->GetOptAddrMemberListTail());
        return list_tail ? list_tail->GetAddrMemberList() : nullptr;
      }

    };  // ItemInfo<Package::Syntax::TAddrMember>

    template <>
    struct ItemInfo<Package::Syntax::TAddrTypeMember> {
      NO_CONSTRUCTION(ItemInfo);

      typedef Package::Syntax::TNoAddrTypeMemberListTail  TNoListTailNode;
      typedef Package::Syntax::TAddrTypeMemberListTail    TListTailNode;
      typedef Package::Syntax::TOptAddrTypeMemberList     TOptNode;
      typedef Package::Syntax::TNoAddrTypeMemberList      TNoNode;
      typedef Package::Syntax::TAddrTypeMemberList        TNode;
      typedef Package::Syntax::TAddrTypeMember            TItem;

      static const TItem *GetItem(const TNode *node) {
        assert(node);
        return node->GetAddrTypeMember();
      }

      static const TNode *TryGetNext(const TNode *node) {
        assert(node);
        auto list_tail = TryGetNode<TListTailNode, TNoListTailNode>(node->GetOptAddrTypeMemberListTail());
        return list_tail ? list_tail->GetAddrTypeMemberList() : nullptr;
      }

    };  // ItemInfo<Package::Syntax::TAddrTypeMember>

    template <>
    struct ItemInfo<Package::Syntax::TAssertion> {
      NO_CONSTRUCTION(ItemInfo);

      typedef Package::Syntax::TOptAssertionSeq TOptNode;
      typedef Package::Syntax::TNoAssertionSeq  TNoNode;
      typedef Package::Syntax::TAssertionSeq    TNode;
      typedef Package::Syntax::TAssertion       TItem;

      static const TItem *GetItem(const TNode *node) {
        assert(node);
        return node->GetAssertion();
      }

      static const TNode *TryGetNext(const TNode *node) {
        assert(node);
        return TryGetNode<TNode, TNoNode>(node->GetOptAssertionSeq());
      }

    };  // ItemInfo<Package::Syntax::TAssertion>

    template <>
    struct ItemInfo<Package::Syntax::TDbKeysMember> {
      NO_CONSTRUCTION(ItemInfo);

      typedef Package::Syntax::TNoDbKeysMemberListTail  TNoListTailNode;
      typedef Package::Syntax::TDbKeysMemberListTail    TListTailNode;
      typedef Package::Syntax::TOptDbKeysMemberList     TOptNode;
      typedef Package::Syntax::TNoDbKeysMemberList      TNoNode;
      typedef Package::Syntax::TDbKeysMemberList        TNode;
      typedef Package::Syntax::TDbKeysMember            TItem;

      static const TItem *GetItem(const TNode *node) {
        assert(node);
        return node->GetDbKeysMember();
      }

      static const TNode *TryGetNext(const TNode *node) {
        assert(node);
        auto list_tail = TryGetNode<TListTailNode, TNoListTailNode>(node->GetOptDbKeysMemberListTail());
        return list_tail ? list_tail->GetDbKeysMemberList() : nullptr;
      }

    };  // ItemInfo<Package::Syntax::TDbKeysExpr>

    template <>
    struct ItemInfo<Package::Syntax::TDef> {
      NO_CONSTRUCTION(ItemInfo);

      typedef Package::Syntax::TOptDefSeq TOptNode;
      typedef Package::Syntax::TNoDefSeq  TNoNode;
      typedef Package::Syntax::TDefSeq    TNode;
      typedef Package::Syntax::TDef       TItem;

      static const TItem *GetItem(const TNode *node) {
        assert(node);
        return node->GetDef();
      }

      static const TNode *TryGetNext(const TNode *node) {
        assert(node);
        return TryGetNode<TNode, TNoNode>(node->GetOptDefSeq());
      }

    };  // ItemInfo<Package::Syntax::TDef>

    template <>
    struct ItemInfo<Package::Syntax::TDictMember> {
      NO_CONSTRUCTION(ItemInfo);

      typedef Package::Syntax::TNoDictMemberListTail  TNoListTailNode;
      typedef Package::Syntax::TDictMemberListTail    TListTailNode;
      typedef Package::Syntax::TDictMemberList        TNode;
      typedef Package::Syntax::TDictMember            TItem;

      static const TItem *GetItem(const TNode *node) {
        assert(node);
        return node->GetDictMember();
      }

      static const TNode *TryGetNext(const TNode *node) {
        assert(node);
        auto list_tail = TryGetNode<TListTailNode, TNoListTailNode>(node->GetOptDictMemberListTail());
        return list_tail ? list_tail->GetDictMemberList() : nullptr;
      }

    };  // ItemInfo<Package::Syntax::TDictMember>

    template <>
    struct ItemInfo<Package::Syntax::TElseIfClause> {
      NO_CONSTRUCTION(ItemInfo);

      typedef Package::Syntax::TOptElseIfClauseSeq TOptNode;
      typedef Package::Syntax::TNoElseIfClauseSeq  TNoNode;
      typedef Package::Syntax::TElseIfClauseSeq    TNode;
      typedef Package::Syntax::TElseIfClause       TItem;

      static const TItem *GetItem(const TNode *node) {
        assert(node);
        return node->GetElseIfClause();
      }

      static const TNode *TryGetNext(const TNode *node) {
        assert(node);
        return TryGetNode<TNode, TNoNode>(node->GetOptElseIfClauseSeq());
      }

    };  // ItemInfo<Package::Syntax::TElseIfClause>

    template <>
    struct ItemInfo<Package::Syntax::TExpr> {
      NO_CONSTRUCTION(ItemInfo);

      typedef Package::Syntax::TNoExprListTail  TNoListTailNode;
      typedef Package::Syntax::TExprListTail    TListTailNode;
      typedef Package::Syntax::TExprList        TNode;
      typedef Package::Syntax::TExpr            TItem;

      static const TItem *GetItem(const TNode *node) {
        assert(node);
        return node->GetExpr();
      }

      static const TNode *TryGetNext(const TNode *node) {
        assert(node);
        auto list_tail = TryGetNode<TListTailNode, TNoListTailNode>(node->GetOptExprListTail());
        return list_tail ? list_tail->GetExprList() : nullptr;
      }

    };  // ItemInfo<Package::Syntax::TExpr>

    template <>
    struct ItemInfo<Package::Syntax::TObjMember> {
      NO_CONSTRUCTION(ItemInfo);

      typedef Package::Syntax::TNoObjMemberListTail  TNoListTailNode;
      typedef Package::Syntax::TObjMemberListTail    TListTailNode;
      typedef Package::Syntax::TOptObjMemberList     TOptNode;
      typedef Package::Syntax::TNoObjMemberList      TNoNode;
      typedef Package::Syntax::TObjMemberList        TNode;
      typedef Package::Syntax::TObjMember            TItem;

      static const TItem *GetItem(const TNode *node) {
        assert(node);
        return node->GetObjMember();
      }

      static const TNode *TryGetNext(const TNode *node) {
        assert(node);
        auto list_tail = TryGetNode<TListTailNode, TNoListTailNode>(node->GetOptObjMemberListTail());
        return list_tail ? list_tail->GetObjMemberList() : nullptr;
      }

    };  // ItemInfo<Package::Syntax::TObjMember>

    template <>
    struct ItemInfo<Package::Syntax::TObjTypeMember> {
      NO_CONSTRUCTION(ItemInfo);

      typedef Package::Syntax::TNoObjTypeMemberListTail  TNoListTailNode;
      typedef Package::Syntax::TObjTypeMemberListTail    TListTailNode;
      typedef Package::Syntax::TOptObjTypeMemberList     TOptNode;
      typedef Package::Syntax::TNoObjTypeMemberList      TNoNode;
      typedef Package::Syntax::TObjTypeMemberList        TNode;
      typedef Package::Syntax::TObjTypeMember            TItem;

      static const TItem *GetItem(const TNode *node) {
        assert(node);
        return node->GetObjTypeMember();
      }

      static const TNode *TryGetNext(const TNode *node) {
        assert(node);
        auto list_tail = TryGetNode<TListTailNode, TNoListTailNode>(node->GetOptObjTypeMemberListTail());
        return list_tail ? list_tail->GetObjTypeMemberList() : nullptr;
      }

    };  // ItemInfo<Package::Syntax::TObjTypeMember>

    template <>
    struct ItemInfo<Package::Syntax::TStmt> {
      NO_CONSTRUCTION(ItemInfo);

      typedef Package::Syntax::TOptStmtSeq TOptNode;
      typedef Package::Syntax::TNoStmtSeq  TNoNode;
      typedef Package::Syntax::TStmtSeq    TNode;
      typedef Package::Syntax::TStmt       TItem;

      static const TItem *GetItem(const TNode *node) {
        assert(node);
        return node->GetStmt();
      }

      static const TNode *TryGetNext(const TNode *node) {
        assert(node);
        return TryGetNode<TNode, TNoNode>(node->GetOptStmtSeq());
      }

    };  // ItemInfo<Package::Syntax::TStmt>

    template <>
    struct ItemInfo<Package::Syntax::TTestCase> {
      NO_CONSTRUCTION(ItemInfo);

      typedef Package::Syntax::TOptTestCaseSeq  TOptNode;
      typedef Package::Syntax::TNoTestCaseSeq   TNoNode;
      typedef Package::Syntax::TTestCaseSeq     TNode;
      typedef Package::Syntax::TTestCase        TItem;

      static const TItem *GetItem(const TNode *node) {
        assert(node);
        return node->GetTestCase();
      }

      static const TNode *TryGetNext(const TNode *node) {
        assert(node);
        return TryGetNode<TNode, TNoNode>(node->GetOptTestCaseSeq());
      }

    };  // ItemInfo<Package::Syntax::TTestCase>

    template <>
    struct ItemInfo<Package::Syntax::TTestKvEntry> {
      NO_CONSTRUCTION(ItemInfo);

      typedef Package::Syntax::TOptTestKvEntrySeq  TOptNode;
      typedef Package::Syntax::TNoTestKvEntrySeq   TNoNode;
      typedef Package::Syntax::TTestKvEntrySeq     TNode;
      typedef Package::Syntax::TTestKvEntry        TItem;

      static const TItem *GetItem(const TNode *node) {
        assert(node);
        return node->GetTestKvEntry();
      }

      static const TNode *TryGetNext(const TNode *node) {
        assert(node);
        return TryGetNode<TNode, TNoNode>(node->GetOptTestKvEntrySeq());
      }

    };  // ItemInfo<Package::Syntax::TTestKvEntry>

  }  // Synth

}  // Stig
