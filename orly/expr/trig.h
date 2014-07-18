/* <orly/expr/trig.h>

   TODO

   Copyright 2010-2014 OrlyAtomics, Inc.

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

#include <base/class_traits.h>
#include <orly/expr/binary.h>
#include <orly/expr/unary.h>

namespace Orly {

  namespace Expr {

    class TCos
        : public TUnary {
      NO_COPY(TCos);
      public:

      typedef std::shared_ptr<TCos> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &expr, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TCos(const TExpr::TPtr &expr, const TPosRange &pos_range);

    };  // TCos

    class TSin
        : public TUnary {
      NO_COPY(TSin);
      public:

      typedef std::shared_ptr<TSin> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &expr, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TSin(const TExpr::TPtr &expr, const TPosRange &pos_range);

    };  // TSin

    class TTan
        : public TUnary {
      NO_COPY(TTan);
      public:

      typedef std::shared_ptr<TTan> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &expr, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TTan(const TExpr::TPtr &expr, const TPosRange &pos_range);

    };  // TTan

    class TAcos
        : public TUnary {
      NO_COPY(TAcos);
      public:

      typedef std::shared_ptr<TAcos> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &expr, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TAcos(const TExpr::TPtr &expr, const TPosRange &pos_range);

    };  // TAcos

    class TAsin
        : public TUnary {
      NO_COPY(TAsin);
      public:

      typedef std::shared_ptr<TAsin> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &expr, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TAsin(const TExpr::TPtr &expr, const TPosRange &pos_range);

    };  // TAsin

    class TAtan
        : public TUnary {
      NO_COPY(TAtan);
      public:

      typedef std::shared_ptr<TAtan> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &expr, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TAtan(const TExpr::TPtr &expr, const TPosRange &pos_range);

    };  // TAtan

    class TAtan2
        : public TBinary {
      NO_COPY(TAtan2);
      public:

      typedef std::shared_ptr<TAtan2> TPtr;

      static TExpr::TPtr New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

      virtual void Accept(const TVisitor &visitor) const;

      virtual Type::TType GetType() const;

      private:

      TAtan2(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range);

    };  // TAtan2

  }  // Expr

}  // Orly
