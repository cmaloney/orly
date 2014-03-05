/* <stig/synth/time_pnt_ctor.cc>

   Implements <stig/synth/time_pnt_ctor.h>.

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

#include <stig/synth/time_pnt_ctor.h>

#include <base/assert_true.h>
#include <stig/expr/ref.h>
#include <stig/symbol/built_in_function.h>
#include <stig/synth/get_pos_range.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Synth;

TTimePntCtor::TTimePntCtor(const Package::Syntax::TTimePntCtor *time_pnt_ctor)
    : TimePntCtor(Base::AssertTrue(time_pnt_ctor)) {}

Expr::TExpr::TPtr TTimePntCtor::Build() const {
  return Expr::TRef::New((*Symbol::TBuiltInFunction::TimePnt)->GetResultDefs()[0], GetPosRange(TimePntCtor));
}
