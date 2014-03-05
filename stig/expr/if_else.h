/* <stig/expr/if_else.h>

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

#include <array>
#include <memory>

#include <base/no_copy_semantics.h>
#include <stig/expr/n_ary.h>

namespace Stig {

  namespace Expr {

    class TIfElse
        : public TNAry<std::array<TExpr::TPtr, 3U>> {
      NO_COPY_SEMANTICS(TIfElse);
      public:

      typedef std::shared_ptr<TIfElse> TPtr;

      typedef std::array<TExpr::TPtr, 3U> TExprArray;

      static TPtr New(
          const TExpr::TPtr &true_case,
          const TExpr::TPtr &predicate,
          const TExpr::TPtr &false_case,
          const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      /* Alias for GetContainer */
      inline const TExprArray &GetExprs() const {
        assert(this);
        return GetContainer();
      }

      const TExpr::TPtr &GetTrue() const;

      const TExpr::TPtr &GetPredicate() const;

      const TExpr::TPtr &GetFalse() const;

      virtual Type::TType GetType() const;

      private:

      TIfElse(
          const TExpr::TPtr &true_case,
          const TExpr::TPtr &predicate,
          const TExpr::TPtr &false_case,
          const TPosRange &pos_range);

    };  // TIfElse

  }  // Expr

}  // Stig
