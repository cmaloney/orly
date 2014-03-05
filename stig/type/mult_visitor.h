/* <stig/type/mult_visitor.h>

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

namespace Stig {

  namespace Type {

    class TMultVisitor
        : public Type::TAssocInfixVisitor {
      protected:

      TMultVisitor(Type::TType &type, const TPosRange &pos_range)
          : Type::TAssocInfixVisitor(type, pos_range) {}

      private:

      virtual void operator()(const Type::TAddr *, const Type::TAddr     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr *, const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TAddr *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool *, const Type::TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool *, const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TBool *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict *, const Type::TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TDict *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId   *, const Type::TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId   *, const Type::TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId   *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId   *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId   *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId   *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId   *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId   *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TId   *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TInt  *, const Type::TInt      *) const {
        Type = Type::TInt::Get();
      }
      virtual void operator()(const Type::TInt  *, const Type::TList     *rhs) const {
        Type = rhs->AsType();
      }
      virtual void operator()(const Type::TInt  *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TInt  *, const Type::TReal     *) const {
        Type = Type::TReal::Get();
      }
      virtual void operator()(const Type::TInt  *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TInt  *, const Type::TStr      *) const {
        Type = Type::TStr::Get();
      }
      virtual void operator()(const Type::TInt  *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TInt  *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList *, const Type::TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TList *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj  *, const Type::TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj  *, const Type::TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj  *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj  *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj  *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TObj  *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TReal *, const Type::TReal     *) const {
        Type = Type::TReal::Get();
      }
      virtual void operator()(const Type::TReal *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TReal *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TReal *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TReal *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TSet  *, const Type::TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TSet  *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TSet  *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TSet  *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TStr  *, const Type::TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TStr  *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TStr  *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimeDiff *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimeDiff *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const Type::TTimePnt  *, const Type::TTimePnt  *) const { throw TExprError(HERE, PosRange); }

    };  // TMultVisitor

  }  // Type

}  // Stig