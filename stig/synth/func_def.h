/* <stig/synth/func_def.h>

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

#include <cassert>
#include <unordered_set>

#include <stig/stig.package.cst.h>
#include <stig/symbol/function.h>
#include <stig/symbol/param_def.h>
#include <stig/symbol/result_def.h>
#include <stig/synth/expr.h>
#include <stig/synth/scope_and_def.h>
#include <stig/synth/type.h>

namespace Stig {

  namespace Synth {

    /* Forward declarations. */
    class TExprFactory;
    class TParamFuncDef;
    class TPureFuncDef;

    /* TODO */
    class TFuncDef
        : public TDef {
      NO_COPY_SEMANTICS(TFuncDef);
      public:

      /* TODO */
      typedef std::unordered_set<TName> TParamNameSet;

      /* TODO */
      virtual ~TFuncDef();

      /* TODO */
      void AddParamName(const TName &name);

      /* TODO */
      Symbol::TFunction::TPtr GetSymbol() const;

      protected:

      /* TODO */
      TFuncDef(TScope *scope, const Package::Syntax::TFuncDef *func_def);

      /* TODO */
      virtual void BuildSecondarySymbol();

      /* TODO */
      void SetExpr(TExpr *expr);

      /* TODO */
      const Package::Syntax::TFuncDef *FuncDef;

      private:

      /* TODO */
      virtual TAction Build(int pass);

      /* TODO */
      virtual void ForEachInnerScope(const std::function<void (TScope *)> &cb);

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      /* TODO */
      TExpr *Expr;

      /* TODO */
      TParamNameSet ParamNames;

      /* TODO */
      Symbol::TFunction::TPtr Symbol;

    };  // TFuncDef

    /* TODO */
    template <>
    struct TDef::TInfo<TFuncDef> {
      static const char *Name;
    };

    /* TODO */
    class TParamFuncDef
        : public TFuncDef {
      NO_COPY_SEMANTICS(TParamFuncDef);
      public:

      /* TODO */
      TParamFuncDef(
          const TExprFactory *expr_factory,
          const Package::Syntax::TFuncDef *func_def,
          const Package::Syntax::TGivenExpr *given_expr);

      const Symbol::TParamDef::TPtr &GetParamDefSymbol() const;

      /* TODO */
      TType *GetType() const;

      private:

      /* TODO */
      void BuildSecondarySymbol();

      /* TODO */
      TFuncDef *EnclosingFunc;

      /* TODO */
      const Package::Syntax::TGivenExpr *GivenExpr;

      /* TODO */
      Symbol::TParamDef::TPtr ParamDefSymbol;

      /* TODO */
      TType *Type;

    };  // TParamFuncDef

    /* TODO */
    template <>
    struct TDef::TInfo<TParamFuncDef> {
      static const char *Name;
    };

    /* TODO */
    class TPureFuncDef
        : public TFuncDef {
      NO_COPY_SEMANTICS(TPureFuncDef);
      public:

      /* TODO */
      TPureFuncDef(const TExprFactory *expr_factory, const Package::Syntax::TFuncDef *func_def);

    };  // TPureFuncDef

    /* TODO */
    template <>
    struct TDef::TInfo<TPureFuncDef> {
      static const char *Name;
    };

  }  // Synth

}  // Stig
