/* <stig/synth/new_expr.h>

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

#include <stig/stig.package.cst.h>
#include <stig/synth/expr.h>
#include <stig/synth/func_def.h>
#include <stig/synth/scope_and_def.h>

namespace Stig {

  namespace Synth {

    /* TODO */
    class TParamFuncDef;
    class TLhsRhsableExpr;
    class TStartableExpr;
    class TThatableExpr;

    /* TODO */
    class TExprFactory {
      public:

      /* TODO */
      TExprFactory(
          TScope *outer_scope,
          TFuncDef *enclosing_func = nullptr,
          TParamFuncDef *param_func = nullptr,
          TLhsRhsableExpr *sort_expr = nullptr,
          TStartableExpr *startable_expr = nullptr,
          TThatableExpr *thatable_expr = nullptr);

      /* TODO */
      TExpr *NewExpr(const Package::Syntax::TExpr *root) const;

      /* TODO */
      TFuncDef *EnclosingFunc;

      /* TODO */
      TScope *OuterScope;

      /* TODO */
      TParamFuncDef *ParamFunc;

      /* TODO */
      TLhsRhsableExpr *LhsRhsableExpr;

      /* TODO */
      TStartableExpr *StartableExpr;

      /* TODO */
      TThatableExpr *ThatableExpr;

    };  // TExprFactory

  }  // Synth

}  // Stig
