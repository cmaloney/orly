/* <stig/symbol/stmt/stmt.h>

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

#include <base/no_copy_semantics.h>

#include <stig/pos_range.h>

namespace Stig {

  namespace Symbol {

    namespace Stmt {

      class TDelete;
      class TIf;
      class TMutate;
      class TNew;
      class TStmtBlock;

      class TStmt {
        NO_COPY_SEMANTICS(TStmt);
        public:

        class TVisitor {
          NO_COPY_SEMANTICS(TVisitor);
          public:

          virtual ~TVisitor() {}

          virtual void operator()(const TDelete *that) const = 0;
          virtual void operator()(const TIf     *that) const = 0;
          virtual void operator()(const TMutate *that) const = 0;
          virtual void operator()(const TNew    *that) const = 0;

          protected:

          TVisitor() {}

        };  // TVisitor

        typedef std::shared_ptr<TStmt> TPtr;

        virtual ~TStmt();

        virtual void Accept(const TVisitor &visitor) const = 0;

        const TPosRange &GetPosRange() const;

        const TStmtBlock *GetStmtBlock() const;

        void SetStmtBlock(const TStmtBlock *stmt_block);

        virtual void TypeCheck() const = 0;

        void UnsetStmtBlock(const TStmtBlock *stmt_block);

        protected:

        TStmt(const TPosRange &pos_range);

        private:

        const TPosRange PosRange;

        const TStmtBlock *StmtBlock;

      };  // TStmt

    }  // Stmt

  }  // Symbol

}  // Stig
