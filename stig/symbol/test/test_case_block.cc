/* <stig/symbol/test/test_case_block.cc>

   Implements <stig/symbol/test/test_case_block.h>

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

#include <stig/symbol/test/test_case_block.h>

#include <stig/error.h>
#include <stig/type/bool.h>
#include <stig/type/unwrap.h>
#include <stig/type/util.h>

using namespace Stig;
using namespace Stig::Symbol;
using namespace Stig::Symbol::Test;

TTestCase::TPtr TTestCase::New(
    const Expr::TExpr::TPtr &expr,
    const Base::TOpt<std::string> &opt_name,
    const TTestCaseBlock::TPtr &opt_test_case_block,
    const TPosRange &pos_range) {
  return TTestCase::TPtr(new TTestCase(expr, opt_name, opt_test_case_block, pos_range));
}

TTestCase::TTestCase(
    const Expr::TExpr::TPtr &expr,
    const Base::TOpt<std::string> &opt_name,
    const TTestCaseBlock::TPtr &opt_test_case_block,
    const TPosRange &pos_range)
      : TRoot(expr),
        OptName(opt_name),
        OptTestCaseBlock(opt_test_case_block),
        PosRange(pos_range) {}

const std::string &TTestCase::GetName() const {
  assert(this);
  assert(OptName);
  return *OptName;
}

const TPosRange &TTestCase::GetPosRange() const {
  assert(this);
  return PosRange;
}

bool TTestCase::HasName() const {
  assert(this);
  return OptName;
}

const TTestCaseBlock::TPtr &TTestCase::GetOptTestCaseBlock() const {
  assert(this);
  return OptTestCaseBlock;
}

void TTestCase::TypeCheck() const {
  assert(this);
  if (!Type::UnwrapMutable(GetExpr()->GetType()).Is<Type::TBool>()) {
    throw TExprError(HERE, PosRange, "Currently test cases must evaluate to bool.");
  }
  if (OptTestCaseBlock) {
    OptTestCaseBlock->TypeCheck();
  }
}

TTestCaseBlock::TPtr TTestCaseBlock::New(const TTestCases &test_cases, const TPosRange &pos_range) {
  return TTestCaseBlock::TPtr(new TTestCaseBlock(test_cases, pos_range));
}

TTestCaseBlock::TTestCaseBlock(const TTestCases &test_cases, const TPosRange &pos_range)
    : TestCases(test_cases), PosRange(pos_range) {}

const TPosRange &TTestCaseBlock::GetPosRange() const {
  assert(this);
  return PosRange;
}

const TTestCaseBlock::TTestCases &TTestCaseBlock::GetTestCases() const {
  assert(this);
  return TestCases;
}

void TTestCaseBlock::TypeCheck() const {
  assert(this);
  for (const auto &test_case : TestCases) {
    test_case->TypeCheck();
  }
}
