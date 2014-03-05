/* <stig/expr/range.h>

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

    class TRange
        : public TNAry<std::array<TExpr::TPtr, 3U>> {
      NO_COPY_SEMANTICS(TRange);
      public:

      typedef std::shared_ptr<TRange> TPtr;

      typedef std::array<TExpr::TPtr, 3U> TExprArray;

      static TPtr New(
          const TExpr::TPtr &start,
          const TExpr::TPtr &opt_stride,
          const TExpr::TPtr &opt_end,
          bool end_included,
          const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      inline const TExprArray &GetExprs() const {
        assert(this);
        return GetContainer();
      }

      /* Return the pointer to the start */
      const TExpr::TPtr &GetStart() const;

      /* Return the pointer to an optional end. This means the pointer can be null */
      const TExpr::TPtr &GetOptStride() const;

      /* Return the pointer to an optional end. This means the pointer can be null */
      const TExpr::TPtr &GetOptEnd() const;

      /* Returns the state of the end */
      bool HasEndIncluded() const;

      private:

      TRange(
          const TExpr::TPtr &start,
          const TExpr::TPtr &opt_stride,
          const TExpr::TPtr &opt_end,
          bool end_included,
          const TPosRange &pos_range);

      bool EndIncluded;

    };  // TRange

  }  // Expr

}  // Stig
