/* <stig/symbol/stmt/mutate.h>

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

#include <stig/shared_enum.h>
#include <stig/symbol/stmt/binary.h>

namespace Stig {

  namespace Symbol {

    namespace Stmt {

      class TMutate
          : public TBinary {
        NO_COPY_SEMANTICS(TMutate);
        public:

        typedef std::shared_ptr<TMutate> TPtr;

        static TPtr New(
            const TStmtArg::TPtr &lhs,
            TMutator mutator,
            const TStmtArg::TPtr &rhs,
            const TPosRange &pos_range);

        virtual void Accept(const TVisitor &visitor) const final;

        const TMutator &GetMutator() const;

        virtual void TypeCheck() const;

        private:

        TMutate(
            const TStmtArg::TPtr &lhs,
            TMutator mutator,
            const TStmtArg::TPtr &rhs,
            const TPosRange &pos_range);

        TMutator Mutator;

      };  // TMutate

    }  // Stmt

  }  // Symbol

}  // Stig
