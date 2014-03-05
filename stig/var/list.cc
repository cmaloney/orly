/* <stig/var/list.cc>

   Implements <stig/var/list.h>.

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

#include <stig/var/list.h>

#include <stig/type/list.h>
#include <stig/type/stigify.h>
#include <stig/var/int.h>

using namespace Stig;
using namespace Var;

size_t TList::GetHash() const {
  assert(this);
  return Hash;
}

Type::TType TList::GetType() const {
  assert(this);
  return Type::TList::Get(Type);
}

void TList::Append(const TListType &other) {
  Val.insert(Val.end(), other.begin(), other.end());
  SetHash();
}

void TList::Write(std::ostream &strm) const {
  strm << "std::vector<" << Type << ">{";
  for (auto iter = Val.begin(); iter != Val.end(); ++iter) {
    if (iter != Val.begin()) {
      strm << ", ";
    }
    strm << *iter;
  }
  strm << "}";
}

void TList::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TList::SetHash() {
  assert(this);
  Hash = 0;
  size_t rotate = 0;
  for (auto iter = Val.begin(); iter != Val.end(); ++iter, rotate += 5) {
    Hash ^= Base::RotatedLeft(iter->GetHash(), rotate);
  }
}

void TList::Touch() {
  assert(this);
  SetHash();
}

Var::TVar &TList::Index(const TVar &key) {
  assert(this);
  int64_t idx = TVar::TDt<int64_t>::As(key);
  if (idx < 0 || idx >= static_cast<int64_t>(Val.size())) {
    throw Rt::TSystemError(HERE, "Dynamic List index out of bounds.");
  }
  return Val[idx];
}

TList &TList::Add(const TVar &rhs) {
  assert(this);
  TListType other = Var::TVar::TDt<TListType>::As(rhs);
  Val.insert(Val.end(), other.begin(), other.end());
  return *this;
}

TList &TList::And(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "And not supported on list.");
}

TList &TList::Div(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Div not supported on list.");
}

TList &TList::Exp(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Exp not supported on list.");
}

TList &TList::Intersection(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Intersection not supported on list.");
}

TList &TList::Mod(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mod not supported on list.");
}

TList &TList::Mult(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mult not supported on list.");
}

TList &TList::Or(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Or not supported on list.");
}

TList &TList::Sub(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Sub not supported on list.");
}

TList &TList::SymmetricDiff(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "SymmetricDiff not supported on list.");
}

TList &TList::Union(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Union not supported on list.");
}

TList &TList::Xor(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Xor not supported on list.");
}


TList::TList(const std::vector<TVar> &that, const Type::TType &type) : Val(that), Type(type) {
  assert(this);
  for (auto iter = Val.begin(); iter != Val.end(); ++iter) {
    if (iter->GetType() != Type) {
      std::ostringstream oss;
      oss << "List constructor requires homogenous element type. List type '";
      Stig::Type::Stigify(oss, Type);
      oss << "' got element of type '";
      Stig::Type::Stigify(oss, iter->GetType());
      oss << "'.";
      throw Rt::TSystemError(HERE, oss.str().c_str());
    }
  }
  SetHash();
}

TList::~TList() {}

TVar TList::Copy() const {
  assert(this);
  TListType copy_list;
  for (auto iter : Val) {
    copy_list.push_back(iter.Copy());
  }
  return (new TList(copy_list, Type))->AsVar();
}

TVar TVar::List(const std::vector<TVar> &that, const Type::TType &type) {
  return (new TList(that, type))->AsVar();
}
