/* <stig/type/unwrap_visitor.h>

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

#include <stig/type.h>
#include <stig/type/ensure_empty_object.h>
#include <base/not_implemented.h>

namespace Stig {

  namespace Type {

    class TUnwrapVisitor
        : public TType::TVisitor {
      NO_COPY_SEMANTICS(TUnwrapVisitor);
      protected:

      TUnwrapVisitor(TType &type, const TPosRange &pos_range)
          : PosRange(pos_range), Type(type) {}

      virtual void operator()(const TAddr     *that) const = 0;
      virtual void operator()(const TBool     *that) const = 0;
      virtual void operator()(const TDict     *that) const = 0;
      virtual void operator()(const TId       *that) const = 0;
      virtual void operator()(const TInt      *that) const = 0;
      virtual void operator()(const TList     *that) const = 0;
      virtual void operator()(const TObj      *that) const = 0;
      virtual void operator()(const TReal     *that) const = 0;
      virtual void operator()(const TSet      *that) const = 0;
      virtual void operator()(const TStr      *that) const = 0;
      virtual void operator()(const TTimeDiff *that) const = 0;
      virtual void operator()(const TTimePnt  *that) const = 0;

      const TPosRange PosRange;

      TType &Type;

      private:

      /* Unwrap */
      virtual void operator()(const TAny      *) const {
        Type = Type::TAny::Get();
      }
      virtual void operator()(const TErr      *) const final { NOT_IMPLEMENTED(); }
      virtual void operator()(const TFunc     *that) const final {
        EnsureEmptyObject(that->GetParamObject(), PosRange);
        that->GetReturnType().Accept(*this);
      }
      virtual void operator()(const TMutable  *that) const {
        that->GetVal().Accept(*this);
      }
      virtual void operator()(const TOpt      *that) const {
        that->GetElem().Accept(*this);
        Type = Type::TOpt::Get(Type);
      }
      virtual void operator()(const TSeq      *that) const {
        that->GetElem().Accept(*this);
        Type = Type::TSeq::Get(Type);
      }

    };  // TUnwrapVisitor

  }  // Type

}  // Stig
