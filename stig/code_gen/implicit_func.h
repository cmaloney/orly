/* <stig/code_gen/implicit_func.h>

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

#include <stig/code_gen/inline_func.h>

namespace Stig {

  namespace CodeGen {

    class TImplicitFunc : public TInlineFunc {
      NO_COPY_SEMANTICS(TImplicitFunc);
      public:

      typedef std::shared_ptr<TImplicitFunc> TPtr;

      enum class TCause {Collect, Filter, Map, Reduce, Sort, While};

      static TPtr New(const L0::TPackage *package, TCause cause, const Type::TType &ret_type, const TNamedArgs &args, const Expr::TExpr::TPtr &expr,
          bool keep_mutable);

      std::string GetName() const;
      Type::TType GetReturnType() const;
      Type::TType GetType() const;

      private:
      TImplicitFunc(const L0::TPackage *package, const TIdScope::TPtr &id_scope, TCause cause, const Type::TType &ret_type, const TNamedArgs &args,
          const Expr::TExpr::TPtr &expr, bool keep_mutable);

      TCause Cause;
      Type::TType Type;

    }; // TImplicitFunc


  } // CodeGen

} // Stig