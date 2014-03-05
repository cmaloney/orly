/* <stig/synth/delete_stmt.h>

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
#include <stig/symbol/stmt/stmt.h>
#include <stig/synth/stmt.h>
#include <stig/synth/type.h>

namespace Stig {

  namespace Synth {

    /* TODO */
    class TExpr;
    class TExprFactory;

    /* TODO */
    class TDeleteStmt
        : public TStmt {
      NO_COPY_SEMANTICS(TDeleteStmt);
      public:

      /* TODO */
      TDeleteStmt(const TExprFactory *expr_factory, const Package::Syntax::TDeleteStmt *delete_stmt);

      /* TODO */
      virtual ~TDeleteStmt();

      /* TODO */
      virtual Symbol::Stmt::TStmt::TPtr Build() const;

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb) const;

      /* TODO */
      virtual void ForEachInnerScope(const std::function<void (TScope *)> &cb) const;

      /* Do-little */
      TType *GetValueType() {
        return ValueType;
      }

      private:

      /* TODO */
      const Package::Syntax::TDeleteStmt *DeleteStmt;

      /* TODO */
      TExpr *Expr;

      /* The type of the value that is being deleted. */
      TType *ValueType;
    };  // TDeleteStmt

  }  // Synth

}  // Stig
