/* <orly/expr/free.cc>

   Implements <orly/expr/free.h>

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

#include <orly/expr/free.h>

#include <orly/expr/visitor.h>

using namespace Orly;
using namespace Orly::Expr;

TFree::TPtr TFree::New(const Type::TType &type, const TPosRange &pos_range, TAddrDir addr_dir) {
  return TFree::TPtr(new TFree(type, pos_range, addr_dir));
}

TFree::TFree(const Type::TType &type, const TPosRange &pos_range, TAddrDir addr_dir)
    : TLeaf(pos_range), Type(type), AddrDir(addr_dir) {}

void TFree::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TFree::GetTypeImpl() const {
  assert(this);
  return Type;
}
