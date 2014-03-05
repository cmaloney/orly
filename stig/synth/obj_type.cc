/* <stig/synth/obj_type.cc>

   Implements <stig/synth/obj_type.h>.

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

#include <stig/synth/obj_type.h>

#include <stig/synth/cst_utils.h>
#include <stig/synth/new_type.h>

using namespace Stig;
using namespace Stig::Synth;

TObjType::TObjType(const Package::Syntax::TObjType *obj_type) {
  assert(obj_type);
  try {
  ForEach<Package::Syntax::TObjTypeMember>(obj_type->GetOptObjTypeMemberList(),
      [this](const Package::Syntax::TObjTypeMember *obj_type_member) -> bool {
        auto result = Members.emplace(std::make_pair(TName(obj_type_member->GetName()).GetText(),
                                                     NewType(obj_type_member->GetType())));
        assert(result.second);
        return true;
      });
  } catch (...) {
    Cleanup();
    throw;
  }
}

TObjType::~TObjType() {
  Cleanup();
}

void TObjType::Cleanup() {
  assert(this);
  for (auto &member : Members) {
    delete member.second;
  }
}

void TObjType::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  for (auto &member : Members) {
    (member.second)->ForEachRef(cb);
  }
}

Type::TType TObjType::ComputeSymbolicType() const {
  assert(this);
  Type::TObj::TElems elems;
  for (auto &member : Members) {
    auto result = elems.insert(std::make_pair(member.first, (member.second)->GetSymbolicType()));
    assert(result.second);
  }
  return Type::TObj::Get(elems);
}
