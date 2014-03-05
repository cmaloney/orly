/* <stig/synth/while_expr.h>

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

#include <cassert>

#include <base/no_copy_semantics.h>
#include <stig/expr/while.h>
#include <stig/expr/thatable.h>
#include <stig/stig.package.cst.h>
#include <stig/synth/thatable_expr.h>

namespace Stig {

  namespace Synth {

    /* TODO */
    class TExprFactory;

    /* TODO */
    class TWhileExpr
        : public TThatableExpr {
      NO_COPY_SEMANTICS(TWhileExpr);
      public:

      /* TODO */
      TWhileExpr(const TExprFactory *expr_factory, const Package::Syntax::TInfixWhile *infix_while);

      /* TODO */
      virtual ~TWhileExpr();

      /* TODO */
      virtual Expr::TExpr::TPtr Build() const;

      /* TODO */
      virtual void ForEachInnerScope(const std::function<void (TScope *)> &cb);

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      /* TODO */
      const Expr::TWhile::TPtr &GetSymbol() const;

      /* TODO */
      virtual Expr::TThatable::TPtr GetThatableSymbol() const;

      private:

      /* TODO */
      const Package::Syntax::TInfixWhile *InfixWhile;

      /* TODO */
      TExpr *Lhs;

      /* TODO */
      TExpr *Rhs;

      /* TODO */
      mutable Expr::TWhile::TPtr Symbol;

    };  // TWhileExpr

  }  // Synth

}  // Stig
