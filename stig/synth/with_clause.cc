/* <stig/synth/with_clause.cc>

   Implements <stig/synth/with_clause.h>

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

#include <stig/synth/with_clause.h>

#include <base/assert_true.h>

#include <stig/symbol/test/with_clause.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/test_kv_entry.h>

using namespace Stig;
using namespace Stig::Synth;

TWithClause::TWithClause(
    const TExprFactory *expr_factory,
    const Package::Syntax::TWithClause *with_clause)
      : WithClause(Base::AssertTrue(with_clause)) {
  assert(expr_factory);
  try {
    ForEach<Package::Syntax::TTestKvEntry>(with_clause->GetOptTestKvEntrySeq(),
        [this, expr_factory](const Package::Syntax::TTestKvEntry *test_kv_entry) -> bool {
          TestKvEntries.emplace_back(new TTestKvEntry(expr_factory, test_kv_entry));
          return true;
        });
  } catch (...) {
    Cleanup();
    throw;
  }
}

TWithClause::~TWithClause() {
  Cleanup();
}

Symbol::Test::TWithClause::TPtr TWithClause::Build() const {
  assert(this);
  Symbol::Test::TWithClause::TNewStmtSet new_stmts;
  for (auto &test_kv_entry : TestKvEntries) {
    auto result = new_stmts.emplace(test_kv_entry->Build());
    assert(result.second);
  }
  return Symbol::Test::TWithClause::New(new_stmts, GetPosRange(WithClause));
}

void TWithClause::Cleanup() {
  assert(this);
  for (auto &test_kv_entry : TestKvEntries) {
    delete test_kv_entry;
  }
}

void TWithClause::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  for (auto &test_kv_entry : TestKvEntries) {
    test_kv_entry->ForEachInnerScope(cb);
  }
}

void TWithClause::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  for (auto &test_kv_entry : TestKvEntries) {
    test_kv_entry->ForEachRef(cb);
  }
}
