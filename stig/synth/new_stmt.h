/* <stig/synth/new_stmt.h>

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

#include <base/no_copy_semantics.h>
#include <stig/stig.package.cst.h>
#include <stig/synth/stmt.h>
#include <stig/symbol/stmt/stmt.h>

namespace Stig {

  namespace Synth {

    /* TODO */
    class TExpr;
    class TExprFactory;

    /* TODO */
    class TNewStmt
        : public TStmt {
      NO_COPY_SEMANTICS(TNewStmt);
      public:

      /* TODO */
      TNewStmt(const TExprFactory *expr_factory, const Package::Syntax::TNewStmt *new_stmt);

      /* TODO */
      virtual ~TNewStmt();

      /* TODO */
      virtual Symbol::Stmt::TStmt::TPtr Build() const;

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb) const;

      /* TODO */
      virtual void ForEachInnerScope(const std::function<void (TScope *)> &cb) const;

      private:

      /* TODO */
      const Package::Syntax::TNewStmt *NewStmt;

      /* TODO */
      TExpr *Lhs;

      /* TODO */
      TExpr *Rhs;

    };  // TNewStmt

  }  // Synth

}  // Stig
