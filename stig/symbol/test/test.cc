/* <stig/symbol/test/test.cc>

   Implements <stig/symbol/test/test.h>

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

#include <stig/symbol/test/test.h>

#include <base/assert_true.h>
#include <stig/symbol/scope.h>

using namespace Stig;
using namespace Stig::Symbol;
using namespace Stig::Symbol::Test;

TTest::TPtr TTest::New(
    const TScope::TPtr &scope,
    const TWithClause::TPtr &opt_with_clause,
    const TTestCaseBlock::TPtr &test_case_block,
    const TPosRange &pos_range) {
  assert(scope);
  auto test = TTest::TPtr(new TTest(scope, opt_with_clause, test_case_block, pos_range));
  scope->Add(test);
  return test;
}

TTest::TTest(
    const TScope::TPtr &scope,
    const TWithClause::TPtr &opt_with_clause,
    const TTestCaseBlock::TPtr &test_case_block,
    const TPosRange &pos_range)
      : Scope(Base::AssertTrue(scope)),
        OptWithClause(opt_with_clause),
        TestCaseBlock(Base::AssertTrue(test_case_block)),
        PosRange(pos_range) {}

TTest::~TTest() {
  assert(this);
  auto scope = TryGetScope();
  if (scope) {
    scope->Remove(shared_from_this());
  }
}

const TPosRange &TTest::GetPosRange() const {
  assert(this);
  return PosRange;
}

TScope::TPtr TTest::GetScope() const {
  return Base::AssertTrue(TryGetScope());
}

const TWithClause::TPtr &TTest::GetOptWithClause() const {
  assert(this);
  return OptWithClause;
}

const TTestCaseBlock::TPtr &TTest::GetTestCaseBlock() const {
  assert(this);
  return TestCaseBlock;
}

TScope::TPtr TTest::TryGetScope() const {
  assert(this);
  return Scope.lock();
}

void TTest::TypeCheck() const {
  assert(this);
  if (OptWithClause) {
    OptWithClause->TypeCheck();
  }
  TestCaseBlock->TypeCheck();
}
