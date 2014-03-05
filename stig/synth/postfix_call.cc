/* <stig/synth/postfix_call.cc>

   Implements <stig/synth/postfix_call.h>.

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

#include <stig/synth/postfix_call.h>

#include <base/assert_true.h>
#include <stig/error.h>
#include <stig/expr/function_app.h>
#include <stig/pos_range.h>
#include <stig/synth/cst_utils.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Synth;

TPostfixCall::TPostfixCall(const TExprFactory *expr_factory, const Package::Syntax::TPostfixCall *postfix_call)
    : PostfixCall(Base::AssertTrue(postfix_call)) {
  class TOptCallArgsVisitor
      : public Package::Syntax::TOptCallArgs::TVisitor {
    NO_COPY_SEMANTICS(TOptCallArgsVisitor);
    public:
    TOptCallArgsVisitor(const TExprFactory *expr_factory, TArgMap &args)
        : Args(args), ExprFactory(expr_factory) {}
    virtual void operator()(const Package::Syntax::TExplicitCallArgs *that) const {
      ForEach<Package::Syntax::TObjMember>(that->GetObjMemberList(),
          [this](const Package::Syntax::TObjMember *obj_member) -> bool {
            auto name = TName(obj_member->GetName());
            auto result = Args.insert(std::make_pair(name, ExprFactory->NewExpr(obj_member->GetExpr())));
            if (!result.second) {
              Tools::Nycr::TError::TBuilder(name.GetPosRange())
                << "Duplicate argument name \"" << name.GetText() << '"';
              Tools::Nycr::TError::TBuilder((result.first)->first.GetPosRange())
                << "  first specified here";
            }
            return true;
          });
    }
    virtual void operator()(const Package::Syntax::TNoCallArgs *) const { /* DO NOTHING */ }
    virtual void operator()(const Package::Syntax::TUnrolledCallArgs *that) const {
      throw TNotImplementedError(HERE, GetPosRange(that->GetStar(), that->GetExpr()),
          "Unrolled call arguments are not yet supported");
    }
    private:
    TArgMap &Args;
    const TExprFactory *ExprFactory;
  };  // TOptCallArgsVisitor
  assert(expr_factory);
  try {
    Expr = expr_factory->NewExpr(PostfixCall->GetExpr());
    PostfixCall->GetOptCallArgs()->Accept(TOptCallArgsVisitor(expr_factory, Args));
  } catch (...) {
    Cleanup();
    throw;
  }
}

TPostfixCall::~TPostfixCall() {
  Cleanup();
}

Expr::TExpr::TPtr TPostfixCall::Build() const {
  assert(this);
  Expr::TFunctionApp::TFunctionAppArgMap args;
  for (auto arg : Args) {
    auto result = args.insert(make_pair((arg.first).GetText(), Expr::TFunctionAppArg::New((arg.second)->Build())));
    assert(result.second);
  }
  return Expr::TFunctionApp::New(Expr->Build(), args, GetPosRange(PostfixCall));
}

void TPostfixCall::Cleanup() {
  assert(this);
  delete Expr;
  for (auto arg : Args) {
    delete arg.second;
  }
}

void TPostfixCall::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachInnerScope(cb);
  for (auto arg : Args) {
    (arg.second)->ForEachInnerScope(cb);
  }
}

void TPostfixCall::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachRef(cb);
  for (auto arg : Args) {
    (arg.second)->ForEachRef(cb);
  }
}
