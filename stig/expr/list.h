/* <stig/expr/list.h>

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
#include <vector>

#include <base/no_copy_semantics.h>
#include <stig/expr/ctor.h>

namespace Stig {

  namespace Expr {

    class TList
        : public TCtor<std::vector<TExpr::TPtr>> {
      NO_COPY_SEMANTICS(TList);
      public:

      typedef std::shared_ptr<TList> TPtr;

      typedef std::vector<TExpr::TPtr> TExprVec;

      static TPtr New(const Type::TType &type, const TPosRange &pos_range);

      static TPtr New(const TExprVec &exprs, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      inline const TExprVec &GetExprs() const {
        assert(this);
        return GetContainer();
      }

      virtual Type::TType GetType() const;

      private:

      /* Empty contructor */
      TList(const Type::TType &type, const TPosRange &pos_range);

      /* Non-empty constructor */
      TList(const TExprVec &exprs, const TPosRange &pos_range);

      mutable Type::TType Type;

    };  // TList

  }  // Expr

}  // Stig
