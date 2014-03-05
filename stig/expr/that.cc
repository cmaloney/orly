/* <stig/expr/that.cc>

   Implements <stig/expr/that.h>

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

#include <stig/expr/that.h>

#include <base/assert_true.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/func.h>
#include <stig/type/obj.h>
#include <stig/type/time_pnt.h>

using namespace Stig;
using namespace Stig::Expr;

TThat::TPtr TThat::New(const TThatable::TPtr &thatable, const TPosRange &pos_range) {
  return TThat::TPtr(new TThat(Base::AssertTrue(thatable.get()), pos_range));
}

TThat::TThat(const TThatable *thatable, const TPosRange &pos_range)
    : TLeaf(pos_range), Thatable(thatable) {}

void TThat::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TThat::GetType() const {
  assert(this);
  return Thatable->GetThatType();
}
