/* <stig/expr/now.cc>

   Implements <stig/expr/now.h>

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

#include <stig/expr/now.h>

#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type/func.h>
#include <stig/type/obj.h>
#include <stig/type/time_pnt.h>

using namespace Stig;
using namespace Stig::Expr;

TNow::TPtr TNow::New(const TPosRange &pos_range) {
  return TNow::TPtr(new TNow(pos_range));
}

TNow::TNow(const TPosRange &pos_range)
    : TLeaf(pos_range) {}

void TNow::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TNow::GetType() const {
  assert(this);
  return Type::TTimePnt::Get();
}
