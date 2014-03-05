/* <stig/var/impl.cc>

   Implements <stig/var/impl.h>.

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

#include <stig/var/impl.h>
#include <stig/var.h>

#include <stig/pos_range.h>
#include <stig/type/get_prec.h>

namespace Stig {

  namespace Var {

    /* TODO */
    template <typename TLhs>
    class TRhsVisitor
        : public TVar::TVisitor {
      public:

      /* TODO */
      TRhsVisitor(const TLhs *lhs, const TVar::TDoubleVisitor &double_visitor)
          : Lhs(lhs), DoubleVisitor(double_visitor) {}

      /* TODO */
      virtual void operator()(const Var::TAddr *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const Var::TBool *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const Var::TDict *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const Var::TErr *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const Var::TFree *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const Var::TId *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const Var::TInt *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const Var::TList *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const Var::TMutable *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const Var::TObj *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const Var::TOpt *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const Var::TReal *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const Var::TSet *rhs) const {

        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const Var::TStr *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const Var::TTimeDiff *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const Var::TTimePnt *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const Var::TUnknown *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }

      private:

      /* TODO */
      const TLhs *Lhs;

      /* TODO */
      const TVar::TDoubleVisitor &DoubleVisitor;

    };  // TRhsVisitor<TLhs>

    /* TODO */
    class TLhsVisitor
        : public TVar::TVisitor {
      public:

      /* TODO */
      TLhsVisitor(const TVar &rhs, const TVar::TDoubleVisitor &double_visitor)
          : Rhs(rhs), DoubleVisitor(double_visitor) {}

      /* TODO */
      virtual void operator()(const Var::TAddr *lhs) const {
        Rhs.Accept(TRhsVisitor<Var::TAddr>(lhs, DoubleVisitor));
      }
      virtual void operator()(const Var::TBool *lhs) const {
        Rhs.Accept(TRhsVisitor<Var::TBool>(lhs, DoubleVisitor));
      }
      virtual void operator()(const Var::TDict *lhs) const {
        Rhs.Accept(TRhsVisitor<Var::TDict>(lhs, DoubleVisitor));
      }
      virtual void operator()(const Var::TErr *lhs) const {
        Rhs.Accept(TRhsVisitor<Var::TErr>(lhs, DoubleVisitor));
      }
      virtual void operator()(const Var::TFree *lhs) const {
        Rhs.Accept(TRhsVisitor<Var::TFree>(lhs, DoubleVisitor));
      }
      virtual void operator()(const Var::TId *lhs) const {
        Rhs.Accept(TRhsVisitor<Var::TId>(lhs, DoubleVisitor));
      }
      virtual void operator()(const Var::TInt *lhs) const {
        Rhs.Accept(TRhsVisitor<Var::TInt>(lhs, DoubleVisitor));
      }
      virtual void operator()(const Var::TList *lhs) const {
        Rhs.Accept(TRhsVisitor<Var::TList>(lhs, DoubleVisitor));
      }
      virtual void operator()(const Var::TMutable *lhs) const {
        Rhs.Accept(TRhsVisitor<Var::TMutable>(lhs, DoubleVisitor));
      }
      virtual void operator()(const Var::TObj *lhs) const {
        Rhs.Accept(TRhsVisitor<Var::TObj>(lhs, DoubleVisitor));
      }
      virtual void operator()(const Var::TOpt *lhs) const {
        Rhs.Accept(TRhsVisitor<Var::TOpt>(lhs, DoubleVisitor));
      }
      virtual void operator()(const Var::TReal *lhs) const {
        Rhs.Accept(TRhsVisitor<Var::TReal>(lhs, DoubleVisitor));
      }
      virtual void operator()(const Var::TSet *lhs) const {
        Rhs.Accept(TRhsVisitor<Var::TSet>(lhs, DoubleVisitor));
      }
      virtual void operator()(const Var::TStr *lhs) const {
        Rhs.Accept(TRhsVisitor<Var::TStr>(lhs, DoubleVisitor));
      }
      virtual void operator()(const Var::TTimeDiff *lhs) const {
        Rhs.Accept(TRhsVisitor<Var::TTimeDiff>(lhs, DoubleVisitor));
      }
      virtual void operator()(const Var::TTimePnt *lhs) const {
        Rhs.Accept(TRhsVisitor<Var::TTimePnt>(lhs, DoubleVisitor));
      }
      virtual void operator()(const Var::TUnknown *lhs) const {
        Rhs.Accept(TRhsVisitor<Var::TUnknown>(lhs, DoubleVisitor));
      }

      private:

      /* TODO */
      const TVar &Rhs;

      /* TODO */
      const TVar::TDoubleVisitor &DoubleVisitor;

    };  // TLhsVisitor

    class TCompareDoubleVisitor
        : public TVar::TDoubleVisitor {
      public:

      class TVarCompareError
          : public Base::TFinalError<TVarCompareError> {
        public:

        TVarCompareError(const Base::TCodeLocation &loc) {
          PostCtor(loc, "Trying to compare types that are not comparable.");
        }

      };  // TVarCompareError

      TCompareDoubleVisitor(int &comp, bool is_eqeq) : Comp(comp), IsEqeq(is_eqeq) {}

      virtual ~TCompareDoubleVisitor() {}

      virtual void operator()(const TAddr *lhs, const TAddr *rhs) const {
        auto lhs_iter = lhs->GetVal().begin();
        auto rhs_iter = rhs->GetVal().begin();
        for (; lhs_iter != lhs->GetVal().end() && rhs_iter != rhs->GetVal().end(); ++lhs_iter, ++rhs_iter) {
          Comp = ToInt(lhs_iter->first) - ToInt(rhs_iter->first);
          if(Comp != 0) {
            return;
          }
          if(lhs_iter->first == Asc) {
            Var::TVar::Accept(lhs_iter->second, rhs_iter->second, *this);
          } else {
            Var::TVar::Accept(rhs_iter->second, lhs_iter->second, *this);
          }
          if (Comp != 0) {
            return;
          }
        }
        Comp = Compare(lhs->GetVal().size(), rhs->GetVal().size());
      }
      virtual void operator()(const TAddr *lhs, const TBool *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TAddr *lhs, const TDict *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TAddr *, const TErr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TAddr *lhs, const TFree *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TAddr *lhs, const TId   *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TAddr *lhs, const TInt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TAddr *lhs, const TList *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TAddr *, const TMutable  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TAddr *, const TOpt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TAddr *lhs, const TObj  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TAddr *lhs, const TReal *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TAddr *lhs, const TSet  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TAddr *lhs, const TStr  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TAddr *lhs, const TTimeDiff *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TAddr *lhs, const TTimePnt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TBool *lhs, const TAddr *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TBool *lhs, const TBool *rhs) const {
        Comp = Compare(lhs->GetVal(), rhs->GetVal());
      }
      virtual void operator()(const TBool *lhs, const TDict *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TBool *, const TErr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TBool *lhs, const TFree *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TBool *lhs, const TId   *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TBool *lhs, const TInt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TBool *lhs, const TList *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TBool *, const TMutable  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TBool *, const TOpt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TBool *lhs, const TObj  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TBool *lhs, const TReal *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TBool *lhs, const TSet  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TBool *lhs, const TStr  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TBool *lhs, const TTimeDiff *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TBool *lhs, const TTimePnt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TDict *lhs, const TAddr *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TDict *lhs, const TBool *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TDict *lhs, const TDict *rhs) const {
        if (!IsEqeq) {
          throw TVarCompareError(HERE);
        }
        Comp = lhs->GetVal() == rhs->GetVal() ? 0 : -1;
      }
      virtual void operator()(const TDict *, const TErr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TDict *lhs, const TFree *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TDict *lhs, const TId   *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TDict *lhs, const TInt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TDict *lhs, const TList *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TDict *, const TMutable  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TDict *, const TOpt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TDict *lhs, const TObj  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TDict *lhs, const TReal *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TDict *lhs, const TSet  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TDict *lhs, const TStr  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TDict *lhs, const TTimeDiff *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TDict *lhs, const TTimePnt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TErr *, const TAddr *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TErr *, const TBool *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TErr *, const TDict *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TErr *, const TErr *) const {
        throw Base::TNotImplementedError(HERE);;
      }
      virtual void operator()(const TErr *, const TFree *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TErr *, const TId   *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TErr *, const TInt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TErr *, const TList *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TErr *, const TMutable *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TErr *, const TOpt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TErr *, const TObj  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TErr *, const TReal *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TErr *, const TSet  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TErr *, const TStr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TErr *, const TTimeDiff *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TErr *, const TTimePnt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TFree *lhs, const TAddr *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TFree *lhs, const TBool *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TFree *lhs, const TDict *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TFree *, const TErr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TFree *, const TFree *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TFree *lhs, const TId   *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TFree *lhs, const TInt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TFree *lhs, const TList *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TFree *, const TMutable  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TFree *, const TOpt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TFree *lhs, const TObj  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TFree *lhs, const TReal *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TFree *lhs, const TSet  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TFree *lhs, const TStr  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TFree *lhs, const TTimeDiff *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TFree *lhs, const TTimePnt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TId *lhs, const TAddr *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TId *lhs, const TBool *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TId *lhs, const TDict *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TId *, const TErr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TId *lhs, const TFree *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TId *lhs, const TId   *rhs) const {
        Comp = lhs->GetVal().Compare(rhs->GetVal());
      }
      virtual void operator()(const TId *lhs, const TInt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TId *lhs, const TList *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TId *, const TMutable  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TId *, const TOpt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TId *lhs, const TObj  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TId *lhs, const TReal *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TId *lhs, const TSet  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TId *lhs, const TStr  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TId *lhs, const TTimeDiff *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TId *lhs, const TTimePnt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TInt *lhs, const TAddr *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TInt *lhs, const TBool *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TInt *lhs, const TDict *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TInt *, const TErr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TInt *lhs, const TFree *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TInt *lhs, const TId   *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TInt *lhs, const TInt  *rhs) const {
        Comp = Compare(lhs->GetVal(), rhs->GetVal());
      }
      virtual void operator()(const TInt *lhs, const TList *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TInt *, const TMutable  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TInt *, const TOpt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TInt *lhs, const TObj  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TInt *lhs, const TReal *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TInt *lhs, const TSet  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TInt *lhs, const TStr  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TInt *lhs, const TTimeDiff *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TInt *lhs, const TTimePnt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TList *lhs, const TAddr *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TList *lhs, const TBool *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TList *lhs, const TDict *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TList *, const TErr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TList *lhs, const TFree *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TList *lhs, const TId   *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TList *lhs, const TInt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TList *lhs, const TList *rhs) const {
        Comp = Compare(lhs->GetVal(), rhs->GetVal());
      }
      virtual void operator()(const TList *, const TMutable  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TList *, const TOpt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TList *lhs, const TObj  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TList *lhs, const TReal *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TList *lhs, const TSet  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TList *lhs, const TStr  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TList *lhs, const TTimeDiff *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TList *lhs, const TTimePnt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TMutable *, const TAddr *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TMutable *, const TBool *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TMutable *, const TDict *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TMutable *, const TErr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TMutable *, const TFree *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TMutable *, const TId   *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TMutable *, const TInt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TMutable *, const TList *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TMutable *, const TMutable *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TMutable *, const TOpt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TMutable *, const TObj  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TMutable *, const TReal *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TMutable *, const TSet  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TMutable *, const TStr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TMutable *, const TTimeDiff *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TMutable *, const TTimePnt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TOpt *, const TAddr *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TOpt *, const TBool *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TOpt *, const TDict *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TOpt *, const TErr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TOpt *, const TFree *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TOpt *, const TId   *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TOpt *, const TInt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TOpt *, const TList *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TOpt *, const TMutable *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TOpt *lhs, const TOpt  *rhs) const {
        Comp = CompareType(lhs->GetInnerType(), rhs->GetInnerType());
        if(Comp == 0) {
          if(lhs->GetVal().IsKnown()) {
            if(rhs->GetVal().IsKnown()) {
              Comp = Compare(lhs->GetVal().GetVal(), rhs->GetVal().GetVal());
            } else {
              Comp = -1;
            }
          } else {
            if(rhs->GetVal().IsKnown()) {
              Comp = 1;
            } else {
              Comp = 0;
            }
          }
        }
      }
      virtual void operator()(const TOpt *, const TObj  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TOpt *, const TReal *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TOpt *, const TSet  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TOpt *, const TStr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TOpt *, const TTimeDiff *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TOpt *, const TTimePnt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TObj *lhs, const TAddr *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TObj *lhs, const TBool *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TObj *lhs, const TDict *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TObj *, const TErr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TObj *lhs, const TId   *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TObj *lhs, const TFree *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TObj *lhs, const TInt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TObj *lhs, const TList *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TObj *, const TMutable  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TObj *, const TOpt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TObj *lhs, const TObj  *rhs) const {
        if (!IsEqeq) {
          throw TVarCompareError(HERE);
        }
        Comp = lhs->GetVal() == rhs->GetVal() ? 0 : -1;
      }
      virtual void operator()(const TObj *lhs, const TReal *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TObj *lhs, const TSet  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TObj *lhs, const TStr  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TObj *lhs, const TTimeDiff *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TObj *lhs, const TTimePnt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TReal *lhs, const TAddr *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TReal *lhs, const TBool *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TReal *lhs, const TDict *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TReal *, const TErr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TReal *lhs, const TId   *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TReal *lhs, const TFree *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TReal *lhs, const TInt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TReal *lhs, const TList *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TReal *, const TMutable  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TReal *, const TOpt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TReal *lhs, const TObj  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TReal *lhs, const TReal *rhs) const {
        Comp = Compare(lhs->GetVal(), rhs->GetVal());
      }
      virtual void operator()(const TReal *lhs, const TSet *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TReal *lhs, const TStr *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TReal *lhs, const TTimeDiff *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TReal *lhs, const TTimePnt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TSet *lhs, const TAddr *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TSet *lhs, const TBool *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TSet *lhs, const TDict *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TSet *, const TErr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TSet *lhs, const TId   *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TSet *lhs, const TFree *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TSet *lhs, const TInt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TSet *lhs, const TList *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TSet *, const TMutable  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TSet *, const TOpt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TSet *lhs, const TObj  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TSet *lhs, const TReal *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TSet *lhs, const TSet  *rhs) const {
        if (!IsEqeq) {
          throw TVarCompareError(HERE);
        }
        Comp = lhs->GetVal() == rhs->GetVal() ? 0 : -1;
      }
      virtual void operator()(const TSet *lhs, const TStr  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TSet *lhs, const TTimeDiff *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TSet *lhs, const TTimePnt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TStr *lhs, const TAddr *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TStr *lhs, const TBool *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TStr *lhs, const TDict *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TStr *, const TErr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TStr *lhs, const TFree *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TStr *lhs, const TId   *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TStr *lhs, const TInt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TStr *lhs, const TList *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TStr *, const TMutable  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TStr *, const TOpt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TStr *lhs, const TObj  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TStr *lhs, const TReal *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TStr *lhs, const TSet  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TStr *lhs, const TStr *rhs) const {
        Comp = lhs->GetVal().compare(rhs->GetVal());
      }
      virtual void operator()(const TStr *lhs, const TTimeDiff *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TStr *lhs, const TTimePnt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimeDiff *lhs, const TAddr *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimeDiff *lhs, const TBool *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimeDiff *lhs, const TDict *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimeDiff *, const TErr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TTimeDiff *lhs, const TFree *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimeDiff *lhs, const TId   *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimeDiff *lhs, const TInt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimeDiff *lhs, const TList *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimeDiff *, const TMutable  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TTimeDiff *, const TOpt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TTimeDiff *lhs, const TObj  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimeDiff *lhs, const TReal *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimeDiff *lhs, const TSet  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimeDiff *lhs, const TStr  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimeDiff *lhs, const TTimeDiff *rhs) const {
        Comp = Compare(lhs->GetVal(), rhs->GetVal());
      }
      virtual void operator()(const TTimeDiff *lhs, const TTimePnt *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimePnt *lhs, const TAddr *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimePnt *lhs, const TBool *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimePnt *lhs, const TDict *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimePnt *, const TErr  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TTimePnt *lhs, const TFree *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimePnt *lhs, const TId   *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimePnt *lhs, const TInt  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimePnt *lhs, const TList *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimePnt *, const TMutable  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TTimePnt *, const TOpt  *) const {throw TVarCompareError(HERE);}
      virtual void operator()(const TTimePnt *lhs, const TObj  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimePnt *lhs, const TReal *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimePnt *lhs, const TSet  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimePnt *lhs, const TStr  *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimePnt *lhs, const TTimeDiff *rhs) const {
        Comp = CompareType(lhs->GetType(), rhs->GetType());
      }
      virtual void operator()(const TTimePnt *lhs, const TTimePnt *rhs) const {
        Comp = Compare(lhs->GetVal(), rhs->GetVal());
      }

      private:

      /* TODO */
      template <typename TVal>
      static int Compare(const TVal &lhs, const TVal &rhs) {
        return lhs < rhs ? -1 : lhs > rhs ? 1 : 0;
      }

      /* TODO */
      static int CompareType(const Type::TType &lhs, const Type::TType &rhs) {
        return Compare(Type::GetPrec(lhs), Type::GetPrec(rhs));
      }

      /* TODO */
      int &Comp;

      /* TODO */
      bool IsEqeq;

    };  // TCompareDoubleVisitor

  }  // Var

}  // Stig

using namespace std;
using namespace Stig::Var;

TVar::TVisitor::~TVisitor() {}

TVar::TDoubleVisitor::~TDoubleVisitor() {}

TVar::TImpl::~TImpl() {}

void TVar::TImpl::Delete(TImpl *impl) {
  delete impl;
}

int TVar::Compare(const TVar &that) const {
  assert(this);
  assert(&that);
  int comp;
  Accept(*this, that, TCompareDoubleVisitor(comp, false));
  return comp;
}

bool TVar::operator==(const TVar &that) const {
  assert(this);
  assert(&that);
  int comp;
  Accept(*this, that, TCompareDoubleVisitor(comp, true));
  return comp == 0;
}

bool TVar::operator<(const TVar &that) const {
  assert(this);
  assert(&that);
  return Compare(that) < 0;
}

bool TVar::operator>(const TVar &that) const {
  assert(this);
  assert(&that);
  return Compare(that) > 0;
}

TVar::operator bool() const {
  assert(this);
  class TIsUnknownVisitor : public TVar::TVisitor {
    NO_COPY_SEMANTICS(TIsUnknownVisitor);
    public:
    TIsUnknownVisitor(bool &is_unknown) : IsUnknown(is_unknown) {}
    virtual void operator()(const Var::TAddr *) const {/* DO NOTHING */}
    virtual void operator()(const Var::TBool *) const {/* DO NOTHING */}
    virtual void operator()(const Var::TDict *) const {/* DO NOTHING */}
    virtual void operator()(const Var::TErr  *) const {/* DO NOTHING */}
    virtual void operator()(const Var::TFree *) const {/* DO NOTHING */}
    virtual void operator()(const Var::TId   *) const {/* DO NOTHING */}
    virtual void operator()(const Var::TInt  *) const {/* DO NOTHING */}
    virtual void operator()(const Var::TList *) const {/* DO NOTHING */}
    virtual void operator()(const Var::TMutable *) const {/* DO NOTHING */}
    virtual void operator()(const Var::TObj  *) const {/* DO NOTHING */}
    virtual void operator()(const Var::TOpt  *) const {/* DO NOTHING */}
    virtual void operator()(const Var::TReal *) const {/* DO NOTHING */}
    virtual void operator()(const Var::TSet  *) const {/* DO NOTHING */}
    virtual void operator()(const Var::TStr  *) const {/* DO NOTHING */}
    virtual void operator()(const Var::TTimeDiff *) const {/* DO NOTHING */}
    virtual void operator()(const Var::TTimePnt  *) const {/* DO NOTHING */}
    virtual void operator()(const Var::TUnknown  *) const {IsUnknown = true;}
    private:
    bool &IsUnknown;
  };
  bool is_unknown = false;
  Impl->Accept(TIsUnknownVisitor(is_unknown));
  return !is_unknown;
}

void TVar::Accept(const TVar &lhs, const TVar &rhs, const TDoubleVisitor &double_visitor) {
  lhs.Accept(TLhsVisitor(rhs, double_visitor));
}

#if defined(STIG_HOST)

TVar::~TVar() {/* WE NEED THIS TO BE DEFINED IN THE SERVER / COMPILER */}

void TVar::Init() {
  Impl = shared_ptr<TImpl>(new TUnknown(), TImpl::Delete);
}

TVar::TVar(TVar &&that) {
  assert(&that);
  Init();
  std::swap(Impl, that.Impl);
}

TVar::TVar(const TVar &that) {
  assert(&that);
  Impl = that.Impl;
}

TVar TVar::Copy() const {
  return Impl->Copy();
}

TVar &TVar::operator=(TVar &&that) {
  assert(this);
  assert(&that);
  std::swap(Impl, that.Impl);
  return *this;
}

TVar &TVar::operator=(const TVar &that) {
  assert(this);
  assert(&that);
  return *this = TVar(that);
}

#endif