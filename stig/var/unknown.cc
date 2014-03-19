/* <stig/var/unknown.cc>

   Implements <stig/var/unknown.h>.

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

#include <stig/var/unknown.h>

#include <base/not_implemented.h>
#include <stig/rt/runtime_error.h>
#include <stig/type/unknown.h>

using namespace std;
using namespace Stig;
using namespace Stig::Var;

size_t TUnknown::GetHash() const {
  assert(this);
  return 0;
}

Type::TType TUnknown::GetType() const {
  assert(this);
  return Type::TUnknown::Get();
}

void TUnknown::Write(std::ostream &) const {
  NOT_IMPLEMENTED();
}

void TUnknown::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TUnknown::Touch() {
  assert(this);
}

Var::TVar &TUnknown::Index(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Index not supported on unknown.");
}

TUnknown &TUnknown::Add(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Add not supported on unknown.");
}

TUnknown &TUnknown::And(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "And not supported on unknown.");
}

TUnknown &TUnknown::Div(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Div not supported on unknown.");
}

TUnknown &TUnknown::Exp(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Exp not supported on unknown.");
}

TUnknown &TUnknown::Intersection(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Intersection not supported on unknown.");
}

TUnknown &TUnknown::Mod(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mod not supported on unknown.");
}

TUnknown &TUnknown::Mult(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mult not supported on unknown.");
}

TUnknown &TUnknown::Or(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Or not supported on unknown.");
}

TUnknown &TUnknown::Sub(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Sub not supported on unknown.");
}

TUnknown &TUnknown::SymmetricDiff(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "SymmetricDiff not supported on unknown.");
}

TUnknown &TUnknown::Union(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Union Or not supported on unknown.");
}

TUnknown &TUnknown::Xor(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Xor not supported on unknown.");
}

TUnknown::~TUnknown() {}

TVar TUnknown::Copy() const {
  return TUnknown::New();
}

TVar TUnknown::New() {
  return (new TUnknown())->AsVar();
}

TVar::TVar() {
  *this = TUnknown::New();
}