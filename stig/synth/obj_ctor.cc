/* <stig/synth/obj_ctor.cc>

   Implements <stig/synth/obj_ctor.h>.

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

#include <stig/synth/obj_ctor.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/pos_range.h>
#include <stig/synth/cst_utils.h>
#include <stig/synth/get_pos_range.h>
#include <stig/expr/obj.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Synth;

TObjCtor::TObjCtor(const TExprFactory *expr_factory, const Package::Syntax::TObjCtor *obj_ctor)
    : ObjCtor(Base::AssertTrue(obj_ctor)) {
  assert(expr_factory);
  try {
    ForEach<Package::Syntax::TObjMember>(ObjCtor->GetOptObjMemberList(),
        [this, expr_factory](const Package::Syntax::TObjMember *obj_member) -> bool {
          auto name = TName(obj_member->GetName());
          auto result = Members.insert(std::make_pair(name, expr_factory->NewExpr(obj_member->GetExpr())));
          if (!result.second) {
            Tools::Nycr::TError::TBuilder(name.GetPosRange())
                << "duplicate object member name \"" << name.GetText() << '"';
            Tools::Nycr::TError::TBuilder((result.first)->first.GetPosRange())
                << "  first specified here";
          }
          return true;
        });
  } catch (...) {
    Cleanup();
    throw;
  }
}

TObjCtor::~TObjCtor() {
  Cleanup();
}

Expr::TExpr::TPtr TObjCtor::Build() const {
  assert(this);
  Expr::TObj::TMemberMap members;
  for (auto member : Members) {
    auto result = members.insert(std::make_pair((member.first).GetText(), (member.second)->Build()));
    assert(result.second);
  }
  return Expr::TObj::New(members, GetPosRange(ObjCtor));
}

void TObjCtor::Cleanup() {
  assert(this);
  for (auto member : Members) {
    delete member.second;
  }
}

void TObjCtor::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  for (auto member : Members) {
    (member.second)->ForEachInnerScope(cb);
  }
}

void TObjCtor::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  for (auto member : Members) {
    (member.second)->ForEachRef(cb);
  }
}
