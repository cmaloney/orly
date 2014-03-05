/* <stig/code_gen/built_in_call.cc>

   Implements <stig/code_gen/built_in_call.h>

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

#include <stig/code_gen/built_in_call.h>

#include <base/split.h>

using namespace Stig::CodeGen;

TBuiltInCall::TBuiltInCall(const L0::TPackage *package,
                           const TFunctionPtr &func,
                           TArguments &&arguments)
    : TInline(package, func->GetReturnType()),
      Arguments(std::move(arguments)),
      Func(func) {}

void TBuiltInCall::WriteExpr(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  out << Func->GetName() << '(';
  Base::Join(", ", Func->GetOrderedParams(), [this](const std::string &param_name, TCppPrinter &out) {
    out << Arguments.at(param_name);
  }, out);
  out << ')';
}