/* <stig/var/opt.cc>

   Implements <stig/var/opt.h>.

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

#include <stig/var/opt.h>

#include <base/not_implemented_error.h>
#include <stig/type/opt.h>

using namespace Stig;
using namespace Stig::Var;

size_t TOpt::GetHash() const {
  assert(this);
  return Hash;
}

Type::TType TOpt::GetType() const {
  assert(this);
  return Type::TOpt::Get(Type);
}

Type::TType TOpt::GetInnerType() const {
  return Type;
}

void TOpt::Write(std::ostream &strm) const {
  strm << "Stig::Rt::TOpt<" << Type << ">(";
  if (Val.IsKnown()) {
    strm << Val.GetVal();
  }

  strm << ')';
}

void TOpt::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TOpt::SetHash() {
  assert(this);
  Hash = Val.IsKnown() ? Val.GetVal().GetHash() : 0;
}

void TOpt::Touch() {
  assert(this);
  SetHash();
}

Var::TVar &TOpt::Index(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Index not supported on Opt.");
}

TOpt &TOpt::Add(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Add not supported on Opt.");
}

TOpt &TOpt::And(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "And not supported on Opt.");
}

TOpt &TOpt::Div(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Div not supported on Opt.");
}

TOpt &TOpt::Exp(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Exp not supported on Opt.");
}

TOpt &TOpt::Intersection(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Intersection supported on Opt.");
}

TOpt &TOpt::Mod(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mod not supported on Opt.");
}

TOpt &TOpt::Mult(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mult not supported on Opt.");
}

TOpt &TOpt::Or(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Or not supported on Opt.");
}

TOpt &TOpt::Sub(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Sub not supported on Opt.");
}

TOpt &TOpt::SymmetricDiff(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "SymmetricDiff not supported on Opt.");
}

TOpt &TOpt::Union(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Union not supported on Opt.");
}

TOpt &TOpt::Xor(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Xor not supported on Opt.");
}

TOpt::TOpt(const Rt::TOpt<TVar> &that, const Type::TType &type) : Val(that), Type(type) {}

TOpt::~TOpt() {}

TVar TOpt::Copy() const {
  assert(this);
  if (Val.IsKnown()) {
    return (new TOpt(TOptType(Val.GetVal().Copy()), Type))->AsVar();
  } else {
    return (new TOpt(TOptType(), Type))->AsVar();
  }
}

TVar TVar::Opt(const Rt::TOpt<TVar> &that, const Type::TType &type) {
  return (new TOpt(that, type))->AsVar();
}