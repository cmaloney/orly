/* <stig/expr/unary.h>

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
#include <stig/expr/expr.h>
#include <stig/expr/interior.h>

namespace Stig {

  namespace Expr {

    class TUnary
        : public TInterior {
      NO_COPY_SEMANTICS(TUnary);
      public:

      ~TUnary();

      const TExpr::TPtr &GetExpr() const;

      void SetExpr(const TExpr::TPtr &expr);

      protected:

      // NOTE: Should only be used by TWhere
      TUnary(const TPosRange &pos_range);

      TUnary(const TExpr::TPtr &expr, const TPosRange &pos_range);

      private:

      TExpr::TPtr Expr;

    };  // TUnary

  }  // Expr

}  // Stig
