/* <stig/code_gen/call.cc>

   Implements <stig/code_gen/call.h>

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

#include <stig/code_gen/call.h>

#include <base/split.h>

using namespace Stig::CodeGen;

TCall::TCall(const L0::TPackage *package,
             const TFunction::TPtr &func,
             const TArgs &args)
    : TInline(package, func->GetReturnType()),
      Args(args),
      Func(func) {}

void TCall::WriteExpr(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  Func->WriteName(out);
  out << '(';

  if (Func->IsTopLevel()) {
    out << "ctx";
    if(!Args.empty()) {
      out << ", ";
    }
  }

  Base::Join(", ", Args, out) << ')';

}