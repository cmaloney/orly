/* <stig/var/obj.cc>

   Implements <stig/var/obj.h>.

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

#include <stig/var/obj.h>

#include <stig/var/str.h>
#include <stig/type/obj.h>
#include <base/not_implemented_error.h>

using namespace std;
using namespace Stig;
using namespace Stig::Var;

/* TODO */
size_t TObj::GetHash() const {
  assert(this);
  return Hash;
}

/* TODO */
Type::TType TObj::GetType() const {
  assert(this);
  return Type::TObj::Get(TypeMap);
}

void TObj::Write(std::ostream &) const {
  throw Base::TNotImplementedError(HERE);
}

const TVar TObj::DefaultVar;

void TObj::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TObj::SetHash() {
  assert(this);
  Hash = 0;
  for (auto iter = FieldsByName.begin(); iter != FieldsByName.end(); ++iter) {
    Hash ^= std::hash<std::string>()(iter->first);
    Hash ^= Base::RotatedLeft(iter->second.GetHash(), 5);
  }
}

void TObj::Touch() {
  assert(this);
  SetHash();
}

Var::TVar &TObj::Index(const TVar &key) {
  assert(this);
  std::string name = TVar::TDt<std::string>::As(key);
  auto pos = FieldsByName.find(name);
  if (pos == FieldsByName.end()) {
    throw Rt::TSystemError(HERE, "Dynamic Object index name does not exist.");
  }
  return pos->second;
}

TObj &TObj::Add(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Add not supported on Obj.");
}

TObj &TObj::And(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "And not supported on Obj.");
}

TObj &TObj::Div(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Div not supported on Obj.");
}

TObj &TObj::Exp(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Exp not supported on Obj.");
}

TObj &TObj::Intersection(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Intersection not supported on Obj.");
}

TObj &TObj::Mod(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mod not supported on Obj.");
}

TObj &TObj::Mult(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mul not supported on Obj.");
}

TObj &TObj::Or(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Or not supported on Obj.");
}

TObj &TObj::Sub(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Sub not supported on Obj.");
}

TObj &TObj::SymmetricDiff(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "SymmetricDiff not supported on Obj.");
}

TObj &TObj::Union(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Union not supported on Obj.");
}

TObj &TObj::Xor(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Xor not supported on Obj.");
}

TObj::TObj(const std::unordered_map<std::string, TVar> &that) : FieldsByName(that) {
  assert(this);
  for (auto iter = FieldsByName.begin(); iter != FieldsByName.end(); ++iter) {
    TypeMap[iter->first] = iter->second.GetType();
  }
  SetHash();
}

TObj::~TObj() {}

TVar TObj::Copy() const {
  assert(this);
  TFieldsByName copy_obj;
  for (auto iter : FieldsByName) {
    copy_obj.insert(std::make_pair(iter.first, iter.second.Copy()));
  }
  return (new TObj(copy_obj))->AsVar();
}

TVar TVar::Obj(const std::unordered_map<std::string, TVar> &that) {
  return (new TObj(that))->AsVar();
}