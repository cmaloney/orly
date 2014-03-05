/* <stig/var/set.cc>

   Implements <stig/var/set.h>.

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

#include <stig/var/set.h>

#include <stig/type/set.h>

using namespace Stig;
using namespace Var;

size_t TSet::GetHash() const {
  assert(this);
  return Hash;
}

Type::TType TSet::GetType() const {
  assert(this);
  return Type::TSet::Get(Type);
}

void TSet::Write(std::ostream &strm) const {
  strm << "TSet<" << Type << ">{";
  for (auto iter = Val.begin(); iter != Val.end(); ++iter) {
    if (iter != Val.begin()) {
      strm << ", ";
    }
    strm << *iter;
  }
  strm << "}";
}

void TSet::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TSet::SetHash() {
  assert(this);
  Hash = 0;
  for (auto iter = Val.begin(); iter != Val.end(); ++iter) {
    Hash ^= iter->GetHash();
  }
}

void TSet::Touch() {
  assert(this);
  SetHash();
}

Var::TVar &TSet::Index(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Index not supported on Set.");
}

TSet &TSet::Add(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Add not supported on Set.");
}

TSet &TSet::And(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "And not supported on Set.");
}

TSet &TSet::Div(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Div not supported on Set.");
}

TSet &TSet::Exp(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Exp not supported on Set.");
}

TSet &TSet::Intersection(const TVar &rhs) {
  assert(this);
  Val = Val & Var::TVar::TDt<TSetType>::As(rhs);
  SetHash();
  return *this;
}

TSet &TSet::Mod(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mod not supported on Set.");
}

TSet &TSet::Mult(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mult not supported on Set.");
}

TSet &TSet::Or(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Or not supported on Set.");
}

TSet &TSet::Sub(const TVar &rhs) {
  assert(this);
  Val = Val - Var::TVar::TDt<TSetType>::As(rhs);
  SetHash();
  return *this;
}

TSet &TSet::SymmetricDiff(const TVar &rhs) {
  assert(this);
  Val = Val ^ Var::TVar::TDt<TSetType>::As(rhs);
  SetHash();
  return *this;
}

TSet &TSet::Union(const TVar &rhs) {
  assert(this);
  Val = Val | Var::TVar::TDt<TSetType>::As(rhs);
  SetHash();
  return *this;
}

TSet &TSet::Xor(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Xor not supported on Set.");
}

TSet::TSet(const Rt::TSet<TVar> &that, const Type::TType &type) : Val(that), Type(type) {
  assert(this);
  for (auto iter = Val.begin(); iter != Val.end(); ++iter) {
    if (iter->GetType() != Type) {
      throw Rt::TSystemError(HERE, "Set constructor requires homogenous element type.");
    }
  }
  SetHash();
}

TSet::~TSet() {}

TVar TSet::Copy() const {
  assert(this);
  TSetType copy_set;
  for (auto iter : Val) {
    copy_set.insert(iter.Copy());
  }
  return (new TSet(copy_set, Type))->AsVar();
}

TVar TVar::Set(const Rt::TSet<TVar> &that, const Type::TType &type) {
  return (new TSet(that, type))->AsVar();
}