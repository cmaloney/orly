/* <orly/expr/where.cc>

   Implements <orly/expr/where.h>

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/expr/where.h>

#include <orly/expr/visitor.h>
#include <orly/pos_range.h>
#include <orly/type/impl.h>

using namespace Orly;
using namespace Orly::Expr;

TWhere::TPtr TWhere::New(const TPosRange &pos_range) {
  return TWhere::TPtr(new TWhere(pos_range));
}

TWhere::TWhere(const TPosRange &pos_range)
    : TUnary(pos_range) {}

void TWhere::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TWhere::GetTypeImpl() const {
  assert(this);
  return GetExpr()->GetType();
}