/* <stig/var/time_pnt.cc>

   Implements <stig/var/time_pnt.h>.

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

#include <stig/var/time_pnt.h>

#include <stig/type/time_pnt.h>

using namespace Stig;
using namespace Stig::Var;

size_t TTimePnt::GetHash() const {
  assert(this);
  return std::hash<Base::Chrono::TTimePnt>()(Val);
}

Type::TType TTimePnt::GetType() const {
  assert(this);
  return Type::TTimePnt::Get();
}

void TTimePnt::Write(std::ostream &strm) const {
  assert(&strm);
  strm
    << "Base::Chrono::TTimePnt(Base::Chrono::TTimeDiff("
    << Base::Chrono::TimeDiffCast(Val.time_since_epoch()).count() << "))";
}

void TTimePnt::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TTimePnt::Touch() {
  assert(this);
}

Var::TVar &TTimePnt::Index(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Index not supported on time_pnt.");
}

TTimePnt &TTimePnt::Add(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Add not supported on time_pnt.");
}

TTimePnt &TTimePnt::And(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "And not supported on time_pnt.");
}

TTimePnt &TTimePnt::Div(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Div not supported on time_pnt.");
}

TTimePnt &TTimePnt::Exp(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Exp not supported on time_pnt.");
}

TTimePnt &TTimePnt::Intersection(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Intersection not supported on time_pnt.");
}

TTimePnt &TTimePnt::Mod(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mod not supported on time_pnt.");
}

TTimePnt &TTimePnt::Mult(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mult not supported on time_pnt.");
}

TTimePnt &TTimePnt::Or(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Or not supported on time_pnt.");
}

TTimePnt &TTimePnt::Sub(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Sub not supported on time_pnt.");
}

TTimePnt &TTimePnt::SymmetricDiff(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "SymmetricDiff not supported on time_pnt.");
}

TTimePnt &TTimePnt::Union(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Union not supported on time_pnt.");
}

TTimePnt &TTimePnt::Xor(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Xor not supported on time_pnt.");
}

TTimePnt::TTimePnt(const Base::Chrono::TTimePnt &that) : Val(that) {}

TTimePnt::~TTimePnt() {}

TVar TTimePnt::Copy() const {
  assert(this);
  return TTimePnt::New(Val);
}

TVar TTimePnt::New(const Base::Chrono::TTimePnt &that) {
  return (new TTimePnt(that))->AsVar();
}

TVar::TVar(const Base::Chrono::TTimePnt &that) {
  *this = TTimePnt::New(that);
}