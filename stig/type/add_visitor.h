/* <stig/type/add_visitor.h>

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
#include <stig/type/assoc_infix_visitor.h>
#include <stig/type/unwrap.h>

namespace Stig {

  namespace Type {

    /* Defines basic behaviours for add. */
    class TAddVisitor
        : public TAssocInfixVisitor {
      NO_COPY_SEMANTICS(TAddVisitor);
      protected:

      TAddVisitor(TType &type, const TPosRange &pos_range)
          : TAssocInfixVisitor(type, pos_range) {}

      private:

      virtual void operator()(const TAddr *, const TAddr     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr *, const TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr *, const TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr *, const TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr *, const TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr *, const TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr *, const TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr *, const TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr *, const TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool *, const TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool *, const TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool *, const TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool *, const TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool *, const TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool *, const TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool *, const TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool *, const TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict *lhs, const TDict     *rhs) const {
        if (Unwrap(lhs->GetKey()) != Unwrap(rhs->GetKey())) {
          throw TExprError(HERE, PosRange, "Cannot add dict to dict of differing type of keys.");
        }
        if (Unwrap(lhs->GetVal()) != Unwrap(rhs->GetVal())) {
          throw TExprError(HERE, PosRange, "Cannot add dict to dict of differing type of values.");
        }
        Type = lhs->AsType();
      }
      virtual void operator()(const TDict *, const TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict *, const TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict *, const TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict *, const TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict *, const TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict *, const TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TId   *, const TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TId   *, const TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TId   *, const TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TId   *, const TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TId   *, const TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TId   *, const TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TId   *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TId   *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TId   *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TInt  *, const TInt      *) const {
        Type = TInt::Get();
      }
      virtual void operator()(const TInt  *, const TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TInt  *, const TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TInt  *, const TReal     *) const {
        Type = Type::TReal::Get();
      }
      virtual void operator()(const TInt  *, const TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TInt  *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TInt  *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TInt  *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TList *lhs, const TList     *rhs) const {
        if (Unwrap(lhs->GetElem()) != Unwrap(rhs->GetElem())) {
          throw TExprError(HERE, PosRange, "Cannot concatenate lists of differing types.");
        }
        Type = lhs->AsType();
      }
      virtual void operator()(const TList *, const TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TList *, const TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TList *, const TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TList *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TList *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TList *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TObj  *, const TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TObj  *, const TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TObj  *, const TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TObj  *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TObj  *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TObj  *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TReal *, const TReal     *) const {
        Type = TReal::Get();
      }
      virtual void operator()(const TReal *, const TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TReal *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TReal *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TReal *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TSet  *, const TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TSet  *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TSet  *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TSet  *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TStr  *, const TStr      *) const {
        Type = TStr::Get();
      }
      virtual void operator()(const TStr  *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TStr  *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TTimeDiff *, const TTimeDiff *) const {
        Type = TTimeDiff::Get();
      }
      virtual void operator()(const TTimeDiff *, const TTimePnt  *) const {
        Type = TTimePnt::Get();
      }
      virtual void operator()(const TTimePnt  *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }

    };  // TAddVisitor

  }  // Type

}  // Stig
