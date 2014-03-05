/* <stig/code_gen/context_var.h>

   Constants that live in the stig execution TContext object

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

    class TContextVar : public TInline {
      NO_COPY_SEMANTICS(TContextVar);
      public:

      typedef std::shared_ptr<const TContextVar> TPtr;

      enum TOp {SessionId, UserId, Now};

      static Type::TType GetType(TOp Op);

      TContextVar(const L0::TPackage *package, TOp op);

      void WriteExpr(TCppPrinter &out) const;

      /* Dependency graph */
      virtual void AppendDependsOn(std::unordered_set<TInline::TPtr> &/*dependency_set*/) const override {
        assert(this);
      }

      private:
      TOp Op;
    }; // TContextVar


  } // CodeGen

} // Stig

namespace std {

  template <>
  struct hash<Stig::CodeGen::TContextVar::TOp> {

    size_t operator()(const Stig::CodeGen::TContextVar::TOp &op) {
      return op;
    }

    typedef size_t return_type;
    typedef Stig::CodeGen::TContextVar::TOp argument_type;
  };
}