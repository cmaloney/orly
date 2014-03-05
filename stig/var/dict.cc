/* <stig/var/dict.cc>

   Implements <stig/var/dict.h>.

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

#include <stig/var/dict.h>

#include <stig/type/dict.h>
#include <stig/var/set.h>

using namespace Stig;
using namespace Var;

void TDict::Insert(const TDictType::const_iterator &begin, const TDictType::const_iterator &end) {
  assert(this);
  Val.insert(begin, end);
  SetHash();
}

void TDict::Remove(const Rt::TSet<TVar> &keys) {
  assert(this);
  for (auto iter : keys) {
    Val.erase(iter);
  }
  SetHash();
}

size_t TDict::GetHash() const {
  assert(this);
  return Hash;
}

Type::TType TDict::GetType() const {
  assert(this);
  return Type::TDict::Get(KeyType, ValType);
}

void TDict::Write(std::ostream &strm) const {
  strm << "TDict<" << KeyType << ", " << ValType << ">{";
  for (auto iter = Val.begin(); iter != Val.end(); ++iter) {
    if (iter != Val.begin()) {
      strm << ", ";
    }
    strm << "{" << iter->first << ", " << iter->second << "}";
  }
  strm << "}";
}

void TDict::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TDict::SetHash() {
  assert(this);
  Hash = 0;
  for (auto iter = Val.begin(); iter != Val.end(); ++iter) {
    Hash ^= iter->first.GetHash();
    Hash ^= Base::RotatedLeft(iter->second.GetHash(), 5);
  }
}

void TDict::Touch() {
  assert(this);
  SetHash();
}

Var::TVar &TDict::Index(const TVar &key) {
  assert(this);
  auto pos = Val.find(key);
  if (pos == Val.end()) {
    throw Rt::TSystemError(HERE, "Dynamic Dict index key does not exist.");
  }
  return pos->second;
}

TDict &TDict::Add(const TVar &rhs) {
  assert(this);
  TDictType other = Var::TVar::TDt<TDictType>::As(rhs);
  for (auto iter : other) {
    auto ret = Val.insert(iter);
    if (!ret.second) {
      ret.first->second = iter.second;
    }
  }
  return *this;
}

TDict &TDict::And(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "And not supported on Dict.");
}

TDict &TDict::Div(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Div not supported on Dict.");
}

TDict &TDict::Exp(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Exp not supported on Dict.");
}

TDict &TDict::Intersection(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Intersection not supported on Dict.");
}

TDict &TDict::Mod(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mod not supported on Dict.");
}

TDict &TDict::Mult(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mult not supported on Dict.");
}

TDict &TDict::Or(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Or not supported on Dict.");
}

TDict &TDict::Sub(const TVar &rhs) {
  assert(this);
  TSet::TSetType other = Var::TVar::TDt<TSet::TSetType>::As(rhs);
  for (auto iter : other) {
    Val.erase(iter);
  }
  return *this;
}

TDict &TDict::SymmetricDiff(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "SymmetricDiff not supported on Dict.");
}

TDict &TDict::Union(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Union not supported on Dict.");
}

TDict &TDict::Xor(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Xor not supported on Dict.");
}

TDict::TDict(const Rt::TDict<TVar, TVar> &that, const Type::TType &key_type, const Type::TType &val_type) : Val(that), KeyType(key_type), ValType(val_type) {
  assert(this);
  for (auto iter = Val.begin(); iter != Val.end(); ++iter) {
    if (iter->first.GetType() != KeyType) {
      throw Rt::TSystemError(HERE, "Dictionary constructor requires homogenous key type.");
    }
    if (iter->second.GetType() != ValType) {
      throw Rt::TSystemError(HERE, "Dictionary constructor requires homogenous val type.");
    }
  }
  SetHash();
}

TDict::~TDict() {}

TVar TDict::Copy() const {
  assert(this);
  TDictType copy_dict;
  for (auto iter : Val) {
    copy_dict.insert(std::make_pair(iter.first.Copy(), iter.second.Copy()));
  }
  return (new TDict(copy_dict, KeyType, ValType))->AsVar();
}

TVar TVar::Dict(const Rt::TDict<TVar, TVar> &that, const Type::TType &key_type, const Type::TType &val_type) {
  return (new TDict(that, key_type, val_type))->AsVar();
}