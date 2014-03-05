/* <stig/var/time_diff.cc>

   Implements <stig/var/time_diff.h>.

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

#include <stig/var/time_diff.h>

#include <stig/type/time_diff.h>

using namespace Stig;
using namespace Stig::Var;

size_t TTimeDiff::GetHash() const {
  assert(this);
  return std::hash<Base::Chrono::TTimeDiff>()(Val);
}

Type::TType TTimeDiff::GetType() const {
  assert(this);
  return Type::TTimeDiff::Get();
}

void TTimeDiff::Write(std::ostream &strm) const {
  assert(&strm);
  strm << "Base::Chrono::TTimeDiff(" << Val.count() << ')';
}

void TTimeDiff::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TTimeDiff::Touch() {
  assert(this);
}

Var::TVar &TTimeDiff::Index(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Index not supported on time_diff.");
}

TTimeDiff &TTimeDiff::Add(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Add not supported on time_diff.");
}

TTimeDiff &TTimeDiff::And(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "And not supported on time_diff.");
}

TTimeDiff &TTimeDiff::Div(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Div not supported on time_diff.");
}

TTimeDiff &TTimeDiff::Exp(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Exp not supported on time_diff.");
}

TTimeDiff &TTimeDiff::Intersection(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Intersection not supported on time_diff.");
}

TTimeDiff &TTimeDiff::Mod(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mod not supported on time_diff.");
}

TTimeDiff &TTimeDiff::Mult(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mult not supported on time_diff.");
}

TTimeDiff &TTimeDiff::Or(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Or not supported on time_diff.");
}

TTimeDiff &TTimeDiff::Sub(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Sub not supported on time_diff.");
}

TTimeDiff &TTimeDiff::SymmetricDiff(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "SymmetricDiff not supported on time_diff.");
}

TTimeDiff &TTimeDiff::Union(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Union not supported on time_diff.");
}

TTimeDiff &TTimeDiff::Xor(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Xor not supported on time_diff.");
}

TTimeDiff::TTimeDiff(const Base::Chrono::TTimeDiff &that) : Val(that) {}

TTimeDiff::~TTimeDiff() {}

TVar TTimeDiff::Copy() const {
  assert(this);
  return TTimeDiff::New(Val);
}

TVar TTimeDiff::New(const Base::Chrono::TTimeDiff &that) {
  return (new TTimeDiff(that))->AsVar();
}

TVar::TVar(const Base::Chrono::TTimeDiff &that) {
  *this = TTimeDiff::New(that);
}