/* <stig/synth/test_case_block.h>

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
#include <base/opt.h>
#include <stig/stig.package.cst.h>
#include <stig/symbol/test/test_case_block.h>
#include <stig/synth/expr.h>

namespace Stig {

  namespace Synth {

    class TExprFactory;

    class TTestCaseBlock {
      NO_COPY_SEMANTICS(TTestCaseBlock);
      public:

      TTestCaseBlock(
          const TExprFactory *expr_factory,
          const Package::Syntax::TTestCaseBlock *test_case_block);

      ~TTestCaseBlock();

      Symbol::Test::TTestCaseBlock::TPtr Build() const;

      void ForEachInnerScope(const std::function<void (TScope *)> &cb);

      void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      private:

      class TTestCase {
        NO_COPY_SEMANTICS(TTestCase);
        public:

        TTestCase(
            const Base::TOpt<std::string> &name,
            TExpr *expr,
            TTestCaseBlock *opt_test_case_block,
            const TPosRange &pos_range);

        ~TTestCase();

        Symbol::Test::TTestCase::TPtr Build() const;

        void ForEachInnerScope(const std::function<void (TScope *)> &cb);

        void ForEachRef(const std::function<void (TAnyRef &)> &cb);

        private:

        TExpr *Expr;

        Base::TOpt<std::string> OptName;

        TTestCaseBlock *OptTestCaseBlock;

        const TPosRange PosRange;

      };  // TTestCase

      typedef std::vector<TTestCase *> TTestCases;

      typedef std::unordered_set<TName> TTestCaseNames;

      const Package::Syntax::TTestCaseBlock *TestCaseBlock;

      TTestCases TestCases;

      TTestCaseNames TestCaseNames; // NOTE: This is only used to keep detect duplicate names.

    };  // TTestCaseBlock

  }  // Synth

}  // Stig