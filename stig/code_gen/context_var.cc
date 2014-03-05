/* <stig/code_gen/context_var.cc>

   Implements <stig/code_gen/context_var.h>

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

#include <stig/code_gen/context_var.h>

#include <stig/type/id.h>
#include <stig/type/opt.h>
#include <stig/type/time_pnt.h>
#include <stig/type/unknown.h>

using namespace Stig;
using namespace Stig::CodeGen;

Type::TType TContextVar::GetType(TOp Op) {
  switch(Op) {
    case SessionId:
      return Type::TId::Get();
    case UserId:
      return Type::TOpt::Get(Type::TId::Get());
    case Now:
      return Type::TTimePnt::Get();
  }

  //NOTE: GCC yells about no return.... So make sure we can't hit this bad return to silence the GCC warning.
  Base::TError::Abort(HERE);
}

TContextVar::TContextVar(const L0::TPackage *package, TOp op) : TInline(package, GetType(op)), Op(op) {}

void TContextVar::WriteExpr(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  out << "ctx.";
  switch(Op) {
    case SessionId: out << "GetSessionId";
      break;
    case UserId: out << "GetAccountId"; //TODO: UserId
      break;
    case Now: out << "Now";
      break;
  }
  out << "()";
}