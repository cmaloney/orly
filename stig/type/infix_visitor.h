/* <stig/type/infix_visitor.h>

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

#include <base/not_implemented_error.h>
#include <stig/pos_range.h>
#include <stig/type.h>
#include <stig/type/ensure_empty_object.h>

namespace Stig {

  namespace Type {

    /* TODO */
    class TInfixVisitor
        : public TType::TDoubleVisitor {
      NO_COPY_SEMANTICS(TInfixVisitor);
      protected:

      /* TODO */
      TInfixVisitor(TType &type, const TPosRange &pos_range)
          : PosRange(pos_range), Type(type) {}

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
      virtual void operator()(const TBool     *, const TAddr     *) const = 0;
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
      virtual void operator()(const TDict     *, const TAddr     *) const = 0;
      virtual void operator()(const TDict     *, const TBool     *) const = 0;
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
      virtual void operator()(const TId       *, const TAddr     *) const = 0;
      virtual void operator()(const TId       *, const TBool     *) const = 0;
      virtual void operator()(const TId       *, const TDict     *) const = 0;
      virtual void operator()(const TId       *, const TId       *) const = 0;
      virtual void operator()(const TId       *, const TInt      *) const = 0;
      virtual void operator()(const TId       *, const TList     *) const = 0;
      virtual void operator()(const TId       *, const TObj      *) const = 0;
      virtual void operator()(const TId       *, const TReal     *) const = 0;
      virtual void operator()(const TId       *, const TSet      *) const = 0;
      virtual void operator()(const TId       *, const TStr      *) const = 0;
      virtual void operator()(const TId       *, const TTimeDiff *) const = 0;
      virtual void operator()(const TId       *, const TTimePnt  *) const = 0;
      virtual void operator()(const TInt      *, const TAddr     *) const = 0;
      virtual void operator()(const TInt      *, const TBool     *) const = 0;
      virtual void operator()(const TInt      *, const TDict     *) const = 0;
      virtual void operator()(const TInt      *, const TId       *) const = 0;
      virtual void operator()(const TInt      *, const TInt      *) const = 0;
      virtual void operator()(const TInt      *, const TList     *) const = 0;
      virtual void operator()(const TInt      *, const TObj      *) const = 0;
      virtual void operator()(const TInt      *, const TReal     *) const = 0;
      virtual void operator()(const TInt      *, const TSet      *) const = 0;
      virtual void operator()(const TInt      *, const TStr      *) const = 0;
      virtual void operator()(const TInt      *, const TTimeDiff *) const = 0;
      virtual void operator()(const TInt      *, const TTimePnt  *) const = 0;
      virtual void operator()(const TList     *, const TAddr     *) const = 0;
      virtual void operator()(const TList     *, const TBool     *) const = 0;
      virtual void operator()(const TList     *, const TDict     *) const = 0;
      virtual void operator()(const TList     *, const TId       *) const = 0;
      virtual void operator()(const TList     *, const TInt      *) const = 0;
      virtual void operator()(const TList     *, const TList     *) const = 0;
      virtual void operator()(const TList     *, const TObj      *) const = 0;
      virtual void operator()(const TList     *, const TReal     *) const = 0;
      virtual void operator()(const TList     *, const TSet      *) const = 0;
      virtual void operator()(const TList     *, const TStr      *) const = 0;
      virtual void operator()(const TList     *, const TTimeDiff *) const = 0;
      virtual void operator()(const TList     *, const TTimePnt  *) const = 0;
      virtual void operator()(const TObj      *, const TAddr     *) const = 0;
      virtual void operator()(const TObj      *, const TBool     *) const = 0;
      virtual void operator()(const TObj      *, const TDict     *) const = 0;
      virtual void operator()(const TObj      *, const TId       *) const = 0;
      virtual void operator()(const TObj      *, const TInt      *) const = 0;
      virtual void operator()(const TObj      *, const TList     *) const = 0;
      virtual void operator()(const TObj      *, const TObj      *) const = 0;
      virtual void operator()(const TObj      *, const TReal     *) const = 0;
      virtual void operator()(const TObj      *, const TSet      *) const = 0;
      virtual void operator()(const TObj      *, const TStr      *) const = 0;
      virtual void operator()(const TObj      *, const TTimeDiff *) const = 0;
      virtual void operator()(const TObj      *, const TTimePnt  *) const = 0;
      virtual void operator()(const TReal     *, const TAddr     *) const = 0;
      virtual void operator()(const TReal     *, const TBool     *) const = 0;
      virtual void operator()(const TReal     *, const TDict     *) const = 0;
      virtual void operator()(const TReal     *, const TId       *) const = 0;
      virtual void operator()(const TReal     *, const TInt      *) const = 0;
      virtual void operator()(const TReal     *, const TList     *) const = 0;
      virtual void operator()(const TReal     *, const TObj      *) const = 0;
      virtual void operator()(const TReal     *, const TReal     *) const = 0;
      virtual void operator()(const TReal     *, const TSet      *) const = 0;
      virtual void operator()(const TReal     *, const TStr      *) const = 0;
      virtual void operator()(const TReal     *, const TTimeDiff *) const = 0;
      virtual void operator()(const TReal     *, const TTimePnt  *) const = 0;
      virtual void operator()(const TSet      *, const TAddr     *) const = 0;
      virtual void operator()(const TSet      *, const TBool     *) const = 0;
      virtual void operator()(const TSet      *, const TDict     *) const = 0;
      virtual void operator()(const TSet      *, const TId       *) const = 0;
      virtual void operator()(const TSet      *, const TInt      *) const = 0;
      virtual void operator()(const TSet      *, const TList     *) const = 0;
      virtual void operator()(const TSet      *, const TObj      *) const = 0;
      virtual void operator()(const TSet      *, const TReal     *) const = 0;
      virtual void operator()(const TSet      *, const TSet      *) const = 0;
      virtual void operator()(const TSet      *, const TStr      *) const = 0;
      virtual void operator()(const TSet      *, const TTimeDiff *) const = 0;
      virtual void operator()(const TSet      *, const TTimePnt  *) const = 0;
      virtual void operator()(const TStr      *, const TAddr     *) const = 0;
      virtual void operator()(const TStr      *, const TBool     *) const = 0;
      virtual void operator()(const TStr      *, const TDict     *) const = 0;
      virtual void operator()(const TStr      *, const TId       *) const = 0;
      virtual void operator()(const TStr      *, const TInt      *) const = 0;
      virtual void operator()(const TStr      *, const TList     *) const = 0;
      virtual void operator()(const TStr      *, const TObj      *) const = 0;
      virtual void operator()(const TStr      *, const TReal     *) const = 0;
      virtual void operator()(const TStr      *, const TSet      *) const = 0;
      virtual void operator()(const TStr      *, const TStr      *) const = 0;
      virtual void operator()(const TStr      *, const TTimeDiff *) const = 0;
      virtual void operator()(const TStr      *, const TTimePnt  *) const = 0;
      virtual void operator()(const TTimeDiff *, const TAddr     *) const = 0;
      virtual void operator()(const TTimeDiff *, const TBool     *) const = 0;
      virtual void operator()(const TTimeDiff *, const TDict     *) const = 0;
      virtual void operator()(const TTimeDiff *, const TId       *) const = 0;
      virtual void operator()(const TTimeDiff *, const TInt      *) const = 0;
      virtual void operator()(const TTimeDiff *, const TList     *) const = 0;
      virtual void operator()(const TTimeDiff *, const TObj      *) const = 0;
      virtual void operator()(const TTimeDiff *, const TReal     *) const = 0;
      virtual void operator()(const TTimeDiff *, const TSet      *) const = 0;
      virtual void operator()(const TTimeDiff *, const TStr      *) const = 0;
      virtual void operator()(const TTimeDiff *, const TTimeDiff *) const = 0;
      virtual void operator()(const TTimeDiff *, const TTimePnt  *) const = 0;
      virtual void operator()(const TTimePnt  *, const TAddr     *) const = 0;
      virtual void operator()(const TTimePnt  *, const TBool     *) const = 0;
      virtual void operator()(const TTimePnt  *, const TDict     *) const = 0;
      virtual void operator()(const TTimePnt  *, const TId       *) const = 0;
      virtual void operator()(const TTimePnt  *, const TInt      *) const = 0;
      virtual void operator()(const TTimePnt  *, const TList     *) const = 0;
      virtual void operator()(const TTimePnt  *, const TObj      *) const = 0;
      virtual void operator()(const TTimePnt  *, const TReal     *) const = 0;
      virtual void operator()(const TTimePnt  *, const TSet      *) const = 0;
      virtual void operator()(const TTimePnt  *, const TStr      *) const = 0;
      virtual void operator()(const TTimePnt  *, const TTimeDiff *) const = 0;
      virtual void operator()(const TTimePnt  *, const TTimePnt  *) const = 0;

      /* TODO */
      const TPosRange &PosRange;

      /* TODO */
      TType &Type;

      private:

      /* DO NOT OVERRIDE THESE!!! */

      /*****************************************************
       ******************** ERR UNWRAP *********************
       *****************************************************/
      virtual void operator()(const TErr      *, const TAddr     *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TErr      *, const TAny      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TErr      *, const TBool     *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TErr      *, const TDict     *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TErr      *, const TErr      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TErr      *, const TFunc     *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TErr      *, const TId       *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TErr      *, const TInt      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TErr      *, const TList     *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TErr      *, const TMutable  *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TErr      *, const TOpt      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TErr      *, const TObj      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TErr      *, const TReal     *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TErr      *, const TSet      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TErr      *, const TSeq      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TErr      *, const TStr      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TErr      *, const TTimeDiff *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TErr      *, const TTimePnt  *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TAddr     *, const TErr      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TAny      *, const TErr      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TBool     *, const TErr      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TDict     *, const TErr      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TFunc     *, const TErr      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TId       *, const TErr      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TInt      *, const TErr      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TList     *, const TErr      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TObj      *, const TErr      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TReal     *, const TErr      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TSet      *, const TErr      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TSeq      *, const TErr      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TStr      *, const TErr      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TTimeDiff *, const TErr      *) const final { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TTimePnt  *, const TErr      *) const final { throw Base::TNotImplementedError(HERE); }

      /*****************************************************
       ******************* FUNC UNWRAP *********************
       *****************************************************/
      virtual void operator()(const TFunc *lhs, const TAddr *rhs) const final {
        EnsureEmptyObject(lhs->GetParamObject(), PosRange);
        TType::Accept(lhs->GetReturnType(), rhs->AsType(), *this);
      }
      virtual void operator()(const TFunc *, const TAny *) const final {
        Type = TAny::Get();
      }
      virtual void operator()(const TFunc *lhs, const TBool *) const final {
        EnsureEmptyObject(lhs->GetParamObject(), PosRange);
        TType::Accept(lhs->GetReturnType(), TBool::Get(), *this);
      }
      virtual void operator()(const TFunc *lhs, const TDict *rhs) const final {
        EnsureEmptyObject(lhs->GetParamObject(), PosRange);
        TType::Accept(lhs->GetReturnType(), rhs->AsType(), *this);
      }
      virtual void operator()(const TFunc *lhs, const TFunc *rhs) const final {
        EnsureEmptyObject(lhs->GetParamObject(), PosRange);
        EnsureEmptyObject(rhs->AsType(), PosRange);
        TType::Accept(lhs->GetReturnType(), rhs->GetReturnType(), *this);
      }
      virtual void operator()(const TFunc *lhs, const TId *) const final {
        EnsureEmptyObject(lhs->GetParamObject(), PosRange);
        TType::Accept(lhs->GetReturnType(), TId::Get(), *this);
      }
      virtual void operator()(const TFunc *lhs, const TInt *) const final {
        EnsureEmptyObject(lhs->GetParamObject(), PosRange);
        TType::Accept(lhs->GetReturnType(), TInt::Get(), *this);
      }
      virtual void operator()(const TFunc *lhs, const TList *rhs) const final {
        EnsureEmptyObject(lhs->GetParamObject(), PosRange);
        TType::Accept(lhs->GetReturnType(), rhs->AsType(), *this);
      }
      virtual void operator()(const TFunc *lhs, const TMutable *rhs) const final {
        EnsureEmptyObject(lhs->GetParamObject(), PosRange);
        TType::Accept(lhs->GetReturnType(), rhs->AsType(), *this);
      }
      virtual void operator()(const TFunc *lhs, const TObj *rhs) const final {
        EnsureEmptyObject(lhs->GetParamObject(), PosRange);
        TType::Accept(lhs->GetReturnType(), rhs->AsType(), *this);
      }
      virtual void operator()(const TFunc *lhs, const TReal *) const final {
        EnsureEmptyObject(lhs->GetParamObject(), PosRange);
        TType::Accept(lhs->GetReturnType(), TReal::Get(), *this);
      }
      virtual void operator()(const TFunc *lhs, const TSeq *rhs) const final {
        EnsureEmptyObject(lhs->GetParamObject(), PosRange);
        TType::Accept(lhs->GetReturnType(), rhs->AsType(), *this);
      }
      virtual void operator()(const TFunc *lhs, const TSet *rhs) const final {
        EnsureEmptyObject(lhs->GetParamObject(), PosRange);
        TType::Accept(lhs->GetReturnType(), rhs->AsType(), *this);
      }
      virtual void operator()(const TFunc *lhs, const TStr *) const final {
        EnsureEmptyObject(lhs->GetParamObject(), PosRange);
        TType::Accept(lhs->GetReturnType(), TStr::Get(), *this);
      }
      virtual void operator()(const TFunc *lhs, const TTimeDiff *) const final {
        EnsureEmptyObject(lhs->GetParamObject(), PosRange);
        TType::Accept(lhs->GetReturnType(), TTimeDiff::Get(), *this);
      }
      virtual void operator()(const TFunc *lhs, const TTimePnt *) const final {
        EnsureEmptyObject(lhs->GetParamObject(), PosRange);
        TType::Accept(lhs->GetReturnType(), TTimePnt::Get(), *this);
      }
      virtual void operator()(const TAddr *lhs, const TFunc *rhs) const final {
        EnsureEmptyObject(rhs->GetParamObject(), PosRange);
        TType::Accept(lhs->AsType(), rhs->GetReturnType(), *this);
      }
      virtual void operator()(const TAny *, const TFunc *) const final {
        Type = TAny::Get();
      }
      virtual void operator()(const TBool *, const TFunc *rhs) const final {
        EnsureEmptyObject(rhs->GetParamObject(), PosRange);
        TType::Accept(TBool::Get(), rhs->GetReturnType(), *this);
      }
      virtual void operator()(const TDict *lhs, const TFunc *rhs) const final {
        EnsureEmptyObject(rhs->GetParamObject(), PosRange);
        TType::Accept(lhs->AsType(), rhs->GetReturnType(), *this);
      }
      virtual void operator()(const TId *, const TFunc *rhs) const final {
        EnsureEmptyObject(rhs->GetParamObject(), PosRange);
        TType::Accept(TId::Get(), rhs->GetReturnType(), *this);
      }
      virtual void operator()(const TInt *, const TFunc *rhs) const final {
        EnsureEmptyObject(rhs->GetParamObject(), PosRange);
        TType::Accept(TInt::Get(), rhs->GetReturnType(), *this);
      }
      virtual void operator()(const TList *lhs, const TFunc *rhs) const final {
        EnsureEmptyObject(rhs->GetParamObject(), PosRange);
        TType::Accept(lhs->AsType(), rhs->GetReturnType(), *this);
      }
      virtual void operator()(const TObj *lhs, const TFunc *rhs) const final {
        EnsureEmptyObject(rhs->GetParamObject(), PosRange);
        TType::Accept(lhs->AsType(), rhs->GetReturnType(), *this);
      }
      virtual void operator()(const TReal *, const TFunc *rhs) const final {
        EnsureEmptyObject(rhs->GetParamObject(), PosRange);
        TType::Accept(TReal::Get(), rhs->GetReturnType(), *this);
      }
      virtual void operator()(const TSeq *lhs, const TFunc *rhs) const final {
        EnsureEmptyObject(rhs->GetParamObject(), PosRange);
        TType::Accept(lhs->AsType(), rhs->GetReturnType(), *this);
      }
      virtual void operator()(const TSet *lhs, const TFunc *rhs) const final {
        EnsureEmptyObject(rhs->GetParamObject(), PosRange);
        TType::Accept(lhs->AsType(), rhs->GetReturnType(), *this);
      }
      virtual void operator()(const TStr *, const TFunc *rhs) const final {
        EnsureEmptyObject(rhs->GetParamObject(), PosRange);
        TType::Accept(TStr::Get(), rhs->GetReturnType(), *this);
      }
      virtual void operator()(const TTimeDiff *, const TFunc *rhs) const final {
        EnsureEmptyObject(rhs->GetParamObject(), PosRange);
        TType::Accept(TTimeDiff::Get(), rhs->GetReturnType(), *this);
      }
      virtual void operator()(const TTimePnt *, const TFunc *rhs) const final {
        EnsureEmptyObject(rhs->GetParamObject(), PosRange);
        TType::Accept(TTimePnt::Get(), rhs->GetReturnType(), *this);
      }

      /*****************************************************
       ****************** MUTABLE UNWRAP *******************
       *****************************************************/
      virtual void operator()(const TMutable  *lhs, const TAddr     *rhs) const final { TType::Accept(lhs->GetVal(), rhs->AsType(),    *this); }
      virtual void operator()(const TMutable  *lhs, const TBool     *rhs) const final { TType::Accept(lhs->GetVal(), rhs->AsType(),    *this); }
      virtual void operator()(const TMutable  *lhs, const TDict     *rhs) const final { TType::Accept(lhs->GetVal(), rhs->AsType(),    *this); }
      virtual void operator()(const TMutable  *lhs, const TErr      *rhs) const final { TType::Accept(lhs->GetVal(), rhs->AsType(),    *this); }
      virtual void operator()(const TMutable  *lhs, const TFunc     *rhs) const final { TType::Accept(lhs->GetVal(), rhs->AsType(),    *this); }
      virtual void operator()(const TMutable  *lhs, const TId       *rhs) const final { TType::Accept(lhs->GetVal(), rhs->AsType(),    *this); }
      virtual void operator()(const TMutable  *lhs, const TInt      *rhs) const final { TType::Accept(lhs->GetVal(), rhs->AsType(),    *this); }
      virtual void operator()(const TMutable  *lhs, const TList     *rhs) const final { TType::Accept(lhs->GetVal(), rhs->AsType(),    *this); }
      virtual void operator()(const TMutable  *lhs, const TMutable  *rhs) const final { TType::Accept(lhs->GetVal(), rhs->GetVal(), *this); }
      virtual void operator()(const TMutable  *lhs, const TOpt      *rhs) const final { TType::Accept(lhs->GetVal(), rhs->AsType(),    *this); }
      virtual void operator()(const TMutable  *lhs, const TObj      *rhs) const final { TType::Accept(lhs->GetVal(), rhs->AsType(),    *this); }
      virtual void operator()(const TMutable  *lhs, const TReal     *rhs) const final { TType::Accept(lhs->GetVal(), rhs->AsType(),    *this); }
      virtual void operator()(const TMutable  *lhs, const TSet      *rhs) const final { TType::Accept(lhs->GetVal(), rhs->AsType(),    *this); }
      virtual void operator()(const TMutable  *lhs, const TSeq      *rhs) const final { TType::Accept(lhs->GetVal(), rhs->AsType(),    *this); }
      virtual void operator()(const TMutable  *lhs, const TStr      *rhs) const final { TType::Accept(lhs->GetVal(), rhs->AsType(),    *this); }
      virtual void operator()(const TMutable  *lhs, const TTimeDiff *rhs) const final { TType::Accept(lhs->GetVal(), rhs->AsType(),    *this); }
      virtual void operator()(const TMutable  *lhs, const TTimePnt  *rhs) const final { TType::Accept(lhs->GetVal(), rhs->AsType(),    *this); }
      virtual void operator()(const TAddr     *lhs, const TMutable  *rhs) const final { TType::Accept(lhs->AsType(), rhs->GetVal(),    *this); }
      virtual void operator()(const TBool     *lhs, const TMutable  *rhs) const final { TType::Accept(lhs->AsType(), rhs->GetVal(),    *this); }
      virtual void operator()(const TDict     *lhs, const TMutable  *rhs) const final { TType::Accept(lhs->AsType(), rhs->GetVal(),    *this); }
      virtual void operator()(const TId       *lhs, const TMutable  *rhs) const final { TType::Accept(lhs->AsType(), rhs->GetVal(),    *this); }
      virtual void operator()(const TInt      *lhs, const TMutable  *rhs) const final { TType::Accept(lhs->AsType(), rhs->GetVal(),    *this); }
      virtual void operator()(const TList     *lhs, const TMutable  *rhs) const final { TType::Accept(lhs->AsType(), rhs->GetVal(),    *this); }
      virtual void operator()(const TObj      *lhs, const TMutable  *rhs) const final { TType::Accept(lhs->AsType(), rhs->GetVal(),    *this); }
      virtual void operator()(const TSet      *lhs, const TMutable  *rhs) const final { TType::Accept(lhs->AsType(), rhs->GetVal(),    *this); }
      virtual void operator()(const TReal     *lhs, const TMutable  *rhs) const final { TType::Accept(lhs->AsType(), rhs->GetVal(),    *this); }
      virtual void operator()(const TStr      *lhs, const TMutable  *rhs) const final { TType::Accept(lhs->AsType(), rhs->GetVal(),    *this); }
      virtual void operator()(const TTimeDiff *lhs, const TMutable  *rhs) const final { TType::Accept(lhs->AsType(), rhs->GetVal(),    *this); }
      virtual void operator()(const TTimePnt  *lhs, const TMutable  *rhs) const final { TType::Accept(lhs->AsType(), rhs->GetVal(),    *this); }

      /*****************************************************
       ******************** OPT UNWRAP *********************
       *****************************************************/
      virtual void operator()(const TOpt *lhs, const TAddr *rhs) const final {
        TType::Accept(lhs->GetElem(), rhs->AsType(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TOpt *, const TAny *) const final {throw Base::TNotImplementedError(HERE);}
      virtual void operator()(const TOpt *lhs, const TBool *) const final {
        TType::Accept(lhs->GetElem(), TBool::Get(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TOpt *lhs, const TDict *rhs) const final {
        TType::Accept(lhs->GetElem(), rhs->AsType(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TOpt *, const TErr *) const final {throw Base::TNotImplementedError(HERE);}
      virtual void operator()(const TOpt *lhs, const TFunc *rhs) const final {
        EnsureEmptyObject(rhs->GetParamObject(), PosRange);
        TType::Accept(lhs->GetElem(), rhs->GetReturnType(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TOpt *lhs, const TId *) const final {
        TType::Accept(lhs->GetElem(), TId::Get(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TOpt *lhs, const TInt *) const final {
        TType::Accept(lhs->GetElem(), TInt::Get(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TOpt *lhs, const TList *rhs) const final {
        TType::Accept(lhs->GetElem(), rhs->AsType(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TOpt *lhs, const TMutable *rhs) const final {
        TType::Accept(lhs->GetElem(), rhs->GetVal(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TOpt *lhs, const TOpt *rhs) const final {
        TType::Accept(lhs->GetElem(), rhs->GetElem(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TOpt *lhs, const TObj *rhs) const final {
        TType::Accept(lhs->GetElem(), rhs->AsType(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TOpt *lhs, const TReal *) const final {
        TType::Accept(lhs->GetElem(), TReal::Get(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TOpt *lhs, const TSet *rhs) const final {
        TType::Accept(lhs->GetElem(), rhs->AsType(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TOpt *lhs, const TStr *) const final {
        TType::Accept(lhs->GetElem(), TStr::Get(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TOpt *lhs, const TTimeDiff *) const final {
        TType::Accept(lhs->GetElem(), TTimeDiff::Get(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TOpt *lhs, const TTimePnt *) const final {
        TType::Accept(lhs->GetElem(), TTimePnt::Get(), *this);
        Type = TOpt::Get(Type);
      }
      /* NOTE: TAddr and TOpt is not 'final' because we have overriding behaviour for 'mutate' statement. */
      virtual void operator()(const TAddr *lhs, const TOpt *rhs) const {
        TType::Accept(lhs->AsType(), rhs->GetElem(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TAny *, const TOpt *) const { throw Base::TNotImplementedError(HERE); }
      virtual void operator()(const TBool *, const TOpt *rhs) const {
        TType::Accept(TBool::Get(), rhs->GetElem(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TDict *lhs, const TOpt *rhs) const {
        TType::Accept(lhs->AsType(), rhs->GetElem(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TFunc *lhs, const TOpt *rhs) const {
        EnsureEmptyObject(lhs->GetParamObject(), PosRange);
        TType::Accept(lhs->GetReturnType(), rhs->GetElem(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TId *, const TOpt *rhs) const {
        TType::Accept(TId::Get(), rhs->GetElem(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TInt *, const TOpt *rhs) const {
        TType::Accept(TInt::Get(), rhs->GetElem(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TList *lhs, const TOpt *rhs) const {
        TType::Accept(lhs->AsType(), rhs->GetElem(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TObj *lhs, const TOpt *rhs) const {
        TType::Accept(lhs->AsType(), rhs->GetElem(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TReal *, const TOpt *rhs) const {
        TType::Accept(TReal::Get(), rhs->GetElem(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TSet *lhs, const TOpt *rhs) const {
        TType::Accept(lhs->AsType(), rhs->GetElem(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TStr *, const TOpt *rhs) const {
        TType::Accept(TStr::Get(), rhs->GetElem(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TTimeDiff *, const TOpt *rhs) const {
        TType::Accept(TTimeDiff::Get(), rhs->GetElem(), *this);
        Type = TOpt::Get(Type);
      }
      virtual void operator()(const TTimePnt *, const TOpt *rhs) const {
        TType::Accept(TTimePnt::Get(), rhs->GetElem(), *this);
        Type = TOpt::Get(Type);
      }

      /*******************************************************
        ******************** ANY UNWRAP **********************
        *****************************************************/
      /* NOTE: TAny and a type is not 'final' because we have overriding behaviour for the 'as' operator
               as well as the if_else expression.
               Refer to TAsTypeVisitor in <stig/expr/as.cc> and TIfElseVisitor in <stig/expr/if_else.cc> */
      virtual void operator()(const TAny *, const TAddr *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TAny *, const TAny *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TAny *, const TBool *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TAny *, const TDict *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TAny *, const TId *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TAny *, const TInt *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TAny *, const TList *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TAny *, const TMutable *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TAny *, const TObj *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TAny *, const TReal *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TAny *, const TSet *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TAny *, const TSeq *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TAny *, const TStr *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TAny *, const TTimeDiff *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TAny *, const TTimePnt *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TAddr *, const TAny *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TBool *, const TAny *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TDict *, const TAny *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TId *, const TAny *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TInt *, const TAny *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TList *, const TAny *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TMutable *, const TAny *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TObj *, const TAny *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TReal *, const TAny *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TSet *, const TAny *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TSeq *, const TAny *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TStr *, const TAny *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TTimeDiff *, const TAny *) const {
        Type = TAny::Get();
      }
      virtual void operator()(const TTimePnt *, const TAny *) const {
        Type = TAny::Get();
      }

      /*****************************************************
       ******************** SEQ UNWRAP *********************
       *****************************************************/
      virtual void operator()(const TSeq *lhs, const TAddr *rhs) const {
        TType::Accept(lhs->GetElem(), rhs->AsType(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TSeq *lhs, const TBool *) const {
        TType::Accept(lhs->GetElem(), TBool::Get(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TSeq *lhs, const TDict *rhs) const {
        TType::Accept(lhs->GetElem(), rhs->AsType(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TSeq *lhs, const TId *) const {
        TType::Accept(lhs->GetElem(), TId::Get(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TSeq *lhs, const TInt *) const {
        TType::Accept(lhs->GetElem(), TInt::Get(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TSeq *lhs, const TList *rhs) const {
        TType::Accept(lhs->GetElem(), rhs->AsType(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TSeq *lhs, const TMutable *rhs) const {
        TType::Accept(lhs->GetElem(), rhs->AsType(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TSeq *lhs, const TObj *rhs) const {
        TType::Accept(lhs->GetElem(), rhs->AsType(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TSeq *lhs, const TOpt *rhs) const {
        TType::Accept(lhs->GetElem(), rhs->AsType(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TSeq *lhs, const TReal *) const {
        TType::Accept(lhs->GetElem(), TReal::Get(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TSeq *lhs, const TSet *rhs) const {
        TType::Accept(lhs->GetElem(), rhs->AsType(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TSeq *lhs, const TSeq *rhs) const {
        TType::Accept(lhs->GetElem(), rhs->GetElem(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TSeq *lhs, const TStr *) const {
        TType::Accept(lhs->GetElem(), TStr::Get(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TSeq *lhs, const TTimeDiff *) const {
        TType::Accept(lhs->GetElem(), TTimeDiff::Get(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TSeq *lhs, const TTimePnt *) const {
        TType::Accept(lhs->GetElem(), TTimePnt::Get(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TAddr *lhs, const TSeq *rhs) const {
        TType::Accept(lhs->AsType(), rhs->GetElem(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TBool *, const TSeq *rhs) const {
        TType::Accept(TBool::Get(), rhs->GetElem(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TDict *lhs, const TSeq *rhs) const {
        TType::Accept(lhs->AsType(), rhs->GetElem(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TId *, const TSeq *rhs) const {
        TType::Accept(TId::Get(), rhs->GetElem(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TInt *, const TSeq *rhs) const {
        TType::Accept(TInt::Get(), rhs->GetElem(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TList *lhs, const TSeq *rhs) const {
        TType::Accept(lhs->AsType(), rhs->GetElem(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TObj *lhs, const TSeq *rhs) const {
        TType::Accept(lhs->AsType(), rhs->GetElem(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TOpt *lhs, const TSeq *rhs) const {
        TType::Accept(lhs->AsType(), rhs->GetElem(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TReal *, const TSeq *rhs) const {
        TType::Accept(TReal::Get(), rhs->GetElem(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TSet *lhs, const TSeq *rhs) const {
        TType::Accept(lhs->AsType(), rhs->GetElem(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TStr *, const TSeq *rhs) const {
        TType::Accept(TStr::Get(), rhs->GetElem(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TTimeDiff *, const TSeq *rhs) const {
        TType::Accept(TTimeDiff::Get(), rhs->GetElem(), *this);
        Type = TSeq::Get(Type);
      }
      virtual void operator()(const TTimePnt *, const TSeq *rhs) const {
        TType::Accept(TTimePnt::Get(), rhs->GetElem(), *this);
        Type = TSeq::Get(Type);
      }

    };  // TInfixVisitor

  }  // Type

}  // Stig
