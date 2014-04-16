/* <orly/expr/expr.h>

   TODO

   Copyright 2010-2014 OrlyAtomics, Inc.

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

#include <memory>

#include <base/assert_true.h>
#include <base/class_traits.h>
#include <orly/expr/expr_parent.h>
#include <orly/pos_range.h>
#include <orly/type/impl.h>

namespace Orly {

  namespace Expr {

    class TExpr {
      NO_COPY(TExpr);
      public:

      class TVisitor;

      typedef std::shared_ptr<TExpr> TPtr;

      virtual ~TExpr() {}

      virtual void Accept(const TVisitor &visitor) const = 0;

      const TExprParent *GetExprParent() const;

      void SetExprParent(const TExprParent *expr_parent);

      template <typename TFinal>
      const TFinal *As() const;

      const TPosRange &GetPosRange() const;

      virtual Type::TType GetType() const = 0;

      template <typename TFinal>
      bool Is() const;

      template <typename TFinal>
      const TFinal *TryAs() const;

      void UnsetExprParent(const TExprParent *expr_parent);

      protected:

      TExpr(const TPosRange &pos_range);

      private:

      const TExprParent *ExprParent;

      const TPosRange PosRange;

    };  // TExpr

  }  // Expr

}  // Orly
