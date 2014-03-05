/* <stig/expr/assert.cc>

   Implements <stig/expr/assert.h>

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

#include <stig/expr/assert.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/type/bool.h>
#include <stig/type/unwrap.h>
#include <stig/type/util.h>

using namespace Stig;
using namespace Stig::Expr;

TAssertCase::TPtr TAssertCase::New(
    const TAssert::TPtr &assert,
    const Base::TOpt<std::string> &opt_name,
    const TExpr::TPtr &expr) {
  assert(assert);
  auto assert_case = TAssertCase::TPtr(new TAssertCase(assert, opt_name, expr));
  assert->Insert(assert_case);
  return assert_case;
}

TAssertCase::TAssertCase(
    const TAssert::TPtr &assert,
    const Base::TOpt<std::string> &opt_name,
    const TExpr::TPtr &expr)
      : Symbol::TRoot(expr),
        Assert(assert),
        OptName(opt_name) {}

TAssertCase::~TAssertCase() {
  assert(this);
  auto assert = TryGetAssert();
  if (assert) {
    assert->Delete(shared_from_this());
  }
}

TAssert::TPtr TAssertCase::GetAssert() const {
  return Base::AssertTrue(TryGetAssert());
}

const std::string &TAssertCase::GetName() const {
  assert(this);
  assert(OptName);
  return *OptName;
}

bool TAssertCase::HasName() const {
  assert(this);
  return OptName;
}

TAssert::TPtr TAssertCase::TryGetAssert() const {
  assert(this);
  return Assert.lock();
}

void TAssertCase::TypeCheck() const {
  assert(this);
  if (!Type::Unwrap(GetExpr()->GetType()).Is<Type::TBool>()) {
    throw TExprError(HERE, GetExpr()->GetPosRange(),
        "Assert cases must evaluate to a boolean");
  }
}

TAssert::TPtr TAssert::New(const TExpr::TPtr &expr, const TPosRange &pos_range) {
  return TAssert::TPtr(new TAssert(expr, pos_range));
}

TAssert::TAssert(const TExpr::TPtr &expr, const TPosRange &pos_range)
    : TThatableUnary(expr, pos_range) {}

void TAssert::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TAssert::Delete(const TAssertCase::TPtr &assert_case) {
  assert(this);
  assert(&assert_case);
  assert(assert_case);
  AssertCases.erase(assert_case);
}

const TAssert::TAssertCaseSet &TAssert::GetAssertCases() const {
  assert(this);
  return AssertCases;
}

Type::TType TAssert::GetType() const {
  assert(this);
  for (const auto &assert_case : AssertCases) {
    assert_case->TypeCheck();
  }
  return GetExpr()->GetType();
}

void TAssert::Insert(const TAssertCase::TPtr &assert_case) {
  assert(this);
  assert(&assert_case);
  assert(assert_case);
  auto result = AssertCases.insert(assert_case);
  assert(result.second);
}
