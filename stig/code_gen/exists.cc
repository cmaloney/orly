/* <stig/code_gen/exists.cc>

   Implements <stig/code_gen/exists.h>

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

#include <stig/code_gen/exists.h>
#include <stig/type/unwrap.h>

using namespace Stig;
using namespace Stig::CodeGen;

TExists::TExists(const L0::TPackage *package,
                 const Type::TType &ret_type,
                 const Type::TType &val_type,
                 const TInline::TPtr &expr)
    : TInline(package, ret_type),
      Expr(expr),
      ValType(val_type) {}

void TExists::WriteExpr(TCppPrinter &out) const {
  const Base::TUuid &index_id = Package->GetIndexIdFor(Expr->GetReturnType(), ValType);
  char uuid[37];
  index_id.FormatUnderscore(uuid);
  out << "Exists(ctx.GetFlux(), " << Expr << ", " << TStigNamespace(Package->GetNamespace()) << "::My" << uuid << ")";
}