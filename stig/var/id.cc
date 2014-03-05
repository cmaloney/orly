/* <stig/var/id.cc>

   Implements <stig/var/id.h>.

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

#include <stig/var/id.h>

#include <stig/type/id.h>

using namespace Stig;
using namespace Var;

size_t TId::GetHash() const {
  assert(this);
  return Val.GetHash();
}

Type::TType TId::GetType() const {
  assert(this);
  return Type::TId::Get();
}

void TId::Write(std::ostream &strm) const {
  strm << "TUUID(\"" << Val << "\")";
}

void TId::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TId::Touch() {
  assert(this);
}

Var::TVar &TId::Index(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Index not supported on Id.");
}

TId &TId::Add(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Add not supported on Id.");
}

TId &TId::And(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "And not supported on Id.");
}

TId &TId::Div(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Div not supported on Id.");
}

TId &TId::Exp(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Exp not supported on Id.");
}

TId &TId::Intersection(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Intersection not supported on Id.");
}

TId &TId::Mod(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mod not supported on Id.");
}

TId &TId::Mult(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mult not supported on Id.");
}

TId &TId::Or(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Or not supported on Id.");
}

TId &TId::Sub(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Sub not supported on Id.");
}

TId &TId::SymmetricDiff(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "SymmetricDiff not supported on Id.");
}

TId &TId::Union(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Union not supported on Id.");
}

TId &TId::Xor(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Xor not supported on Id.");
}

TId::TId(const TUUID &that) : Val(that) {}

TId::~TId() {}

TVar TId::Copy() const {
  assert(this);
  return TId::New(Val);
}

TVar TId::New(const TUUID &that) {
  return (new TId(that))->AsVar();
}

TVar::TVar(const TUUID &that) {
  *this = TId::New(that);
}