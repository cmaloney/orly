/* <stig/synth/time_diff_ctor.cc>

   Implements <stig/synth/time_diff_ctor.h>.

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

#include <stig/synth/time_diff_ctor.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/expr/ref.h>
#include <stig/symbol/built_in_function.h>
#include <stig/synth/get_pos_range.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Synth;

TTimeDiffCtor::TTimeDiffCtor(const Package::Syntax::TTimeDiffCtor *time_diff_ctor)
    : TimeDiffCtor(Base::AssertTrue(time_diff_ctor)) {}

Expr::TExpr::TPtr TTimeDiffCtor::Build() const {
  assert(this);
  return Expr::TRef::New((*Symbol::TBuiltInFunction::TimeDiff)->GetResultDefs()[0], GetPosRange(TimeDiffCtor));
}
