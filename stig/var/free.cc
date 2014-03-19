/* <stig/var/free.cc>

   Implements <stig/var/free.h>.

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

#include <stig/var/free.h>

#include <base/not_implemented.h>
#include <stig/rt/runtime_error.h>

using namespace std;
using namespace Stig;
using namespace Stig::Var;

size_t TFree::GetHash() const {
  assert(this);
  return 0;
}

Type::TType TFree::GetType() const {
  assert(this);
  return Type;
}

void TFree::Write(std::ostream &) const {
  NOT_IMPLEMENTED();
}

void TFree::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TFree::Touch() {
  assert(this);
}

Var::TVar &TFree::Index(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Index not supported on free.");
}

TFree &TFree::Add(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Add not supported on free.");
}

TFree &TFree::And(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "And not supported on free.");
}

TFree &TFree::Div(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Div not supported on free.");
}

TFree &TFree::Exp(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Exp not supported on free.");
}

TFree &TFree::Intersection(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Intersection not supported on free.");
}

TFree &TFree::Mod(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mod not supported on free.");
}

TFree &TFree::Mult(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mult not supported on free.");
}

TFree &TFree::Or(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Or not supported on free.");
}

TFree &TFree::Sub(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Sub not supported on free.");
}

TFree &TFree::SymmetricDiff(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "SymmetricDiff not supported on free.");
}

TFree &TFree::Union(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Union not supported on free.");
}

TFree &TFree::Xor(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Xor not supported on free.");
}

TFree::~TFree() {}

TVar TFree::Copy() const {
  return TFree::New(Type);
}

TVar TFree::New(const Type::TType &type) {
  return (new TFree(type))->AsVar();
}

TVar TVar::Free(const Type::TType &type) {
  return (new TFree(type))->AsVar();
}