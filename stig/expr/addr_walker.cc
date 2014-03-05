/* <stig/expr/addr_walker.cc>

   Implements addr_walker.h

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


#include <stig/expr/addr_walker.h>

#include <unordered_set>

#include <stig/expr.h>
#include <stig/expr/expr.h>
#include <stig/expr/visitor.h>
#include <stig/symbol/function.h>
#include <stig/symbol/stmt/if.h>
#include <stig/symbol/stmt/mutate.h>
#include <stig/symbol/stmt/new_and_delete.h>
#include <stig/type/managed_type.h>

using namespace Stig;
using namespace Stig::Expr;
using namespace std;

/* Similar to walker.cc, for walking through the expressions, but
   once we hit a keys expression, or an effecting expression we move
   in to the TEffectingExprVisitor. */
class expr_visitor_t
    : public TExpr::TVisitor {
  NO_COPY_SEMANTICS(expr_visitor_t);
  public:
  expr_visitor_t(unordered_set<pair<Type::TType, Type::TType>> &addr_set)
      : Addrs(addr_set) {}
  virtual ~expr_visitor_t() noexcept {};
  // Nullary
  virtual void operator()(const TFree *)        const {}
  virtual void operator()(const TLhs *)         const {}
  virtual void operator()(const TLiteral *)     const {}
  virtual void operator()(const TNow *)         const {}
  virtual void operator()(const TRef *)         const {}
  virtual void operator()(const TRhs *)         const {}
  virtual void operator()(const TSessionId *)   const {}
  virtual void operator()(const TStart *)       const {}
  virtual void operator()(const TThat *)        const {}
  virtual void operator()(const TUnknown *)     const {}
  virtual void operator()(const TUserId *)      const {}
  // Unary
  virtual void operator()(const TAddrMember *that)      const { Unary(that); }
  virtual void operator()(const TAddrOf *that)          const { Unary(that); }
  virtual void operator()(const TAs *that)              const { Unary(that); }
  virtual void operator()(const TCeiling *that)         const { Unary(that); }
  virtual void operator()(const TExists *that)          const {
    Addrs.insert(make_pair(that->GetExpr()->GetType(), that->GetValueType()));
    Unary(that);
  }
  virtual void operator()(const TFloor *that)           const { Unary(that); }
  virtual void operator()(const TIsEmpty *that)         const { Unary(that); }
  virtual void operator()(const TIsKnown *that)         const { Unary(that); }
  virtual void operator()(const TIsUnknown *that)       const { Unary(that); }
  virtual void operator()(const TKnown *that)           const { Unary(that); }
  virtual void operator()(const TLengthOf *that)        const { Unary(that); }
  virtual void operator()(const TLog *that)             const { Unary(that); }
  virtual void operator()(const TLog2 *that)            const { Unary(that); }
  virtual void operator()(const TLog10 *that)           const { Unary(that); }
  virtual void operator()(const TNegative *that)        const { Unary(that); }
  virtual void operator()(const TNot *that)             const { Unary(that); }
  virtual void operator()(const TObjMember *that)       const { Unary(that); }
  virtual void operator()(const TPositive *that)        const { Unary(that); }
  virtual void operator()(const TRead *that)            const {
    Type::TType val_type = Type::UnwrapSequence(that->GetType());
    val_type = val_type.As<Type::TMutable>()->GetSrcAtAddr();
    Addrs.insert(make_pair(that->GetExpr()->GetType(), val_type));
    Unary(that);
  }
  virtual void operator()(const TReverseOf *that)       const { Unary(that); }
  virtual void operator()(const TSequenceOf *that)      const { Unary(that); }
  virtual void operator()(const TTimeObj *that)         const { Unary(that); }
  virtual void operator()(const TToLower *that)         const { Unary(that); }
  virtual void operator()(const TToUpper *that)         const { Unary(that); }
  // Binary
  virtual void operator()(const TAdd *that)             const { Binary(that); }
  virtual void operator()(const TAnd *that)             const { Binary(that); }
  virtual void operator()(const TAndThen *that)         const { Binary(that); }
  virtual void operator()(const TCollectedBy *that)     const { Binary(that); }
  virtual void operator()(const TDiv *that)             const { Binary(that); }
  virtual void operator()(const TEqEq *that)            const { Binary(that); }
  virtual void operator()(const TExp *that)             const { Binary(that); }
  virtual void operator()(const TFilter *that)          const { Binary(that); }
  virtual void operator()(const TGt *that)              const { Binary(that); }
  virtual void operator()(const TGtEq *that)            const { Binary(that); }
  virtual void operator()(const TIn *that)              const { Binary(that); }
  virtual void operator()(const TIntersection  *that)   const { Binary(that); }
  virtual void operator()(const TIsKnownExpr *that)     const { Binary(that); }
  virtual void operator()(const TLt *that)              const { Binary(that); }
  virtual void operator()(const TLtEq *that)            const { Binary(that); }
  virtual void operator()(const TMatch *that)           const { Binary(that); }
  virtual void operator()(const TMod *that)             const { Binary(that); }
  virtual void operator()(const TMult *that)            const { Binary(that); }
  virtual void operator()(const TNeq *that)             const { Binary(that); }
  virtual void operator()(const TOr *that)              const { Binary(that); }
  virtual void operator()(const TOrElse *that)          const { Binary(that); }
  virtual void operator()(const TReduce *that)          const { Binary(that); }
  virtual void operator()(const TSkip *that)            const { Binary(that); }
  virtual void operator()(const TSort *that)            const { Binary(that); }
  virtual void operator()(const TSplit *that)           const { Binary(that); }
  virtual void operator()(const TSub *that)             const { Binary(that); }
  virtual void operator()(const TSymmetricDiff *that)   const { Binary(that); }
  virtual void operator()(const TTake *that)            const { Binary(that); }
  virtual void operator()(const TUnion *that)           const { Binary(that); }
  virtual void operator()(const TWhile *that)           const { Binary(that); }
  virtual void operator()(const TXor *that)             const { Binary(that); }
  // Nary
  virtual void operator()(const TAddr *that) const {
    for(auto &it: that->GetMembers()) {
      Yield(it.second);
    }
  }
  virtual void operator()(const TAssert *that) const {
    Unary(that);
    for(auto &it: that->GetAssertCases()) {
      Yield(it->GetExpr());
    }
  }
  virtual void operator()(const TCollatedBy *that) const {
    Yield(that->GetSeq());
    Yield(that->GetReduce());
    Yield(that->GetHaving());
  }
  virtual void operator()(const TDict *that) const {
    for(auto &it: that->GetMembers()) {
      Yield(it.first);
      Yield(it.second);
    }
  }
  virtual void operator()(const TEffect *that) const {
    Unary(that);
    StmtBlock(that->GetStmtBlock().get());
  }
  virtual void operator()(const TFunctionApp *that) const {
    Yield(that->GetExpr());
    for(auto &it: that->GetFunctionAppArgs()) {
      Yield(it.second->GetExpr());
    }
  }
  virtual void operator()(const TIfElse *that) const {
    Yield(that->GetPredicate());
    Yield(that->GetTrue());
    Yield(that->GetFalse());
  }
  virtual void operator()(const TKeys *that) const {
    Addrs.insert(make_pair(that->GetAddrType(), that->GetValueType()));
    for (auto &member : that->GetMembers()) {
      Yield(member.second);
    }
  }
  virtual void operator()(const TList *that) const {
    for(auto &it: that->GetExprs()) {
      Yield(it);
    }
  }
  virtual void operator()(const TObj *that) const {
    for(auto &it: that->GetMembers()) {
      Yield(it.second);
    }
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
  virtual void operator()(const TSet *that) const {
    for(auto &it: that->GetExprs()) {
      Yield(it);
    }
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
  virtual void operator()(const TWhere *that) const {
    Unary(that);
    for (const auto &func: that->GetFunctions()) {
      Yield(func->GetExpr());
    }
  }
  void Yield(const TExpr::TPtr &expr) const {
    assert(this);
    assert(&expr);
    assert(expr);
    expr->Accept(*this);
  }
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
  /* used by StmtBlock, but need access to it for DatabaseAddrsInStmtBlock */
  class effecting_expr_visitor_t
        : public TExpr::TVisitor {
    NO_COPY_SEMANTICS(effecting_expr_visitor_t);
    public:
    effecting_expr_visitor_t(const expr_visitor_t &visitor, unordered_set<pair<Type::TType, Type::TType>> &addr_set, Type::TType rhs_type)
        : ExprVisitor(visitor), Addrs(addr_set), RhsType(rhs_type) {}
    virtual ~effecting_expr_visitor_t() noexcept {};
    // Nullary
    virtual void operator()(const TFree *)        const {}
    virtual void operator()(const TLhs *)         const {}
    virtual void operator()(const TLiteral *)     const {}
    virtual void operator()(const TNow *)         const {}
    virtual void operator()(const TRef *that)     const {
      const Type::TType type = that->GetType();
      class TypeVisitor : public Type::TType::TVisitor {
        public:
        TypeVisitor(bool &should_insert) : ShouldInsert(should_insert) {}
        virtual void operator()(const Type::TAddr *)      const {
          ShouldInsert = true;
        }
        virtual void operator()(const Type::TAny *)       const {/* DO NOTHING */}
        virtual void operator()(const Type::TBool *)      const {/* DO NOTHING */}
        virtual void operator()(const Type::TDict *)      const {/* DO NOTHING */}
        virtual void operator()(const Type::TErr *)       const {/* DO NOTHING */}
        virtual void operator()(const Type::TFunc *)      const {/* DO NOTHING */}
        virtual void operator()(const Type::TId *)        const {/* DO NOTHING */}
        virtual void operator()(const Type::TInt *)       const {/* DO NOTHING */}
        virtual void operator()(const Type::TList *)      const {/* DO NOTHING */}
        virtual void operator()(const Type::TMutable *)   const {/* DO NOTHING */}
        virtual void operator()(const Type::TObj *)       const {/* DO NOTHING */}
        virtual void operator()(const Type::TOpt *)       const {/* DO NOTHING */}
        virtual void operator()(const Type::TReal *)      const {/* DO NOTHING */}
        virtual void operator()(const Type::TSeq *)       const {/* DO NOTHING */}
        virtual void operator()(const Type::TSet *)       const {/* DO NOTHING */}
        virtual void operator()(const Type::TStr *)       const {/* DO NOTHING */}
        virtual void operator()(const Type::TTimeDiff *)  const {/* DO NOTHING */}
        virtual void operator()(const Type::TTimePnt *)   const {/* DO NOTHING */}
        private:
        bool &ShouldInsert;
      };
      bool should_insert = false;
      type.Accept(TypeVisitor(should_insert));
      if (should_insert) {
        Addrs.insert(make_pair(type, RhsType));
      }
    }
    virtual void operator()(const TRhs *)         const {}
    virtual void operator()(const TSessionId *)   const {}
    virtual void operator()(const TStart *)       const {}
    virtual void operator()(const TThat *)        const {}
    virtual void operator()(const TUnknown *)     const {}
    virtual void operator()(const TUserId *)      const {}
    // Unary
    virtual void operator()(const TAddrMember *that)      const { Unary(that); }
    virtual void operator()(const TAddrOf *that)          const { Unary(that); }
    virtual void operator()(const TAs *that)              const { Unary(that); }
    virtual void operator()(const TCeiling *that)         const { Unary(that); }
    virtual void operator()(const TExists *that)          const { Unary(that); }
    virtual void operator()(const TFloor *that)           const { Unary(that); }
    virtual void operator()(const TIsEmpty *that)         const { Unary(that); }
    virtual void operator()(const TIsKnown *that)         const { Unary(that); }
    virtual void operator()(const TIsUnknown *that)       const { Unary(that); }
    virtual void operator()(const TKnown *that)           const { Unary(that); }
    virtual void operator()(const TLengthOf *that)        const { Unary(that); }
    virtual void operator()(const TLog *that)             const { Unary(that); }
    virtual void operator()(const TLog2 *that)            const { Unary(that); }
    virtual void operator()(const TLog10 *that)           const { Unary(that); }
    virtual void operator()(const TNegative *that)        const { Unary(that); }
    virtual void operator()(const TNot *that)             const { Unary(that); }
    virtual void operator()(const TObjMember *that)       const { Unary(that); }
    virtual void operator()(const TPositive *that)        const { Unary(that); }
    virtual void operator()(const TRead *that)            const { Unary(that); }
    virtual void operator()(const TReverseOf *that)       const { Unary(that); }
    virtual void operator()(const TSequenceOf *that)      const { Unary(that); }
    virtual void operator()(const TTimeObj *that)         const { Unary(that); }
    virtual void operator()(const TToLower *that)         const { Unary(that); }
    virtual void operator()(const TToUpper *that)         const { Unary(that); }
    // Binary
    virtual void operator()(const TAdd *that)             const { Binary(that); }
    virtual void operator()(const TAnd *that)             const { Binary(that); }
    virtual void operator()(const TAndThen *that)         const { Binary(that); }
    virtual void operator()(const TCollectedBy *that)     const { Binary(that); }
    virtual void operator()(const TDiv *that)             const { Binary(that); }
    virtual void operator()(const TEqEq *that)            const { Binary(that); }
    virtual void operator()(const TExp *that)             const { Binary(that); }
    virtual void operator()(const TFilter *that)          const { Binary(that); }
    virtual void operator()(const TGt *that)              const { Binary(that); }
    virtual void operator()(const TGtEq *that)            const { Binary(that); }
    virtual void operator()(const TIn *that)              const { Binary(that); }
    virtual void operator()(const TIntersection  *that)   const { Binary(that); }
    virtual void operator()(const TIsKnownExpr *that)     const { Binary(that); }
    virtual void operator()(const TLt *that)              const { Binary(that); }
    virtual void operator()(const TLtEq *that)            const { Binary(that); }
    virtual void operator()(const TMatch *that)           const { Binary(that); }
    virtual void operator()(const TMod *that)             const { Binary(that); }
    virtual void operator()(const TMult *that)            const { Binary(that); }
    virtual void operator()(const TNeq *that)             const { Binary(that); }
    virtual void operator()(const TOr *that)              const { Binary(that); }
    virtual void operator()(const TOrElse *that)          const { Binary(that); }
    virtual void operator()(const TReduce *that)          const { Binary(that); }
    virtual void operator()(const TSkip *that)            const { Binary(that); }
    virtual void operator()(const TSort *that)            const { Binary(that); }
    virtual void operator()(const TSplit *that)           const { Binary(that); }
    virtual void operator()(const TSub *that)             const { Binary(that); }
    virtual void operator()(const TSymmetricDiff *that)   const { Binary(that); }
    virtual void operator()(const TTake *that)            const { Binary(that); }
    virtual void operator()(const TUnion *that)           const { Binary(that); }
    virtual void operator()(const TWhile *that)           const { Binary(that); }
    virtual void operator()(const TXor *that)             const { Binary(that); }
    // Nary
    virtual void operator()(const TAddr *that) const {
      Addrs.insert(make_pair(that->GetType(), RhsType));
      for(auto &it: that->GetMembers()) {
        Yield(it.second);
      }
    }
    virtual void operator()(const TAssert *that) const {
      Unary(that);
      for(auto &it: that->GetAssertCases()) {
        Yield(it->GetExpr());
      }
    }
    virtual void operator()(const TCollatedBy *that) const {
      Yield(that->GetSeq());
      Yield(that->GetReduce());
      Yield(that->GetHaving());
    }
    virtual void operator()(const TDict *that) const {
      for(auto &it: that->GetMembers()) {
        Yield(it.first);
        Yield(it.second);
      }
    }
    virtual void operator()(const TEffect *that) const {
      Unary(that);
      StmtBlock(that->GetStmtBlock().get());
    }
    virtual void operator()(const TFunctionApp *that) const {
      Yield(that->GetExpr());
      for(auto &it: that->GetFunctionAppArgs()) {
        Yield(it.second->GetExpr());
      }
    }
    virtual void operator()(const TIfElse *that) const {
      Yield(that->GetPredicate());
      Yield(that->GetTrue());
      Yield(that->GetFalse());
    }
    virtual void operator()(const TKeys *that) const {
      Addrs.insert(make_pair(that->GetAddrType(), that->GetValueType()));
      for (auto &member : that->GetMembers()) {
        Yield(member.second);
      }
    }
    virtual void operator()(const TList *that) const {
      for(auto &it: that->GetExprs()) {
        Yield(it);
      }
    }
    virtual void operator()(const TObj *that) const {
      for(auto &it: that->GetMembers()) {
        Yield(it.second);
      }
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
    virtual void operator()(const TSet *that) const {
      for(auto &it: that->GetExprs()) {
        Yield(it);
      }
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
    virtual void operator()(const TWhere *that) const {
      Unary(that);
      for (const auto &func: that->GetFunctions()) {
        Yield(func->GetExpr());
      }
    }
    void Yield(const TExpr::TPtr &expr) const {
      assert(this);
      assert(&expr);
      assert(expr);
      expr->Accept(*this);
    }
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
    void StmtBlock(const Symbol::Stmt::TStmtBlock *stmt_block) const {
      /* This is for a statement block already inside of a effecting block. */
      class effecting_stmt_visitor_t : public Symbol::Stmt::TStmt::TVisitor {
        public:
        effecting_stmt_visitor_t(const effecting_expr_visitor_t &effecting_expr_visitor)
          : EffectingExprVisitor(effecting_expr_visitor) {}
        virtual void operator()(const Symbol::Stmt::TDelete *that) const override { Unary(that); }
        virtual void operator()(const Symbol::Stmt::TMutate *that) const override { Binary(that); }
        virtual void operator()(const Symbol::Stmt::TNew *that) const override { Binary(that); }
        virtual void operator()(const Symbol::Stmt::TIf *that) const override {
          for (const auto &if_clause: that->GetIfClauses()) {
            EffectingExprVisitor.Yield(if_clause->GetExpr());
            EffectingExprVisitor.StmtBlock(if_clause->GetStmtBlock().get());
          }
          const auto &else_clause = that->GetOptElseClause();
          if (else_clause) {
            EffectingExprVisitor.StmtBlock(else_clause->GetStmtBlock().get());
          }
        }
        private:
        void Unary(const Symbol::Stmt::TUnary *that) const {
          assert(this);
          assert(that);
          EffectingExprVisitor.Yield(that->GetStmtArg()->GetExpr());
        }
        void Binary(const Symbol::Stmt::TBinary *that) const {
          assert(this);
          assert(that);
          EffectingExprVisitor.Yield(that->GetLhs()->GetExpr());
          EffectingExprVisitor.Yield(that->GetRhs()->GetExpr());
        }
        const effecting_expr_visitor_t &EffectingExprVisitor;
      }; // effecting_stmt_visitor_t
      const effecting_stmt_visitor_t stmt_visitor(*this);
      for (const auto &stmt: stmt_block->GetStmts()) {
        stmt->Accept(stmt_visitor);
      }
    } // StmtBlock (in effecting_expr_visitor_t)
    const expr_visitor_t &ExprVisitor;
    unordered_set<pair<Type::TType, Type::TType>> &Addrs;
    Type::TType RhsType;
  };  // effecting_expr_visitor_t
  class stmt_visitor_t : public Symbol::Stmt::TStmt::TVisitor {
    public:
    stmt_visitor_t(const expr_visitor_t &expr_visitor, unordered_set<pair<Type::TType, Type::TType>> &addr_set)
      : ExprVisitor(expr_visitor), Addrs(addr_set) {}
    virtual void operator()(const Symbol::Stmt::TDelete *that) const override {
      assert(this);
      assert(that);
      effecting_expr_visitor_t(ExprVisitor, Addrs, that->GetValueType()).Yield(that->GetStmtArg()->GetExpr());
    }
    virtual void operator()(const Symbol::Stmt::TMutate *that) const override { Binary(that); }
    virtual void operator()(const Symbol::Stmt::TNew *that) const override { Binary(that); }
    virtual void operator()(const Symbol::Stmt::TIf *that) const override {
      for (const auto &if_clause: that->GetIfClauses()) {
        ExprVisitor.Yield(if_clause->GetExpr());
        ExprVisitor.StmtBlock(if_clause->GetStmtBlock().get());
      }
      const auto &else_clause = that->GetOptElseClause();
      if (else_clause) {
        ExprVisitor.StmtBlock(else_clause->GetStmtBlock().get());
      }
    }
    private:
    void Binary(const Symbol::Stmt::TBinary *that) const {
      assert(this);
      assert(that);
      effecting_expr_visitor_t(ExprVisitor, Addrs, that->GetRhs()->GetExpr()->GetType()).Yield(that->GetLhs()->GetExpr());
      ExprVisitor.Yield(that->GetRhs()->GetExpr());
    }
    private:
    const expr_visitor_t &ExprVisitor;
    unordered_set<pair<Type::TType, Type::TType>> &Addrs;
  }; // stmt_visitor_t
  void StmtBlock(const Symbol::Stmt::TStmtBlock *stmt_block) const {
    /* This is the visitor we use for statements in an effective block which interact with the database. */
    const stmt_visitor_t stmt_visitor(*this, Addrs);
    for (const auto &stmt: stmt_block->GetStmts()) {
      stmt->Accept(stmt_visitor);
    }
  } // StmtBlock (in expr_visitor_t)
  private:
  unordered_set<pair<Type::TType, Type::TType>> &Addrs;
};  // expr_visitor_t

/* For every Addr that is part of a keys, new, delete, or mutation expression,
   add that Addr to addr_set. */
void Stig::Expr::DatabaseAddrsInExpr(const TExpr::TPtr &root,
                                     unordered_set<pair<Type::TType, Type::TType>> &addr_set) {
  expr_visitor_t(addr_set).Yield(root);
}

/* For every Addr that accesses the db in the statement block,
   add that Addr to the addr set. */
void Stig::Expr::DatabaseAddrsFromNewStmt(const Stig::Symbol::Stmt::TNew *stmt,
                              std::unordered_set<std::pair<Stig::Type::TType, Stig::Type::TType>> &addr_set) {
  stmt->Accept(expr_visitor_t::stmt_visitor_t(expr_visitor_t(addr_set), addr_set));
}