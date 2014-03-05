/* <stig/expr/div.h>

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

#include <memory>

#include <base/no_copy_semantics.h>
#include <stig/expr/binary.h>

namespace Stig {

  namespace Expr {

    class TDiv
        : public TBinary {
      NO_COPY_SEMANTICS(TDiv);
      public:

      typedef std::shared_ptr<TDiv> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TDiv(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

    };  // TDiv

  }  // Expr

}  // Stig
