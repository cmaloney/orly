/* <stig/synth/reduce_expr.h>

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
#include <stig/expr/reduce.h>
#include <stig/expr/thatable.h>
#include <stig/stig.package.cst.h>
#include <stig/synth/startable_expr.h>
#include <stig/synth/thatable_expr.h>

namespace Stig {

  namespace Synth {

    /* TODO */
    class TExprFactory;

    /* TODO */
    class TReduceExpr
        : public TStartableExpr,
          public TThatableExpr {
      NO_COPY_SEMANTICS(TReduceExpr);
      public:

      /* TODO */
      TReduceExpr(const TExprFactory *expr_factory, const Package::Syntax::TInfixReduce *infix_reduce);

      /* TODO */
      virtual ~TReduceExpr();

      /* TODO */
      virtual Expr::TExpr::TPtr Build() const;

      /* TODO */
      virtual void ForEachInnerScope(const std::function<void (TScope *)> &cb);

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      /* TODO */
      const Expr::TReduce::TPtr &GetSymbol() const;

      /* TODO */
      virtual Expr::TStartable::TPtr GetStartableSymbol() const;

      /* TODO */
      virtual Expr::TThatable::TPtr GetThatableSymbol() const;

      private:

      /* TODO */
      const Package::Syntax::TInfixReduce *InfixReduce;

      /* TODO */
      TExpr *Lhs;

      /* TODO */
      TExpr *Rhs;

      /* TODO */
      mutable Expr::TReduce::TPtr Symbol;

    };  // TReduceExpr

  }  // Synth

}  // Stig
