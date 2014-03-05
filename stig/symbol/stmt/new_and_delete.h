/* <stig/symbol/stmt/new_and_delete.h>

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

#include <stig/symbol/stmt/binary.h>
#include <stig/symbol/stmt/unary.h>
#include <stig/type/impl.h>

namespace Stig {

  namespace Symbol {

    namespace Stmt {

      class TDelete
          : public TUnary {
        NO_COPY_SEMANTICS(TDelete);
        public:

        typedef std::shared_ptr<TDelete> TPtr;

        static TPtr New(const TStmtArg::TPtr &stmt_arg, Type::TType value_type, const TPosRange &pos_range);

        virtual void Accept(const TVisitor &visitor) const final;

        virtual void TypeCheck() const;

        /* Do-little */
        Type::TType GetValueType() const {
          return ValueType;
        }

        private:

        TDelete(const TStmtArg::TPtr &stmt_arg, Type::TType value_type, const TPosRange &pos_range);

        /* The type of the value that is being deleted. */
        Type::TType ValueType;
      };  // TDelete

      class TNew
          : public TBinary {
        NO_COPY_SEMANTICS(TNew);
        public:

        typedef std::shared_ptr<TNew> TPtr;

        static TPtr New(
            const TStmtArg::TPtr &lhs,
            const TStmtArg::TPtr &rhs,
            const TPosRange &pos_range);

        virtual void Accept(const TVisitor &visitor) const final;

        virtual void TypeCheck() const;

        private:

        TNew(
            const TStmtArg::TPtr &lhs,
            const TStmtArg::TPtr &rhs,
            const TPosRange &pos_range);

      };  // TNew

    }  // Stmt

  }  // Symbol

}  // Stig
