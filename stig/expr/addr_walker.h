/* <stig/expr/addr_walker.h>

   This is for retrieivng a set of all the Addrs (Tuples) that are being used in the Database,
   that is, all of the <[ ... ]>s used in keys, new, delete, and mutation expressions.

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

#include <utility>
#include <unordered_set>

#include <stig/expr/expr.h>
#include <stig/symbol/stmt/stmt_block.h>

namespace Stig {

  namespace Expr {

    /* For every Addr that is part of a keys, new, delete, or mutation expression,
       add that Addr to addr_set. */
    void DatabaseAddrsInExpr(const TExpr::TPtr &root,
                             std::unordered_set<std::pair<Stig::Type::TType, Stig::Type::TType>> &addr_set);

    /* For every Addr that accesses the db in the statement block,
       add that Addr to the addr set. */
    void DatabaseAddrsFromNewStmt(const Stig::Symbol::Stmt::TNew *stmt,
                                  std::unordered_set<std::pair<Stig::Type::TType, Stig::Type::TType>> &addr_set);

  } // Expr

} // Stig