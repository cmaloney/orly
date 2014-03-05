/* <stig/synth/addr_ctor.cc>

   Implements <stig/synth/addr_ctor.h>.

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

#include <stig/synth/addr_ctor.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/expr/addr.h>
#include <stig/synth/cst_utils.h>
#include <stig/synth/get_pos_range.h>

using namespace Stig;
using namespace Stig::Synth;

TAddrCtor::TAddrCtor(const TExprFactory *expr_factory, const Package::Syntax::TAddrCtor *addr_ctor)
    : AddrCtor(Base::AssertTrue(addr_ctor)) {
  assert(expr_factory);
  TAddrDir addr_dir;
  TAddrDirVisitor visitor(addr_dir);
  try {
    ForEach<Package::Syntax::TAddrMember>(AddrCtor->GetOptAddrMemberList(),
        [this, expr_factory, &visitor]
        (const Package::Syntax::TAddrMember *addr_member) -> bool {
          addr_member->GetOptOrdering()->Accept(visitor);  // Visit to extract direction
          Members.emplace_back(std::make_pair(visitor.GetAddrDir(),
                                              expr_factory->NewExpr(addr_member->GetExpr())));
          return true;
        });
  } catch (...) {
    Cleanup();
    throw;
  }
}

TAddrCtor::~TAddrCtor() {
  Cleanup();
}

Expr::TExpr::TPtr TAddrCtor::Build() const {
  assert(this);
  Expr::TAddr::TMemberVec members;
  for (auto &member : Members) {
    members.emplace_back(std::make_pair(member.first, (member.second)->Build()));
  }
  return Expr::TAddr::New(members, GetPosRange(AddrCtor));
}

void TAddrCtor::Cleanup() {
  assert(this);
  for (auto &member : Members) {
    delete member.second;
  }
}

void TAddrCtor::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  for (auto &member : Members) {
    (member.second)->ForEachInnerScope(cb);
  }
}

void TAddrCtor::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  for (auto &member : Members) {
    (member.second)->ForEachRef(cb);
  }
}
