/* <tools/nycr/symbol/compound.cc>

   Implements <tools/nycr/symbol/compound.h>.

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

#include <tools/nycr/symbol/compound.h>

#include <algorithm>

#include <base/thrower.h>

using namespace std;
using namespace Tools::Nycr;
using namespace Tools::Nycr::Symbol;

TCompound::TMember::~TMember() {
  assert(this);
  assert(!Compound);
}

void TCompound::TMember::SetCompound(TCompound *compound) {
  assert(this);
  if (Compound) {
    Compound->OnPart(this);
  }
  Compound = compound;
  if (compound) {
    try {
      compound->OnJoin(this);
    } catch (...) {
      Compound = 0;
      throw;
    }
  }
}

void TCompound::OnJoin(TMember *member) {
  assert(this);
  assert(member);
  pair<TName, TMember *> item(member->GetName(), member);
  auto result = MembersByName.insert(item);
  if (!result.second) {
    THROW
        << "duplicate member name \"" << TLower(member->GetName())
        << "\" in compound \"" << TLower(GetName()) << '"';
  }
  try {
    MembersInOrder.push_back(member);
  } catch (...) {
    MembersByName.erase(item.first);
    throw;
  }
}

void TCompound::OnPart(TMember *member) {
  assert(this);
  assert(member);
  MembersByName.erase(member->GetName());
  auto iter = find(MembersInOrder.begin(), MembersInOrder.end(), member);
  if (iter != MembersInOrder.end()) {
    MembersInOrder.erase(iter);
  }
}
