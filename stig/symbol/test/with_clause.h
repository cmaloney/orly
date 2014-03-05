/* <stig/symbol/test/with_clause.h>

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
#include <unordered_set>

#include <stig/symbol/stmt/new_and_delete.h>

namespace Stig {

  namespace Symbol {

    namespace Test {

      class TWithClause {
        NO_COPY_SEMANTICS(TWithClause);
        public:

        typedef std::shared_ptr<TWithClause> TPtr;

        typedef std::unordered_set<Stmt::TNew::TPtr> TNewStmtSet;

        static TPtr New(const TNewStmtSet &new_stmts, const TPosRange &pos_range);

        const TNewStmtSet &GetNewStmts() const;

        const TPosRange &GetPosRange() const;

        void TypeCheck() const;

        private:

        TWithClause(const TNewStmtSet &new_stmts, const TPosRange &pos_range);

        TNewStmtSet NewStmts;

        const TPosRange PosRange;

      };  // TWithClause

    }  // Test

  }  // Symbol

}  // Stig
