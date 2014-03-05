/* <stig/type/equal_visitor.h>

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

#include <stig/type/assoc_infix_visitor.h>

namespace Stig {

  namespace Type {

    class TEqualVisitor
        : public TAssocInfixVisitor {
      NO_COPY_SEMANTICS(TEqualVisitor);
      protected:

      TEqualVisitor(TType &type, const TPosRange &pos_range)
          : TAssocInfixVisitor(type, pos_range) {}

      virtual void operator()(const TAddr     *, const TAddr     *) const = 0;
      virtual void operator()(const TBool     *, const TBool     *) const = 0;
      virtual void operator()(const TDict     *, const TDict     *) const = 0;
      virtual void operator()(const TId       *, const TId       *) const = 0;
      virtual void operator()(const TInt      *, const TInt      *) const = 0;
      virtual void operator()(const TList     *, const TList     *) const = 0;
      virtual void operator()(const TObj      *, const TObj      *) const = 0;
      virtual void operator()(const TReal     *, const TReal     *) const = 0;
      virtual void operator()(const TSet      *, const TSet      *) const = 0;
      virtual void operator()(const TStr      *, const TStr      *) const = 0;
      virtual void operator()(const TTimeDiff *, const TTimeDiff *) const = 0;
      virtual void operator()(const TTimePnt  *, const TTimePnt  *) const = 0;

      private:

      virtual void operator()(const TAddr     *, const TBool     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr     *, const TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr     *, const TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr     *, const TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr     *, const TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr     *, const TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr     *, const TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr     *, const TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr     *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr     *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TAddr     *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool     *, const TDict     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool     *, const TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool     *, const TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool     *, const TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool     *, const TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool     *, const TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool     *, const TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool     *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool     *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TBool     *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict     *, const TId       *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict     *, const TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict     *, const TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict     *, const TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict     *, const TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict     *, const TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict     *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict     *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TDict     *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TId       *, const TInt      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TId       *, const TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TId       *, const TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TId       *, const TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TId       *, const TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TId       *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TId       *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TId       *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TInt      *, const TList     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TInt      *, const TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TInt      *, const TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TInt      *, const TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TInt      *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TInt      *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TInt      *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TList     *, const TObj      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TList     *, const TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TList     *, const TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TList     *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TList     *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TList     *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TObj      *, const TReal     *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TObj      *, const TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TObj      *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TObj      *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TObj      *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TReal     *, const TSet      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TReal     *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TReal     *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TReal     *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TSet      *, const TStr      *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TSet      *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TSet      *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TStr      *, const TTimeDiff *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TStr      *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }
      virtual void operator()(const TTimeDiff *, const TTimePnt  *) const { throw TExprError(HERE, PosRange); }

    };  // TEqualVisitor

  }  // Type

}  // Stig
