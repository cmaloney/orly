/* <stig/synth/addr_type.cc>

   Implements <stig/synth/addr_type.h>.

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

#include <stig/synth/addr_type.h>

#include <stig/synth/cst_utils.h>
#include <stig/synth/new_type.h>

using namespace Stig;
using namespace Stig::Synth;

TAddrType::TAddrType(const Package::Syntax::TAddrType *addr_type) {
  assert(addr_type);
  TAddrDir addr_dir;
  TAddrDirVisitor visitor(addr_dir);
  try {
    ForEach<Package::Syntax::TAddrTypeMember>(addr_type->GetOptAddrTypeMemberList(),
        [this, &visitor](const Package::Syntax::TAddrTypeMember *addr_type_member) -> bool {
          addr_type_member->GetOptOrdering()->Accept(visitor);  // Visit to extract direction
          Members.emplace_back(std::make_pair(visitor.GetAddrDir(),
                                              NewType(addr_type_member->GetType())));
          return true;
        });
  } catch (...) {
    Cleanup();
    throw;
  }
}

TAddrType::~TAddrType() {
  Cleanup();
}

void TAddrType::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  for (auto &member : Members) {
    member.second->ForEachRef(cb);
  }
}

void TAddrType::Cleanup() {
  assert(this);
  for (auto &member : Members) {
    delete member.second;
  }
}

Type::TType TAddrType::ComputeSymbolicType() const {
  assert(this);
  Type::TAddr::TElems members;
  for (auto &member : Members) {
    members.emplace_back(std::make_pair(member.first, (member.second)->GetSymbolicType()));
  }
  return Type::TAddr::Get(members);
}
