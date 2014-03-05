/* <stig/symbol/root.h>

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

#include <base/assert_true.h>
#include <base/no_copy_semantics.h>
#include <stig/expr/expr.h>
#include <stig/expr/expr_parent.h>

namespace Stig {

  namespace Symbol {

    class TRoot
        : public Expr::TExprParent {
      NO_COPY_SEMANTICS(TRoot);
      public:

      virtual ~TRoot();

      const Expr::TExpr::TPtr &GetExpr() const;

      void SetExpr(const Expr::TExpr::TPtr &expr);

      protected:

      TRoot();  // Should only be used by TFunction

      TRoot(const Expr::TExpr::TPtr &expr);

      private:

      Expr::TExpr::TPtr Expr;

    };  // TRoot

  }  // Symbol

}  // Symbol
