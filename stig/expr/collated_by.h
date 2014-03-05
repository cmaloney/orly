/* <stig/expr/collated_by.h>

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

#include <base/no_copy_semantics.h>
#include <stig/expr/start.h>
#include <stig/expr/startable.h>
#include <stig/expr/thatable.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/type/addr.h>
#include <stig/type/seq.h>
#include <stig/type/unwrap.h>

namespace Stig {

  namespace Expr {

    class TCollatedBy
        : public TInterior,
          public TThatable,
          public TStartable {
      NO_COPY_SEMANTICS(TCollatedBy);
      public:

      using TPtr = std::shared_ptr<TCollatedBy>;

      static TPtr New(const TExpr::TPtr &seq, const TPosRange &pos_range) {
        return TCollatedBy::TPtr(new TCollatedBy(seq, pos_range));
      }

      ~TCollatedBy() {
        assert(this);
        if (Seq) {
          Seq->UnsetExprParent(this);
        }  // if
        if (Reduce) {
          Reduce->UnsetExprParent(this);
        }  // if
        if (Having) {
          Having->UnsetExprParent(this);
        }  // if
      }

      virtual void Accept(const TVisitor &visitor) const {
        assert(this);
        assert(&visitor);
        visitor(this);
      }

      virtual Type::TType GetType() const {
        assert(this);
        if (!GetSeq()->GetType().Is<Type::TSeq>()) {
          throw TExprError(
                  HERE,
                  GetPosRange(),
                  "The expression to the left of 'collated_by' must be a sequence");
        }  // if
        if (!Start) {
          throw TExprError(
                  HERE,
                  GetPosRange(),
                  "We must have exactly one occurence of 'start'"
                  " expression in the reduce portion of collated_by()");

        }  // if
        Type::TType key = Type::Unwrap(GetHaving()->GetType());
        Type::TType val = Type::Unwrap(GetReduce()->GetType());
        if (Type::Unwrap(Start->GetType()) != val) {
          throw TExprError(
                  HERE,
                  GetPosRange(),
                  "The reduce portion of the collated_by expression must have the same type as"
                  " the starting expression within it.");
        }  // if
        Type::TAddr::TElems elems {std::make_pair(TAddrDir::Asc, key),
                                   std::make_pair(TAddrDir::Asc, val)};
        return Type::TSeq::Get(Type::TAddr::Get(elems));
      }

      Type::TType GetThatType() const {
        assert(this);
        return Type::Unwrap(GetSeq()->GetType());
      }

      const TExpr::TPtr &GetSeq() const {
        assert(this);
        assert(Seq);
        return Seq;
      }

      const TExpr::TPtr &GetReduce() const {
        assert(this);
        assert(Reduce);
        return Reduce;
      }

      const TExpr::TPtr &GetHaving() const {
        assert(this);
        assert(Having);
        return Having;
      }

      const TExpr::TPtr &TryGetReduce() const {
        assert(this);
        return Reduce;
      }

      const TExpr::TPtr &TryGetHaving() const {
        assert(this);
        return Having;
      }

      void SetReduce(const TExpr::TPtr &reduce) {
        assert(this);
        assert(reduce);
        assert(!Reduce);
        Reduce = reduce;
        Reduce->SetExprParent(this);
      }

      void SetHaving(const TExpr::TPtr &having) {
        assert(this);
        assert(having);
        assert(!Having);
        Having = having;
        Having->SetExprParent(this);
      }

      private:

      TCollatedBy(const TExpr::TPtr &seq, const TPosRange &pos_range)
          : TInterior{pos_range},
            Seq{Base::AssertTrue(seq)},
            Reduce{nullptr},
            Having{nullptr} {
        Seq->SetExprParent(this);
      }

      TExpr::TPtr Seq;

      TExpr::TPtr Reduce;

      TExpr::TPtr Having;

    };  // TCollatedBy

  }  // namespace Expr

}  // namespace Stig