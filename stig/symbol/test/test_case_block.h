/* <stig/symbol/test/test_case_block.h>

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
#include <vector>

#include <base/no_copy_semantics.h>
#include <base/opt.h>
#include <stig/expr/expr.h>
#include <stig/symbol/root.h>

namespace Stig {

  namespace Symbol {

    namespace Test {

      class TTestCaseBlock;

      class TTestCase
          : public TRoot {
        NO_COPY_SEMANTICS(TTestCase);
        public:

        typedef std::shared_ptr<TTestCase> TPtr;

        typedef std::shared_ptr<TTestCaseBlock> TTestCaseBlockPtr;

        static TPtr New(
            const Expr::TExpr::TPtr &expr,
            const Base::TOpt<std::string> &name,
            const TTestCaseBlockPtr &opt_test_case_block,
            const TPosRange &pos_range);

        const std::string &GetName() const;

        const TPosRange &GetPosRange() const;

        bool HasName() const;

        const TTestCaseBlockPtr &GetOptTestCaseBlock() const;

        void TypeCheck() const;

        private:

        TTestCase(
            const Expr::TExpr::TPtr &expr,
            const Base::TOpt<std::string> &name,
            const TTestCaseBlockPtr &opt_test_case_block,
            const TPosRange &pos_range);

        Base::TOpt<std::string> OptName;

        TTestCaseBlockPtr OptTestCaseBlock;

        const TPosRange PosRange;

      };  // TTestCase

      class TTestCaseBlock {
        NO_COPY_SEMANTICS(TTestCaseBlock);
        public:

        typedef std::shared_ptr<TTestCaseBlock> TPtr;

        // NOTE: We use a vector rather than a set here to keep ordering.
        typedef std::vector<TTestCase::TPtr> TTestCases;

        static TPtr New(const TTestCases &test_cases, const TPosRange &pos_range);

        const TPosRange &GetPosRange() const;

        const TTestCases &GetTestCases() const;

        void TypeCheck() const;

        private:

        TTestCaseBlock(const TTestCases &test_cases, const TPosRange &pos_range);

        TTestCases TestCases;

        const TPosRange PosRange;

      };  // TTestCaseBlock

    }  // Test

  }  // Symbol

}  // Stig
