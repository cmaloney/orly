/* <stig/code_gen/inline.h>

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

#include <base/no_copy_semantics.h>
#include <base/opt.h>
#include <stig/code_gen/id.h>
#include <stig/code_gen/package_base.h>
#include <stig/type/impl.h>

namespace Stig {

  namespace CodeGen {

    class TInline {
      NO_COPY_SEMANTICS(TInline);
      public:

      typedef std::shared_ptr<const TInline> TPtr;

      virtual ~TInline() = default;

      const TId<TIdKind::Var> &GetId() const;

      Type::TType GetReturnType() const;

      bool HasId() const;

      //Used by common sub expression eliminator.
      void SetCommonSubexpressionId(TId<TIdKind::Var> &&id) const;

      /* Writes the common subexpression eliminated variant of this inline. */
      void Write(TCppPrinter &out) const;

      /* Pure virtual inlines should provide which emits the inline's C++ expression. */
      virtual void WriteExpr(TCppPrinter &out) const = 0;

      /* Dependency graph */
      virtual void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const = 0;

      /* We need to know if this expression is free */
      virtual bool IsFree() const {
        return false;
      }

      protected:
      TInline(const L0::TPackage *package, Type::TType type);

      const L0::TPackage *Package;

      private:
      Type::TType ReturnType; //The return type.
      //This is set by SetCommonSubExpressionId for common subexpression elimination.
      mutable Base::TOpt<TId<TIdKind::Var>> Id;

    }; // TInline

    Stig::CodeGen::TCppPrinter &operator<<(Stig::CodeGen::TCppPrinter &out,
        const Stig::CodeGen::TInline::TPtr &ptr);

  } // CodeGen

} // Stig