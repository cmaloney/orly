/* <stig/synth/test_def.cc>

   Implements <stig/synth/test_def.h>

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

#include <stig/synth/test_def.h>

#include <sstream>

#include <base/assert_true.h>
#include <stig/symbol/test/test.h>
#include <stig/synth/cst_utils.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>
#include <stig/synth/test_case_block.h>
#include <stig/synth/with_clause.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Synth;

unsigned int TTestDef::TestId = 0;

TName TTestDef::GenName(const TPosRange &pos_range) {
  std::ostringstream strm;
  strm << "@Test" << ++TestId;
  return TName(strm.str(), pos_range);
}

TTestDef::TTestDef(TScope *scope, const Package::Syntax::TTestDef *test_def)
    : TDef(scope, GenName(GetPosRange(test_def))),
      TestDef(Base::AssertTrue(test_def)),
      OptWithClause(nullptr),
      TestCaseBlock(nullptr) {
  auto expr_factory = TExprFactory(scope);
  auto with_clause = TryGetNode<Package::Syntax::TWithClause,
                                Package::Syntax::TNoWithClause>(TestDef->GetOptWithClause());
  if (with_clause) {
    OptWithClause = new TWithClause(&expr_factory, with_clause);
  }
  try {
    TestCaseBlock = new TTestCaseBlock(&expr_factory, TestDef->GetTestCaseBlock());
  } catch (...) {
    delete OptWithClause;
    delete TestCaseBlock;
    throw;
  }
}

TTestDef::~TTestDef() {
  assert(this);
  delete OptWithClause;
  delete TestCaseBlock;
}

TAction TTestDef::Build(int pass) {
  assert(this);
  TAction action;
  switch (pass) {
    case 1:
    case 2:
    case 3: {
      action = Continue;
      break;
    }
    case 4: {
      Symbol::Test::TTest::New(
          GetOuterScope()->GetScopeSymbol(), /* scope */
          OptWithClause ? OptWithClause->Build() : nullptr, /* opt_with_clause */
          TestCaseBlock->Build(), /* test_case_block */
          GetPosRange(TestDef)); /* pos_range */
      action = Finish;
      break;
    }
    NO_DEFAULT_CASE;
  }
  return action;
}

void TTestDef::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  if (OptWithClause) {
    OptWithClause->ForEachInnerScope(cb);
  }
  TestCaseBlock->ForEachInnerScope(cb);
}

void TTestDef::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  if (OptWithClause) {
    OptWithClause->ForEachRef(cb);
  }
  TestCaseBlock->ForEachRef(cb);
}
