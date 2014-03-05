/* <stig/var/addr.cc>

   Implements <stig/var/addr.h>.

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

#include <stig/var/addr.h>

#include <base/split.h>
#include <stig/type/addr.h>
#include <stig/var/int.h>

using namespace Stig;
using namespace Var;

size_t TAddr::GetHash() const {
  assert(this);
  return Hash;
}

Type::TType TAddr::GetType() const {
  assert(this);
  return Type::TAddr::Get(TypeVec);
}

void TAddr::Write(std::ostream &strm) const {
  assert(this);
  strm << GetType() << '(';
  Base::Join(", ", Val, [](const std::pair<TAddrDir, Var::TVar> &elem, std::ostream &strm) {
    strm << elem.second;
  }, strm);
  strm << ')';
}

void TAddr::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

void TAddr::SetHash() {
  assert(this);
  Hash = 0;
  size_t rotate = 0;
  //TODO: Really need to normalize the TAddr hash functions...
  for (auto iter = Val.begin(); iter != Val.end(); ++iter, rotate += 5) {
    Hash ^= Base::RotatedLeft(iter->second.GetHash(), rotate + ToInt(iter->first));
  }
}

void TAddr::Touch() {
  assert(this);
  SetHash();
}

Var::TVar &TAddr::Index(const TVar &key) {
  assert(this);
  int64_t idx = TVar::TDt<int64_t>::As(key);
  if (idx < 0 || idx >= static_cast<int64_t>(Val.size())) {
    throw Rt::TSystemError(HERE, "Dynamic addr index out of bounds.");
  }
  return Val[idx].second;
}

TAddr &TAddr::Add(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Add not supported on Addr.");
}

TAddr &TAddr::And(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "And not supported on Addr.");
}

TAddr &TAddr::Div(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Div not supported on Addr.");
}

TAddr &TAddr::Exp(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Exp not supported on Addr.");
}

TAddr &TAddr::Intersection(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Intersection not supported on Addr.");
}

TAddr &TAddr::Mod(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mod not supported on Addr.");
}

TAddr &TAddr::Mult(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Mult not supported on Addr.");
}

TAddr &TAddr::Or(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Or not supported on Addr.");
}

TAddr &TAddr::Sub(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Sub not supported on Addr.");
}

TAddr &TAddr::SymmetricDiff(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "SymmetricDiff not supported on Addr.");
}

TAddr &TAddr::Union(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Union not supported on Addr.");
}

TAddr &TAddr::Xor(const TVar &) {
  assert(this);
  throw Rt::TSystemError(HERE, "Xor not supported on Addr.");
}

TAddr::TAddr(const TAddrType &that) {
  assert(this);
  for (auto iter = that.begin(); iter != that.end(); ++iter) {
    Val.push_back(*iter);
    TypeVec.push_back(std::make_pair(iter->first, iter->second.GetType()));
  }
  SetHash();
}

TAddr::~TAddr() {}

TVar TAddr::Copy() const {
  assert(this);
  TAddrType copy_vec;
  for (auto &iter : Val) {
    copy_vec.push_back(std::make_pair(iter.first, iter.second.Copy()));
  }
  return (new TAddr(copy_vec))->AsVar();
}

TVar TVar::Addr(const TAddr::TAddrType &that) {
  return (new TAddr(that))->AsVar();
}
