/* <stig/code_gen/builder.h>

   TODO

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

#pragma once

#include <stig/code_gen/inline.h>
#include <stig/code_gen/function.h>
#include <stig/expr/expr.h>

namespace Stig {

  namespace CodeGen {

    class TCodeScope;

    /* Builds a new inline from an expression tree. This entry point deals with maps. */
    TInline::TPtr BuildInline(const L0::TPackage *package, const Expr::TExpr::TPtr &expr, bool keep_mutable);

    /* The raw build function. Do not use this unless you know what you are doing (the code is part of the code
       generation machinery). This assumes that maps and the like have been dealt with in specific ways. */
    TInline::TPtr Build(const L0::TPackage *package, const Expr::TExpr::TPtr &expr, bool keep_mutable, bool keep_sequence=false);

  } // CodeGen

} // Stig



