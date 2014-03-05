/* <stig/expr/binary.h>

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
#include <stig/expr/interior.h>

namespace Stig {

  namespace Expr {

    class TBinary
        : public TInterior {
      NO_COPY_SEMANTICS(TBinary);
      public:

      ~TBinary();

      const TExpr::TPtr &GetLhs() const;

      const TExpr::TPtr &GetRhs() const;

      void SetRhs(const TExpr::TPtr &rhs);

      protected:

      // NOTE: Should only be used by TThatableBinary (TReduce and TFilter) and TSort
      TBinary(const TExpr::TPtr &lhs, const TPosRange &pos_range);

      TBinary(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

      private:

      TExpr::TPtr Lhs;

      TExpr::TPtr Rhs;

    };  // TBinary

  }  // Expr

}  // Stig
