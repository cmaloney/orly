/* <stig/expr/collected_by.h>

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

#include <memory>

#include <base/no_copy_semantics.h>
#include <stig/error.h>
#include <stig/expr/binary.h>
#include <stig/expr/lhsrhsable.h>
#include <stig/expr/visitor.h>
#include <stig/type/addr.h>
#include <stig/type/dict.h>
#include <stig/type/equal_visitor.h>
#include <stig/type/seq.h>

namespace Stig {

  namespace Expr {

    class TCollectedBy
        : public TBinary,
          public TLhsRhsable {
      NO_COPY_SEMANTICS(TCollectedBy);
      public:

      using TPtr = std::shared_ptr<TCollectedBy>;

      static TPtr New(const TExpr::TPtr &lhs, const TPosRange &pos_range) {
        return TCollectedBy::TPtr(new TCollectedBy(lhs, pos_range));
      }

      virtual void Accept(const TVisitor &visitor) const {
        assert(this);
        assert(&visitor);
        visitor(this);
      }

      virtual Type::TType GetLhsRhsType() const {
        return GetAddrElems()[1].second;
      }

      virtual Type::TType GetType() const {
        assert(this);
        /* This visitor is used to make sure that the type that arises from the expression on the
           right of the collected_by matches the type of the 'lhs' and 'rhs' keywords. */
        class TCollectVisitor
            : public Type::TEqualVisitor {
          NO_COPY_SEMANTICS(TCollectVisitor);
          public:
          TCollectVisitor(Type::TType &type, const TPosRange &pos_range)
              : Type::TEqualVisitor(type, pos_range) {}
          virtual void operator()(const Type::TAddr     *, const Type::TAddr     *) const {}
          virtual void operator()(const Type::TBool     *, const Type::TBool     *) const {}
          virtual void operator()(const Type::TDict     *, const Type::TDict     *) const {}
          virtual void operator()(const Type::TId       *, const Type::TId       *) const {}
          virtual void operator()(const Type::TInt      *, const Type::TInt      *) const {}
          virtual void operator()(const Type::TList     *, const Type::TList     *) const {}
          virtual void operator()(const Type::TObj      *, const Type::TObj      *) const {}
          virtual void operator()(const Type::TReal     *, const Type::TReal     *) const {}
          virtual void operator()(const Type::TSet      *, const Type::TSet      *) const {}
          virtual void operator()(const Type::TStr      *, const Type::TStr      *) const {}
          virtual void operator()(const Type::TTimeDiff *, const Type::TTimeDiff *) const {}
          virtual void operator()(const Type::TTimePnt  *, const Type::TTimePnt  *) const {}
        };  // TCollectVisitor
        Type::TType dummy;
        Type::TType::Accept(
            GetLhsRhsType(),
            GetRhs()->GetType(),
            TCollectVisitor(dummy, GetPosRange()));
        return Type::TDict::Get(GetAddrElems()[0].second, GetRhs()->GetType());
      }

      private:

      Type::TAddr::TElems GetAddrElems() const {
        auto type = GetLhs()->GetType();
        auto seq = type.TryAs<Type::TSeq>();
        if (!seq) {
          throw TExprError(
                  HERE,
                  GetPosRange(),
                  "The expression to the left of 'collected_by' must be a sequence");
        }  // if
        auto elem = seq->GetElem();
        auto addr = elem.TryAs<Type::TAddr>();
        if (!addr) {
          throw TExprError(
                  HERE,
                  GetPosRange(),
                  "The sequence to the left of 'collected_by' must be of addresses.");
        }  // if
        auto elems = addr->GetElems();
        if (elems.size() != 2) {
          throw TExprError(
                  HERE,
                  GetPosRange(),
                  "The addresses in the sequence to the left of 'collected_by' must be of size 2.");
        }  // if
        return elems;
      }

      TCollectedBy(const TExpr::TPtr &lhs, const TPosRange &pos_range)
          : TBinary(lhs, pos_range) {}

    };  // TCollectedBy

  }  // Expr

}  // Stig
