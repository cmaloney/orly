/* <stig/expr/addr_member.h>

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
#include <stig/expr/unary.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/impl.h>

namespace Stig {

  namespace Expr {

    class TAddrMember
        : public TUnary {
      NO_COPY_SEMANTICS(TAddrMember);
      public:

      typedef std::shared_ptr<TAddrMember> TPtr;

      static TPtr New(const TExpr::TPtr &expr, size_t index, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      size_t GetIndex() const;

      virtual Type::TType GetType() const;

      private:

      TAddrMember(const TExpr::TPtr &expr, size_t index, const TPosRange &pos_range);

      size_t Index;

    };  // TAddrMember

  }  // Expr

}  // Stig
