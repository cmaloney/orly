/* <orly/symbol/stmt/stmt_arg.h>

   TODO

   Copyright 2010-2014 OrlyAtomics, Inc.

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

#include <base/class_traits.h>
#include <orly/symbol/root.h>

namespace Orly {

  namespace Symbol {

    namespace Stmt {

      class TStmt;

      class TStmtArg
          : public TRoot {
        NO_COPY(TStmtArg);
        public:

        typedef std::shared_ptr<TStmtArg> TPtr;

        static TPtr New(const Expr::TExpr::TPtr &expr);

        const TStmt *GetStmt() const;

        void SetStmt(const TStmt *stmt);

        void UnsetStmt(const TStmt *stmt);

        private:

        TStmtArg(const Expr::TExpr::TPtr &expr);

        const TStmt *Stmt;

      };  // TStmtArg

    }  // Stmt

  }  // Symbol

}  // Orly
