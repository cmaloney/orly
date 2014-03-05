/* <stig/var/real.cc>

   Implements <stig/var/real.h>.

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

#include <stig/var/real.h>

#include <stig/type/real.h>

using namespace Stig;
using namespace Var;

size_t TReal::GetHash() const {
  assert(this);
  return std::hash<double>()(Val);
}

Type::TType TReal::GetType() const {
  assert(this);
  return Type::TReal::Get();
}

void TReal::Write(std::ostream &stream) const {
  assert(this);
  stream << std::showpoint << Val;
}

void TReal::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TReal::Touch() {
  assert(this);
}

Var::TVar &TReal::Index(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Index not supported on Real.");
}

TReal &TReal::Add(const TVar &rhs) {
  assert(this);
  Val += Var::TVar::TDt<double>::As(rhs);
  return *this;
}

TReal &TReal::And(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "And not supported on Real.");
}

TReal &TReal::Div(const TVar &rhs) {
  assert(this);
  Val /= Var::TVar::TDt<double>::As(rhs);
  return *this;
}

TReal &TReal::Exp(const TVar &rhs) {
  assert(this);
  Val = pow(Val, Var::TVar::TDt<double>::As(rhs));
  return *this;
}

TReal &TReal::Intersection(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Intersection not supported on Real.");
}

TReal &TReal::Mod(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mod not supported on Real.");
}

TReal &TReal::Mult(const TVar &rhs) {
  assert(this);
  Val *= Var::TVar::TDt<double>::As(rhs);
  return *this;
}

TReal &TReal::Or(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Or not supported on Real.");
}

TReal &TReal::Sub(const TVar &rhs) {
  assert(this);
  Val -= Var::TVar::TDt<double>::As(rhs);
  return *this;
}

TReal &TReal::SymmetricDiff(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "SymmetricDiff not supported on Real.");
}

TReal &TReal::Union(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Union not supported on Real.");
}

TReal &TReal::Xor(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Xor not supported on Real.");
}

TReal::TReal(double that) : Val(that) {}

TReal::~TReal() {}

TVar TReal::Copy() const {
  assert(this);
  return TReal::New(Val);
}

TVar TReal::New(double that) {
  return (new TReal(that))->AsVar();
}

TVar::TVar(const double &that) {
  *this = TReal::New(that);
}