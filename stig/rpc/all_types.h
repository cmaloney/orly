/* <stig/rpc/all_types.h> 

   Closes the hierarchy of types.

   Copyright 2010-2014 Tagged
   
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

#include <cassert>

#include <stig/rpc/type/addr.h>
#include <stig/rpc/type/mono.h>
#include <stig/rpc/type/obj.h>
#include <stig/rpc/type/poly1.h>
#include <stig/rpc/type/poly2.h>

namespace Stig {

  namespace Rpc {

    /* The visitor to the hierarchy of types. */
    class TType::TVisitor {
      public:

      /* Do-little. */
      virtual ~TVisitor();

      /* Mono. */
      virtual void operator()(const Type::TBool     *type) const = 0;
      virtual void operator()(const Type::TId       *type) const = 0;
      virtual void operator()(const Type::TInt      *type) const = 0;
      virtual void operator()(const Type::TReal     *type) const = 0;
      virtual void operator()(const Type::TStr      *type) const = 0;
      virtual void operator()(const Type::TTimeDiff *type) const = 0;
      virtual void operator()(const Type::TTimePnt  *type) const = 0;

      /* Poly-1. */
      virtual void operator()(const Type::TList *type) const = 0;
      virtual void operator()(const Type::TOpt  *type) const = 0;
      virtual void operator()(const Type::TSet  *type) const = 0;

      /* Poly-2. */
      virtual void operator()(const Type::TDict *type) const = 0;

      /* Compound. */
      virtual void operator()(const Type::TAddr *type) const = 0;
      virtual void operator()(const Type::TObj  *type) const = 0;

      protected:

      /* Do-little. */
      TVisitor() {}

    };  // TType::TVisitor

    /* Accept the visitor. */
    template <TKind Kind>
    inline void Type::TMono<Kind>::Accept(const TVisitor &visitor) const {
      assert(this);
      assert(&visitor);
      visitor(this);
    }

    /* Accept the visitor. */
    template <TKind Kind>
    inline void Type::TPoly1<Kind>::Accept(const TVisitor &visitor) const {
      assert(this);
      assert(&visitor);
      visitor(this);
    }

    /* Accept the visitor. */
    template <TKind Kind>
    inline void Type::TPoly2<Kind>::Accept(const TVisitor &visitor) const {
      assert(this);
      assert(&visitor);
      visitor(this);
    }

    /* Accept the visitor. */
    inline void Type::TAddr::Accept(const TVisitor &visitor) const {
      assert(this);
      assert(&visitor);
      visitor(this);
    }

    /* Accept the visitor. */
    inline void Type::TObj::Accept(const TVisitor &visitor) const {
      assert(this);
      assert(&visitor);
      visitor(this);
    }

    /* The double-visitor to the hierarchy of types. */
    class TType::TDoubleVisitor {
      public:

      /* Do-little. */
      virtual ~TDoubleVisitor();

      /* Apply this double-visitor to the given pair of types. */
      void Visit(const TType *lhs, const TType *rhs) const;

      /* Handlers. */
      virtual void operator()(const Type::TAddr     *lhs, const Type::TAddr     *rhs) const = 0;
      virtual void operator()(const Type::TAddr     *lhs, const Type::TBool     *rhs) const = 0;
      virtual void operator()(const Type::TAddr     *lhs, const Type::TDict     *rhs) const = 0;
      virtual void operator()(const Type::TAddr     *lhs, const Type::TId       *rhs) const = 0;
      virtual void operator()(const Type::TAddr     *lhs, const Type::TInt      *rhs) const = 0;
      virtual void operator()(const Type::TAddr     *lhs, const Type::TList     *rhs) const = 0;
      virtual void operator()(const Type::TAddr     *lhs, const Type::TObj      *rhs) const = 0;
      virtual void operator()(const Type::TAddr     *lhs, const Type::TOpt      *rhs) const = 0;
      virtual void operator()(const Type::TAddr     *lhs, const Type::TReal     *rhs) const = 0;
      virtual void operator()(const Type::TAddr     *lhs, const Type::TSet      *rhs) const = 0;
      virtual void operator()(const Type::TAddr     *lhs, const Type::TStr      *rhs) const = 0;
      virtual void operator()(const Type::TAddr     *lhs, const Type::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Type::TAddr     *lhs, const Type::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Type::TBool     *lhs, const Type::TAddr     *rhs) const = 0;
      virtual void operator()(const Type::TBool     *lhs, const Type::TBool     *rhs) const = 0;
      virtual void operator()(const Type::TBool     *lhs, const Type::TDict     *rhs) const = 0;
      virtual void operator()(const Type::TBool     *lhs, const Type::TId       *rhs) const = 0;
      virtual void operator()(const Type::TBool     *lhs, const Type::TInt      *rhs) const = 0;
      virtual void operator()(const Type::TBool     *lhs, const Type::TList     *rhs) const = 0;
      virtual void operator()(const Type::TBool     *lhs, const Type::TObj      *rhs) const = 0;
      virtual void operator()(const Type::TBool     *lhs, const Type::TOpt      *rhs) const = 0;
      virtual void operator()(const Type::TBool     *lhs, const Type::TReal     *rhs) const = 0;
      virtual void operator()(const Type::TBool     *lhs, const Type::TSet      *rhs) const = 0;
      virtual void operator()(const Type::TBool     *lhs, const Type::TStr      *rhs) const = 0;
      virtual void operator()(const Type::TBool     *lhs, const Type::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Type::TBool     *lhs, const Type::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Type::TDict     *lhs, const Type::TAddr     *rhs) const = 0;
      virtual void operator()(const Type::TDict     *lhs, const Type::TBool     *rhs) const = 0;
      virtual void operator()(const Type::TDict     *lhs, const Type::TDict     *rhs) const = 0;
      virtual void operator()(const Type::TDict     *lhs, const Type::TId       *rhs) const = 0;
      virtual void operator()(const Type::TDict     *lhs, const Type::TInt      *rhs) const = 0;
      virtual void operator()(const Type::TDict     *lhs, const Type::TList     *rhs) const = 0;
      virtual void operator()(const Type::TDict     *lhs, const Type::TObj      *rhs) const = 0;
      virtual void operator()(const Type::TDict     *lhs, const Type::TOpt      *rhs) const = 0;
      virtual void operator()(const Type::TDict     *lhs, const Type::TReal     *rhs) const = 0;
      virtual void operator()(const Type::TDict     *lhs, const Type::TSet      *rhs) const = 0;
      virtual void operator()(const Type::TDict     *lhs, const Type::TStr      *rhs) const = 0;
      virtual void operator()(const Type::TDict     *lhs, const Type::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Type::TDict     *lhs, const Type::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Type::TId       *lhs, const Type::TAddr     *rhs) const = 0;
      virtual void operator()(const Type::TId       *lhs, const Type::TBool     *rhs) const = 0;
      virtual void operator()(const Type::TId       *lhs, const Type::TDict     *rhs) const = 0;
      virtual void operator()(const Type::TId       *lhs, const Type::TId       *rhs) const = 0;
      virtual void operator()(const Type::TId       *lhs, const Type::TInt      *rhs) const = 0;
      virtual void operator()(const Type::TId       *lhs, const Type::TList     *rhs) const = 0;
      virtual void operator()(const Type::TId       *lhs, const Type::TObj      *rhs) const = 0;
      virtual void operator()(const Type::TId       *lhs, const Type::TOpt      *rhs) const = 0;
      virtual void operator()(const Type::TId       *lhs, const Type::TReal     *rhs) const = 0;
      virtual void operator()(const Type::TId       *lhs, const Type::TSet      *rhs) const = 0;
      virtual void operator()(const Type::TId       *lhs, const Type::TStr      *rhs) const = 0;
      virtual void operator()(const Type::TId       *lhs, const Type::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Type::TId       *lhs, const Type::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Type::TInt      *lhs, const Type::TAddr     *rhs) const = 0;
      virtual void operator()(const Type::TInt      *lhs, const Type::TBool     *rhs) const = 0;
      virtual void operator()(const Type::TInt      *lhs, const Type::TDict     *rhs) const = 0;
      virtual void operator()(const Type::TInt      *lhs, const Type::TId       *rhs) const = 0;
      virtual void operator()(const Type::TInt      *lhs, const Type::TInt      *rhs) const = 0;
      virtual void operator()(const Type::TInt      *lhs, const Type::TList     *rhs) const = 0;
      virtual void operator()(const Type::TInt      *lhs, const Type::TObj      *rhs) const = 0;
      virtual void operator()(const Type::TInt      *lhs, const Type::TOpt      *rhs) const = 0;
      virtual void operator()(const Type::TInt      *lhs, const Type::TReal     *rhs) const = 0;
      virtual void operator()(const Type::TInt      *lhs, const Type::TSet      *rhs) const = 0;
      virtual void operator()(const Type::TInt      *lhs, const Type::TStr      *rhs) const = 0;
      virtual void operator()(const Type::TInt      *lhs, const Type::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Type::TInt      *lhs, const Type::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Type::TList     *lhs, const Type::TAddr     *rhs) const = 0;
      virtual void operator()(const Type::TList     *lhs, const Type::TBool     *rhs) const = 0;
      virtual void operator()(const Type::TList     *lhs, const Type::TDict     *rhs) const = 0;
      virtual void operator()(const Type::TList     *lhs, const Type::TId       *rhs) const = 0;
      virtual void operator()(const Type::TList     *lhs, const Type::TInt      *rhs) const = 0;
      virtual void operator()(const Type::TList     *lhs, const Type::TList     *rhs) const = 0;
      virtual void operator()(const Type::TList     *lhs, const Type::TObj      *rhs) const = 0;
      virtual void operator()(const Type::TList     *lhs, const Type::TOpt      *rhs) const = 0;
      virtual void operator()(const Type::TList     *lhs, const Type::TReal     *rhs) const = 0;
      virtual void operator()(const Type::TList     *lhs, const Type::TSet      *rhs) const = 0;
      virtual void operator()(const Type::TList     *lhs, const Type::TStr      *rhs) const = 0;
      virtual void operator()(const Type::TList     *lhs, const Type::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Type::TList     *lhs, const Type::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Type::TObj      *lhs, const Type::TAddr     *rhs) const = 0;
      virtual void operator()(const Type::TObj      *lhs, const Type::TBool     *rhs) const = 0;
      virtual void operator()(const Type::TObj      *lhs, const Type::TDict     *rhs) const = 0;
      virtual void operator()(const Type::TObj      *lhs, const Type::TId       *rhs) const = 0;
      virtual void operator()(const Type::TObj      *lhs, const Type::TInt      *rhs) const = 0;
      virtual void operator()(const Type::TObj      *lhs, const Type::TList     *rhs) const = 0;
      virtual void operator()(const Type::TObj      *lhs, const Type::TObj      *rhs) const = 0;
      virtual void operator()(const Type::TObj      *lhs, const Type::TOpt      *rhs) const = 0;
      virtual void operator()(const Type::TObj      *lhs, const Type::TReal     *rhs) const = 0;
      virtual void operator()(const Type::TObj      *lhs, const Type::TSet      *rhs) const = 0;
      virtual void operator()(const Type::TObj      *lhs, const Type::TStr      *rhs) const = 0;
      virtual void operator()(const Type::TObj      *lhs, const Type::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Type::TObj      *lhs, const Type::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Type::TOpt      *lhs, const Type::TAddr     *rhs) const = 0;
      virtual void operator()(const Type::TOpt      *lhs, const Type::TBool     *rhs) const = 0;
      virtual void operator()(const Type::TOpt      *lhs, const Type::TDict     *rhs) const = 0;
      virtual void operator()(const Type::TOpt      *lhs, const Type::TId       *rhs) const = 0;
      virtual void operator()(const Type::TOpt      *lhs, const Type::TInt      *rhs) const = 0;
      virtual void operator()(const Type::TOpt      *lhs, const Type::TList     *rhs) const = 0;
      virtual void operator()(const Type::TOpt      *lhs, const Type::TObj      *rhs) const = 0;
      virtual void operator()(const Type::TOpt      *lhs, const Type::TOpt      *rhs) const = 0;
      virtual void operator()(const Type::TOpt      *lhs, const Type::TReal     *rhs) const = 0;
      virtual void operator()(const Type::TOpt      *lhs, const Type::TSet      *rhs) const = 0;
      virtual void operator()(const Type::TOpt      *lhs, const Type::TStr      *rhs) const = 0;
      virtual void operator()(const Type::TOpt      *lhs, const Type::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Type::TOpt      *lhs, const Type::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Type::TReal     *lhs, const Type::TAddr     *rhs) const = 0;
      virtual void operator()(const Type::TReal     *lhs, const Type::TBool     *rhs) const = 0;
      virtual void operator()(const Type::TReal     *lhs, const Type::TDict     *rhs) const = 0;
      virtual void operator()(const Type::TReal     *lhs, const Type::TId       *rhs) const = 0;
      virtual void operator()(const Type::TReal     *lhs, const Type::TInt      *rhs) const = 0;
      virtual void operator()(const Type::TReal     *lhs, const Type::TList     *rhs) const = 0;
      virtual void operator()(const Type::TReal     *lhs, const Type::TObj      *rhs) const = 0;
      virtual void operator()(const Type::TReal     *lhs, const Type::TOpt      *rhs) const = 0;
      virtual void operator()(const Type::TReal     *lhs, const Type::TReal     *rhs) const = 0;
      virtual void operator()(const Type::TReal     *lhs, const Type::TSet      *rhs) const = 0;
      virtual void operator()(const Type::TReal     *lhs, const Type::TStr      *rhs) const = 0;
      virtual void operator()(const Type::TReal     *lhs, const Type::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Type::TReal     *lhs, const Type::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Type::TSet      *lhs, const Type::TAddr     *rhs) const = 0;
      virtual void operator()(const Type::TSet      *lhs, const Type::TBool     *rhs) const = 0;
      virtual void operator()(const Type::TSet      *lhs, const Type::TDict     *rhs) const = 0;
      virtual void operator()(const Type::TSet      *lhs, const Type::TId       *rhs) const = 0;
      virtual void operator()(const Type::TSet      *lhs, const Type::TInt      *rhs) const = 0;
      virtual void operator()(const Type::TSet      *lhs, const Type::TList     *rhs) const = 0;
      virtual void operator()(const Type::TSet      *lhs, const Type::TObj      *rhs) const = 0;
      virtual void operator()(const Type::TSet      *lhs, const Type::TOpt      *rhs) const = 0;
      virtual void operator()(const Type::TSet      *lhs, const Type::TReal     *rhs) const = 0;
      virtual void operator()(const Type::TSet      *lhs, const Type::TSet      *rhs) const = 0;
      virtual void operator()(const Type::TSet      *lhs, const Type::TStr      *rhs) const = 0;
      virtual void operator()(const Type::TSet      *lhs, const Type::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Type::TSet      *lhs, const Type::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Type::TStr      *lhs, const Type::TAddr     *rhs) const = 0;
      virtual void operator()(const Type::TStr      *lhs, const Type::TBool     *rhs) const = 0;
      virtual void operator()(const Type::TStr      *lhs, const Type::TDict     *rhs) const = 0;
      virtual void operator()(const Type::TStr      *lhs, const Type::TId       *rhs) const = 0;
      virtual void operator()(const Type::TStr      *lhs, const Type::TInt      *rhs) const = 0;
      virtual void operator()(const Type::TStr      *lhs, const Type::TList     *rhs) const = 0;
      virtual void operator()(const Type::TStr      *lhs, const Type::TObj      *rhs) const = 0;
      virtual void operator()(const Type::TStr      *lhs, const Type::TOpt      *rhs) const = 0;
      virtual void operator()(const Type::TStr      *lhs, const Type::TReal     *rhs) const = 0;
      virtual void operator()(const Type::TStr      *lhs, const Type::TSet      *rhs) const = 0;
      virtual void operator()(const Type::TStr      *lhs, const Type::TStr      *rhs) const = 0;
      virtual void operator()(const Type::TStr      *lhs, const Type::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Type::TStr      *lhs, const Type::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Type::TTimeDiff *lhs, const Type::TAddr     *rhs) const = 0;
      virtual void operator()(const Type::TTimeDiff *lhs, const Type::TBool     *rhs) const = 0;
      virtual void operator()(const Type::TTimeDiff *lhs, const Type::TDict     *rhs) const = 0;
      virtual void operator()(const Type::TTimeDiff *lhs, const Type::TId       *rhs) const = 0;
      virtual void operator()(const Type::TTimeDiff *lhs, const Type::TInt      *rhs) const = 0;
      virtual void operator()(const Type::TTimeDiff *lhs, const Type::TList     *rhs) const = 0;
      virtual void operator()(const Type::TTimeDiff *lhs, const Type::TObj      *rhs) const = 0;
      virtual void operator()(const Type::TTimeDiff *lhs, const Type::TOpt      *rhs) const = 0;
      virtual void operator()(const Type::TTimeDiff *lhs, const Type::TReal     *rhs) const = 0;
      virtual void operator()(const Type::TTimeDiff *lhs, const Type::TSet      *rhs) const = 0;
      virtual void operator()(const Type::TTimeDiff *lhs, const Type::TStr      *rhs) const = 0;
      virtual void operator()(const Type::TTimeDiff *lhs, const Type::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Type::TTimeDiff *lhs, const Type::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Type::TTimePnt  *lhs, const Type::TAddr     *rhs) const = 0;
      virtual void operator()(const Type::TTimePnt  *lhs, const Type::TBool     *rhs) const = 0;
      virtual void operator()(const Type::TTimePnt  *lhs, const Type::TDict     *rhs) const = 0;
      virtual void operator()(const Type::TTimePnt  *lhs, const Type::TId       *rhs) const = 0;
      virtual void operator()(const Type::TTimePnt  *lhs, const Type::TInt      *rhs) const = 0;
      virtual void operator()(const Type::TTimePnt  *lhs, const Type::TList     *rhs) const = 0;
      virtual void operator()(const Type::TTimePnt  *lhs, const Type::TObj      *rhs) const = 0;
      virtual void operator()(const Type::TTimePnt  *lhs, const Type::TOpt      *rhs) const = 0;
      virtual void operator()(const Type::TTimePnt  *lhs, const Type::TReal     *rhs) const = 0;
      virtual void operator()(const Type::TTimePnt  *lhs, const Type::TSet      *rhs) const = 0;
      virtual void operator()(const Type::TTimePnt  *lhs, const Type::TStr      *rhs) const = 0;
      virtual void operator()(const Type::TTimePnt  *lhs, const Type::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Type::TTimePnt  *lhs, const Type::TTimePnt  *rhs) const = 0;

      protected:

      /* Do-little. */
      TDoubleVisitor() {}

      private:

      /* Used by Visit(). */
      class TLhsVisitor final
          : public TVisitor {
        public:

        /* Cache the arguments. */
        TLhsVisitor(const TType *rhs, const TType::TDoubleVisitor &double_visitor)
            : Rhs(rhs), DoubleVisitor(double_visitor) {}

        /* Handlers.  Each caches the lhs and dispatches to the rhs. */
        virtual void operator()(const Type::TAddr     *lhs) const override;
        virtual void operator()(const Type::TBool     *lhs) const override;
        virtual void operator()(const Type::TDict     *lhs) const override;
        virtual void operator()(const Type::TId       *lhs) const override;
        virtual void operator()(const Type::TInt      *lhs) const override;
        virtual void operator()(const Type::TList     *lhs) const override;
        virtual void operator()(const Type::TObj      *lhs) const override;
        virtual void operator()(const Type::TOpt      *lhs) const override;
        virtual void operator()(const Type::TReal     *lhs) const override;
        virtual void operator()(const Type::TSet      *lhs) const override;
        virtual void operator()(const Type::TStr      *lhs) const override;
        virtual void operator()(const Type::TTimeDiff *lhs) const override;
        virtual void operator()(const Type::TTimePnt  *lhs) const override;

        private:

        /* The rhs to dispatch to. */
        const TType *Rhs;

        /* The double-visitor to dispatch to. */
        const TDoubleVisitor &DoubleVisitor;

      };  // TType::TDoubleVisitor::TLhsVisitor

      /* Used by Visit().  Defined in the .cc file. */
      template <typename TLhs>
      class TRhsVisitor final
          : public TVisitor {
        public:

        /* Cache the arguments. */
        TRhsVisitor(const TLhs *lhs, const TType::TDoubleVisitor &double_visitor)
            : Lhs(lhs), DoubleVisitor(double_visitor) {}

        /* Handlers.  Each dispatches to the correct handler in the double-visitor. */
        virtual void operator()(const Type::TAddr     *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Type::TBool     *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Type::TDict     *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Type::TId       *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Type::TInt      *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Type::TList     *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Type::TObj      *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Type::TOpt      *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Type::TReal     *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Type::TSet      *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Type::TStr      *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Type::TTimeDiff *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Type::TTimePnt  *rhs) const override { DoubleVisitor(Lhs, rhs); }

        private:

        /* The lhs to dispatch to. */
        const TLhs *Lhs;

        /* The double-visitor to dispatch to. */
        const TType::TDoubleVisitor &DoubleVisitor;

      };  // TType::TDoubleVisitor::TRhsVisitor<TLhs>

    };  // TType::TDoubleVisitor

  }  // Rpc

}  // Stig

