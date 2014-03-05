/* <stig/synth/db_keys_expr.cc>

   Implements <stig/synth/db_keys_expr.h>.

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

#include <stig/synth/db_keys_expr.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/expr/free.h>
#include <stig/expr/keys.h>
#include <stig/pos_range.h>
#include <stig/synth/addr_dir_visitor.h>
#include <stig/synth/cst_utils.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_type.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Synth;

TDbKeysExpr::TMember::TMember(TAddrDir addr_dir)
    : AddrDir(addr_dir) {}

TDbKeysExpr::TMember::~TMember() {}

TDbKeysExpr::TFixedMember::TFixedMember(TAddrDir addr_dir, TExpr *expr)
    : TMember(addr_dir), Expr(Base::AssertTrue(expr)) {}

TDbKeysExpr::TFixedMember::~TFixedMember() {
  assert(this);
  delete Expr;
}

Expr::TExpr::TPtr TDbKeysExpr::TFixedMember::Build() const {
  assert(this);
  return Expr->Build();
}

void TDbKeysExpr::TFixedMember::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachInnerScope(cb);
}

void TDbKeysExpr::TFixedMember::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachRef(cb);
}

TDbKeysExpr::TFreeMember::TFreeMember(TAddrDir addr_dir, TType *type, const TPosRange &pos_range)
    : TMember(addr_dir), Type(Base::AssertTrue(type)), PosRange(pos_range) {}

TDbKeysExpr::TFreeMember::~TFreeMember() {
  assert(this);
  delete Type;
}

Expr::TExpr::TPtr TDbKeysExpr::TFreeMember::Build() const {
  assert(this);
  return Expr::TFree::New(Type->GetSymbolicType(), PosRange, GetAddrDir());
}

void TDbKeysExpr::TFreeMember::ForEachInnerScope(const std::function<void (TScope *)> &) {}

void TDbKeysExpr::TFreeMember::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Type->ForEachRef(cb);
}

TDbKeysExpr::TDbKeysExpr(const TExprFactory *expr_factory, const Package::Syntax::TDbKeysExpr *db_keys_expr)
    : DbKeysExpr(Base::AssertTrue(db_keys_expr)), ValueType(NewType(DbKeysExpr->GetType())) {
  class TDbKeysMemberVisitor
      : public Package::Syntax::TDbKeysMember::TVisitor {
    NO_COPY_SEMANTICS(TDbKeysMemberVisitor);
    public:
    TDbKeysMemberVisitor(
        const TAddrDirVisitor *addr_dir_visitor,
        const TExprFactory *expr_factory,
        bool &is_valid,
        TMemberVec &members)
          : AddrDirVisitor(addr_dir_visitor),
            ExprFactory(expr_factory),
            IsValid(is_valid),
            Members(members) {}
    virtual void operator()(const Package::Syntax::TFixedDbKeysMember *that) const {
      that->GetOptOrdering()->Accept(*AddrDirVisitor);
      if (!Members.empty() && dynamic_cast<TFreeMember *>(Members.back())) {
        /* NOTE: TFixedMember must not have a TFreeMember to its left.
                 This restriction exists because of prefix-hashing on addresses. */
        IsValid = false;
      }
      AddMember(new TFixedMember(AddrDirVisitor->GetAddrDir(), ExprFactory->NewExpr(that->GetExpr())));
    }
    virtual void operator()(const Package::Syntax::TFreeDbKeysMember *that) const {
      that->GetOptOrdering()->Accept(*AddrDirVisitor);
      AddMember(new TFreeMember(AddrDirVisitor->GetAddrDir(), NewType(that->GetType()), GetPosRange(that)));
    }
    private:
    void AddMember(TMember *member) const {
      try {
        Members.push_back(member);
      } catch (...) {
        delete member;
        throw;
      }
    }
    const TAddrDirVisitor *AddrDirVisitor;
    const TExprFactory *ExprFactory;
    bool &IsValid;
    TMemberVec &Members;
  };  // TDbKeysMemberVisitor
  assert(expr_factory);
  TAddrDir addr_dir;
  TAddrDirVisitor addr_dir_visitor(addr_dir);
  bool is_valid = true;
  TDbKeysMemberVisitor visitor(&addr_dir_visitor, expr_factory, is_valid, Members);
  try {
    ForEach<Package::Syntax::TDbKeysMember>(DbKeysExpr->GetOptDbKeysMemberList(),
        [&is_valid, &visitor](const Package::Syntax::TDbKeysMember *db_keys_member) -> bool {
          db_keys_member->Accept(visitor);
          return is_valid; // If there is a fixed member to the right of free member, stop.
        });
    if (!is_valid) {
      Tools::Nycr::TError::TBuilder(GetPosRange(DbKeysExpr))
          << "all free expressions must appear to the right of fixed expressions";
    }
  } catch (...) {
    Cleanup();
    throw;
  }
}

TDbKeysExpr::~TDbKeysExpr() {
  Cleanup();
}

Expr::TExpr::TPtr TDbKeysExpr::Build() const {
  assert(this);
  Expr::TAddr::TMemberVec members;
  for (auto &member : Members) {
    members.emplace_back(std::make_pair(member->GetAddrDir(), member->Build()));
  }
  return Expr::TKeys::New(members, ValueType->GetSymbolicType(), GetPosRange(DbKeysExpr));
}

void TDbKeysExpr::Cleanup() {
  assert(this);
  for (auto member: Members) {
    delete member;
  }
}

void TDbKeysExpr::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  for (auto member : Members) {
    member->ForEachInnerScope(cb);
  }
}

void TDbKeysExpr::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  for (auto member : Members) {
    member->ForEachRef(cb);
  }
  ValueType->ForEachRef(cb);
}
