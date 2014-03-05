/* <stig/var/mutable.cc>

   Implements <stig/var/mutable.h>.

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

#include <stig/var/mutable.h>

#include <base/not_implemented_error.h>
#include <stig/type/mutable.h>

using namespace Stig;
using namespace Stig::Var;

size_t TMutable::GetHash() const {
  assert(this);
  return Hash;
}

Type::TType TMutable::GetType() const {
  assert(this);
  return Type;
}

void TMutable::Write(std::ostream &) const {
  throw Base::TNotImplementedError(HERE);
}

void TMutable::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TMutable::SetHash() {
  assert(this);
  //TODO: Make better hash
  Hash = Val.GetHash() ^ Addr.GetHash();
}

void TMutable::Touch() {
  assert(this);
  SetHash();
}

Var::TVar &TMutable::Index(const TVar &that) {
  assert(this);

  return Val.Index(that);
}

//TODO: We really should actually support all of these... but that requires a large refactor fo all of them...
TVar::TImpl &TMutable::Add(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Add not currently implemented on Mutable.");
}

TVar::TImpl &TMutable::And(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "And not currently implemented on Mutable.");
}

TVar::TImpl &TMutable::Div(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Div not currently implemented on Mutable.");
}

TVar::TImpl &TMutable::Exp(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Exp not currently implemented on Mutable.");
}

TVar::TImpl &TMutable::Intersection(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Intersection not currently implemented on Mutable.");
}

TVar::TImpl &TMutable::Mod(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mod not currently implemented on Mutable.");
}

TVar::TImpl &TMutable::Mult(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mul not currently implemented on Mutable.");
}

TVar::TImpl &TMutable::Or(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Or not currently implemented on Mutable.");
}

TVar::TImpl &TMutable::Sub(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Sub not currently implemented on Mutable.");
}

TVar::TImpl &TMutable::SymmetricDiff(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "SymmetricDiff not currently implemented on Mutable.");
}

TVar::TImpl &TMutable::Union(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Union not currently implemented on Mutable.");
}

TVar::TImpl &TMutable::Xor(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Xor not currently implemented on Mutable.");
}

TMutable::TMutable(const Var::TVar &addr, const Var::TVar &val)
    : Addr(addr), Val(val), Type(Type::TMutable::Get(addr.GetType(), val.GetType())) {
  SetHash();
}

//NOTE: Here so that packages don't get the definition.
TMutable::~TMutable() {}

TVar TMutable::Copy() const {
  assert(this);
  return (new TMutable(Addr.Copy(), Val.Copy()))->AsVar();
}