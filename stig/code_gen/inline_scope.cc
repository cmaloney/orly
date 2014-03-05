/* <stig/code_gen/inline_scope.cc>

   Implements <stig/code_gen/inline_scope.h>

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

#include <stig/code_gen/inline_scope.h>

#include <base/assert_true.h>
#include <stig/code_gen/builder.h>
#include <stig/code_gen/context.h>

using namespace Stig;
using namespace Stig::CodeGen;

TInlineScope::TPtr TInlineScope::New(const L0::TPackage *package, const Expr::TExpr::TPtr &expr, bool keep_mutable) {
  TCodeScope *cs = new TCodeScope(Context::GetScope()->GetIdScope());
  try {
    TScopeCtx ctx(cs);
    return TPtr(new TInlineScope(package, cs, BuildInline(package, expr, keep_mutable)));
  } catch (...) {
    delete cs;
    throw;
  }
}

TInlineScope::~TInlineScope() {
  delete Scope;
}

void TInlineScope::WriteExpr(TCppPrinter &out) const {
  assert(this);
  assert(&out);
  out << "[=, &ctx] () -> " << GetReturnType() << " {" << Eol;
  /* Indent */ {
    TIndent indent(out);

    //TODO: The whole "WriteStart followed by writing out the body" should probably live in a CodeScope.
    Scope->WriteStart(out);
    out << "return " << Body << ';' << Eol;
  }
  out << "}";
}

TInlineScope::TInlineScope(const L0::TPackage *package, TCodeScope *scope, const TInline::TPtr &body) : TInline(package, body->GetReturnType()), Body(body),
    Scope(Base::AssertTrue(scope)) {}

TCppPrinter &Stig::CodeGen::operator<<(TCppPrinter &out, const Stig::CodeGen::TInlineScope::TPtr &that) {
  that->Write(out);

  return out;
}