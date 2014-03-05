/* <stig/synth/assert_expr.h>

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
#include <vector>

#include <base/no_copy_semantics.h>
#include <base/opt.h>
#include <stig/expr/assert.h>
#include <stig/expr/thatable.h>
#include <stig/stig.package.cst.h>
#include <stig/synth/thatable_expr.h>

namespace Stig {

  namespace Synth {

    /* TODO */
    class TExprFactory;

    /* TODO */
    class TAssertExpr
        : public TThatableExpr {
      NO_COPY_SEMANTICS(TAssertExpr);
      public:

      /* TODO */
      TAssertExpr(const TExprFactory *expr_factory, const Package::Syntax::TAssertExpr *assert_expr);

      /* TODO */
      virtual ~TAssertExpr();

      /* TODO */
      virtual Expr::TExpr::TPtr Build() const;

      /* TODO */
      virtual void ForEachInnerScope(const std::function<void (TScope *)> &cb);

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      /* TODO */
      const Expr::TAssert::TPtr &GetSymbol() const;

      /* TODO */
      virtual Expr::TThatable::TPtr GetThatableSymbol() const;

      private:

      /* TODO */
      class TAssertCase {
        NO_COPY_SEMANTICS(TAssertCase);
        public:

        TAssertCase(const Base::TOpt<std::string> &opt_name, TExpr *expr);

        ~TAssertCase();

        Expr::TAssertCase::TPtr Build(const Expr::TAssert::TPtr &assert) const;

        void ForEachInnerScope(const std::function<void (TScope *)> &cb);

        void ForEachRef(const std::function<void (TAnyRef &)> &cb);

        private:

        const Base::TOpt<std::string> OptName;

        TExpr *Expr;

      };  // TAssertCase

      /* TODO */
      typedef std::vector<TAssertCase *> TAssertCaseVec;

      void Cleanup();

      /* TODO */
      const Package::Syntax::TAssertExpr *AssertExpr;

      /* TODO */
      TAssertCaseVec AssertCases;

      /* TODO */
      TExpr *Expr;

      /* TODO */
      mutable Expr::TAssert::TPtr Symbol;

    };  // TAssertExpr

  }  // Synth

}  // Stig
