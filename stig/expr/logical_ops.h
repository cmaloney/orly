/* <stig/expr/logical_ops.h>

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
#include <stig/expr/unary.h>

namespace Stig {

  namespace Expr {

    class TAnd
        : public TBinary {
      NO_COPY_SEMANTICS(TAnd);
      public:

      typedef std::shared_ptr<TAnd> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TAnd(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

    };  // TAnd

    class TAndThen
        : public TBinary {
      NO_COPY_SEMANTICS(TAndThen);
      public:

      typedef std::shared_ptr<TAndThen> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TAndThen(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

    };  // TAndThen

    class TNot
        : public TUnary {
      NO_COPY_SEMANTICS(TNot);
      public:

      typedef std::shared_ptr<TNot> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &expr, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TNot(const TExpr::TPtr &expr, const TPosRange &pos_range);

    };  // TNot

    class TOr
        : public TBinary {
      NO_COPY_SEMANTICS(TOr);
      public:

      typedef std::shared_ptr<TOr> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TOr(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

    };  // TOr

    class TOrElse
        : public TBinary {
      NO_COPY_SEMANTICS(TOrElse);
      public:

      typedef std::shared_ptr<TOrElse> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TOrElse(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

    };  // TOrElse

    class TXor
        : public TBinary {
      NO_COPY_SEMANTICS(TXor);
      public:

      typedef std::shared_ptr<TXor> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TXor(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

    };  // TXor

  }  // Expr

}  // Stig
