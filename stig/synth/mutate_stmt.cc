/* <stig/synth/mutate_stmt.cc>

   Implements <stig/synth/mutate_stmt.h>.

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

#include <stig/synth/mutate_stmt.h>

#include <cassert>

#include <base/assert_true.h>
#include <stig/symbol/stmt/mutate.h>
#include <stig/symbol/stmt/stmt_arg.h>
#include <stig/synth/expr.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>

using namespace Stig;
using namespace Stig::Synth;

TMutateStmt::TMutateStmt(const TExprFactory *expr_factory, const Package::Syntax::TMutateStmt *mutate_stmt)
    : MutateStmt(Base::AssertTrue(mutate_stmt)), Lhs(nullptr), Rhs(nullptr) {
  assert(expr_factory);
  try {
    Lhs = expr_factory->NewExpr(MutateStmt->GetLhs());
    Rhs = expr_factory->NewExpr(MutateStmt->GetRhs());
  } catch (...) {
    delete Lhs;
    delete Rhs;
    throw;
  }
}

TMutateStmt::~TMutateStmt() {
  assert(this);
  delete Lhs;
  delete Rhs;
}

Symbol::Stmt::TStmt::TPtr TMutateStmt::Build() const {
  assert(this);
  class TMutationVisitor
      : public Package::Syntax::TMutation::TVisitor {
    public:
    TMutationVisitor(TMutator &mutator)
        : Mutator(mutator) {}
    virtual void operator()(const Package::Syntax::TMutationAssign     *) const { Mutator = TMutator::Assign;        }
    virtual void operator()(const Package::Syntax::TMutationBitwiseAnd *) const { Mutator = TMutator::Intersection;  }
    virtual void operator()(const Package::Syntax::TMutationBitwiseOr  *) const { Mutator = TMutator::Union;         }
    virtual void operator()(const Package::Syntax::TMutationBitwiseXor *) const { Mutator = TMutator::SymmetricDiff; }
    virtual void operator()(const Package::Syntax::TMutationDiv        *) const { Mutator = TMutator::Div;           }
    virtual void operator()(const Package::Syntax::TMutationExp        *) const { Mutator = TMutator::Exp;           }
    virtual void operator()(const Package::Syntax::TMutationLogicalAnd *) const { Mutator = TMutator::And;           }
    virtual void operator()(const Package::Syntax::TMutationLogicalOr  *) const { Mutator = TMutator::Or;            }
    virtual void operator()(const Package::Syntax::TMutationLogicalXor *) const { Mutator = TMutator::Xor;           }
    virtual void operator()(const Package::Syntax::TMutationMinus      *) const { Mutator = TMutator::Sub;           }
    virtual void operator()(const Package::Syntax::TMutationMod        *) const { Mutator = TMutator::Mod;           }
    virtual void operator()(const Package::Syntax::TMutationMul        *) const { Mutator = TMutator::Mult;          }
    virtual void operator()(const Package::Syntax::TMutationPlus       *) const { Mutator = TMutator::Add;           }
    private:
    TMutator &Mutator;
  };  // TMutationVisitor
  TMutator mutator;
  MutateStmt->GetMutation()->Accept(TMutationVisitor(mutator));
  return Symbol::Stmt::TMutate::New(
           Symbol::Stmt::TStmtArg::New(Lhs->Build()),
           mutator,
           Symbol::Stmt::TStmtArg::New(Rhs->Build()),
           GetPosRange(MutateStmt));
}

void TMutateStmt::ForEachRef(const std::function<void (TAnyRef &)> &cb) const {
  assert(this);
  assert(&cb);
  assert(cb);
  Lhs->ForEachRef(cb);
  Rhs->ForEachRef(cb);
}

void TMutateStmt::ForEachInnerScope(const std::function<void (TScope *)> &cb) const {
  assert(this);
  assert(&cb);
  assert(cb);
  Lhs->ForEachInnerScope(cb);
  Rhs->ForEachInnerScope(cb);
}