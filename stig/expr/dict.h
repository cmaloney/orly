/* <stig/expr/dict.h>

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
#include <map>
#include <memory>

#include <base/no_copy_semantics.h>
#include <stig/expr/ctor.h>

namespace Stig {

  namespace Expr {

    class TDict
        : public TCtor<std::unordered_map<TExpr::TPtr, TExpr::TPtr>> {
      NO_COPY_SEMANTICS(TDict);
      public:

      typedef std::shared_ptr<TDict> TPtr;

      typedef std::unordered_map<TExpr::TPtr, TExpr::TPtr> TMemberMap;

      static TPtr New(const Type::TType &type, const TPosRange &pos_range);

      static TPtr New(const TMemberMap &exprs, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      inline const TMemberMap &GetMembers() const {
        assert(this);
        return GetContainer();
      }

      virtual Type::TType GetType() const;

      private:

      /* Empty constructor */
      TDict(const Type::TType &type, const TPosRange &pos_range);

      /* Non-empty constructor */
      TDict(const TMemberMap &members, const TPosRange &pos_range);

      mutable Type::TType Type;

    };  // TDict

  }  // Expr

}  // Stig
