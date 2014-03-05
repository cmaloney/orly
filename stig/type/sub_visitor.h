/* <stig/type/sub_visitor.h>

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
#include <stig/type/infix_visitor.h>
#include <stig/type/unwrap.h>

namespace Stig {

  namespace Type {

    class TSubVisitor
        : public Type::TInfixVisitor {
      NO_COPY_SEMANTICS(TSubVisitor);
      protected:

      TSubVisitor(Type::TType &type, const TPosRange &pos_range)
          : Type::TInfixVisitor(type, pos_range) {}

      private:

      virtual void operator()(const Type::TAddr     *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr     *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr     *, const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr     *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr     *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr     *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr     *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr     *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr     *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr     *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr     *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr     *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool     *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool     *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool     *, const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool     *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool     *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool     *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool     *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool     *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool     *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool     *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool     *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool     *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict     *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict     *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict     *, const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict     *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict     *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict     *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict     *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict     *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict     *lhs, const Type::TSet      *rhs) const {
        if (Type::Unwrap(lhs->GetKey()) != Type::Unwrap(rhs->GetElem())) {
          throw TExprError(HERE, PosRange, "Cannot remove keys from dictionary using set of different type");
        }
        Type = lhs->AsType();
      }
      virtual void operator()(const Type::TDict     *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict     *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict     *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId       *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId       *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId       *, const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId       *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId       *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId       *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId       *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId       *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId       *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId       *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId       *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId       *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TInt      *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TInt      *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TInt      *, const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TInt      *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TInt      *, const Type::TInt      *) const {
        Type = Type::TInt::Get();
      }
      virtual void operator()(const Type::TInt      *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TInt      *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TInt      *, const Type::TReal     *) const {
        Type = Type::TReal::Get();
      }
      virtual void operator()(const Type::TInt      *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TInt      *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TInt      *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TInt      *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList     *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList     *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList     *, const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList     *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList     *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList     *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList     *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList     *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList     *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList     *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList     *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList     *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj      *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj      *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj      *, const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj      *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj      *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj      *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj      *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj      *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj      *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj      *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj      *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj      *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TReal     *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TReal     *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TReal     *, const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TReal     *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TReal     *, const Type::TInt      *) const {
        Type = Type::TReal::Get();
      }
      virtual void operator()(const Type::TReal     *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TReal     *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TReal     *, const Type::TReal     *) const {
        Type = Type::TReal::Get();
      }
      virtual void operator()(const Type::TReal     *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TReal     *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TReal     *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TReal     *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TSet      *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TSet      *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TSet      *, const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TSet      *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TSet      *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TSet      *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TSet      *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TSet      *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TSet      *lhs, const Type::TSet      *rhs) const {
        if (Type::Unwrap(lhs->GetElem()) != Type::Unwrap(rhs->GetElem())) {
          throw TExprError(HERE, PosRange, "Cannot get asymmetric difference of sets of different types");
        }
        Type = lhs->AsType();
      }
      virtual void operator()(const Type::TSet      *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TSet      *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TSet      *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TStr      *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TStr      *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TStr      *, const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TStr      *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TStr      *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TStr      *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TStr      *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TStr      *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TStr      *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TStr      *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TStr      *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TStr      *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimeDiff *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimeDiff *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimeDiff *, const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimeDiff *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimeDiff *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimeDiff *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimeDiff *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimeDiff *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimeDiff *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimeDiff *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimeDiff *, const Type::TTimeDiff *) const {
        Type = Type::TTimeDiff::Get();
      }
      virtual void operator()(const Type::TTimeDiff *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimePnt  *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimePnt  *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimePnt  *, const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimePnt  *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimePnt  *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimePnt  *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimePnt  *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimePnt  *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimePnt  *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimePnt  *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimePnt  *, const Type::TTimeDiff *) const {
        Type = Type::TTimePnt::Get();
      }
      virtual void operator()(const Type::TTimePnt  *, const Type::TTimePnt  *) const {
        Type = Type::TTimeDiff::Get();
      }

    };  // TSubVisitor

  }  // Type

}  // Stig
