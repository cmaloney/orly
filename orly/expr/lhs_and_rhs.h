/* <orly/expr/lhs_and_rhs.h>

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
#include <orly/expr/binary.h>
#include <orly/expr/leaf.h>
#include <orly/expr/lhsrhsable.h>

namespace Orly {

  namespace Expr {

    class TLhs
        : public TLeaf {
      NO_COPY(TLhs);
      public:

      typedef std::shared_ptr<TLhs> TPtr;

      static TPtr New(const TLhsRhsable::TPtr &lhsrhsable, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      const TLhsRhsable *GetLhsRhsable() const;

      virtual Type::TType GetType() const;

      private:

      TLhs(const TLhsRhsable *lhsrhsable, const TPosRange &pos_range);

      const TLhsRhsable *LhsRhsable;

    };  // TLhs

    class TRhs
        : public TLeaf {
      NO_COPY(TRhs);
      public:

      typedef std::shared_ptr<TRhs> TPtr;

      static TPtr New(const TLhsRhsable::TPtr &lhsrhsable, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      const TLhsRhsable *GetLhsRhsable() const;

      virtual Type::TType GetType() const;

      private:

      TRhs(const TLhsRhsable *lhsrhsable, const TPosRange &pos_range);

      const TLhsRhsable *LhsRhsable;

    };  // TRhs

  }  // Expr

}  // Orly