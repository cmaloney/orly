/* <stig/type/assoc_infix_visitor.h>

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

#include <stig/type/infix_visitor.h>

namespace Stig {

  namespace Type {

    class TAssocInfixVisitor
        : public TInfixVisitor {
      NO_COPY_SEMANTICS(TAssocInfixVisitor);
      protected:

      /* TODO */
      TAssocInfixVisitor(TType &type, const TPosRange &pos_range)
          : TInfixVisitor(type, pos_range) {}

      /* TODO */
      virtual void operator()(const TAddr     *, const TAddr     *) const = 0;
      virtual void operator()(const TAddr     *, const TBool     *) const = 0;
      virtual void operator()(const TAddr     *, const TDict     *) const = 0;
      virtual void operator()(const TAddr     *, const TId       *) const = 0;
      virtual void operator()(const TAddr     *, const TInt      *) const = 0;
      virtual void operator()(const TAddr     *, const TList     *) const = 0;
      virtual void operator()(const TAddr     *, const TObj      *) const = 0;
      virtual void operator()(const TAddr     *, const TReal     *) const = 0;
      virtual void operator()(const TAddr     *, const TSet      *) const = 0;
      virtual void operator()(const TAddr     *, const TStr      *) const = 0;
      virtual void operator()(const TAddr     *, const TTimeDiff *) const = 0;
      virtual void operator()(const TAddr     *, const TTimePnt  *) const = 0;
      virtual void operator()(const TBool     *, const TBool     *) const = 0;
      virtual void operator()(const TBool     *, const TDict     *) const = 0;
      virtual void operator()(const TBool     *, const TId       *) const = 0;
      virtual void operator()(const TBool     *, const TInt      *) const = 0;
      virtual void operator()(const TBool     *, const TList     *) const = 0;
      virtual void operator()(const TBool     *, const TObj      *) const = 0;
      virtual void operator()(const TBool     *, const TReal     *) const = 0;
      virtual void operator()(const TBool     *, const TSet      *) const = 0;
      virtual void operator()(const TBool     *, const TStr      *) const = 0;
      virtual void operator()(const TBool     *, const TTimeDiff *) const = 0;
      virtual void operator()(const TBool     *, const TTimePnt  *) const = 0;
      virtual void operator()(const TDict     *, const TDict     *) const = 0;
      virtual void operator()(const TDict     *, const TId       *) const = 0;
      virtual void operator()(const TDict     *, const TInt      *) const = 0;
      virtual void operator()(const TDict     *, const TList     *) const = 0;
      virtual void operator()(const TDict     *, const TObj      *) const = 0;
      virtual void operator()(const TDict     *, const TReal     *) const = 0;
      virtual void operator()(const TDict     *, const TSet      *) const = 0;
      virtual void operator()(const TDict     *, const TStr      *) const = 0;
      virtual void operator()(const TDict     *, const TTimeDiff *) const = 0;
      virtual void operator()(const TDict     *, const TTimePnt  *) const = 0;
      virtual void operator()(const TId       *, const TId       *) const = 0;
      virtual void operator()(const TId       *, const TInt      *) const = 0;
      virtual void operator()(const TId       *, const TList     *) const = 0;
      virtual void operator()(const TId       *, const TObj      *) const = 0;
      virtual void operator()(const TId       *, const TReal     *) const = 0;
      virtual void operator()(const TId       *, const TSet      *) const = 0;
      virtual void operator()(const TId       *, const TStr      *) const = 0;
      virtual void operator()(const TId       *, const TTimeDiff *) const = 0;
      virtual void operator()(const TId       *, const TTimePnt  *) const = 0;
      virtual void operator()(const TInt      *, const TInt      *) const = 0;
      virtual void operator()(const TInt      *, const TList     *) const = 0;
      virtual void operator()(const TInt      *, const TObj      *) const = 0;
      virtual void operator()(const TInt      *, const TReal     *) const = 0;
      virtual void operator()(const TInt      *, const TSet      *) const = 0;
      virtual void operator()(const TInt      *, const TStr      *) const = 0;
      virtual void operator()(const TInt      *, const TTimeDiff *) const = 0;
      virtual void operator()(const TInt      *, const TTimePnt  *) const = 0;
      virtual void operator()(const TList     *, const TList     *) const = 0;
      virtual void operator()(const TList     *, const TObj      *) const = 0;
      virtual void operator()(const TList     *, const TReal     *) const = 0;
      virtual void operator()(const TList     *, const TSet      *) const = 0;
      virtual void operator()(const TList     *, const TStr      *) const = 0;
      virtual void operator()(const TList     *, const TTimeDiff *) const = 0;
      virtual void operator()(const TList     *, const TTimePnt  *) const = 0;
      virtual void operator()(const TObj      *, const TObj      *) const = 0;
      virtual void operator()(const TObj      *, const TReal     *) const = 0;
      virtual void operator()(const TObj      *, const TSet      *) const = 0;
      virtual void operator()(const TObj      *, const TStr      *) const = 0;
      virtual void operator()(const TObj      *, const TTimeDiff *) const = 0;
      virtual void operator()(const TObj      *, const TTimePnt  *) const = 0;
      virtual void operator()(const TReal     *, const TReal     *) const = 0;
      virtual void operator()(const TReal     *, const TSet      *) const = 0;
      virtual void operator()(const TReal     *, const TStr      *) const = 0;
      virtual void operator()(const TReal     *, const TTimeDiff *) const = 0;
      virtual void operator()(const TReal     *, const TTimePnt  *) const = 0;
      virtual void operator()(const TSet      *, const TSet      *) const = 0;
      virtual void operator()(const TSet      *, const TStr      *) const = 0;
      virtual void operator()(const TSet      *, const TTimeDiff *) const = 0;
      virtual void operator()(const TSet      *, const TTimePnt  *) const = 0;
      virtual void operator()(const TStr      *, const TStr      *) const = 0;
      virtual void operator()(const TStr      *, const TTimeDiff *) const = 0;
      virtual void operator()(const TStr      *, const TTimePnt  *) const = 0;
      virtual void operator()(const TTimeDiff *, const TTimeDiff *) const = 0;
      virtual void operator()(const TTimeDiff *, const TTimePnt  *) const = 0;
      virtual void operator()(const TTimePnt  *, const TTimePnt  *) const = 0;

      private:

      virtual void operator()(const TBool     *lhs, const TAddr     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TDict     *lhs, const TAddr     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TId       *lhs, const TAddr     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TInt      *lhs, const TAddr     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TList     *lhs, const TAddr     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TObj      *lhs, const TAddr     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TReal     *lhs, const TAddr     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TSet      *lhs, const TAddr     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TStr      *lhs, const TAddr     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimeDiff *lhs, const TAddr     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimePnt  *lhs, const TAddr     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TDict     *lhs, const TBool     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TId       *lhs, const TBool     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TInt      *lhs, const TBool     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TList     *lhs, const TBool     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TObj      *lhs, const TBool     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TReal     *lhs, const TBool     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TSet      *lhs, const TBool     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TStr      *lhs, const TBool     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimeDiff *lhs, const TBool     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimePnt  *lhs, const TBool     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TId       *lhs, const TDict     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TInt      *lhs, const TDict     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TList     *lhs, const TDict     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TObj      *lhs, const TDict     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TReal     *lhs, const TDict     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TSet      *lhs, const TDict     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TStr      *lhs, const TDict     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimeDiff *lhs, const TDict     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimePnt  *lhs, const TDict     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TInt      *lhs, const TId       *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TList     *lhs, const TId       *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TObj      *lhs, const TId       *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TReal     *lhs, const TId       *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TSet      *lhs, const TId       *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TStr      *lhs, const TId       *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimeDiff *lhs, const TId       *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimePnt  *lhs, const TId       *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TList     *lhs, const TInt      *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TObj      *lhs, const TInt      *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TReal     *lhs, const TInt      *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TSet      *lhs, const TInt      *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TStr      *lhs, const TInt      *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimeDiff *lhs, const TInt      *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimePnt  *lhs, const TInt      *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TObj      *lhs, const TList     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TReal     *lhs, const TList     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TSet      *lhs, const TList     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TStr      *lhs, const TList     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimeDiff *lhs, const TList     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimePnt  *lhs, const TList     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TReal     *lhs, const TObj      *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TSet      *lhs, const TObj      *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TStr      *lhs, const TObj      *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimeDiff *lhs, const TObj      *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimePnt  *lhs, const TObj      *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TSet      *lhs, const TReal     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TStr      *lhs, const TReal     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimeDiff *lhs, const TReal     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimePnt  *lhs, const TReal     *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TStr      *lhs, const TSet      *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimeDiff *lhs, const TSet      *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimePnt  *lhs, const TSet      *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimeDiff *lhs, const TStr      *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimePnt  *lhs, const TStr      *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }
      virtual void operator()(const TTimePnt  *lhs, const TTimeDiff *rhs) const final { TType::Accept(rhs->AsType(), lhs->AsType(), *this); }

    };  // TAssocInfixVisitor

  }  // Type

}  // Stig