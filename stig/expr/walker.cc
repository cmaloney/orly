/* <stig/expr/walker.cc>

   Implements <stig/expr/walker.h>

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

#include <stig/expr/walker.h>

#include <stig/expr.h>
#include <stig/expr/visitor.h>
#include <stig/symbol/function.h>
#include <stig/symbol/stmt/if.h>
#include <stig/symbol/stmt/mutate.h>
#include <stig/symbol/stmt/new_and_delete.h>

using namespace Stig;
using namespace Stig::Expr;

typedef std::function<bool (const TExpr::TPtr &expr)> TCb;

void Stig::Expr::ForEachExpr(const TExpr::TPtr &root, const TCb &cb, bool include_inner_funcs) {
  class expr_visitor_t
      : public TExpr::TVisitor {
    NO_COPY_SEMANTICS(expr_visitor_t);
    public:
    expr_visitor_t(const TCb &cb, bool include_inner_funcs)
        : Cb(cb), IncludeInnerFuncs(include_inner_funcs) {}
    virtual ~expr_visitor_t() noexcept {};
    virtual void operator()(const TAdd *that) const {
      Binary(that);
    }
    virtual void operator()(const TAddr *that) const {
      for(auto &it: that->GetMembers()) {
        Yield(it.second);
      }
    }
    virtual void operator()(const TAddrMember *that) const {
      Unary(that);
    }
    virtual void operator()(const TAddrOf *that) const {
      Unary(that);
    }
    virtual void operator()(const TAnd *that) const {
      Binary(that);
    }
    virtual void operator()(const TAndThen *that) const {
      Binary(that);
    }
    virtual void operator()(const TAs *that) const {
      Unary(that);
    }
    virtual void operator()(const TAssert *that) const {
      Unary(that);
      for(auto &it: that->GetAssertCases()) {
        Yield(it->GetExpr());
      }
    }
    virtual void operator()(const TCeiling *that) const {
      Unary(that);
    }
    virtual void operator()(const TCollatedBy *that) const {
      Yield(that->GetSeq());
      Yield(that->GetReduce());
      Yield(that->GetHaving());
    }
    virtual void operator()(const TCollectedBy *that) const {
      Binary(that);
    }
    virtual void operator()(const TDict *that) const {
      for(auto &it: that->GetMembers()) {
        Yield(it.first);
        Yield(it.second);
      }
    }
    virtual void operator()(const TDiv *that) const {
      Binary(that);
    }
    virtual void operator()(const TEffect *that) const {
      Unary(that);
      if (IncludeInnerFuncs) {
        StmtBlock(that->GetStmtBlock());
      }
    }
    virtual void operator()(const TEqEq *that) const {
      Binary(that);
    }
    virtual void operator()(const TExists *that) const {
      Unary(that);
    }
    virtual void operator()(const TExp *that) const {
      Binary(that);
    }
    virtual void operator()(const TFilter *that) const {
      BinaryRhsInnerFunc(that);
    }
    virtual void operator()(const TFloor *that) const {
      Unary(that);
    }
    virtual void operator()(const TFree *) const {}
    virtual void operator()(const TFunctionApp *that) const {
      Yield(that->GetExpr());
      for(auto &it: that->GetFunctionAppArgs()) {
        Yield(it.second->GetExpr());
      }
    }
    virtual void operator()(const TGt *that) const {
      Binary(that);
    }
    virtual void operator()(const TGtEq *that) const {
      Binary(that);
    }
    virtual void operator()(const TIfElse *that) const {
      Yield(that->GetPredicate());
      Yield(that->GetTrue());
      Yield(that->GetFalse());
    }
    virtual void operator()(const TIn *that) const {
      Binary(that);
    }
    virtual void operator()(const TIntersection  *that) const {
      Binary(that);
    }
    virtual void operator()(const TIsEmpty *that) const {
      Unary(that);
    }
    virtual void operator()(const TIsKnown *that) const {
      Unary(that);
    }
    virtual void operator()(const TIsKnownExpr *that) const {
      Binary(that);
    }
    virtual void operator()(const TIsUnknown *that) const {
      Unary(that);
    }
    virtual void operator()(const TKeys *that) const {
      for (auto &member : that->GetMembers()) {
        Yield(member.second);
      }
    }
    virtual void operator()(const TKnown *that) const {
      Unary(that);
    }
    virtual void operator()(const TLengthOf *that) const {
      Unary(that);
    }
    virtual void operator()(const TLhs *) const {}
    virtual void operator()(const TList *that) const {
      for(auto &it: that->GetExprs()) {
        Yield(it);
      }
    }
    virtual void operator()(const TLiteral *) const {}
    virtual void operator()(const TLog *that) const {
      Unary(that);
    }
    virtual void operator()(const TLog2 *that) const {
      Unary(that);
    }
    virtual void operator()(const TLog10 *that) const {
      Unary(that);
    }
    virtual void operator()(const TLt *that) const {
      Binary(that);
    }
    virtual void operator()(const TLtEq *that) const {
      Binary(that);
    }
    virtual void operator()(const TMatch *that) const {
      Binary(that);
    }
    virtual void operator()(const TMod *that) const {
      Binary(that);
    }
    virtual void operator()(const TMult *that) const {
      Binary(that);
    }
    virtual void operator()(const TNeq *that) const {
      Binary(that);
    }
    virtual void operator()(const TNegative *that) const {
      Unary(that);
    }
    virtual void operator()(const TNot *that) const {
      Unary(that);
    }
    virtual void operator()(const TNow *) const {}
    virtual void operator()(const TObj *that) const {
      for(auto &it: that->GetMembers()) {
        Yield(it.second);
      }
    }
    virtual void operator()(const TObjMember *that) const {
      Unary(that);
    }
    virtual void operator()(const TOr *that) const {
      Binary(that);
    }
    virtual void operator()(const TOrElse *that) const {
      Binary(that);
    }
    virtual void operator()(const TPositive *that) const {
      Unary(that);
    }
    virtual void operator()(const TRange *that) const {
      Yield(that->GetStart());
      if(that->GetOptStride()) {
        Yield(that->GetOptStride());
      }
      if(that->GetOptEnd()) {
        Yield(that->GetOptEnd());
      }
    }
    virtual void operator()(const TRead *that) const {
      Unary(that);
    }
    virtual void operator()(const TReduce *that) const {
      BinaryRhsInnerFunc(that);
    }
    virtual void operator()(const TRef *) const {}
    virtual void operator()(const TReverseOf *that) const {
      Unary(that);
    }
    virtual void operator()(const TRhs *) const {}
    virtual void operator()(const TSessionId *) const {}
    virtual void operator()(const TSequenceOf *that) const {
      Unary(that);
    }
    virtual void operator()(const TSet *that) const {
      for(auto &it: that->GetExprs()) {
        Yield(it);
      }
    }
    virtual void operator()(const TSkip *that) const {
      Binary(that);
    }
    virtual void operator()(const TSlice *that) const {
      Yield(that->GetContainer());
      if(that->GetOptLhs()) {
        Yield(that->GetOptLhs());
      }
      if(that->GetOptRhs()) {
        Yield(that->GetOptRhs());
      }
    }
    virtual void operator()(const TSort *that) const {
      BinaryRhsInnerFunc(that);
    }
    virtual void operator()(const TSplit *that) const {
      Binary(that);
    }
    virtual void operator()(const TStart *) const {}
    virtual void operator()(const TSub *that) const {
      Binary(that);
    }
    virtual void operator()(const TSymmetricDiff *that) const {
      Binary(that);
    }
    virtual void operator()(const TTake *that) const {
      Binary(that);
    }
    virtual void operator()(const TThat *) const {}
    virtual void operator()(const TTimeObj *that) const {
      Unary(that);
    }
    virtual void operator()(const TUnion *that) const {
      Binary(that);
    }
    virtual void operator()(const TToLower *that) const {
      Unary(that);
    }
    virtual void operator()(const TToUpper *that) const {
      Unary(that);
    }
    virtual void operator()(const TUnknown *) const {}
    virtual void operator()(const TUserId *) const {}
    virtual void operator()(const TWhere *that) const {
      Unary(that);
      if (IncludeInnerFuncs) {
        for (const auto &func: that->GetFunctions()) {
          Yield(func->GetExpr());
        }
      }
    }
    virtual void operator()(const TWhile *that) const {
      BinaryRhsInnerFunc(that);
    }
    virtual void operator()(const TXor *that) const {
      Binary(that);
    }
    void Yield(const TExpr::TPtr &expr) const {
      assert(this);
      assert(&expr);
      assert(expr);
      // For this callback, true means stop, false means keep going
      if (!Cb(expr)) {
        expr->Accept(*this);
      }
    }
    private:
    void Unary(const TUnary *unary) const {
      assert(this);
      assert(unary);
      Yield(unary->GetExpr());
    }
    void Binary(const TBinary *binary) const {
      assert(this);
      assert(binary);
      Yield(binary->GetLhs());
      Yield(binary->GetRhs());
    }
    void BinaryRhsInnerFunc(const TBinary *binary) const {
      assert(this);
      assert(binary);
      Yield(binary->GetLhs());
      if (IncludeInnerFuncs) {
        Yield(binary->GetRhs());
      }
    }
    void StmtBlock(const Symbol::Stmt::TStmtBlock::TPtr &stmt_block) const {
      class stmt_visitor_t : public Symbol::Stmt::TStmt::TVisitor {
        public:
        stmt_visitor_t(const expr_visitor_t &expr_visitor) : ExprVisitor(expr_visitor) {}
        virtual void operator()(const Symbol::Stmt::TDelete *that) const override { Unary(that); }
        virtual void operator()(const Symbol::Stmt::TMutate *that) const override { Binary(that); }
        virtual void operator()(const Symbol::Stmt::TNew *that) const override { Binary(that); }
        virtual void operator()(const Symbol::Stmt::TIf *that) const override {
          for (const auto &if_clause: that->GetIfClauses()) {
            ExprVisitor.Yield(if_clause->GetExpr());
            ExprVisitor.StmtBlock(if_clause->GetStmtBlock());
          }
          const auto &else_clause = that->GetOptElseClause();
          if (else_clause) {
            ExprVisitor.StmtBlock(else_clause->GetStmtBlock());
          }
        }
        private:
        void Unary(const Symbol::Stmt::TUnary *that) const {
          assert(this);
          assert(that);
          ExprVisitor.Yield(that->GetStmtArg()->GetExpr());
        }
        void Binary(const Symbol::Stmt::TBinary *that) const {
          assert(this);
          assert(that);
          ExprVisitor.Yield(that->GetLhs()->GetExpr());
          ExprVisitor.Yield(that->GetRhs()->GetExpr());
        }
        const expr_visitor_t &ExprVisitor;
      };
      const stmt_visitor_t stmt_visitor(*this);
      for (const auto &stmt: stmt_block->GetStmts()) {
        stmt->Accept(stmt_visitor);
      }
    }
    const TCb &Cb;
    bool IncludeInnerFuncs;
  };  // expr_visitor_t
  expr_visitor_t(cb, include_inner_funcs).Yield(root);
}