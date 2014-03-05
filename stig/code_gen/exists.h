/* <stig/code_gen/exists.h>

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

namespace Stig {

  namespace CodeGen {

    /* TODO */
    class TExists : public TInline {
      NO_COPY_SEMANTICS(TExists);
      public:

      /* TODO */
      typedef std::shared_ptr<const TExists> TPtr;

      /* TODO */
      TExists(const L0::TPackage *package,
              const Type::TType &ret_type,
              const Type::TType &val_type,
              const TInline::TPtr &expr);

      /* TODO */
      void WriteExpr(TCppPrinter &out) const;

      /* Dependency graph */
      virtual void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const override {
        assert(this);
        dependency_set.insert(Expr);
        Expr->AppendDependsOn(dependency_set);
      }

      private:

      /* TODO */
      TInline::TPtr Expr;

      /* TODO */
      Type::TType ValType;

    }; // TExists

  } // CodeGen

} // Stig
