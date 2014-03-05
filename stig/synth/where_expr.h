/* <stig/synth/where_expr.h>

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
#include <functional>

#include <base/no_copy_semantics.h>
#include <stig/expr/where.h>
#include <stig/stig.package.cst.h>
#include <stig/symbol/scope.h>
#include <stig/synth/expr.h>
#include <stig/synth/def_factory.h>
#include <stig/synth/scope_and_def.h>
#include <tools/nycr/lexeme.h>

namespace Stig {

  namespace Synth {

    /* TODO */
    class TExprFactory;

    /* TODO */
    class TWhereExpr
        : public TExpr, public TScope {
      NO_COPY_SEMANTICS(TWhereExpr);
      public:

      /* TODO */
      TWhereExpr(const TExprFactory *expr_factory, const Package::Syntax::TWhereExpr *where_expr);

      /* TODO */
      ~TWhereExpr();

      /* TODO */
      Expr::TExpr::TPtr Build() const;

      /* TODO */
      void BuildSymbol();

      /* TODO */
      virtual void ForEachInnerScope(const std::function<void (TScope *)> &cb);

      Symbol::TScope::TPtr GetScopeSymbol() const;

      Expr::TWhere::TPtr GetSymbol() const;

      bool HasSymbol() const;

      private:

      /* TODO */
      class TLocalDefFactory
          : public TDefFactory {
        NO_COPY_SEMANTICS(TLocalDefFactory);
        public:

        /* TODO */
        static void NewDefs(
              const TExprFactory *expr_factory,
              const Package::Syntax::TOptDefSeq *opt_def_seq);

        private:

        /* TODO */
        TLocalDefFactory(const TExprFactory *expr_factory);

        /* TODO */
        virtual void operator()(const Package::Syntax::TInstallerDef *that) const;

        /* TODO */
        virtual void operator()(const Package::Syntax::TUpgraderDef *that) const;

        /* TODO */
        virtual void operator()(const Package::Syntax::TUninstallerDef *that) const;

        /* TODO */
        void OnTopLevel(const char *desc, const TPosRange &pos_range) const;

      };  // TLocalDefFactory

      /* TODO */
      virtual void ForEachControlledRef(const std::function<void (TAnyRef &)> &cb) const;

      /* TODO */
      const Package::Syntax::TWhereExpr *WhereExpr;

      /* TODO */
      TExpr *Expr;

      /* TODO */
      Expr::TWhere::TPtr Symbol;

    };  // TWhereExpr

  }  // Synth

}  // Stig
