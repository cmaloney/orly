/* <stig/synth/postfix_is_known.h>

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
#include <stig/synth/type.h>
#include <stig/synth/expr.h>

namespace Stig {

  namespace Synth {

    /* TODO */
    class TExprFactory;

    /* TODO */
    class TPostfixIsKnown
        : public TExpr {
      NO_COPY_SEMANTICS(TPostfixIsKnown);
      public:

      /* TODO */
      TPostfixIsKnown(const TExprFactory *expr_factory, const Package::Syntax::TPostfixIsKnown *postfix_is_known);

      /* TODO */
      virtual ~TPostfixIsKnown();

      /* TODO */
      virtual Expr::TExpr::TPtr Build() const;

      /* TODO */
      virtual void ForEachInnerScope(const std::function<void (TScope *)> &cb);

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      private:

      /* TODO */
      const Package::Syntax::TPostfixIsKnown *PostfixIsKnown;

      /* TODO */
      TExpr *Expr;

    };  // TPostfixIsKnown

  }  // Synth

}  // Stig
