/* <stig/synth/str_replace.cc>

   Implements str_replace.h

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

#include <stig/synth/str_replace.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/expr/ref.h>
#include <stig/symbol/built_in_function.h>
#include <stig/synth/get_pos_range.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Synth;

TStrReplace::TStrReplace(const Package::Syntax::TBuiltInReplace *str_replace)
    : StrReplace(Base::AssertTrue(str_replace)) {}

Expr::TExpr::TPtr TStrReplace::Build() const {
  return Expr::TRef::New((*Symbol::TBuiltInFunction::Replace)->GetResultDefs()[0], GetPosRange(StrReplace));
}
