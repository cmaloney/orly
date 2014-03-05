/* <stig/type/set_ops_visitor.h>

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

#include <stig/error.h>
#include <stig/pos_range.h>
#include <stig/type/equal_visitor.h>
#include <stig/type/unwrap.h>

namespace Stig {

  namespace Type {

    class TSetOpsVisitor
        : public Type::TEqualVisitor {
      NO_COPY_SEMANTICS(TSetOpsVisitor);
      protected:

      TSetOpsVisitor(Type::TType &type, const TPosRange &pos_range)
          : Type::TEqualVisitor(type, pos_range) {}

      private:

      virtual void operator()(const Type::TAddr     *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool     *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict     *, const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId       *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TInt      *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList     *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj      *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TReal     *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TSet      *lhs, const Type::TSet      *rhs) const {
        if (Type::Unwrap(lhs->GetElem()) != Type::Unwrap(rhs->GetElem())) {
          throw TExprError(HERE, PosRange, "Cannot perform set operations on sets of differing type");
        }
        Type = lhs->AsType();
      }
      virtual void operator()(const Type::TStr      *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimeDiff *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimePnt  *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }

    };  // TSetOpsVisitor

  }  // Type

}  // Stig
