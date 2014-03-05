/* <stig/symbol/stmt/if.h>

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

#include <memory>

#include <stig/expr/expr.h>
#include <stig/symbol/root.h>
#include <stig/symbol/stmt/stmt_block.h>

namespace Stig {

  namespace Symbol {

    namespace Stmt {

      class TIf;

      class TIfClause
          : public TRoot {
        NO_COPY_SEMANTICS(TIfClause);
        public:

        typedef std::shared_ptr<TIfClause> TPtr;

        static TPtr New(const Expr::TExpr::TPtr &expr, const TStmtBlock::TPtr &stmt_block);

        const TStmtBlock::TPtr &GetStmtBlock() const;

        void TypeCheck() const;

        private:

        TIfClause(
            const Expr::TExpr::TPtr &expr,
            const TStmtBlock::TPtr &stmt_block);

        TStmtBlock::TPtr StmtBlock;

      };  // TIfClause

      class TElseClause {
        NO_COPY_SEMANTICS(TElseClause);
        public:

        typedef std::shared_ptr<TElseClause> TPtr;

        static TPtr New(const TStmtBlock::TPtr &stmt_block);

        const TStmtBlock::TPtr &GetStmtBlock() const;

        void TypeCheck() const;

        private:

        TElseClause(const TStmtBlock::TPtr &stmt_block);

        TStmtBlock::TPtr StmtBlock;

      };  // TElseClause

      class TIf
          : public TStmt {
        NO_COPY_SEMANTICS(TIf);
        public:

        typedef std::shared_ptr<TIf> TPtr;

        typedef std::vector<TIfClause::TPtr> TIfClauseVec;

        static TPtr New(const TIfClauseVec &if_cases, const TElseClause::TPtr &opt_else_case, const TPosRange &pos_range);

        virtual void Accept(const TVisitor &visitor) const final;

        const TIfClauseVec &GetIfClauses() const;

        const TElseClause::TPtr &GetOptElseClause() const;

        virtual void TypeCheck() const;

        private:

        TIf(const TIfClauseVec &if_cases, const TElseClause::TPtr &opt_else_case, const TPosRange &pos_range);

        TIfClauseVec IfClauses;

        TElseClause::TPtr OptElseClause;

      };  // TIf

    }  // Stmt

  }  // Symbol

}  // Stig
