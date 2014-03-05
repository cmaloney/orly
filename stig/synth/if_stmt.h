/* <stig/synth/if_stmt.h>

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

#include <functional>
#include <vector>

#include <base/no_copy_semantics.h>
#include <stig/stig.package.cst.h>
#include <stig/symbol/stmt/if.h>
#include <stig/synth/new_expr.h>
#include <stig/synth/stmt.h>
#include <stig/synth/stmt_block.h>

namespace Stig {

  namespace Synth {

    /* TODO */
    class TExpr;
    class TStmtBlock;

    /* TODO */
    class TIfStmt
        : public TStmt {
      NO_COPY_SEMANTICS(TIfStmt);
      public:

      /* TODO */
      TIfStmt(const TExprFactory *expr_factory, const Package::Syntax::TIfStmt *if_stmt);

      /* TODO */
      virtual ~TIfStmt();

      /* TODO */
      virtual Symbol::Stmt::TStmt::TPtr Build() const;

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb) const;

      /* TODO */
      virtual void ForEachInnerScope(const std::function<void (TScope *)> &cb) const;

      private:

      class TIfClause {
        NO_COPY_SEMANTICS(TIfClause);
        public:

        template <typename TNode>
        TIfClause(const TExprFactory *expr_factory, const TNode *node)
            : Expr(nullptr), StmtBlock(nullptr) {
          assert(expr_factory);
          try {
            Expr = expr_factory->NewExpr(node->GetExpr());
            StmtBlock = new TStmtBlock(expr_factory, node->GetStmtBlock());
          } catch (...) {
            delete Expr;
            delete StmtBlock;
            throw;
          }
        }

        ~TIfClause();

        Symbol::Stmt::TIfClause::TPtr Build() const;

        virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb) const;

        virtual void ForEachInnerScope(const std::function<void (TScope *)> &cb) const;

        private:

        TExpr *Expr;

        const TStmtBlock *StmtBlock;

      };  // TIfClause

      typedef std::vector<TIfClause *> TIfClauseVec;

      /* TODO */
      void Cleanup();

      /* TODO */
      const Package::Syntax::TIfStmt *IfStmt;

      /* TODO */
      TIfClauseVec IfClauses;

      /* TODO */
      TStmtBlock *OptElseBlock;

    };  // TIfStmt

  }  // Synth

}  // Stig
