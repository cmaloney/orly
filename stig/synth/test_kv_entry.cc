/* <stig/synth/test_kv_entry.cc>

   Implements <stig/synth/test_kv_entry.h>.

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

#include <stig/synth/test_kv_entry.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/synth/get_pos_range.h>
#include <stig/symbol/stmt/stmt_arg.h>

using namespace Stig;
using namespace Stig::Synth;

TTestKvEntry::TTestKvEntry(
    const TExprFactory *expr_factory,
    const Package::Syntax::TTestKvEntry *test_kv_entry)
      : TestKvEntry(Base::AssertTrue(test_kv_entry)),
        Key(nullptr),
        Val(nullptr) {
  assert(expr_factory);
  try {
    Key = expr_factory->NewExpr(TestKvEntry->GetKey());
    Val = expr_factory->NewExpr(TestKvEntry->GetValue());
  } catch (...) {
    delete Key;
    delete Val;
    throw;
  }
}

TTestKvEntry::~TTestKvEntry() {
  assert(this);
  delete Key;
  delete Val;
}

Symbol::Stmt::TNew::TPtr TTestKvEntry::Build() const {
  assert(this);
  return Symbol::Stmt::TNew::New(
             Symbol::Stmt::TStmtArg::New(Key->Build()),
             Symbol::Stmt::TStmtArg::New(Val->Build()),
             GetPosRange(TestKvEntry));
}

void TTestKvEntry::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Key->ForEachInnerScope(cb);
  Val->ForEachInnerScope(cb);
}

void TTestKvEntry::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Key->ForEachRef(cb);
  Val->ForEachRef(cb);
}
