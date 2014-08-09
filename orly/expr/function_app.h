/* <orly/expr/function_app.h>

   TODO

   Copyright 2010-2014 OrlyAtomics, Inc.

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

#include <base/class_traits.h>
#include <orly/expr/unary.h>
#include <orly/expr/visitor.h>
#include <orly/symbol/function.h>
#include <orly/symbol/root.h>

namespace Orly {

  namespace Expr {

    class TFunctionApp;

    class TFunctionAppArg
        : public Symbol::TRoot {
      NO_COPY(TFunctionAppArg);
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
      NO_COPY(TFunctionApp);
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

      virtual Type::TType GetTypeImpl() const override;

      private:

      TFunctionApp(
          const TExpr::TPtr &expr,
          const TFunctionAppArgMap &function_app_args,
          const TPosRange &pos_range);

      TFunctionAppArgMap FunctionAppArgs;

    };  // TFunctionApp

  }  // Expr

}  // Orly
