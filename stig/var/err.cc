/* <stig/var/err.cc>

   Implements <stig/var/err.h>.

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

#include <stig/var/err.h>

#include <base/not_implemented.h>
#include <stig/type/err.h>

using namespace Stig;
using namespace Stig::Var;

size_t TErr::GetHash() const {
  assert(this);
  return Hash;
}

Type::TType TErr::GetType() const {
  assert(this);
  return Type::TErr::Get(Type);
}

void TErr::Write(std::ostream &) const {
  NOT_IMPLEMENTED();
}

TErr::~TErr() {}

void TErr::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

TVar TErr::Copy() const {
  NOT_IMPLEMENTED();
}

void TErr::SetHash() {
  assert(this);
  Hash = Val.GetHash();
}

void TErr::Touch() {
  assert(this);
  SetHash();
}

Var::TVar &TErr::Index(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Index not supported on Err.");
}

TErr &TErr::Add(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Add not supported on Err.");
}

TErr &TErr::And(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "And not supported on Err.");
}

TErr &TErr::Div(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Div not supported on Err.");
}

TErr &TErr::Exp(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Exp not supported on Err.");
}

TErr &TErr::Intersection(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Intersection not supported on Err.");
}

TErr &TErr::Mod(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mod not supported on Err.");
}

TErr &TErr::Mult(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mult not supported on Err.");
}

TErr &TErr::Or(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Or not supported on Err.");
}

TErr &TErr::Sub(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Sub not supported on Err.");
}

TErr &TErr::SymmetricDiff(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "SymmetricDiff not supported on Err.");
}

TErr &TErr::Union(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Union not supported on Err.");
}

TErr &TErr::Xor(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Xor not supported on Err.");
}
