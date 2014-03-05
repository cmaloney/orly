/* <stig/code_gen/binary_scoped_rhs.h>

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
#include <stig/code_gen/inline_scope.h>
#include <stig/expr/expr.h>

namespace Stig {

  namespace CodeGen {

    class TBinaryScopedRhs : public TInline {
      NO_COPY_SEMANTICS(TBinaryScopedRhs);
      public:

      typedef std::shared_ptr<const TBinaryScopedRhs> TPtr;

      enum TOp { AndThen, OrElse };  // TOp

      static TPtr New(const L0::TPackage *package, const Type::TType &ret_type, TOp op, const TInline::TPtr &lhs, const Expr::TExpr::TPtr &rhs);

      void WriteExpr(TCppPrinter &out) const;

      /* Dependency graph */
      virtual void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const override {
        assert(this);
        dependency_set.insert(Lhs);
        Lhs->AppendDependsOn(dependency_set);
        dependency_set.insert(Rhs);
        Rhs->AppendDependsOn(dependency_set);
      }

      private:

      TBinaryScopedRhs(const L0::TPackage *package, const Type::TType &ret_type, TOp op, const TInline::TPtr &lhs, const Expr::TExpr::TPtr &rhs);

      TOp Op;
      TInline::TPtr Lhs;
      TInlineScope::TPtr Rhs;

    }; // TBinaryScopedRhs

  } // CodeGen

} // Stig

namespace std {

  template <>
  struct hash<Stig::CodeGen::TBinaryScopedRhs::TOp> {

    typedef size_t result_type;
    typedef Stig::CodeGen::TBinaryScopedRhs::TOp argument_type;

    result_type operator()(const argument_type &that) {
      assert(&that);
      return static_cast<result_type>(that);
    }

  };  // hash<Stig::CodeGen::TBinaryScopedRhs::TOp>

}  // std
