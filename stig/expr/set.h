/* <stig/expr/set.h>

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

#include <functional>
#include <memory>
#include <unordered_set>

#include <base/no_copy_semantics.h>
#include <stig/expr/ctor.h>

namespace Stig {

  namespace Expr {

    class TSet
        : public TCtor<std::unordered_set<TExpr::TPtr>> {
      NO_COPY_SEMANTICS(TSet);
      public:

      typedef std::shared_ptr<TSet> TPtr;

      typedef std::unordered_set<TExpr::TPtr> TExprSet;

      static TPtr New(const Type::TType &type, const TPosRange &pos_range);

      static TPtr New(const TExprSet &exprs, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      inline const TExprSet &GetExprs() const {
        assert(this);
        return GetContainer();
      }

      virtual Type::TType GetType() const;

      private:

      /* Empty contructor */
      TSet(const Type::TType &type, const TPosRange &pos_range);

      /* Non-empty constructor */
      TSet(const TExprSet &exprs, const TPosRange &pos_range);

      mutable Type::TType Type;

    };  // TSet

  }  // Expr

}  // Stig
