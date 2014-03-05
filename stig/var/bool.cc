/* <stig/var/bool.cc>

   Implements <stig/var/bool.h>.

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

#include <stig/var/bool.h>

#include <stig/type/bool.h>
#include <stig/var/dynamic_cast.h>

using namespace Stig;
using namespace Stig::Var;

size_t TBool::GetHash() const {
  assert(this);
  return std::hash<bool>()(Val);
}

Type::TType TBool::GetType() const {
  assert(this);
  return Type::TBool::Get();
}

void TBool::Write(std::ostream &stream) const {
  assert(this);
  stream << (Val ? "true" : "false");
}

void TBool::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TBool::Touch() {
  assert(this);
}

Var::TVar &TBool::Index(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Index not supported on Bool.");
}

TBool &TBool::Add(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Add not supported on Bool.");
}

TBool &TBool::And(const TVar &rhs) {
  assert(this);
  Val = static_cast<bool>(Val && DynamicCast<bool>(rhs));
  return *this;
}

TBool &TBool::Div(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Div not supported on Bool.");
}

TBool &TBool::Exp(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Exp not supported on Bool.");
}

TBool &TBool::Intersection(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Intersection not supported on Bool.");
}

TBool &TBool::Mod(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mod not supported on Bool.");
}

TBool &TBool::Mult(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mul not supported on Bool.");
}

TBool &TBool::Or(const TVar &rhs) {
  assert(this);
  Val = static_cast<bool>(Val || DynamicCast<bool>(rhs));
  return *this;
}

TBool &TBool::Sub(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Sub not supported on Bool.");
}

TBool &TBool::SymmetricDiff(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "SymmetricDiff not supported on Bool.");
}

TBool &TBool::Union(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Union not supported on Bool.");
}

TBool &TBool::Xor(const TVar &rhs) {
  assert(this);
  Val = static_cast<bool>(Val ^ DynamicCast<bool>(rhs));
  return *this;
}

TBool::TBool(bool that) : Val(that) {}

TBool::~TBool() {}

TVar TBool::Copy() const {
  assert(this);
  return TBool::New(Val);
}

TVar TBool::New(bool that) {
  return (new TBool(that))->AsVar();
}

TVar::TVar(const bool &that) {
  *this = TBool::New(that);
}