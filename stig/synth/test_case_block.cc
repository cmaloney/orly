/* <stig/synth/test_case_block.cc>

   Implements <stig/synth/test_case_block.h>

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

#include <stig/synth/test_case_block.h>

#include <base/assert_true.h>
#include <stig/symbol/test/test_case_block.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Synth;

TTestCaseBlock::TTestCase::TTestCase(
    const Base::TOpt<std::string> &opt_name,
    TExpr *expr,
    TTestCaseBlock *opt_test_case_block,
    const TPosRange &pos_range)
      : Expr(Base::AssertTrue(expr)),
        OptName(opt_name),
        OptTestCaseBlock(opt_test_case_block),
        PosRange(pos_range) {}

TTestCaseBlock::TTestCase::~TTestCase() {
  assert(this);
  delete Expr;
  delete OptTestCaseBlock;
}

Symbol::Test::TTestCase::TPtr TTestCaseBlock::TTestCase::Build() const {
  assert(this);
  return Symbol::Test::TTestCase::New(
             Expr->Build(),
             OptName,
             OptTestCaseBlock ? OptTestCaseBlock->Build() : nullptr,
             PosRange);
}

void TTestCaseBlock::TTestCase::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  Expr->ForEachInnerScope(cb);
  if (OptTestCaseBlock) {
    OptTestCaseBlock->ForEachInnerScope(cb);
  }
}

void TTestCaseBlock::TTestCase::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  Expr->ForEachRef(cb);
  if (OptTestCaseBlock) {
    OptTestCaseBlock->ForEachRef(cb);
  }
}

TTestCaseBlock::TTestCaseBlock(const TExprFactory *expr_factory, const Package::Syntax::TTestCaseBlock *test_case_block)
    : TestCaseBlock(Base::AssertTrue(test_case_block)) {
  class TTestCaseVisitor
      : public Package::Syntax::TTestCase::TVisitor {
    NO_COPY_SEMANTICS(TTestCaseVisitor);
    public:
    TTestCaseVisitor(const TExprFactory *expr_factory, TTestCases &test_cases, TTestCaseNames &test_case_names)
        : ExprFactory(expr_factory), TestCases(test_cases), TestCaseNames(test_case_names) {}
    virtual void operator()(const Package::Syntax::TBadTestCase *) const { /* DO NOTHING */ }
    virtual void operator()(const Package::Syntax::TLabeledTestCase *that) const {
      TName name(that->GetName());
      auto result = TestCaseNames.insert(name);
      if (result.second) {
        AddTestCase(name.GetText(), that->GetExpr(), that->GetOptTestCaseBlock(), GetPosRange(that));
      } else {
        Tools::Nycr::TError::TBuilder(name.GetPosRange())
            << "duplicate name \"" << name.GetText()
            << "\" previously declared at " << result.first->GetPosRange();
      }
    }
    virtual void operator()(const Package::Syntax::TUnlabeledTestCase *that) const {
      AddTestCase(*Base::TOpt<std::string>::Unknown, that->GetExpr(), that->GetOptTestCaseBlock(), GetPosRange(that));
    }
    private:
    void AddTestCase(
        const Base::TOpt<std::string> &opt_name,
        const Package::Syntax::TExpr *expr,
        const Package::Syntax::TOptTestCaseBlock *opt_test_case_block,
        const TPosRange &pos_range) const {
      auto test_case_block = TryGetNode<Package::Syntax::TTestCaseBlock, Package::Syntax::TNoTestCaseBlock>(opt_test_case_block);
      TestCases.emplace_back(new TTestCase(opt_name,
                                           ExprFactory->NewExpr(expr),
                                           test_case_block ? new TTestCaseBlock(ExprFactory, test_case_block) : nullptr,
                                           pos_range));
    }
    const TExprFactory *ExprFactory;
    TTestCases &TestCases;
    TTestCaseNames &TestCaseNames;
  };  // TTestCaseVisitor
  assert(expr_factory);
  TTestCaseVisitor visitor(expr_factory, TestCases, TestCaseNames);
  try {
    ForEach<Package::Syntax::TTestCase>(TestCaseBlock->GetTestCaseSeq(),
        [&visitor](const Package::Syntax::TTestCase *test_case) -> bool {
          test_case->Accept(visitor);
          return true;
        });
  } catch (...) {
    throw;
  }
}

TTestCaseBlock::~TTestCaseBlock() {
  assert(this);
  for (auto &test_case : TestCases) {
    delete test_case;
  }
}

Symbol::Test::TTestCaseBlock::TPtr TTestCaseBlock::Build() const {
  assert(this);
  Symbol::Test::TTestCaseBlock::TTestCases test_cases;
  for (auto &test_case : TestCases) {
    test_cases.push_back(test_case->Build());
  }
  return Symbol::Test::TTestCaseBlock::New(test_cases, GetPosRange(TestCaseBlock));
}

void TTestCaseBlock::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  for (auto &test_case : TestCases) {
    test_case->ForEachInnerScope(cb);
  }
}

void TTestCaseBlock::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  for (auto &test_case : TestCases) {
    test_case->ForEachRef(cb);
  }
}
