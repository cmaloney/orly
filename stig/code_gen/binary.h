/* <stig/code_gen/binary.h>

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
#include <stig/expr/binary.h>

namespace Stig {

  namespace CodeGen {

    class TBinary : public TInline {
      NO_COPY_SEMANTICS(TBinary);
      public:

      typedef std::shared_ptr<const TBinary> TPtr;

      enum TOp {Add, And, Div, EqEq, Exponent, Gt, GtEq, In, Intersection, IsKnownExpr, Lt, LtEq, Modulo, Mult,
                Neq, Or, Sub, SymmetricDiff, Union, Xor};

      TBinary(const L0::TPackage *package, const Type::TType &ret_type, TOp op, const TInline::TPtr &lhs, const TInline::TPtr &rhs);

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
      /* Helper functions for WriteExpr */
      void Template(TCppPrinter &out, const char *name) const;
      void Infix(TCppPrinter &out, char op) const;
      void Call(TCppPrinter &out, const char *func) const;

      //Like Call, but the lhs and rhs are reversed.
      void CallReverse(TCppPrinter &out, const char *func) const;

      TOp Op;
      TInline::TPtr Lhs, Rhs;


    }; // TBinary


  } // CodeGen

} // Stig

namespace std {

  template <>
  struct hash<Stig::CodeGen::TBinary::TOp> {

    typedef size_t result_type;
    typedef Stig::CodeGen::TBinary::TOp argument_type;

    result_type operator()(const argument_type &that) {
      assert(&that);
      return static_cast<result_type>(that);
    }

  };  // hash<Stig::CodeGen::TBinary::TOp>

}  // std
