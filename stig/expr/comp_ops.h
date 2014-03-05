/* <stig/expr/comp_ops.h>

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
#include <stig/expr/binary.h>

namespace Stig {

  namespace Expr {

    class TEqEq
        : public TBinary {
      NO_COPY_SEMANTICS(TEqEq);
      public:

      typedef std::shared_ptr<TEqEq> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TEqEq(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

    };  // TEqEq

    class TNeq
        : public TBinary {
      NO_COPY_SEMANTICS(TNeq);
      public:

      typedef std::shared_ptr<TNeq> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TNeq(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

    };  // TNeq

    class TLt
        : public TBinary {
      NO_COPY_SEMANTICS(TLt);
      public:

      typedef std::shared_ptr<TLt> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TLt(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

    };  // TLt

    class TLtEq
        : public TBinary {
      NO_COPY_SEMANTICS(TLtEq);
      public:

      typedef std::shared_ptr<TLtEq> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TLtEq(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

    };  // TLtEq

    class TGt
        : public TBinary {
      NO_COPY_SEMANTICS(TGt);
      public:

      typedef std::shared_ptr<TGt> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TGt(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

    };  // TGt

    class TGtEq
        : public TBinary {
      NO_COPY_SEMANTICS(TGtEq);
      public:

      typedef std::shared_ptr<TGtEq> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TGtEq(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

    };  // TGtEq

  }  // Expr

}  // Stig
