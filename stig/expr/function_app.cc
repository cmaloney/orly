/* <stig/expr/function_app.cc>

   Implements <stig/expr/function_app.h>

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

#include <stig/expr/function_app.h>

#include <stig/error.h>
#include <stig/expr/ref.h>
#include <stig/expr/util.h>
#include <stig/type/unwrap.h>
#include <stig/type/unwrap_visitor.h>
#include <stig/symbol/function.h>
#include <stig/symbol/result_def.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Expr;

TFunctionAppArg::TPtr TFunctionAppArg::New(const TExpr::TPtr &expr) {
  return TFunctionAppArg::TPtr(new TFunctionAppArg(expr));
}

TFunctionAppArg::TFunctionAppArg(const TExpr::TPtr &expr)
    : Symbol::TRoot(expr), FunctionApp(nullptr) {}

TFunctionApp *TFunctionAppArg::GetFunctionApp() const {
  assert(this);
  assert(FunctionApp);
  return FunctionApp;
}

void TFunctionAppArg::SetFunctionApp(TFunctionApp *function_app) {
  assert(this);
  assert(function_app);
  assert(!FunctionApp);
  FunctionApp = function_app;
}

void TFunctionAppArg::UnsetFunctionApp(TFunctionApp *function_app) {
  assert(this);
  assert(function_app);
  assert(FunctionApp == function_app);
  FunctionApp = nullptr;
}

Symbol::TAnyFunction::TPtr TFunctionApp::GetFunction() const {
  assert(this);
  auto ref = GetExpr()->TryAs<TRef>();
  Symbol::TAnyFunction::TPtr function = nullptr;
  if (ref) {
    auto result_def = dynamic_cast<const Symbol::TResultDef *>(ref->GetDef());
    if (!result_def) {
      throw TExprError(HERE, GetPosRange(), "Ths lhs of the function call must be a callable function expression");
    }
    function = result_def->GetFunction();
  } else {
    bool is_func = GetExpr()->GetType().Is<Type::TFunc>();
    if (!is_func) {
      throw TExprError(HERE, GetPosRange(), "Ths lhs of the function call must be a callable function expression");
    }
    throw TNotImplementedError(HERE, GetPosRange(), "Expressions returning function objects are not supported");
  }
  assert(function);
  return function;
}

TFunctionApp::TPtr TFunctionApp::New(
    const TExpr::TPtr &expr,
    const TFunctionAppArgMap &function_app_args,
    const TPosRange &pos_range) {
  return TFunctionApp::TPtr(new TFunctionApp(expr, function_app_args, pos_range));
}

TFunctionApp::TFunctionApp(
    const TExpr::TPtr &expr,
    const TFunctionAppArgMap &function_app_args,
    const TPosRange &pos_range)
      : TUnary(expr, pos_range),
        FunctionAppArgs(function_app_args) {
  for (auto function_app_arg : FunctionAppArgs) {
    (function_app_arg.second)->SetFunctionApp(this);
  }
}

TFunctionApp::~TFunctionApp() {
  assert(this);
  for (auto function_app_arg : FunctionAppArgs) {
    (function_app_arg.second)->UnsetFunctionApp(this);
  }
}

void TFunctionApp::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

const TFunctionApp::TFunctionAppArgMap &TFunctionApp::GetFunctionAppArgs() const {
  assert(this);
  return FunctionAppArgs;
}

Type::TType TFunctionApp::GetType() const {
  assert(this);
  auto function = GetFunction();
  auto function_app_args = FunctionAppArgs;
  auto params = function->GetParams();
  bool is_sequence = false;
  // Iterate through the parameters we need and check if it was given in the arguments
  for (auto param : params) {
    auto arg = function_app_args.find(param.first /* name */);
    if (arg == function_app_args.end()) { // expected argument not found
      // TODO: Tell them exactly which parameters are missing
      throw TExprError(HERE, GetPosRange(), "Function call with missing expected argument");
    }
    // Found
    auto arg_type = (arg->second)->GetExpr()->GetType();
    auto param_type = param.second;
    auto unwrapped_arg_type = Type::Unwrap(arg_type);
    is_sequence |= arg_type.Is<Type::TSeq>();
    if (unwrapped_arg_type != param_type && Type::TOpt::Get(unwrapped_arg_type) != param_type) {
      throw TExprError(HERE, GetPosRange(), "Function call with parameter type mismatch");
    }
    function_app_args.erase(arg);
  }
  if (!function_app_args.empty()) {
    for (auto function_app_arg : function_app_args) {
      Tools::Nycr::TError::TBuilder((function_app_arg.second)->GetExpr()->GetPosRange())
          << "unexpected argument: \"" << function_app_arg.first << '"';
    }
  }
  return is_sequence ? Type::TSeq::Get(function->GetReturnType()) : function->GetReturnType();
}