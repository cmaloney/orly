/* <stig/rpc/all_vars.h> 

   Closes the hierarchy of variants.

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

#include <stig/rpc/var/addr.h>
#include <stig/rpc/var/dict.h>
#include <stig/rpc/var/list.h>
#include <stig/rpc/var/mono.h>
#include <stig/rpc/var/obj.h>
#include <stig/rpc/var/opt.h>
#include <stig/rpc/var/set.h>

namespace Stig {

  namespace Rpc {

    /* The visitor to the hierarchy of variants. */
    class TVar::TVisitor {
      public:

      /* Do-little. */
      virtual ~TVisitor();

      /* Mono. */
      virtual void operator()(const Var::TBool     *var) const = 0;
      virtual void operator()(const Var::TId       *var) const = 0;
      virtual void operator()(const Var::TInt      *var) const = 0;
      virtual void operator()(const Var::TReal     *var) const = 0;
      virtual void operator()(const Var::TStr      *var) const = 0;
      virtual void operator()(const Var::TTimeDiff *var) const = 0;
      virtual void operator()(const Var::TTimePnt  *var) const = 0;

      /* Poly-1. */
      virtual void operator()(const Var::TList *var) const = 0;
      virtual void operator()(const Var::TOpt  *var) const = 0;
      virtual void operator()(const Var::TSet  *var) const = 0;

      /* Poly-2. */
      virtual void operator()(const Var::TDict *var) const = 0;

      /* Compound. */
      virtual void operator()(const Var::TAddr *var) const = 0;
      virtual void operator()(const Var::TObj  *var) const = 0;

      protected:

      /* Do-little. */
      TVisitor() {}

    };  // TVar::TVisitor

    /* Accept the visitor. */
    template <TKind Kind, typename TVal>
    inline void Var::TMono<Kind, TVal>::Accept(const TVisitor &visitor) const {
      assert(this);
      assert(&visitor);
      visitor(this);
    }

    /* Accept the visitor. */
    inline void Var::TList::Accept(const TVisitor &visitor) const {
      assert(this);
      assert(&visitor);
      visitor(this);
    }

    /* Accept the visitor. */
    inline void Var::TOpt::Accept(const TVisitor &visitor) const {
      assert(this);
      assert(&visitor);
      visitor(this);
    }

    /* Accept the visitor. */
    inline void Var::TSet::Accept(const TVisitor &visitor) const {
      assert(this);
      assert(&visitor);
      visitor(this);
    }

    /* Accept the visitor. */
    inline void Var::TDict::Accept(const TVisitor &visitor) const {
      assert(this);
      assert(&visitor);
      visitor(this);
    }

    /* Accept the visitor. */
    inline void Var::TAddr::Accept(const TVisitor &visitor) const {
      assert(this);
      assert(&visitor);
      visitor(this);
    }

    /* Accept the visitor. */
    inline void Var::TObj::Accept(const TVisitor &visitor) const {
      assert(this);
      assert(&visitor);
      visitor(this);
    }

    /* The double-visitor to the hierarchy of vars. */
    class TVar::TDoubleVisitor {
      public:

      /* Do-little. */
      virtual ~TDoubleVisitor();

      /* Apply this double-visitor to the given pair of vars. */
      void Visit(const TVar *lhs, const TVar *rhs) const;

      /* Handlers. */
      virtual void operator()(const Var::TAddr     *lhs, const Var::TAddr     *rhs) const = 0;
      virtual void operator()(const Var::TAddr     *lhs, const Var::TBool     *rhs) const = 0;
      virtual void operator()(const Var::TAddr     *lhs, const Var::TDict     *rhs) const = 0;
      virtual void operator()(const Var::TAddr     *lhs, const Var::TId       *rhs) const = 0;
      virtual void operator()(const Var::TAddr     *lhs, const Var::TInt      *rhs) const = 0;
      virtual void operator()(const Var::TAddr     *lhs, const Var::TList     *rhs) const = 0;
      virtual void operator()(const Var::TAddr     *lhs, const Var::TObj      *rhs) const = 0;
      virtual void operator()(const Var::TAddr     *lhs, const Var::TOpt      *rhs) const = 0;
      virtual void operator()(const Var::TAddr     *lhs, const Var::TReal     *rhs) const = 0;
      virtual void operator()(const Var::TAddr     *lhs, const Var::TSet      *rhs) const = 0;
      virtual void operator()(const Var::TAddr     *lhs, const Var::TStr      *rhs) const = 0;
      virtual void operator()(const Var::TAddr     *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TAddr     *lhs, const Var::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Var::TBool     *lhs, const Var::TAddr     *rhs) const = 0;
      virtual void operator()(const Var::TBool     *lhs, const Var::TBool     *rhs) const = 0;
      virtual void operator()(const Var::TBool     *lhs, const Var::TDict     *rhs) const = 0;
      virtual void operator()(const Var::TBool     *lhs, const Var::TId       *rhs) const = 0;
      virtual void operator()(const Var::TBool     *lhs, const Var::TInt      *rhs) const = 0;
      virtual void operator()(const Var::TBool     *lhs, const Var::TList     *rhs) const = 0;
      virtual void operator()(const Var::TBool     *lhs, const Var::TObj      *rhs) const = 0;
      virtual void operator()(const Var::TBool     *lhs, const Var::TOpt      *rhs) const = 0;
      virtual void operator()(const Var::TBool     *lhs, const Var::TReal     *rhs) const = 0;
      virtual void operator()(const Var::TBool     *lhs, const Var::TSet      *rhs) const = 0;
      virtual void operator()(const Var::TBool     *lhs, const Var::TStr      *rhs) const = 0;
      virtual void operator()(const Var::TBool     *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TBool     *lhs, const Var::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Var::TDict     *lhs, const Var::TAddr     *rhs) const = 0;
      virtual void operator()(const Var::TDict     *lhs, const Var::TBool     *rhs) const = 0;
      virtual void operator()(const Var::TDict     *lhs, const Var::TDict     *rhs) const = 0;
      virtual void operator()(const Var::TDict     *lhs, const Var::TId       *rhs) const = 0;
      virtual void operator()(const Var::TDict     *lhs, const Var::TInt      *rhs) const = 0;
      virtual void operator()(const Var::TDict     *lhs, const Var::TList     *rhs) const = 0;
      virtual void operator()(const Var::TDict     *lhs, const Var::TObj      *rhs) const = 0;
      virtual void operator()(const Var::TDict     *lhs, const Var::TOpt      *rhs) const = 0;
      virtual void operator()(const Var::TDict     *lhs, const Var::TReal     *rhs) const = 0;
      virtual void operator()(const Var::TDict     *lhs, const Var::TSet      *rhs) const = 0;
      virtual void operator()(const Var::TDict     *lhs, const Var::TStr      *rhs) const = 0;
      virtual void operator()(const Var::TDict     *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TDict     *lhs, const Var::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Var::TId       *lhs, const Var::TAddr     *rhs) const = 0;
      virtual void operator()(const Var::TId       *lhs, const Var::TBool     *rhs) const = 0;
      virtual void operator()(const Var::TId       *lhs, const Var::TDict     *rhs) const = 0;
      virtual void operator()(const Var::TId       *lhs, const Var::TId       *rhs) const = 0;
      virtual void operator()(const Var::TId       *lhs, const Var::TInt      *rhs) const = 0;
      virtual void operator()(const Var::TId       *lhs, const Var::TList     *rhs) const = 0;
      virtual void operator()(const Var::TId       *lhs, const Var::TObj      *rhs) const = 0;
      virtual void operator()(const Var::TId       *lhs, const Var::TOpt      *rhs) const = 0;
      virtual void operator()(const Var::TId       *lhs, const Var::TReal     *rhs) const = 0;
      virtual void operator()(const Var::TId       *lhs, const Var::TSet      *rhs) const = 0;
      virtual void operator()(const Var::TId       *lhs, const Var::TStr      *rhs) const = 0;
      virtual void operator()(const Var::TId       *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TId       *lhs, const Var::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Var::TInt      *lhs, const Var::TAddr     *rhs) const = 0;
      virtual void operator()(const Var::TInt      *lhs, const Var::TBool     *rhs) const = 0;
      virtual void operator()(const Var::TInt      *lhs, const Var::TDict     *rhs) const = 0;
      virtual void operator()(const Var::TInt      *lhs, const Var::TId       *rhs) const = 0;
      virtual void operator()(const Var::TInt      *lhs, const Var::TInt      *rhs) const = 0;
      virtual void operator()(const Var::TInt      *lhs, const Var::TList     *rhs) const = 0;
      virtual void operator()(const Var::TInt      *lhs, const Var::TObj      *rhs) const = 0;
      virtual void operator()(const Var::TInt      *lhs, const Var::TOpt      *rhs) const = 0;
      virtual void operator()(const Var::TInt      *lhs, const Var::TReal     *rhs) const = 0;
      virtual void operator()(const Var::TInt      *lhs, const Var::TSet      *rhs) const = 0;
      virtual void operator()(const Var::TInt      *lhs, const Var::TStr      *rhs) const = 0;
      virtual void operator()(const Var::TInt      *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TInt      *lhs, const Var::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Var::TList     *lhs, const Var::TAddr     *rhs) const = 0;
      virtual void operator()(const Var::TList     *lhs, const Var::TBool     *rhs) const = 0;
      virtual void operator()(const Var::TList     *lhs, const Var::TDict     *rhs) const = 0;
      virtual void operator()(const Var::TList     *lhs, const Var::TId       *rhs) const = 0;
      virtual void operator()(const Var::TList     *lhs, const Var::TInt      *rhs) const = 0;
      virtual void operator()(const Var::TList     *lhs, const Var::TList     *rhs) const = 0;
      virtual void operator()(const Var::TList     *lhs, const Var::TObj      *rhs) const = 0;
      virtual void operator()(const Var::TList     *lhs, const Var::TOpt      *rhs) const = 0;
      virtual void operator()(const Var::TList     *lhs, const Var::TReal     *rhs) const = 0;
      virtual void operator()(const Var::TList     *lhs, const Var::TSet      *rhs) const = 0;
      virtual void operator()(const Var::TList     *lhs, const Var::TStr      *rhs) const = 0;
      virtual void operator()(const Var::TList     *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TList     *lhs, const Var::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Var::TObj      *lhs, const Var::TAddr     *rhs) const = 0;
      virtual void operator()(const Var::TObj      *lhs, const Var::TBool     *rhs) const = 0;
      virtual void operator()(const Var::TObj      *lhs, const Var::TDict     *rhs) const = 0;
      virtual void operator()(const Var::TObj      *lhs, const Var::TId       *rhs) const = 0;
      virtual void operator()(const Var::TObj      *lhs, const Var::TInt      *rhs) const = 0;
      virtual void operator()(const Var::TObj      *lhs, const Var::TList     *rhs) const = 0;
      virtual void operator()(const Var::TObj      *lhs, const Var::TObj      *rhs) const = 0;
      virtual void operator()(const Var::TObj      *lhs, const Var::TOpt      *rhs) const = 0;
      virtual void operator()(const Var::TObj      *lhs, const Var::TReal     *rhs) const = 0;
      virtual void operator()(const Var::TObj      *lhs, const Var::TSet      *rhs) const = 0;
      virtual void operator()(const Var::TObj      *lhs, const Var::TStr      *rhs) const = 0;
      virtual void operator()(const Var::TObj      *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TObj      *lhs, const Var::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Var::TOpt      *lhs, const Var::TAddr     *rhs) const = 0;
      virtual void operator()(const Var::TOpt      *lhs, const Var::TBool     *rhs) const = 0;
      virtual void operator()(const Var::TOpt      *lhs, const Var::TDict     *rhs) const = 0;
      virtual void operator()(const Var::TOpt      *lhs, const Var::TId       *rhs) const = 0;
      virtual void operator()(const Var::TOpt      *lhs, const Var::TInt      *rhs) const = 0;
      virtual void operator()(const Var::TOpt      *lhs, const Var::TList     *rhs) const = 0;
      virtual void operator()(const Var::TOpt      *lhs, const Var::TObj      *rhs) const = 0;
      virtual void operator()(const Var::TOpt      *lhs, const Var::TOpt      *rhs) const = 0;
      virtual void operator()(const Var::TOpt      *lhs, const Var::TReal     *rhs) const = 0;
      virtual void operator()(const Var::TOpt      *lhs, const Var::TSet      *rhs) const = 0;
      virtual void operator()(const Var::TOpt      *lhs, const Var::TStr      *rhs) const = 0;
      virtual void operator()(const Var::TOpt      *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TOpt      *lhs, const Var::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Var::TReal     *lhs, const Var::TAddr     *rhs) const = 0;
      virtual void operator()(const Var::TReal     *lhs, const Var::TBool     *rhs) const = 0;
      virtual void operator()(const Var::TReal     *lhs, const Var::TDict     *rhs) const = 0;
      virtual void operator()(const Var::TReal     *lhs, const Var::TId       *rhs) const = 0;
      virtual void operator()(const Var::TReal     *lhs, const Var::TInt      *rhs) const = 0;
      virtual void operator()(const Var::TReal     *lhs, const Var::TList     *rhs) const = 0;
      virtual void operator()(const Var::TReal     *lhs, const Var::TObj      *rhs) const = 0;
      virtual void operator()(const Var::TReal     *lhs, const Var::TOpt      *rhs) const = 0;
      virtual void operator()(const Var::TReal     *lhs, const Var::TReal     *rhs) const = 0;
      virtual void operator()(const Var::TReal     *lhs, const Var::TSet      *rhs) const = 0;
      virtual void operator()(const Var::TReal     *lhs, const Var::TStr      *rhs) const = 0;
      virtual void operator()(const Var::TReal     *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TReal     *lhs, const Var::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Var::TSet      *lhs, const Var::TAddr     *rhs) const = 0;
      virtual void operator()(const Var::TSet      *lhs, const Var::TBool     *rhs) const = 0;
      virtual void operator()(const Var::TSet      *lhs, const Var::TDict     *rhs) const = 0;
      virtual void operator()(const Var::TSet      *lhs, const Var::TId       *rhs) const = 0;
      virtual void operator()(const Var::TSet      *lhs, const Var::TInt      *rhs) const = 0;
      virtual void operator()(const Var::TSet      *lhs, const Var::TList     *rhs) const = 0;
      virtual void operator()(const Var::TSet      *lhs, const Var::TObj      *rhs) const = 0;
      virtual void operator()(const Var::TSet      *lhs, const Var::TOpt      *rhs) const = 0;
      virtual void operator()(const Var::TSet      *lhs, const Var::TReal     *rhs) const = 0;
      virtual void operator()(const Var::TSet      *lhs, const Var::TSet      *rhs) const = 0;
      virtual void operator()(const Var::TSet      *lhs, const Var::TStr      *rhs) const = 0;
      virtual void operator()(const Var::TSet      *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TSet      *lhs, const Var::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Var::TStr      *lhs, const Var::TAddr     *rhs) const = 0;
      virtual void operator()(const Var::TStr      *lhs, const Var::TBool     *rhs) const = 0;
      virtual void operator()(const Var::TStr      *lhs, const Var::TDict     *rhs) const = 0;
      virtual void operator()(const Var::TStr      *lhs, const Var::TId       *rhs) const = 0;
      virtual void operator()(const Var::TStr      *lhs, const Var::TInt      *rhs) const = 0;
      virtual void operator()(const Var::TStr      *lhs, const Var::TList     *rhs) const = 0;
      virtual void operator()(const Var::TStr      *lhs, const Var::TObj      *rhs) const = 0;
      virtual void operator()(const Var::TStr      *lhs, const Var::TOpt      *rhs) const = 0;
      virtual void operator()(const Var::TStr      *lhs, const Var::TReal     *rhs) const = 0;
      virtual void operator()(const Var::TStr      *lhs, const Var::TSet      *rhs) const = 0;
      virtual void operator()(const Var::TStr      *lhs, const Var::TStr      *rhs) const = 0;
      virtual void operator()(const Var::TStr      *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TStr      *lhs, const Var::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TAddr     *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TBool     *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TDict     *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TId       *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TInt      *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TList     *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TObj      *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TOpt      *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TReal     *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TSet      *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TStr      *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TTimePnt  *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt  *lhs, const Var::TAddr     *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt  *lhs, const Var::TBool     *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt  *lhs, const Var::TDict     *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt  *lhs, const Var::TId       *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt  *lhs, const Var::TInt      *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt  *lhs, const Var::TList     *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt  *lhs, const Var::TObj      *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt  *lhs, const Var::TOpt      *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt  *lhs, const Var::TReal     *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt  *lhs, const Var::TSet      *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt  *lhs, const Var::TStr      *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt  *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt  *lhs, const Var::TTimePnt  *rhs) const = 0;

      protected:

      /* Do-little. */
      TDoubleVisitor() {}

      private:

      /* Used by Visit(). */
      class TLhsVisitor final
          : public TVisitor {
        public:

        /* Cache the arguments. */
        TLhsVisitor(const TVar *rhs, const TVar::TDoubleVisitor &double_visitor)
            : Rhs(rhs), DoubleVisitor(double_visitor) {}

        /* Handlers.  Each caches the lhs and dispatches to the rhs. */
        virtual void operator()(const Var::TAddr     *lhs) const override;
        virtual void operator()(const Var::TBool     *lhs) const override;
        virtual void operator()(const Var::TDict     *lhs) const override;
        virtual void operator()(const Var::TId       *lhs) const override;
        virtual void operator()(const Var::TInt      *lhs) const override;
        virtual void operator()(const Var::TList     *lhs) const override;
        virtual void operator()(const Var::TObj      *lhs) const override;
        virtual void operator()(const Var::TOpt      *lhs) const override;
        virtual void operator()(const Var::TReal     *lhs) const override;
        virtual void operator()(const Var::TSet      *lhs) const override;
        virtual void operator()(const Var::TStr      *lhs) const override;
        virtual void operator()(const Var::TTimeDiff *lhs) const override;
        virtual void operator()(const Var::TTimePnt  *lhs) const override;

        private:

        /* The rhs to dispatch to. */
        const TVar *Rhs;

        /* The double-visitor to dispatch to. */
        const TDoubleVisitor &DoubleVisitor;

      };  // TVar::TDoubleVisitor::TLhsVisitor

      /* Used by Visit().  Defined in the .cc file. */
      template <typename TLhs>
      class TRhsVisitor final
          : public TVisitor {
        public:

        /* Cache the arguments. */
        TRhsVisitor(const TLhs *lhs, const TVar::TDoubleVisitor &double_visitor)
            : Lhs(lhs), DoubleVisitor(double_visitor) {}

        /* Handlers.  Each dispatches to the correct handler in the double-visitor. */
        virtual void operator()(const Var::TAddr     *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Var::TBool     *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Var::TDict     *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Var::TId       *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Var::TInt      *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Var::TList     *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Var::TObj      *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Var::TOpt      *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Var::TReal     *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Var::TSet      *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Var::TStr      *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Var::TTimeDiff *rhs) const override { DoubleVisitor(Lhs, rhs); }
        virtual void operator()(const Var::TTimePnt  *rhs) const override { DoubleVisitor(Lhs, rhs); }

        private:

        /* The lhs to dispatch to. */
        const TLhs *Lhs;

        /* The double-visitor to dispatch to. */
        const TVar::TDoubleVisitor &DoubleVisitor;

      };  // TVar::TDoubleVisitor::TRhsVisitor<TLhs>

    };  // TVar::TDoubleVisitor

  }  // Rpc

}  // Stig

