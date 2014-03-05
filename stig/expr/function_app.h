/* <stig/expr/function_app.h>

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

#include <map>

#include <base/no_copy_semantics.h>
#include <stig/expr/unary.h>
#include <stig/expr/visitor.h>
#include <stig/symbol/function.h>
#include <stig/symbol/root.h>

namespace Stig {

  namespace Expr {

    class TFunctionApp;

    class TFunctionAppArg
        : public Symbol::TRoot {
      NO_COPY_SEMANTICS(TFunctionAppArg);
      public:

      typedef std::shared_ptr<TFunctionAppArg> TPtr;

      static TPtr New(const TExpr::TPtr &expr);

      TFunctionApp *GetFunctionApp() const;

      void SetFunctionApp(TFunctionApp *function_app);

      void UnsetFunctionApp(TFunctionApp *function_app);

      private:

      TFunctionAppArg(const TExpr::TPtr &expr);

      TFunctionApp *FunctionApp;

    };  // TFunctionAppArg

    class TFunctionApp
        : public TUnary {
      NO_COPY_SEMANTICS(TFunctionApp);
      public:

      typedef std::shared_ptr<TFunctionApp> TPtr;

      // NOTE: This is a std::map because when we go to code gen, we want them all ordered by name.
      typedef std::map<std::string, TFunctionAppArg::TPtr> TFunctionAppArgMap;

      static TPtr New(
          const TExpr::TPtr &expr,
          const TFunctionAppArgMap &function_app_args,
          const TPosRange &pos_range);

      ~TFunctionApp();

      virtual void Accept(const TVisitor &visitor) const;

      Symbol::TAnyFunction::TPtr GetFunction() const;

      const TFunctionAppArgMap &GetFunctionAppArgs() const;

      virtual Type::TType GetType() const;

      private:

      TFunctionApp(
          const TExpr::TPtr &expr,
          const TFunctionAppArgMap &function_app_args,
          const TPosRange &pos_range);

      TFunctionAppArgMap FunctionAppArgs;

    };  // TFunctionApp

  }  // Expr

}  // Stig
