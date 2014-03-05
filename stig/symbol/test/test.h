/* <stig/symbol/test/test.h>

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
#include <stig/pos_range.h>
#include <stig/symbol/root.h>
#include <stig/symbol/test/test_case_block.h>
#include <stig/symbol/test/with_clause.h>

namespace Stig {

  namespace Symbol {

    class TScope;

    namespace Test {

      class TTest
          : public std::enable_shared_from_this<TTest> {
        NO_COPY_SEMANTICS(TTest);
        public:

        typedef std::shared_ptr<TTest> TPtr;

        typedef std::shared_ptr<TScope> TScopePtr;

        static TPtr New(
            const TScopePtr &scope,
            const TWithClause::TPtr &opt_with_clause,
            const TTestCaseBlock::TPtr &test_case_block,
            const TPosRange &pos_range);

        virtual ~TTest();

        const TPosRange &GetPosRange() const;

        TScopePtr GetScope() const;

        const TWithClause::TPtr &GetOptWithClause() const;

        const TTestCaseBlock::TPtr &GetTestCaseBlock() const;

        TScopePtr TryGetScope() const;

        void TypeCheck() const;

        private:

        TTest(
            const TScopePtr &scope,
            const TWithClause::TPtr &opt_with_clause,
            const TTestCaseBlock::TPtr &test_case_block,
            const TPosRange &pos_range);

        std::weak_ptr<TScope> Scope;

        TWithClause::TPtr OptWithClause;

        TTestCaseBlock::TPtr TestCaseBlock;

        const TPosRange PosRange;

      };  // TTest

    }  // Test

  }  // Symbol

}  // Stig
