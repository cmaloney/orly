/* <stig/symbol/stmt/stmt_block.h>

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
#include <vector>

#include <base/no_copy_semantics.h>
#include <stig/pos_range.h>
#include <stig/symbol/stmt/stmt.h>

namespace Stig {

  namespace Symbol {

    namespace Stmt {

      class TStmtBlock {
        NO_COPY_SEMANTICS(TStmtBlock);
        public:

        typedef std::shared_ptr<TStmtBlock> TPtr;

        typedef std::vector<TStmt::TPtr> TStmtVec;

        static TPtr New(const TStmtVec &stmts, const TPosRange &pos_range);

        ~TStmtBlock();

        const TPosRange &GetPosRange() const;

        const TStmtVec &GetStmts() const;

        void TypeCheck() const;

        private:

        TStmtBlock(const TStmtVec &stmts, const TPosRange &pos_range);

        const TPosRange PosRange;

        TStmtVec Stmts;

      };  // Stmt

    }  // Stmt

  }  // Symbol

}  // Stig
